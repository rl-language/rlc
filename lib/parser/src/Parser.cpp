/*
Copyright 2024 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "rlc/parser/Parser.hpp"

#include <system_error>

#include "llvm/Support/Error.h"
#include "rlc/parser/Lexer.hpp"
#include "rlc/utils/Error.hpp"

using namespace llvm;
using namespace rlc;
using namespace std;

[[nodiscard]] mlir::Location Parser::getCurrentSourcePos() const { return pos; }

[[nodiscard]] mlir::Location Parser::getLastTokenEndPos() const
{
	return lastTokenEndpos;
}

void Parser::next()
{
	lastTokenEndpos = mlir::FileLineColLoc::get(
			ctx,
			fileName,
			lexer.getCurrentLine(),
			std::max<int64_t>(1, lexer.getLastNonWhiteSpaceColumn()));
	if (current == Token::Identifier)
		lIdent = lexer.lastIndent();
	if (current == Token::Int64 or current == Token::Character)
	{
		lInt64 = lexer.lastInt64();
		lString = lexer.lastString();
	}
	if (current == Token::Double)
	{
		lDouble = lexer.lastDouble();
		lString = lexer.lastString();
	}
	if (current == Token::KeywordComment)
	{
		lString = lexer.lastString();
	}
	if (current == Token::String)
		lString = lexer.lastString();
	current = lexer.next();
	pos = mlir::FileLineColLoc::get(
			ctx,
			fileName,
			lexer.getStartOfTokenLine(),
			std::max<int64_t>(1, lexer.getStartOfTokenCol()));
}

bool Parser::accept(Token t)
{
	if (current != t)
		return false;

	next();
	return true;
}

bool Parser::acceptEndOfLine()
{
	if (current == Token::KeywordComment)
	{
		auto c = comment();
		if (!c)
		{
			abort();
		}
		return true;
	}

	return accept(Token::Newline);
}

Expected<Token> Parser::expect(Token t)
{
	if (accept(t))
		return t;

	std::string errorMessage = "Unexpected token: \'";
	errorMessage += tokenToString(current);
	errorMessage += "\', expected \'";
	errorMessage += tokenToString(t);
	errorMessage += "\'";
	return make_error<RlcError>(
			std::move(errorMessage),
			RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
			getCurrentSourcePos());
}

#define EXPECT(Token, ...)                                                     \
	if (auto e = expect(Token); !e)                                              \
	{                                                                            \
		__VA_ARGS__;                                                               \
		return e.takeError();                                                      \
	}

#define TRY(outVar, expression, ...)                                           \
	auto outVar = expression;                                                    \
	if (!outVar)                                                                 \
	{                                                                            \
		__VA_ARGS__;                                                               \
		return outVar.takeError();                                                 \
	}

Expected<mlir::rlc::Comment> Parser::endOfLine()
{
	if (current == Token::KeywordComment)
	{
		return comment();
	}

	EXPECT(Token::Newline);
	return mlir::rlc::Comment(nullptr);
}

llvm::Expected<mlir::Value> Parser::builtinFromArray()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordFromArray);
	EXPECT(Token::LAng);
	TRY(shugarType, singleTypeUse());
	EXPECT(Token::RAng);
	EXPECT(Token::LPar);
	TRY(size, expression());
	EXPECT(Token::RPar);

	return builder.createFromByteArrayOp(
			location, shugarType->getType(), *size, *shugarType);
}

llvm::Expected<mlir::Value> Parser::stringExpression()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::String);
	auto value = builder.create<mlir::rlc::StringLiteralOp>(location, lString);
	if (not accept<Token::Identifier>())
		return value;

	location = getCurrentSourcePos();
	auto ref = builder.createUnresolvedReference(location, lIdent);

	location = getCurrentSourcePos();
	auto op =
			builder
					.create<mlir::rlc::CallOp>(
							location, unkType(), ref, false, mlir::ValueRange({ value }))
					.getResult(0);
	op.getDefiningOp()->setAttr("post_fix_call", builder.getUnitAttr());
	return op;
}

// initializerList: "[" (expression ( "," expression)*)? "]"
llvm::Expected<mlir::Value> Parser::initializerList()
{
	auto location = getCurrentSourcePos();
	auto insertionPoint = builder.saveInsertionPoint();
	EXPECT(Token::LSquare);
	mlir::Region region;
	auto* bb = builder.createBlock(&region);
	builder.setInsertionPointToStart(bb);

	llvm::SmallVector<mlir::Value> expressions;

	auto onExit = [&]() -> mlir::Value {
		builder.create<mlir::rlc::Yield>(location, expressions);
		builder.restoreInsertionPoint(insertionPoint);
		auto toReturn = builder.create<mlir::rlc::InitializerListOp>(
				location, mlir::rlc::UnknownType::get(builder.getContext()));
		toReturn.getBody().takeBody(region);
		return toReturn;
	};

	if (accept<Token::RSquare>())
		return onExit();

	do
	{
		while (acceptEndOfLine() or accept<Token::Indent>() or
					 accept<Token::Deindent>())
			;
		TRY(arg, expression(), onExit());
		expressions.push_back(*arg);
	} while (accept<Token::Comma>());
	EXPECT(Token::RSquare, onExit());

	return onExit();
}

llvm::Expected<mlir::Value> Parser::builtinToArray()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordToArray);
	EXPECT(Token::LPar);
	TRY(size, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::AsByteArrayOp>(
			location, mlir::rlc::UnknownType::get(builder.getContext()), *size);
}

// builtinMangedName : "__builtin_mangled_name(" expression ")"
Expected<mlir::Value> Parser::builtinMangledName()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordMangledName);
	EXPECT(Token::LPar);
	TRY(arg, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::BuiltinMangledNameOp>(location, *arg);
}

// builtinMangedName : "__builtin_as_ptr_do_no_use(" expression ")"
Expected<mlir::Value> Parser::builtinAsPtr()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordAsPtr);
	EXPECT(Token::LPar);
	TRY(arg, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::BuiltinAsPtr>(location, *arg);
}

// builtinMalloc : "__builtin_destroy_do_not_use(" expression ")"
Expected<mlir::Operation*> Parser::builtinDestroy()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordDestroy);
	EXPECT(Token::LPar);
	TRY(arg, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::DestroyOp>(location, *arg);
}

// builtinMalloc : "__builtin_construct_do_not_use(" expression ")"
Expected<mlir::Operation*> Parser::builtinConstruct()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordContstruct);
	EXPECT(Token::LPar);
	TRY(arg, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::InplaceInitializeOp>(location, *arg);
}

// builtinMalloc : "__builtin_malloc_do_not_use<" typeUse ">(" expression ")"
Expected<mlir::Value> Parser::builtinMalloc()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordMalloc);
	EXPECT(Token::LAng);
	TRY(shugarType, singleTypeUse());
	EXPECT(Token::RAng);
	EXPECT(Token::LPar);
	TRY(size, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::MallocOp>(
			location,
			mlir::rlc::OwningPtrType::get(shugarType->getType()),
			*size,
			*shugarType);
}

// builtinFree : "__builtin_free_do_not_use(" expression ")\n"
Expected<mlir::rlc::FreeOp> Parser::builtinFree()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordFree);
	EXPECT(Token::LPar);
	TRY(toDelete, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::FreeOp>(location, *toDelete);
}

/**
 * primaryExpression : Ident ("::" Ident)? | Double | int64 | "true" | "false" |
 * "(" expression ")"  | builtinMalloc | builtinFromArray | builtinToArray |
 * initializerList | string
 */
Expected<mlir::Value> Parser::primaryExpression()
{
	auto location = getCurrentSourcePos();

	if (accept<Token::Identifier>())
	{
		if (not accept<Token::ColonsColons>())
			return builder.create<mlir::rlc::UnresolvedReference>(location, lIdent);

		auto enumName = lIdent;
		std::string enumField = "";
		if (accept(Token::Identifier))
		{
			enumField = lIdent;
		}

		auto toReturn = builder.create<mlir::rlc::UncheckedEnumUse>(
				location,
				mlir::rlc::UnknownType::get(builder.getContext()),
				enumName,
				enumField);
		if (enumField.empty())
			EXPECT(Token::Identifier);
		return toReturn;
	}

	if (accept<Token::Double>())
		return builder.create<mlir::rlc::Constant>(location, lDouble, lString);

	if (accept<Token::Int64>())
		return builder.create<mlir::rlc::Constant>(location, lInt64, lString);

	if (accept<Token::Character>())
		return builder.create<mlir::rlc::Constant>(
				location, int8_t(lInt64), lString);

	if (accept<Token::KeywordFalse>())
		return builder.create<mlir::rlc::Constant>(location, false);

	if (accept<Token::KeywordTrue>())
		return builder.create<mlir::rlc::Constant>(location, true);

	if (current == Token::KeywordMalloc)
		return builtinMalloc();

	if (current == Token::KeywordFromArray)
		return builtinFromArray();

	if (current == Token::KeywordMangledName)
		return builtinMangledName();

	if (current == Token::KeywordAsPtr)
		return builtinAsPtr();

	if (current == Token::String)
		return stringExpression();

	if (current == Token::KeywordToArray)
		return builtinToArray();

	auto pos = getCurrentSourcePos();
	if (accept<Token::LPar>())
	{
		TRY(exp, expression());
		EXPECT(Token::RPar);
		return builder.createBracketsOp(pos, (*exp).getType(), *exp);
	}

	if (current == Token::LSquare)
	{
		return initializerList();
	}

	return make_error<RlcError>(
			" empty expression",
			RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
			location);
}

/**
 * argumentExpressionList : (expression",")*expression
 */
Expected<SmallVector<mlir::Value, 3>> Parser::argumentExpressionList()
{
	SmallVector<mlir::Value, 3> exps;
	if (current == Token::RPar)
		return exps;

	do
	{
		TRY(exp, expression());
		exps.emplace_back(std::move(*exp));
	} while (accept<Token::Comma>());

	return exps;
}

mlir::Type Parser::unkType() { return mlir::rlc::UnknownType::get(ctx); }

/**
 * usingTypeStatement : `using` ident `=` `Type` `(` expression `)`
 */
Expected<mlir::Operation*> Parser::usingTypeStatement()
{
	EXPECT(Token::KeywordUsing);
	auto location = getCurrentSourcePos();
	auto startLocation = getCurrentSourcePos().cast<mlir::FileLineColLoc>();
	EXPECT(Token::Identifier);
	auto endLocation = getLastTokenEndPos().cast<mlir::FileLineColLoc>();
	auto typeName = lIdent;
	EXPECT(Token::Equal);
	if (not accept<Token::KeywordType>())
	{
		TRY(shugarType, singleTypeUse());
		return builder.create<mlir::rlc::TypeAliasOp>(
				location,
				typeName,
				shugarType->getType(),
				mlir::rlc::SourceRangeAttr::get(startLocation, endLocation),
				*shugarType);
	}
	EXPECT(Token::LPar);

	auto usingOp = builder.create<mlir::rlc::UsingTypeOp>(location, lIdent);
	auto* bb = builder.createBlock(&usingOp.getBody());
	builder.setInsertionPointToStart(bb);

	auto onExit = [&](mlir::Value val) {
		if (not val)
			val = builder.create<mlir::rlc::Constant>(getCurrentSourcePos(), false);
		builder.create<mlir::rlc::Yield>(location, mlir::ValueRange({ val }));
		builder.setInsertionPointAfter(usingOp);
	};

	TRY(accExp, expression(), onExit(nullptr));
	EXPECT(Token::RPar);

	onExit(*accExp);

	return usingOp;
}

// canExpression : "can"? postFixExpression
llvm::Expected<mlir::Value> Parser::canCallExpression()
{
	auto location = getCurrentSourcePos();
	if (not accept<Token::KeywordCan>())
		return postFixExpression();

	auto canExp = builder.create<mlir::rlc::UncheckedCanOp>(location);
	auto* bb = builder.createBlock(&canExp.getBody());
	builder.setInsertionPointToStart(bb);

	auto onExit = [&]() {
		canExp.erase();
		builder.setInsertionPointAfter(canExp);
	};

	TRY(exp, postFixExpression(), onExit());
	builder.setInsertionPointAfter(canExp);
	return canExp;
}

/**
 * postFixExpression :
 *			((postFixExpression)*
 *					"is" type
 *					| "is" "Alternative"
 *					|("["expression"]"
 *					|"("argumentExpressionList ")" |
 *					. identifier? ["(" argumentExpressionList")"]))
 *			| primaryExpression
 */
Expected<mlir::Value> Parser::postFixExpression()
{
	TRY(maybeExp, primaryExpression());
	auto exp = std::move(*maybeExp);

	while (true)
	{
		auto location = getCurrentSourcePos();
		if (accept<Token::KeywordIs>())
		{
			if (accept<Token::KeywordAlternative>())
			{
				exp = builder.create<mlir::rlc::IsAlternativeTypeOp>(location, exp)
									.getResult();
				continue;
			}

			TRY(shugarType, singleTypeUse());
			exp = builder
								.create<mlir::rlc::UncheckedIsOp>(
										location, exp, shugarType->getType(), *shugarType)
								.getResult();
			continue;
		}
		if (accept<Token::LPar>())
		{
			TRY(args, argumentExpressionList());
			EXPECT(Token::RPar);
			exp =
					builder
							.create<mlir::rlc::CallOp>(location, unkType(), exp, false, *args)
							.getResult(0);
			continue;
		}

		if (accept<Token::LSquare>())
		{
			TRY(accExp, expression());
			EXPECT(Token::RSquare);
			exp = builder.create<mlir::rlc::ArrayAccess>(
					location, unkType(), exp, *accExp);
			continue;
		}

		if (accept<Token::Dot>())
		{
			// if the member name is missing emit the access anyway and so that the
			// lsp still knows there is a member access and can autocomplete. 			if
			std::string memberName = "";
			if (accept<Token::Identifier>())
				memberName = lIdent;
			auto callLoc = getCurrentSourcePos();
			if (not accept<Token::LPar>())
			{
				exp = builder.create<mlir::rlc::UnresolvedMemberAccess>(
						location, unkType(), exp, memberName);
				continue;
			}

			TRY(arguments, argumentExpressionList());
			EXPECT(Token::RPar);
			arguments->insert(arguments->begin(), std::move(exp));

			auto ref = builder.create<mlir::rlc::UnresolvedReference>(
					location, unkType(), memberName);

			exp = builder
								.create<mlir::rlc::CallOp>(
										callLoc, unkType(), ref->getResult(0), true, *arguments)
								.getResult(0);
			continue;
		}
		break;
	}
	return exp;
}

/**
 * unaryExpression : postFixExpression | unaryOperator unaryExpression
 */
Expected<mlir::Value> Parser::unaryExpression()
{
	auto location = getCurrentSourcePos();
	if (accept<Token::Plus>())
	{
		TRY(exp, unaryExpression());
		return std::move(*exp);
	}

	if (accept<Token::Minus>())
	{
		TRY(exp, unaryExpression());
		return builder.create<mlir::rlc::MinusOp>(location, unkType(), *exp);
	}

	if (accept<Token::Tilde>())
	{
		TRY(exp, unaryExpression());
		return builder.create<mlir::rlc::BitNotOp>(location, unkType(), *exp);
	}

	if (accept<Token::ExMark>())
	{
		TRY(exp, unaryExpression());
		return builder.create<mlir::rlc::NotOp>(location, unkType(), *exp);
	}

	return canCallExpression();
}

/**
 * multicativeExpression : unaryExpression | multiplicativeExpresion ('*' | '%'
 * | '/') unaryExpression
 */
Expected<mlir::Value> Parser::multyplicativeExpression()
{
	TRY(exp, unaryExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::Mult>())
	{
		TRY(rhs, multyplicativeExpression());
		return builder.create<mlir::rlc::MultOp>(location, unkType(), *exp, *rhs);
	}
	if (accept<Token::Divide>())
	{
		TRY(rhs, multyplicativeExpression());
		return builder.create<mlir::rlc::DivOp>(location, unkType(), *exp, *rhs);
	}
	if (accept<Token::Module>())
	{
		TRY(rhs, multyplicativeExpression());
		return builder.create<mlir::rlc::ReminderOp>(
				location, unkType(), *exp, *rhs);
	}

	return std::move(*exp);
}

/**
 * additiveExpression : multiplicativeExpression | additiveExpression ('+' |
 * '-')multiplicativeExpression
 */
Expected<mlir::Value> Parser::additiveExpression()
{
	TRY(exp, multyplicativeExpression());

	auto location = getCurrentSourcePos();
	while (true)
	{
		if (accept<Token::Plus>())
		{
			TRY(rhs, multyplicativeExpression());
			*exp = builder.create<mlir::rlc::AddOp>(location, *exp, *rhs);
		}
		else if (accept<Token::Minus>())
		{
			TRY(rhs, multyplicativeExpression());
			*exp = builder.create<mlir::rlc::SubOp>(location, *exp, *rhs);
		}
		else
			return std::move(*exp);
	}
}

/**
 * orExpression : shiftExpression (('<' | '>' | '<=' | '>=')
 * relationalExpression)*
 */
Expected<mlir::Value> Parser::relationalExpression()
{
	TRY(exp, additiveExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::LAng>())
	{
		if (accept<Token::LAng>())
		{
			TRY(rhs, additiveExpression());
			return builder.create<mlir::rlc::LeftShiftOp>(
					location, unkType(), *exp, *rhs);
		}
		TRY(rhs, relationalExpression());
		return builder.create<mlir::rlc::LessOp>(location, unkType(), *exp, *rhs);
	}
	if (accept<Token::RAng>())
	{
		if (accept<Token::RAng>())
		{
			TRY(rhs, additiveExpression());
			return builder.create<mlir::rlc::RightShiftOp>(
					location, unkType(), *exp, *rhs);
		}

		TRY(rhs, relationalExpression());
		return builder.create<mlir::rlc::GreaterOp>(
				location, unkType(), *exp, *rhs);
	}
	if (accept<Token::GEqual>())
	{
		TRY(rhs, relationalExpression());
		return builder.create<mlir::rlc::GreaterEqualOp>(
				location, unkType(), *exp, *rhs);
	}
	if (accept<Token::LEqual>())
	{
		TRY(rhs, relationalExpression());
		return builder.create<mlir::rlc::LessEqualOp>(
				location, unkType(), *exp, *rhs);
	}
	return std::move(*exp);
}

/**
 * orExpression : relationalExpression (('==' | '!=') relationalExpression)*
 */
Expected<mlir::Value> Parser::equalityExpression()
{
	TRY(exp, relationalExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::EqualEqual>())
	{
		TRY(rhs, equalityExpression());
		return builder.create<mlir::rlc::EqualOp>(location, *exp, *rhs);
	}
	if (accept<Token::NEqual>())
	{
		TRY(rhs, equalityExpression());
		return builder.create<mlir::rlc::NotEqualOp>(location, *exp, *rhs);
	}
	return std::move(*exp);
}

/**
 * orExpression : equalityExpression ('and' equalityExpression)*
 */
Expected<mlir::Value> Parser::andExpression()
{
	auto preLocation = getCurrentSourcePos();
	// precreate the and, if it is not needed, erase it afterwad
	auto andOp = builder.create<mlir::rlc::ShortCircuitingAnd>(preLocation);

	auto* lhs = builder.createBlock(&andOp.getLhs());
	builder.setInsertionPointToStart(lhs);

	// if we fail or there is not `and`, move all operations before the andOp
	// containg them and erase the andOp
	auto onExitWithoutAnd = [&, this]() {
		while (not lhs->empty())
			lhs->front().moveBefore(andOp);

		builder.setInsertionPointAfter(andOp);
		andOp.erase();
	};

	TRY(exp, equalityExpression(), onExitWithoutAnd());

	auto location = getCurrentSourcePos();
	if (not accept<Token::KeywordAnd>())
	{
		onExitWithoutAnd();
		return *exp;
	}

	// otherwise make sure we are yielding the lhs expression
	builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), *exp);

	// fix the location so that it points to the and token
	andOp->setLoc(location);

	auto* rhs = builder.createBlock(&andOp.getRhs());
	builder.setInsertionPointToStart(rhs);

	// if we have parsed the lhs correctly, then on failure wee need to clean up
	// the rhs . the insertion point must be after the andOp and we must yield
	// something
	auto onExitWithAnd = [&, this](mlir::Value toYield) {
		if (toYield == nullptr)
			toYield = builder.create<mlir::rlc::Constant>(location, false);

		builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), toYield);
		builder.setInsertionPointAfter(andOp);
	};

	TRY(rhsExp, andExpression(), onExitWithAnd(nullptr));
	onExitWithAnd(*rhsExp);

	return andOp;
}

/**
 * orExpression : andExpression ('or' andExpression)*
 */
Expected<mlir::Value> Parser::orExpression()
{
	auto preLocation = getCurrentSourcePos();
	// precreate the and, if it is not needed, erase it afterwad
	auto orOp = builder.create<mlir::rlc::ShortCircuitingOr>(preLocation);

	auto* lhs = builder.createBlock(&orOp.getLhs());
	builder.setInsertionPointToStart(lhs);

	// if we fail or there is not `and`, move all operations before the orOp
	// containg them and erase the orOp
	auto onExitWithoutAnd = [&, this]() {
		while (not lhs->empty())
			lhs->front().moveBefore(orOp);

		builder.setInsertionPointAfter(orOp);
		orOp.erase();
	};

	TRY(exp, andExpression(), onExitWithoutAnd());

	auto location = getCurrentSourcePos();
	if (not accept<Token::KeywordOr>())
	{
		onExitWithoutAnd();
		return *exp;
	}

	// otherwise make sure we are yielding the lhs expression
	builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), *exp);

	// fix the location so that it points to the and token
	orOp->setLoc(location);

	auto* rhs = builder.createBlock(&orOp.getRhs());
	builder.setInsertionPointToStart(rhs);

	// if we have parsed the lhs correctly, then on failure wee need to clean up
	// the rhs . the insertion point must be after the orOp and we must yield
	// something
	auto onExitWithAnd = [&, this](mlir::Value toYield) {
		if (toYield == nullptr)
			toYield = builder.create<mlir::rlc::Constant>(location, false);

		builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), toYield);
		builder.setInsertionPointAfter(orOp);
	};

	TRY(rhsExp, orExpression(), onExitWithAnd(nullptr));
	onExitWithAnd(*rhsExp);

	return orOp;
}

Expected<mlir::Value> Parser::bitWiseAndExpression()
{
	auto location = getCurrentSourcePos();
	TRY(lhs, orExpression());

	while (accept<Token::KeywordBitAnd>())
	{
		TRY(rhs, orExpression());
		*lhs = builder.create<mlir::rlc::BitAndOp>(location, *lhs, *rhs);
	}
	return *lhs;
}

Expected<mlir::Value> Parser::bitWiseXorExpression()
{
	auto location = getCurrentSourcePos();
	TRY(lhs, bitWiseAndExpression());

	while (accept<Token::KeywordBitXor>())
	{
		TRY(rhs, bitWiseAndExpression());
		*lhs = builder.create<mlir::rlc::BitXorOp>(location, *lhs, *rhs);
	}
	return *lhs;
}

Expected<mlir::Value> Parser::bitWiseOrExpression()
{
	auto location = getCurrentSourcePos();
	TRY(lhs, bitWiseXorExpression());

	while (accept<Token::VerticalPipe>())
	{
		TRY(rhs, bitWiseXorExpression());
		*lhs = builder.create<mlir::rlc::BitOrOp>(location, *lhs, *rhs);
	}
	return *lhs;
}

Expected<mlir::Value> Parser::expression()
{
	auto result = bitWiseOrExpression();

	return result;
}

/**
 * ClassField : TypeUse Identifier
 */
llvm::Expected<mlir::rlc::ClassFieldDeclarationAttr> Parser::classField()
{
	TRY(shugarType, singleTypeUse());
	EXPECT(Token::Identifier);
	return mlir::rlc::ClassFieldDeclarationAttr::get(
			lIdent, shugarType->getType(), *shugarType);
}

/**
 * ClassDeclaration : Ent[templateArguments] Identifier Colons Newline+ Indent
 * (classField Newline | functionDefinition)* Deindent
 */
llvm::Expected<mlir::rlc::ClassDeclaration> Parser::classDeclaration()
{
	auto location = getCurrentSourcePos();
	auto insertionPoint = builder.saveInsertionPoint();
	EXPECT(Token::KeywordClass);
	SmallVector<mlir::Type, 3> templateParameters;
	if (current == Token::LAng)
	{
		TRY(parameters, templateArguments());
		for (auto type : std::move(*parameters))
			templateParameters.push_back(type);
	}

	auto startLocation = getCurrentSourcePos().cast<mlir::FileLineColLoc>();
	EXPECT(Token::Identifier);
	string name = lIdent;
	auto endLocation = getLastTokenEndPos().cast<mlir::FileLineColLoc>();
	auto tokenLocation =
			mlir::rlc::SourceRangeAttr::get(startLocation, endLocation);
	EXPECT(Token::Colons);

	TRY(classComment, endOfLine());
	EXPECT(Token::Indent);

	mlir::Region region;
	auto* bb = builder.createBlock(&region);
	builder.setInsertionPointToStart(bb);

	const auto on_exit = [&]() -> mlir::rlc::ClassDeclaration {
		builder.restoreInsertionPoint(insertionPoint);
		auto toReturn = builder.create<mlir::rlc::ClassDeclaration>(
				location,
				unkType(),
				builder.getStringAttr(name),
				builder.getTypeArrayAttr(templateParameters),
				tokenLocation);

		toReturn.getBody().takeBody(region);
		mlir::rlc::setInlineComment(toReturn, *classComment);

		return toReturn;
	};

	while (acceptEndOfLine())
		;

	while (!accept<Token::Deindent>())
	{
		if (current == Token::KeywordComment)
		{
			TRY(c, comment());
		}
		else if (current == Token::KeywordFun)
		{
			TRY(f, functionDefinition(true), on_exit());
		}
		else if (accept<Token::KeywordPass>())
		{
		}
		else
		{
			auto loc = getCurrentSourcePos();
			TRY(field, classField(), on_exit());
			auto created = builder.createClassFieldDeclaration(loc, *field);
			TRY(fieldComment, endOfLine(), on_exit());
			mlir::rlc::setInlineComment(created, *fieldComment);
		}
		while (acceptEndOfLine())
			;
	}

	return on_exit();
}

/**
 * expressionStatement : expression (= expression)? '\n'
 */
llvm::Expected<mlir::rlc::ExpressionStatement> Parser::expressionStatement()
{
	auto location = getCurrentSourcePos();
	auto expStatement = builder.create<mlir::rlc::ExpressionStatement>(location);

	auto pos = builder.saveInsertionPoint();
	builder.createBlock(&expStatement.getBody());

	auto onExit = [&, this]() {
		builder.create<mlir::rlc::Yield>(getCurrentSourcePos());
		builder.restoreInsertionPoint(pos);
	};

	if (current == Token::KeywordFree)
	{
		TRY(_, builtinFree(), onExit());
	}
	else if (current == Token::KeywordDestroy)
	{
		TRY(_, builtinDestroy(), onExit());
	}
	else if (current == Token::KeywordContstruct)
	{
		TRY(_, builtinConstruct(), onExit());
	}
	else if (current == Token::KeywordAssert)
	{
		TRY(_, assertStatement(), onExit());
	}
	else
	{
		TRY(exp, expression(), onExit());
		auto locAssign = getCurrentSourcePos();
		if (accept<Token::Equal>())
		{
			TRY(rightHand, expression(), onExit());
			builder.create<mlir::rlc::AssignOp>(locAssign, *exp, *rightHand);
		}
	}

	onExit();
	TRY(comment, endOfLine(), onExit());
	mlir::rlc::setInlineComment(expStatement, *comment);

	return expStatement;
}

/**
 * requirementList: "{" expression ( ',' expression) * "}"
 */
llvm::Expected<bool> Parser::requirementList()
{
	if (!accept<Token::LBracket>())
	{
		builder.create<mlir::rlc::Yield>(getCurrentSourcePos());
		return false;
	}

	mlir::rlc::ShortCircuitingAnd lastAnd = nullptr;

	auto location = getCurrentSourcePos();
	auto insertionPoint = builder.saveInsertionPoint();
	auto onExit = [&]() {
		mlir::rlc::Yield lastAndYield = nullptr;

		// if we faild to parse and there is no yield, just create one returning
		// false.
		if (lastAnd.getLhs().front().empty() or
				!mlir::isa<mlir::rlc::Yield>(lastAnd.getLhs().front().back()))
		{
			auto toYield = builder.create<mlir::rlc::Constant>(location, false);
			lastAndYield = builder.create<mlir::rlc::Yield>(
					location, mlir::ValueRange{ toYield });
		}
		else
		{
			lastAndYield =
					mlir::cast<mlir::rlc::Yield>(lastAnd.getLhs().front().back());
		}

		lastAnd.replaceAllUsesWith(lastAndYield

																	 .getArguments()[0]);
		lastAndYield.erase();
		while (!lastAnd.getLhs().front().empty())
			lastAnd.getLhs().front().front().moveBefore(lastAnd);

		lastAnd.erase();
		builder.restoreInsertionPoint(insertionPoint);
	};

	// constructs a patter such as (and x, (and y, (and z, <empty>))) and then it
	// remove the inner most and turning into (and x, (and y, z))
	do
	{
		auto location = getCurrentSourcePos();
		lastAnd = builder.create<mlir::rlc::ShortCircuitingAnd>(location);
		builder.create<mlir::rlc::Yield>(
				getCurrentSourcePos(), mlir::ValueRange{ lastAnd });

		auto lhs = builder.createBlock(&lastAnd.getLhs());

		TRY(rest, expression(), onExit());
		builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), *rest);

		auto rhs = builder.createBlock(&lastAnd.getRhs());

	} while (accept<Token::Comma>());

	EXPECT(Token::RBracket, onExit());
	onExit();

	return true;
}

/**
 * subActionStatement: subaction (*)? ("(" argumentExpressionList ")")?
 * (`ident` =)? expression \n \n
 */
llvm::Expected<mlir::rlc::SubActionStatement> Parser::subActionStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordSubAction);

	bool runOnce = not accept<Token::Mult>();

	llvm::SmallVector<mlir::Value, 3> expressions;
	auto insertionPoint = builder.saveInsertionPoint();

	mlir::Region forwardedArgsRegion;
	mlir::Region bodyRegion;
	std::string name;
	mlir::rlc::Comment maybeComment = nullptr;

	auto onExit = [&, this](bool success) -> mlir::rlc::SubActionStatement {
		builder.restoreInsertionPoint(insertionPoint);
		if (not success)
			return nullptr;
		auto operation =
				builder.create<mlir::rlc::SubActionStatement>(location, name, runOnce);
		operation.getBody().takeBody(bodyRegion);
		operation.getForwardedArgs().takeBody(forwardedArgsRegion);
		mlir::rlc::setInlineComment(operation, maybeComment);
		return operation;
	};

	builder.createBlock(&forwardedArgsRegion);
	if (accept<Token::LPar>())
	{
		TRY(list, argumentExpressionList());
		expressions = *list;
		EXPECT(Token::RPar);
	}
	builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), expressions);
	builder.createBlock(&bodyRegion);
	TRY(exp, expression(), onExit(false));
	auto maybeName =
			mlir::dyn_cast<mlir::rlc::UnresolvedReference>(*exp->getDefiningOp());
	if (!maybeName or not accept(Token::Equal))
	{
		llvm::SmallVector<mlir::Value, 4> expressions;
		expressions.push_back(*exp);
		while (accept<Token::Comma>())
		{
			TRY(exp, expression(), onExit(false));
			expressions.push_back(*exp);
		}

		TRY(comment, endOfLine(), onExit(false));
		maybeComment = *comment;
		builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), expressions);
		return onExit(true);
	}

	name = maybeName.getName();
	(*exp).getDefiningOp()->erase();

	builder.setInsertionPointToStart(&bodyRegion.front());
	TRY(body, expression(), onExit(false));
	TRY(comment, endOfLine(), onExit(false));
	maybeComment = *comment;
	builder.create<mlir::rlc::Yield>(
			getCurrentSourcePos(), mlir::ValueRange(*body));

	return onExit(true);
}

llvm::Expected<mlir::rlc::ActionsStatement> Parser::actionsStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordActions);
	EXPECT(Token::Colons);
	TRY(comment, endOfLine());
	EXPECT(Token::Indent);

	llvm::SmallVector<llvm::SmallVector<mlir::Operation*, 2>, 4> ops;

	ops.push_back({});
	do
	{
		TRY(op, statement());
		ops.back().push_back(*op);
		while (acceptEndOfLine())
			;
	} while (current != Token::KeywordAction and current != Token::Deindent);

	while (not accept<Token::Deindent>())
	{
		while (acceptEndOfLine())
			;
		if (current == Token::KeywordAction)
			ops.push_back({});

		TRY(op, statement());
		ops.back().push_back(*op);
	}

	auto statements =
			builder.create<mlir::rlc::ActionsStatement>(location, ops.size());

	mlir::rlc::setInlineComment(statements, *comment);

	for (size_t i = 0; i < ops.size(); i++)
	{
		auto* bb = builder.createBlock(
				&statements.getActions()[i], statements.getActions()[i].begin());
		for (auto* op : ops[i])
			op->moveBefore(bb, bb->end());

		if (not mlir::isa<mlir::rlc::Yield>(ops[i].back()) and
				not mlir::isa<mlir::rlc::ReturnStatement>(ops[i].back()))
		{
			builder.setInsertionPointAfter(ops[i].back());
			builder.create<mlir::rlc::Yield>(ops[i].back()->getLoc());
		}
	}
	builder.setInsertionPointAfter(statements);

	return statements;
}

/**
 * actionStatement : actionDeclaration '\n'
 */
llvm::Expected<mlir::rlc::ActionStatement> Parser::actionStatement()
{
	TRY(op, actionDeclaration(false));
	auto action = llvm::cast<mlir::rlc::ActionStatement>(*op);

	auto pos = builder.saveInsertionPoint();
	auto onExit = [&, this]() { builder.restoreInsertionPoint(pos); };

	llvm::SmallVector<mlir::Location> locs;
	for (size_t i = 0; i < action.getResults().size(); i++)
		locs.push_back(action->getLoc());

	builder.createBlock(
			&action.getPrecondition(), {}, action.getResults().getTypes(), locs);
	TRY(list, requirementList(), onExit());

	TRY(comment, endOfLine(), onExit());
	mlir::rlc::setInlineComment(action, *comment);
	onExit();

	return action;
}

/**
 * ifStatement : if expression ':\nindent statementList ( else ifStatement |
 * else ':\n' statementList )?
 */
llvm::Expected<mlir::rlc::IfStatement> Parser::ifStatement()
{
	auto location = getCurrentSourcePos();
	auto expStatement = builder.create<mlir::rlc::IfStatement>(location);
	auto pos = builder.saveInsertionPoint();

	EXPECT(Token::KeywordIf);
	auto* condB = builder.createBlock(&expStatement.getCondition());
	auto* trueB = builder.createBlock(&expStatement.getTrueBranch());
	auto* elseB = builder.createBlock(&expStatement.getElseBranch());

	auto onExit = [&, this](mlir::Value condExp) {
		builder.setInsertionPointToEnd(condB);

		if (condExp == nullptr)
			condExp =
					builder.create<mlir::rlc::Constant>(getCurrentSourcePos(), false);
		auto yieldLoc = getCurrentSourcePos();
		builder.create<mlir::rlc::Yield>(yieldLoc, mlir::ValueRange(condExp));

		builder.setInsertionPointToEnd(trueB);
		emitYieldIfNeeded(getCurrentSourcePos());

		builder.setInsertionPointToEnd(elseB);
		emitYieldIfNeeded(
				elseB->empty() ? trueB->back().getLoc() : elseB->back().getLoc());

		builder.restoreInsertionPoint(pos);
	};

	builder.setInsertionPointToStart(condB);

	TRY(exp, expression(), onExit(nullptr));
	EXPECT(Token::Colons, onExit(*exp));

	TRY(commentCond, endOfLine(), onExit(*exp));
	mlir::rlc::setInlineComment(expStatement, *commentCond);

	builder.setInsertionPointToStart(trueB);
	TRY(tBranch, statementList(), onExit(*exp));

	builder.setInsertionPointToStart(elseB);
	if (accept<Token::KeywordElse>())
	{
		if (current == Token::KeywordIf)
		{
			TRY(fBranch, ifStatement(), onExit(*exp));
		}
		else
		{
			EXPECT(Token::Colons, onExit(*exp));
			TRY(commentElse, endOfLine(), onExit(*exp));
			TRY(fBranch, statementList(), onExit(*exp));
		}
	}

	onExit(*exp);
	return expStatement;
}

/**
 * forFieldStatment:  `in` expression ':\n' statementList
 */
Expected<mlir::rlc::ForLoopStatement> Parser::forLoopStatement(
		llvm::StringRef varName)
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordIn);
	auto pos = builder.saveInsertionPoint();
	mlir::Region expBB;
	auto* expBlock = builder.createBlock(&expBB, expBB.begin());
	TRY(exp, expression());
	builder.create<mlir::rlc::Yield>(location, mlir::ValueRange({ *exp }));

	EXPECT(Token::Colons);
	TRY(comment, endOfLine());

	mlir::Region region;
	auto* block = builder.createBlock(&region, region.begin());
	builder.setInsertionPointToStart(block);

	TRY(list, statementList());
	builder.restoreInsertionPoint(pos);
	auto forLoop = builder.createForLoopStatement(location);
	mlir::rlc::setInlineComment(forLoop, *comment);
	forLoop.getBody().takeBody(region);
	forLoop.getRangeExpression().takeBody(expBB);
	builder.setInsertionPointToEnd(&forLoop.getBody().front());

	builder.create<mlir::rlc::Yield>(location, mlir::ValueRange({}));
	builder.setInsertionPointToStart(&forLoop.getBody().front());
	builder.createForLoopVarDeclOp(
			location, mlir::rlc::UnknownType::get(builder.getContext()), varName);
	builder.setInsertionPointAfter(forLoop);
	return forLoop;
}

/**
 * forFieldStatment: `for` ident (`,` ident)* `of` expression (`,` expression)*
 * ':\n' statementList
 */
Expected<mlir::Operation*> Parser::forFieldStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordFor);

	llvm::SmallVector<std::string, 2> names;
	llvm::SmallVector<llvm::StringRef, 2> namesRef;
	llvm::SmallVector<mlir::Value, 2> values;

	do
	{
		EXPECT(Token::Identifier);
		names.push_back(lIdent);
	} while (accept<Token::Comma>());

	for (auto& name : names)
		namesRef.push_back(name);

	if (current == Token::KeywordIn)
	{
		if (names.size() != 1)
		{
			return make_error<RlcError>(
					"For loops can only declare a single induction variable",
					RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
					location);
		}
		TRY(forLoop, forLoopStatement(names[0]));
		return *forLoop;
	}

	EXPECT(Token::KeywordOf);

	auto expStatement = builder.create<mlir::rlc::ForFieldStatement>(
			location, builder.getStrArrayAttr(namesRef));

	auto pos = builder.saveInsertionPoint();

	llvm::SmallVector<mlir::Type, 2> introducedTypes;
	llvm::SmallVector<mlir::Location, 2> locations;
	for (auto name : names)
	{
		auto templateParameter = mlir::rlc::TemplateParameterType::get(
				builder.getContext(),
				(Twine("implicit_template_") + Twine(currentTemplateTypeIndex++)).str(),
				nullptr,
				false);

		introducedTypes.push_back(templateParameter);
		locations.push_back(location);
	}
	auto* bodyB = builder.createBlock(
			&expStatement.getBody(),
			expStatement.getBody().end(),
			introducedTypes,
			locations);

	auto* condB = builder.createBlock(&expStatement.getCondition());

	auto onExit = [&, this]() {
		builder.setInsertionPointToEnd(condB);
		builder.create<mlir::rlc::Yield>(location, values);

		builder.setInsertionPointToEnd(bodyB);
		emitYieldIfNeeded(
				bodyB->empty() ? expStatement.getLoc() : bodyB->back().getLoc());
		builder.restoreInsertionPoint(pos);
	};

	do
	{
		TRY(exp, expression(), onExit());
		values.push_back(*exp);
	} while (accept<Token::Comma>());

	EXPECT(Token::Colons, onExit());
	TRY(comment, endOfLine(), onExit());
	mlir::rlc::setInlineComment(expStatement, *comment);

	builder.setInsertionPointToStart(bodyB);
	TRY(statLis, statementList(), onExit());

	onExit();
	return expStatement;
}

/**
 * whileStatement : While exp ':\n' statementList
 */
Expected<mlir::rlc::WhileStatement> Parser::whileStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordWhile);

	auto expStatement = builder.create<mlir::rlc::WhileStatement>(location);
	auto pos = builder.saveInsertionPoint();

	auto* bodyB = builder.createBlock(&expStatement.getBody());
	auto* condB = builder.createBlock(&expStatement.getCondition());

	auto onExit = [&, this](mlir::Value exp) {
		builder.setInsertionPointToEnd(condB);
		if (exp == nullptr)
			exp = builder.create<mlir::rlc::Constant>(getCurrentSourcePos(), false);
		builder.create<mlir::rlc::Yield>(
				getLastTokenEndPos(), mlir::ValueRange({ exp }));
		builder.setInsertionPointToEnd(bodyB);
		emitYieldIfNeeded(bodyB->empty() ? location : bodyB->back().getLoc());
		builder.restoreInsertionPoint(pos);
	};

	TRY(exp, expression(), onExit(nullptr));
	EXPECT(Token::Colons, onExit(*exp));

	TRY(comment, endOfLine(), onExit(*exp));
	mlir::rlc::setInlineComment(expStatement, *comment);

	builder.setInsertionPointToStart(bodyB);
	TRY(statLis, statementList(), onExit(*exp));

	onExit(*exp);

	return expStatement;
}

/**
 * breakStatement : break '\n'
 */
Expected<mlir::rlc::BreakStatement> Parser::breakStatement()
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordBreak);
	TRY(comment, endOfLine());
	auto toReturn = builder.create<mlir::rlc::BreakStatement>(location);
	mlir::rlc::setInlineComment(toReturn, *comment);
	builder.createBlock(&toReturn.getOnEnd());
	builder.create<mlir::rlc::Yield>(location);
	builder.setInsertionPointAfter(toReturn);

	return toReturn;
}

/**
 * continueStatement: break '\n'
 */
Expected<mlir::rlc::ContinueStatement> Parser::continueStatement()
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordContinue);

	TRY(comment, endOfLine());
	auto toReturn = builder.create<mlir::rlc::ContinueStatement>(location);
	mlir::rlc::setInlineComment(toReturn, *comment);

	builder.createBlock(&toReturn.getOnEnd());
	builder.create<mlir::rlc::Yield>(location);
	builder.setInsertionPointAfter(toReturn);

	return toReturn;
}

/**
 * returnStatement : return [expression] '\n'
 */
Expected<mlir::rlc::ReturnStatement> Parser::returnStatement()
{
	auto location = getCurrentSourcePos();
	auto expStatement =
			builder.create<mlir::rlc::ReturnStatement>(location, unkType());

	auto pos = builder.saveInsertionPoint();
	builder.createBlock(&expStatement.getBody());

	EXPECT(Token::KeywordReturn);
	auto onExit = [&, this](mlir::Value toReturn) {
		builder.create<mlir::rlc::Yield>(
				getCurrentSourcePos(),
				!toReturn ? mlir::ValueRange() : mlir::ValueRange({ toReturn }));
		builder.restoreInsertionPoint(pos);
	};

	if (acceptEndOfLine())
	{
		onExit(nullptr);
		return expStatement;
	}

	TRY(exp, expression(), onExit(nullptr));

	TRY(comment, endOfLine(), onExit(*exp));
	mlir::rlc::setInlineComment(expStatement, *comment);
	onExit(*exp);
	return expStatement;
}

/**
 * assertStatements : assert`(` expression `,` strlit`)`'\n'
 */
Expected<mlir::rlc::AssertOp> Parser::assertStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordAssert);
	EXPECT(Token::LPar);
	TRY(exp, expression());
	EXPECT(Token::Comma);
	EXPECT(Token::String);
	auto message = lexer.lastString();
	EXPECT(Token::RPar);
	return builder.create<mlir::rlc::AssertOp>(location, *exp, message);
}

Expected<mlir::Operation*> Parser::statement()
{
	if (current == Token::KeywordAction)
		return actionStatement();

	if (current == Token::KeywordIf)
		return ifStatement();

	if (current == Token::KeywordReturn)
		return returnStatement();

	if (current == Token::KeywordBreak)
		return breakStatement();

	if (current == Token::KeywordContinue)
		return continueStatement();

	if (current == Token::KeywordSubAction)
		return subActionStatement();

	if (current == Token::KeywordWhile)
		return whileStatement();

	if (current == Token::KeywordUsing)
		return usingTypeStatement();

	if (current == Token::KeywordActions)
	{
		TRY(f, actionsStatement());
		return *f;
	}

	if (current == Token::KeywordFor)
		return forFieldStatement();

	if (current == Token::KeywordLet || current == Token::KeywordRef ||
			current == Token::KeywordFrame)
		return declarationStatement();

	if (current == Token::Indent)
	{
		auto location = getCurrentSourcePos();
		auto list = builder.create<mlir::rlc::StatementList>(location);
		builder.createBlock(&list.getBody());
		builder.setInsertionPointToStart(&list.getBody().front());
		auto onExit = [&, this]() {
			emitYieldIfNeeded(location);
			builder.setInsertionPointAfter(list);
		};
		TRY(dc, statementList(), onExit());
		onExit();
		return list;
	}

	return expressionStatement();
}

void Parser::removeYieldIfNotNeeded()
{
	auto& ops = builder.getBlock()->getOperations();
	auto size = ops.size();
	if (size == 0)
		return;
	if (not mlir::isa<mlir::rlc::Yield>(ops.back()))
		return;

	builder.getBlock()->back().erase();
	builder.setInsertionPointToEnd(builder.getBlock());
}

void Parser::emitYieldIfNeeded(mlir::Location loc)
{
	if (not builder.getBlock()->empty() and
			builder.getBlock()->back().hasTrait<mlir::OpTrait::IsTerminator>())
		return;

	builder.create<mlir::rlc::Yield>(loc);
}

/**
 * declarationStatement : ('let' | `ref` | `frm`) identifier ['=' expression |
 * ':' type_use
 * ]
 */
Expected<mlir::rlc::DeclarationStatement> Parser::declarationStatement()
{
	auto location = getCurrentSourcePos();
	mlir::Type type = mlir::rlc::UnknownType::get(builder.getContext());
	if (accept<Token::KeywordRef>())
		type = mlir::rlc::ReferenceType::get(type);
	else if (accept<Token::KeywordFrame>())
		type = mlir::rlc::FrameType::get(type);
	else
		EXPECT(Token::KeywordLet);

	EXPECT(Token::Identifier);
	auto name = lIdent;

	auto expStatement =
			builder.create<mlir::rlc::DeclarationStatement>(location, type, name);

	auto pos = builder.saveInsertionPoint();
	builder.createBlock(&expStatement.getBody());

	auto onExit = [&, this](mlir::Value initializer) {
		if (initializer == nullptr)
			initializer =
					builder.create<mlir::rlc::Constant>(getCurrentSourcePos(), false);

		builder.create<mlir::rlc::Yield>(
				getCurrentSourcePos(), mlir::ValueRange({ initializer }));
		builder.restoreInsertionPoint(pos);
	};

	if (accept<Token::Equal>())
	{
		TRY(exp, expression(), onExit(nullptr));

		TRY(comment, endOfLine(), onExit(*exp));
		onExit(*exp);
		mlir::rlc::setInlineComment(expStatement, *comment);

		return expStatement;
	}

	EXPECT(Token::Colons, onExit(nullptr));
	auto typeLoc = getCurrentSourcePos();
	TRY(shugarType, singleTypeUse(), onExit(nullptr));
	auto exp = builder.create<mlir::rlc::ConstructOp>(
			typeLoc, shugarType->getType(), *shugarType);

	TRY(comment, endOfLine(), onExit(exp));
	mlir::rlc::setInlineComment(expStatement, *comment);
	onExit(exp);
	return expStatement;
}

/**
 * statmentList : indent (statement)* deindent
 */
Expected<bool> Parser::statementList()
{
	EXPECT(Token::Indent);
	while (!accept<Token::Deindent>())
	{
		while (acceptEndOfLine())
			;

		TRY(s, statement());
	}

	return true;
}

/**
 * functionTypeUse : [singleTypeUse ("->" singleTypeUse )*]
 */
Expected<mlir::rlc::FunctionUseType> Parser::functionTypeUse()
{
	SmallVector<mlir::Type, 2> tpUse;

	TRY(singleTp, singleTypeUse());
	tpUse.emplace_back(singleTp->getType());
	while (accept<Token::Arrow>())
	{
		TRY(singleTp, singleTypeUse());
		tpUse.emplace_back((*singleTp).getType());
	}

	return mlir::rlc::FunctionUseType::get(ctx, tpUse);
}

/**
 * singleNonArrayTypeUse : "(" functionType ")" | identifier | "OwningPtr<"
 * singleTypeUse ">"
 */
Expected<mlir::rlc::ScalarUseType> Parser::singleNonArrayTypeUse()
{
	if (accept<Token::LPar>())
	{
		TRY(fType, functionTypeUse());
		EXPECT(Token::RPar);
		return mlir::rlc::ScalarUseType::get(ctx, *fType);
	}

	if (accept<Token::KeywordOwningPtr>())
	{
		EXPECT(Token::LAng);
		TRY(subType, singleTypeUse());
		EXPECT(Token::RAng);
		return mlir::rlc::ScalarUseType::get(
				ctx, mlir::rlc::OwningPtrType::get((*subType).getType()));
	}

	EXPECT(Token::Identifier);
	auto nm = lIdent;
	mlir::Type arraySize = mlir::rlc::IntegerLiteralType::get(ctx, 0);
	llvm::SmallVector<mlir::Type, 2> templateParametersTypes;
	if (accept<Token::LAng>())
	{
		do
		{
			if (accept<Token::Int64>())
			{
				templateParametersTypes.push_back(
						mlir::rlc::IntegerLiteralType::get(builder.getContext(), lInt64));
			}
			else
			{
				TRY(templateParameter, singleTypeUse());
				templateParametersTypes.push_back((*templateParameter).getType());
			}
		} while (accept<Token::Comma>());
		EXPECT(Token::RAng);
	}

	return mlir::rlc::ScalarUseType::get(ctx, nm, 0, templateParametersTypes);
}

/**
 * singleTypeUse : singleNonArrayTypeUse (["["int64"|Ident "]"] )* (|
 * singleTypeUse)*
 */
Expected<mlir::rlc::ShugarizedTypeAttr> Parser::singleTypeUse()
{
	mlir::FileLineColLoc typeBegin =
			getCurrentSourcePos().cast<mlir::FileLineColLoc>();
	llvm::SmallVector<mlir::Type, 2> seenTypes;

	do
	{
		TRY(typeUse, singleNonArrayTypeUse());

		while (accept<Token::LSquare>())
		{
			mlir::Type arraySize;
			if (accept<Token::Int64>())
			{
				arraySize = mlir::rlc::IntegerLiteralType::get(ctx, lInt64);
			}
			else
			{
				EXPECT(Token::Identifier);
				arraySize = mlir::rlc::ScalarUseType::get(ctx, lIdent, 0);
			}
			*typeUse =
					mlir::rlc::ScalarUseType::get(ctx, *typeUse, "", arraySize, {});
			EXPECT(Token::RSquare);
		}

		seenTypes.push_back(*typeUse);
	} while (accept<Token::VerticalPipe>());
	auto typeEnd = getLastTokenEndPos().cast<mlir::FileLineColLoc>();

	if (seenTypes.size() == 1)
		return mlir::rlc::ShugarizedTypeAttr::get(
				builder.getContext(),
				mlir::rlc::SourceRangeAttr::get(typeBegin, typeEnd),
				seenTypes.front());

	return mlir::rlc::ShugarizedTypeAttr::get(
			builder.getContext(),
			mlir::rlc::SourceRangeAttr::get(typeBegin, typeEnd),
			mlir::rlc::AlternativeType::get(ctx, seenTypes, ""));
}

Expected<mlir::rlc::FunctionArgumentAttr> Parser::argDeclaration()
{
	auto isFrame = accept<Token::KeywordFrame>();
	bool isCtx = not isFrame && accept<Token::KeywordCtx>();

	TRY(tp, singleTypeUse());
	if (isFrame)
		(*tp) = tp->replaceType(mlir::rlc::FrameType::get((*tp).getType()));
	if (isCtx)
		(*tp) = tp->replaceType(mlir::rlc::ContextType::get((*tp).getType()));
	auto nameStart = getCurrentSourcePos().cast<mlir::FileLineColLoc>();
	EXPECT(Token::Identifier);
	auto nameEnd = getLastTokenEndPos().cast<mlir::FileLineColLoc>();
	auto parName = lIdent;
	return mlir::rlc::FunctionArgumentAttr::get(
			builder.getContext(),
			parName,
			mlir::rlc::SourceRangeAttr::get(nameStart, nameEnd),
			*tp);
}

/**
 * functionDefinition : "(" [argDeclaration ("," argDeclaration)*] ")"
 */
Expected<llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 3>>
Parser::functionArguments()
{
	EXPECT(Token::LPar);

	llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 3> args;

	if (current != Token::RPar)
	{
		do
		{
			TRY(arg, argDeclaration());
			args.emplace_back(std::move(*arg));
		} while (accept<Token::Comma>());
	}

	EXPECT(Token::RPar);
	return args;
}

/**
 * templateArguments: "<" ident ident? ("," ident ident?)* ">"
 */
Expected<llvm::SmallVector<mlir::rlc::UncheckedTemplateParameterType, 2>>
Parser::templateArguments()
{
	llvm::SmallVector<mlir::rlc::UncheckedTemplateParameterType, 2> toReturn;
	EXPECT(Token::LAng);

	do
	{
		EXPECT(Token::Identifier);
		auto first = lIdent;
		if (accept<Token::Identifier>())
		{
			auto second = lIdent;
			toReturn.push_back(
					mlir::rlc::UncheckedTemplateParameterType::get(
							builder.getContext(), second, builder.getStringAttr(first)));
		}
		else
		{
			toReturn.push_back(
					mlir::rlc::UncheckedTemplateParameterType::get(
							builder.getContext(), first, builder.getStringAttr("")));
		}

	} while (accept<Token::Comma>());

	EXPECT(Token::RAng);
	return toReturn;
}

Expected<mlir::rlc::FunctionOp> Parser::externFunctionDeclaration()
{
	EXPECT(Token::KeywordExtern);
	TRY(result, functionDeclaration());

	TRY(comment, endOfLine());
	mlir::rlc::setInlineComment(*result, *comment);
	return std::move(*result);
}

/**
 * functionDeclaration : "fun" templateArguments identifier "("
 * [argDeclaration
 * ("," argDeclaration)*] ")" ["->" "ref"? singleTypeUse]
 */
Expected<mlir::rlc::FunctionOp> Parser::functionDeclaration(
		bool templateFunction, bool isMemberFunction)
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordFun);

	llvm::SmallVector<mlir::Type, 2> templateParameters;
	if (templateFunction and current == Token::LAng)
	{
		TRY(parameters, templateArguments());
		for (auto type : std::move(*parameters))
			templateParameters.push_back(type);
	}

	llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 3> args;
	mlir::Type retType = mlir::rlc::VoidType::get(ctx);
	mlir::rlc::SourceRangeAttr retTypeRange = nullptr;

	EXPECT(Token::Identifier);
	auto nm = lIdent;

	auto onExit = [&]() {
		llvm::SmallVector<mlir::Type, 3> argTypes;
		for (auto arg : args)
			argTypes.push_back(arg.getShugarizedType().getType());

		mlir::rlc::ShugarizedTypeAttr retTypeInfo =
				retTypeRange != nullptr
						? mlir::rlc::ShugarizedTypeAttr::get(retTypeRange, retType)
						: nullptr;

		auto fun = builder.create<mlir::rlc::FunctionOp>(
				location,
				builder.getStringAttr(nm),
				mlir::FunctionType::get(ctx, argTypes, { retType }),
				mlir::rlc::FunctionInfoAttr::get(
						builder.getContext(), args, retTypeInfo),
				isMemberFunction,
				templateParameters);
		return fun;
	};
	TRY(_args, functionArguments(), onExit());
	args = std::move(*_args);

	if (accept<Token::Arrow>())
	{
		bool isRef = accept<Token::KeywordRef>();
		TRY(actualRetType, singleTypeUse(), onExit());
		retType = (*actualRetType).getType();
		retTypeRange = (*actualRetType).getLocation();
		if (isRef)
			retType = mlir::rlc::ReferenceType::get(retType);
	}

	return onExit();
}

/**
 * functionDefinition : functionDeclaration ":\n" statementList
 */
Expected<mlir::rlc::FunctionOp> Parser::functionDefinition(
		bool isMemberFunction)
{
	TRY(fun, functionDeclaration(true, isMemberFunction));
	auto location = getCurrentSourcePos();
	auto pos = builder.saveInsertionPoint();
	llvm::SmallVector<mlir::Location, 2> argLocs;
	for (auto arg : fun->getInfo().getArguments())
		argLocs.push_back(arg.getNameLocation().getStart());

	auto* bodyB = builder.createBlock(
			&fun->getBody(), {}, fun->getArgumentTypes(), argLocs);

	auto onExit = [&, this]() {
		builder.setInsertionPointToEnd(bodyB);
		emitYieldIfNeeded(getCurrentSourcePos());
		builder.restoreInsertionPoint(pos);
	};

	// the precondition region is optional
	mlir::Region region;
	auto* condB =
			builder.createBlock(&region, {}, fun->getArgumentTypes(), argLocs);
	TRY(list, requirementList(), onExit());
	if (*list)
		fun->getPrecondition().takeBody(region);

	EXPECT(Token::Colons, onExit());

	TRY(comment, endOfLine(), onExit());
	mlir::rlc::setInlineComment(*fun, *comment);

	builder.setInsertionPointToEnd(bodyB);
	TRY(body, statementList(), onExit());
	onExit();

	return fun;
}

/**
 * actionDeclaration : "act" identifier "(" [argDeclaration (","
 * argDeclaration)*] ")" -> Type
 */
Expected<mlir::Operation*> Parser::actionDeclaration(bool actionFunction)
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordAction);
	EXPECT(Token::Identifier);
	auto nm = lIdent;

	mlir::SmallVector<mlir::rlc::FunctionArgumentAttr, 3> info;

	mlir::Type retType = mlir::rlc::ClassType::getIdentified(ctx, "", {});
	mlir::rlc::SourceRangeAttr retTypeSourceRange = nullptr;
	auto onExit = [&]() -> mlir::Operation* {
		mlir::SmallVector<mlir::Type, 3> argTypes;
		for (auto argument : info)
			argTypes.push_back(argument.getShugarizedType().getType());

		if (actionFunction)
		{
			mlir::rlc::ShugarizedTypeAttr retTypeInfo =
					retTypeSourceRange != nullptr
							? mlir::rlc::ShugarizedTypeAttr::get(retTypeSourceRange, retType)
							: nullptr;

			auto decl = builder.create<mlir::rlc::ActionFunction>(
					location,
					mlir::FunctionType::get(ctx, argTypes, { retType }),
					mlir::rlc::UnknownType::get(builder.getContext()),
					mlir::TypeRange(),
					builder.getStringAttr(nm),
					mlir::rlc::FunctionInfoAttr::get(
							builder.getContext(), info, retTypeInfo));
			auto pos = builder.saveInsertionPoint();
			llvm::SmallVector<mlir::Location> locs;
			for (size_t i = 0; i < decl.getFunctionType().getInputs().size(); i++)
				locs.push_back(decl->getLoc());

			auto* block = builder.createBlock(
					&decl.getBody(), {}, decl.getFunctionType().getInputs(), locs);
			auto* condB = builder.createBlock(
					&decl.getPrecondition(),
					{},
					decl.getFunctionType().getInputs(),
					locs);

			builder.setInsertionPointToStart(block);
			emitYieldIfNeeded(getCurrentSourcePos());
			builder.restoreInsertionPoint(pos);
			return decl;
		}
		else
		{
			auto toReturn = builder.create<mlir::rlc::ActionStatement>(
					location,
					argTypes,
					builder.getStringAttr(nm),
					mlir::rlc::FunctionInfoAttr::get(builder.getContext(), info));
			return toReturn;
		}
	};

	TRY(args, functionArguments(), onExit());

	info = *args;

	std::string name;
	if (actionFunction)
	{
		EXPECT(Token::Arrow, onExit());
		EXPECT(Token::Identifier, onExit());
		name = lIdent;
	}
	retType = mlir::rlc::ClassType::getIdentified(ctx, name, {});
	return onExit();
}

/**
 * actionDefinition : actionDeclaration ":\n" statementList
 */
Expected<mlir::rlc::ActionFunction> Parser::actionDefinition()
{
	TRY(op, actionDeclaration(true));
	auto decl = llvm::dyn_cast<mlir::rlc::ActionFunction>(*op);

	auto pos = builder.saveInsertionPoint();

	auto* block = &decl.getBody().front();
	auto* condB = &decl.getPrecondition().front();

	auto onExit = [&, this]() {
		removeYieldIfNotNeeded();
		emitYieldIfNeeded(getCurrentSourcePos());
		builder.restoreInsertionPoint(pos);
	};

	builder.setInsertionPoint(condB, condB->begin());
	TRY(list, requirementList(), onExit());

	builder.setInsertionPoint(block, block->begin());
	EXPECT(Token::Colons, onExit());

	TRY(comment, endOfLine(), onExit());
	mlir::rlc::setInlineComment(decl, *comment);
	TRY(body, statementList(), onExit());
	onExit();
	return decl;
}

/*
 * enumFieldDeclaration: "enum" colons ident (name = expression newline )*
 * deindent
 */
Expected<mlir::rlc::EnumFieldDeclarationOp> Parser::enumFieldDeclaration()
{
	auto loc = getCurrentSourcePos();
	auto pos = builder.saveInsertionPoint();
	auto onExit = [&]() { builder.restoreInsertionPoint(pos); };
	EXPECT(Token::Identifier, onExit());
	auto enumField = lIdent;

	if (not accept(Token::Colons))
	{
		TRY(comment, endOfLine(), onExit());
		auto toReturn =
				builder.create<mlir::rlc::EnumFieldDeclarationOp>(loc, lIdent);
		mlir::rlc::setInlineComment(toReturn, *comment);
		onExit();
		return toReturn;
	}

	auto toReturn =
			builder.create<mlir::rlc::EnumFieldDeclarationOp>(loc, lIdent);
	auto bb = builder.createBlock(&toReturn.getBody());
	TRY(comment, endOfLine(), onExit());
	mlir::rlc::setInlineComment(toReturn, *comment);
	EXPECT(Token::Indent, onExit());

	do
	{
		builder.setInsertionPointToEnd(bb);
		TRY(type, singleTypeUse(), onExit());
		EXPECT(Token::Identifier, onExit());
		auto current = builder.create<mlir::rlc::EnumFieldExpressionOp>(
				loc, (*type).getType(), lIdent);
		auto* inner = builder.createBlock(&current.getBody());
		builder.setInsertionPointToEnd(inner);

		EXPECT(Token::Equal, onExit());
		TRY(exp, expression(), onExit());
		current->setLoc((*exp).getLoc());

		builder.create<mlir::rlc::Yield>(
				getLastTokenEndPos(), mlir::ValueRange({ *exp }));
		TRY(comment, endOfLine(), onExit());
		mlir::rlc::setInlineComment(current, *comment);
	} while (not accept(Token::Deindent));

	onExit();
	return toReturn;
}

/*
 * enumDeclaration: "enum" ident:\nindent ident(\n indent)*\ndeindent
 */
Expected<mlir::rlc::EnumDeclarationOp> Parser::enumDeclaration()
{
	auto location = getCurrentSourcePos();
	auto pos = builder.saveInsertionPoint();
	EXPECT(Token::KeywordEnum);
	auto startLocation = getCurrentSourcePos().cast<mlir::FileLineColLoc>();
	EXPECT(Token::Identifier);
	auto endLocation = getLastTokenEndPos().cast<mlir::FileLineColLoc>();
	auto tokenLocation =
			mlir::rlc::SourceRangeAttr::get(startLocation, endLocation);
	auto enumName = lIdent;
	EXPECT(Token::Colons);
	TRY(comment, endOfLine());
	EXPECT(Token::Indent);

	mlir::Region region;
	auto* bb = builder.createBlock(&region);
	builder.setInsertionPointToStart(bb);

	const auto onExit = [&]() {
		builder.restoreInsertionPoint(pos);
		auto toReturn = builder.create<mlir::rlc::EnumDeclarationOp>(
				location, enumName, tokenLocation);

		toReturn.getBody().takeBody(region);
		mlir::rlc::setInlineComment(toReturn, *comment);

		return toReturn;
	};

	while (not accept<Token::Deindent>())
	{
		while (acceptEndOfLine())
			;
		if (current == Token::KeywordFun)
		{
			TRY(_, functionDefinition(true), onExit());
		}
		else
		{
			TRY(_, enumFieldDeclaration(), onExit());
		}
	}

	return onExit();
}

/**
 * traitDefinition: trait "<" ident ">:\n" indent (functionDeclaration "\n")*
 * deindent
 */
Expected<mlir::rlc::UncheckedTraitDefinition> Parser::traitDefinition()
{
	auto location = getCurrentSourcePos();
	auto pos = builder.saveInsertionPoint();
	auto onExit = [&, this]() { builder.restoreInsertionPoint(pos); };
	EXPECT(Token::KeywordTrait, onExit());
	EXPECT(Token::LAng, onExit());
	llvm::SmallVector<std::string, 3> identifiers;
	do
	{
		EXPECT(Token::Identifier, onExit());
		identifiers.push_back(lIdent);
	} while (accept<Token::Comma>());
	EXPECT(Token::RAng, onExit());
	EXPECT(Token::Identifier, onExit());
	auto traitName = lIdent;
	auto trait = builder.create<mlir::rlc::UncheckedTraitDefinition>(
			location,
			traitName,
			builder.getStrArrayAttr(
					llvm::SmallVector<llvm::StringRef, 3>(
							identifiers.begin(), identifiers.end())));
	builder.createBlock(&trait.getBody());
	builder.setInsertionPoint(
			&trait.getBody().front(), trait.getBody().front().begin());

	EXPECT(Token::Colons, onExit());
	TRY(comment, endOfLine(), onExit());
	mlir::rlc::setInlineComment(trait, *comment);
	EXPECT(Token::Indent, onExit());
	while (not accept<Token::Deindent>())
	{
		TRY(fun, functionDeclaration(), onExit());
		TRY(comment, endOfLine(), onExit());
		mlir::rlc::setInlineComment(*fun, *comment);
	}
	onExit();

	return trait;
}

llvm::Expected<mlir::rlc::TypeAliasOp> Parser::usingStatement()
{
	EXPECT(Token::KeywordUsing);
	auto location = getCurrentSourcePos();
	auto startLocation = getCurrentSourcePos().cast<mlir::FileLineColLoc>();
	EXPECT(Token::Identifier);
	std::string name = lIdent;
	auto endLocation = getLastTokenEndPos().cast<mlir::FileLineColLoc>();

	EXPECT(Token::Equal);
	TRY(typeUse, singleTypeUse());
	return builder.create<mlir::rlc::TypeAliasOp>(
			location,
			name,
			(*typeUse).getType(),
			mlir::rlc::SourceRangeAttr::get(startLocation, endLocation),
			*typeUse);
}

llvm::Expected<mlir::rlc::ConstantGlobalOp> Parser::globalConstant()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordConst);
	EXPECT(Token::Identifier);
	std::string name = lIdent;
	EXPECT(Token::Equal);
	int mult = accept(Token::Minus) ? -1 : 1;
	if (accept<Token::Double>())
		return builder.create<mlir::rlc::ConstantGlobalOp>(
				location,
				mlir::rlc::FloatType::get(builder.getContext()),
				builder.getF64FloatAttr(lDouble * mult),
				name);

	if (accept<Token::Int64>())
		return builder.create<mlir::rlc::ConstantGlobalOp>(
				location,
				mlir::rlc::IntegerType::getInt64(builder.getContext()),
				builder.getIntegerAttr(builder.getIntegerType(64), lInt64 * mult),
				name);

	if (accept<Token::Character>())
		return builder.create<mlir::rlc::ConstantGlobalOp>(
				location,
				mlir::rlc::IntegerType::getInt8(builder.getContext()),
				builder.getIntegerAttr(builder.getIntegerType(8), lInt64 * mult),
				name);

	auto location2 = getCurrentSourcePos();
	return make_error<RlcError>(
			"Expected int, float or char",
			RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
			location2);
}

Expected<mlir::rlc::Comment> Parser::comment()
{
	auto loc = getCurrentSourcePos();
	EXPECT(Token::KeywordComment);
	if (attachComments)
		return builder.createComment(loc, lString);
	return mlir::rlc::Comment(nullptr);
}

Expected<mlir::ModuleOp> Parser::system(mlir::ModuleOp destination)
{
	auto location = getCurrentSourcePos();
	std::string name = "unknown";
	if (accept<Token::KeywordSystem>())
	{
		EXPECT(Token::Identifier);
		name = lIdent;
		TRY(comment, endOfLine());
	}

	auto module = destination == nullptr
										? mlir::ModuleOp::create(location, llvm::StringRef(name))
										: destination;
	builder.setInsertionPointToStart(
			&*module.getBodyRegion().getBlocks().begin());

	while (current != Token::End)
	{
		while (acceptEndOfLine() or accept<Token::Indent>() or
					 accept<Token::Deindent>())
			;

		if (current == Token::KeywordComment)
		{
			TRY(c, comment());
			continue;
		}

		if (current == Token::End)
		{
			continue;
		}

		if (current == Token::KeywordExtern)
		{
			TRY(f, externFunctionDeclaration());
			continue;
		}

		bool emitClasses = false;
		if (accept(Token::AnnotationIntroducer))
		{
			EXPECT(Token::KeywordActionClass);
			TRY(comment, endOfLine());
			emitClasses = true;
		}
		if (current == Token::KeywordAction)
		{
			TRY(f, actionDefinition());
			if (emitClasses)
				f->getOperation()->setAttr("emit_classes", builder.getBoolAttr(true));
			continue;
		}

		if (current == Token::KeywordConst)
		{
			TRY(f, globalConstant());
			continue;
		}

		if (current == Token::KeywordFun)
		{
			TRY(f, functionDefinition());
			continue;
		}

		if (current == Token::KeywordEnum)
		{
			TRY(f, enumDeclaration());
			continue;
		}

		if (current == Token::KeywordClass)
		{
			TRY(f, classDeclaration());
			continue;
		}

		if (current == Token::KeywordUsing)
		{
			TRY(f, usingStatement());
			continue;
		}

		if (accept<Token::KeywordImport>())
		{
			auto loc = getCurrentSourcePos();
			EXPECT(Token::Identifier);

			std::string subFileAsWritten = lIdent;
			std::string subFile = lIdent;
			while (accept<Token::Dot>())
			{
				subFile.append("/");
				subFileAsWritten.append(".");
				EXPECT(Token::Identifier);
				subFile.append(lIdent);
				subFileAsWritten.append(lIdent);
			}
			TRY(comment, endOfLine());
			importedFiles.push_back(subFile + ".rl");
			auto op = builder.createImport(loc, subFileAsWritten);
			mlir::rlc::setInlineComment(op, *comment);
			continue;
		}

		if (current == Token::KeywordTrait)
		{
			TRY(f, traitDefinition());
			continue;
		}
		auto location = getCurrentSourcePos();
		return make_error<RlcError>(
				"Expected declaration of function, action, class, constant or alias ",
				RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
				location);
	}
	return module;
}

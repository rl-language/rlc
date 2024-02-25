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

void Parser::next()
{
	pos = mlir::FileLineColLoc::get(
			ctx,
			fileName,
			lexer.getCurrentLine(),
			std::max<int64_t>(1, lexer.getCurrentColumn()));
	if (current == Token::Identifier)
		lIdent = lexer.lastIndent();
	if (current == Token::Int64 or current == Token::Character)
		lInt64 = lexer.lastInt64();
	if (current == Token::Double)
		lDouble = lexer.lastDouble();
	if (current == Token::String)
		lString = lexer.lastString();
	current = lexer.next();
}

bool Parser::accept(Token t)
{
	if (current != t)
		return false;

	next();
	return true;
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

llvm::Expected<mlir::Value> Parser::builtinFromArray()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordFromArray);
	EXPECT(Token::LAng);
	TRY(type, singleTypeUse());
	EXPECT(Token::RAng);
	EXPECT(Token::LPar);
	TRY(size, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::FromByteArrayOp>(location, *type, *size);
}

llvm::Expected<mlir::Value> Parser::stringExpression()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::String);
	return builder.create<mlir::rlc::StringLiteralOp>(location, lString);
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
		while (accept<Token::Newline>() or accept<Token::Indent>() or
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

// builtinMalloc : "__builtin_destroy_do_not_use(" expression ")"
Expected<mlir::Operation*> Parser::builtinDestroy()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordDestroy);
	EXPECT(Token::LPar);
	TRY(arg, expression());
	EXPECT(Token::RPar);
	EXPECT(Token::Newline);

	return builder.create<mlir::rlc::DestroyOp>(location, *arg);
}

// builtinMalloc : "__builtin_malloc_do_not_use<" typeUse ">(" expression ")"
Expected<mlir::Value> Parser::builtinMalloc()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordMalloc);
	EXPECT(Token::LAng);
	TRY(type, singleTypeUse());
	EXPECT(Token::RAng);
	EXPECT(Token::LPar);
	TRY(size, expression());
	EXPECT(Token::RPar);

	return builder.create<mlir::rlc::MallocOp>(
			location,
			mlir::rlc::OwningPtrType::get(type->getContext(), *type),
			*size);
}

// builtinFree : "__builtin_free_do_not_use(" expression ")\n"
Expected<mlir::rlc::FreeOp> Parser::builtinFree()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordFree);
	EXPECT(Token::LPar);
	TRY(toDelete, expression());
	EXPECT(Token::RPar);
	EXPECT(Token::Newline);

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
		EXPECT(Token::Identifier);
		auto enumField = lIdent;
		return builder.create<mlir::rlc::UncheckedEnumUse>(
				location,
				mlir::rlc::UnknownType::get(builder.getContext()),
				enumName,
				enumField);
	}

	if (accept<Token::Double>())
		return builder.create<mlir::rlc::Constant>(location, lDouble);

	if (accept<Token::Int64>())
		return builder.create<mlir::rlc::Constant>(location, lInt64);

	if (accept<Token::Character>())
		return builder.create<mlir::rlc::Constant>(location, int8_t(lInt64));

	if (accept<Token::KeywordFalse>())
		return builder.create<mlir::rlc::Constant>(location, false);

	if (accept<Token::KeywordTrue>())
		return builder.create<mlir::rlc::Constant>(location, true);

	if (current == Token::KeywordMalloc)
		return builtinMalloc();

	if (current == Token::KeywordFromArray)
		return builtinFromArray();

	if (current == Token::String)
		return stringExpression();

	if (current == Token::KeywordToArray)
		return builtinToArray();

	if (accept<Token::LPar>())
	{
		TRY(exp, expression());
		EXPECT(Token::RPar);
		return exp;
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
Expected<mlir::rlc::UsingTypeOp> Parser::usingTypeStatement()
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordUsing);
	EXPECT(Token::Identifier);
	auto typeName = lIdent;
	EXPECT(Token::Equal);
	EXPECT(Token::KeywordType);
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

			TRY(type, singleTypeUse());
			exp = builder.create<mlir::rlc::UncheckedIsOp>(location, exp, *type)
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

	if (accept<Token::ExMark>())
	{
		TRY(exp, unaryExpression());
		return builder.create<mlir::rlc::NotOp>(location, unkType(), *exp);
	}

	return postFixExpression();
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
	if (accept<Token::Plus>())
	{
		TRY(rhs, additiveExpression());
		return builder.create<mlir::rlc::AddOp>(location, unkType(), *exp, *rhs);
	}
	if (accept<Token::Minus>())
	{
		TRY(rhs, additiveExpression());
		return builder.create<mlir::rlc::SubOp>(location, unkType(), *exp, *rhs);
	}
	return std::move(*exp);
}

/**
 * orExpression : additiveExpression (('<' | '>' | '<=' | '>=')
 * additiveExpression)*
 */
Expected<mlir::Value> Parser::relationalExpression()
{
	TRY(exp, additiveExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::LAng>())
	{
		TRY(rhs, relationalExpression());
		return builder.create<mlir::rlc::LessOp>(location, unkType(), *exp, *rhs);
	}
	if (accept<Token::RAng>())
	{
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
	TRY(exp, equalityExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::KeywordAnd>())
	{
		TRY(rhs, andExpression());
		return builder.create<mlir::rlc::AndOp>(location, unkType(), *exp, *rhs);
	}
	return std::move(*exp);
}

/**
 * orExpression : andExpression ('or' andExpression)*
 */
Expected<mlir::Value> Parser::orExpression()
{
	TRY(exp, andExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::KeywordOr>())
	{
		TRY(rhs, orExpression());
		return builder.create<mlir::rlc::OrOp>(location, unkType(), *exp, *rhs);
	}
	return std::move(*exp);
}

Expected<mlir::Value> Parser::expression() { return orExpression(); }

/**
 * EntityField : TypeUse Identifier
 */
llvm::Expected<std::pair<std::string, mlir::Type>> Parser::entityField()
{
	TRY(type, singleTypeUse());
	EXPECT(Token::Identifier);
	return std::pair{ lIdent, *type };
}

/**
 * EntityDeclaration : Ent[templateArguments] Identifier Colons Newline Indent
 * (entityField Newline | functionDefinition)* Deindent
 */
llvm::Expected<mlir::rlc::EntityDeclaration> Parser::entityDeclaration()
{
	auto location = getCurrentSourcePos();
	auto insertionPoint = builder.saveInsertionPoint();
	EXPECT(Token::KeywordEntity);
	SmallVector<mlir::Type, 3> templateParameters;
	if (current == Token::LAng)
	{
		TRY(parameters, templateArguments());
		for (auto type : std::move(*parameters))
			templateParameters.push_back(type);
	}

	EXPECT(Token::Identifier);
	string name = lIdent;
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	EXPECT(Token::Indent);
	SmallVector<mlir::Type, 3> fieldTypes;
	SmallVector<mlir::Attribute, 3> fieldNames;

	mlir::Region region;
	auto* bb = builder.createBlock(&region);
	builder.setInsertionPointToStart(bb);

	const auto on_exit = [&]() -> mlir::rlc::EntityDeclaration {
		builder.restoreInsertionPoint(insertionPoint);
		auto toReturn = builder.create<mlir::rlc::EntityDeclaration>(
				location,
				unkType(),
				builder.getStringAttr(name),
				builder.getTypeArrayAttr(fieldTypes),
				builder.getArrayAttr(fieldNames),
				builder.getTypeArrayAttr(templateParameters));

		toReturn.getBody().takeBody(region);

		return toReturn;
	};

	while (!accept<Token::Deindent>())
	{
		if (current == Token::KeywordFun)
		{
			TRY(_, functionDefinition(true), on_exit());
		}
		else
		{
			TRY(field, entityField(), on_exit());
			fieldTypes.emplace_back(field->second);
			fieldNames.emplace_back(builder.getStringAttr(field->first));
			EXPECT(Token::Newline, on_exit());
		}
		while (accept<Token::Newline>())
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

	TRY(exp, expression(), onExit());

	auto locAssign = getCurrentSourcePos();
	if (accept<Token::Equal>())
	{
		TRY(rightHand, expression(), onExit());
		builder.create<mlir::rlc::AssignOp>(locAssign, *exp, *rightHand);
	}

	onExit();
	EXPECT(Token::Newline);

	return expStatement;
}

/**
 * prerequsitites: "req"" expression [Indent (expression* \n) Deindent ] \n
 */
llvm::Expected<bool> Parser::requirementList()
{
	llvm::SmallVector<mlir::Value, 4> values;

	auto onExit = [&, this]() {
		builder.create<mlir::rlc::Yield>(getCurrentSourcePos(), values);
	};

	if (accept<Token::LBracket>())
	{
		do
		{
			TRY(rest, expression(), onExit());
			values.push_back(*rest);
		} while (accept<Token::Comma>());

		EXPECT(Token::RBracket, onExit());
	}

	onExit();
	return true;
}

/**
 * subActionStatement: subaction (*)? ("(" argumentExpressionList ")")?
 * `ident` = expression \n \n
 */
llvm::Expected<mlir::rlc::SubActionStatement> Parser::subActionStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordSubAction);

	bool runOnce = not accept<Token::Mult>();

	llvm::SmallVector<mlir::Value, 3> expressions;
	if (accept<Token::LPar>())
	{
		TRY(list, argumentExpressionList());
		expressions = *list;
		EXPECT(Token::RPar);
	}

	std::string name;
	EXPECT(Token::Identifier);
	name = lIdent;
	EXPECT(Token::Equal);

	auto operation = builder.create<mlir::rlc::SubActionStatement>(
			location, name, runOnce, expressions);
	builder.createBlock(&operation.getBody());
	auto onExit = [&, this](mlir::Value exp) {
		if (exp)
			builder.create<mlir::rlc::Yield>(
					getCurrentSourcePos(), mlir::ValueRange(exp));
		builder.setInsertionPointAfter(operation);
		if (not exp)
			operation.erase();
	};

	TRY(exp, expression(), onExit(nullptr));
	EXPECT(Token::Newline);
	onExit(*exp);

	return operation;
}

llvm::Expected<mlir::rlc::ActionsStatement> Parser::actionsStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordActions);
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	EXPECT(Token::Indent);

	llvm::SmallVector<llvm::SmallVector<mlir::Operation*, 2>, 4> ops;

	ops.push_back({});
	do
	{
		TRY(op, statement());
		ops.back().push_back(*op);
		while (accept<Token::Newline>())
			;
	} while (current != Token::KeywordAction and current != Token::Deindent);

	while (not accept<Token::Deindent>())
	{
		while (accept<Token::Newline>())
			;
		if (current == Token::KeywordAction)
			ops.push_back({});

		TRY(op, statement());
		ops.back().push_back(*op);
	}

	auto statements =
			builder.create<mlir::rlc::ActionsStatement>(location, ops.size());

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
			builder.create<mlir::rlc::Yield>(location);
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
	TRY(action, actionDeclaration(false));
	auto pos = builder.saveInsertionPoint();
	auto onExit = [&, this]() {
		builder.restoreInsertionPoint(pos);
		if (action)
			action->erase();
	};

	llvm::SmallVector<std::string, 3> argNames;
	for (auto name : action->getArgNames())
		argNames.push_back(name.cast<mlir::StringAttr>().str());

	auto op = builder.create<mlir::rlc::ActionStatement>(
			action->getLoc(),
			action->getArgumentTypes(),
			action->getUnmangledName(),
			argNames);

	llvm::SmallVector<mlir::Location> locs;
	for (size_t i = 0; i < action->getArgumentTypes().size(); i++)
		locs.push_back(op->getLoc());

	builder.createBlock(
			&op.getPrecondition(), {}, action->getArgumentTypes(), locs);
	TRY(list, requirementList(), onExit());

	EXPECT(Token::Newline, onExit());
	onExit();

	return op;
}

/**
 * ifStatement : if expression ':\nindent statementList [else ':\n'
 * statementList ]
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
		builder.create<mlir::rlc::Yield>(location, mlir::ValueRange(condExp));

		builder.setInsertionPointToEnd(trueB);
		emitYieldIfNeeded(getCurrentSourcePos());

		builder.setInsertionPointToEnd(elseB);
		emitYieldIfNeeded(getCurrentSourcePos());

		builder.restoreInsertionPoint(pos);
	};

	builder.setInsertionPointToStart(condB);

	TRY(exp, expression(), onExit(nullptr));
	EXPECT(Token::Colons, onExit(*exp));
	EXPECT(Token::Newline, onExit(*exp));

	builder.setInsertionPointToStart(trueB);
	TRY(tBranch, statementList(), onExit(*exp));

	builder.setInsertionPointToStart(elseB);
	if (accept<Token::KeywordElse>())
	{
		EXPECT(Token::Colons, onExit(*exp));
		EXPECT(Token::Newline, onExit(*exp));
		TRY(fBranch, statementList(), onExit(*exp));
	}

	onExit(*exp);
	return expStatement;
}

/**
 * forFieldStatment: `for` ident (`,` ident)* `of` expression (`,` expression)*
 * ':\n' statementList
 */
Expected<mlir::rlc::ForFieldStatement> Parser::forFieldStatement()
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
		emitYieldIfNeeded(location);
		builder.restoreInsertionPoint(pos);
	};

	do
	{
		TRY(exp, expression(), onExit());
		values.push_back(*exp);
	} while (accept<Token::Comma>());

	EXPECT(Token::Colons, onExit());
	EXPECT(Token::Newline, onExit());

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
		builder.create<mlir::rlc::Yield>(location, mlir::ValueRange({ exp }));
		builder.setInsertionPointToEnd(bodyB);
		emitYieldIfNeeded(getCurrentSourcePos());
		builder.restoreInsertionPoint(pos);
	};

	TRY(exp, expression(), onExit(nullptr));
	EXPECT(Token::Colons, onExit(*exp));
	EXPECT(Token::Newline, onExit(*exp));

	builder.setInsertionPointToStart(bodyB);
	TRY(statLis, statementList(), onExit(*exp));

	onExit(*exp);

	return expStatement;
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

	if (accept(Token::Newline))
	{
		onExit(nullptr);
		return expStatement;
	}

	TRY(exp, expression(), onExit(nullptr));
	EXPECT(Token::Newline, onExit(*exp));
	onExit(*exp);
	return expStatement;
}

Expected<mlir::Operation*> Parser::statement()
{
	if (current == Token::KeywordAction)
		return actionStatement();

	if (current == Token::KeywordIf)
		return ifStatement();

	if (current == Token::KeywordReturn)
		return returnStatement();

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

	if (current == Token::KeywordFree)
		return builtinFree();

	if (current == Token::KeywordDestroy)
		return builtinDestroy();

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
		onExit(*exp);
		EXPECT(Token::Newline);

		return expStatement;
	}

	EXPECT(Token::Colons, onExit(nullptr));
	auto typeLoc = getCurrentSourcePos();
	TRY(use, singleTypeUse(), onExit(nullptr));
	auto exp = builder.create<mlir::rlc::ConstructOp>(typeLoc, *use);
	onExit(exp);
	EXPECT(Token::Newline);
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
		while (accept<Token::Newline>())
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
	tpUse.emplace_back(std::move(*singleTp));
	while (accept<Token::Arrow>())
	{
		TRY(singleTp, singleTypeUse());
		tpUse.emplace_back(std::move(*singleTp));
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
				ctx, mlir::rlc::OwningPtrType::get(subType->getContext(), *subType));
	}

	EXPECT(Token::Identifier);
	auto nm = lIdent;
	mlir::Type arraySize = mlir::rlc::IntegerLiteralType::get(ctx, 0);
	llvm::SmallVector<mlir::Type, 2> templateParametersTypes;
	if (accept<Token::LAng>())
	{
		do
		{
			TRY(templateParameter, singleTypeUse());
			templateParametersTypes.push_back(*templateParameter);
		} while (accept<Token::Comma>());
		EXPECT(Token::RAng);
	}

	return mlir::rlc::ScalarUseType::get(ctx, nm, 0, templateParametersTypes);
}

/**
 * singleTypeUse : singleNonArrayTypeUse (["["int64"|Ident "]"] )* (|
 * singleTypeUse)*
 */
Expected<mlir::Type> Parser::singleTypeUse()
{
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

	if (seenTypes.size() == 1)
		return seenTypes.front();

	return mlir::rlc::AlternativeType::get(ctx, seenTypes);
}

Expected<std::tuple<std::string, mlir::Type>> Parser::argDeclaration()
{
	auto isFrame = accept<Token::KeywordFrame>();
	bool isCtx = not isFrame && accept<Token::KeywordCtx>();

	TRY(tp, singleTypeUse());
	if (isFrame)
		*tp = mlir::rlc::FrameType::get(*tp);
	if (isCtx)
		*tp = mlir::rlc::ContextType::get(*tp);
	EXPECT(Token::Identifier);
	auto parName = lIdent;
	return std::tuple{ parName, *tp };
}

/**
 * functionDefinition : "(" [argDeclaration ("," argDeclaration)*] ")"
 */
Expected<llvm::SmallVector<std::tuple<std::string, mlir::Type>, 3>>
Parser::functionArguments()
{
	EXPECT(Token::LPar);

	llvm::SmallVector<std::tuple<std::string, mlir::Type>, 3> args;

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
			toReturn.push_back(mlir::rlc::UncheckedTemplateParameterType::get(
					builder.getContext(), second, builder.getStringAttr(first)));
		}
		else
		{
			toReturn.push_back(mlir::rlc::UncheckedTemplateParameterType::get(
					builder.getContext(), first, builder.getStringAttr("")));
		}

	} while (accept<Token::Comma>());

	EXPECT(Token::RAng);
	return toReturn;
}

Expected<Parser::FunctionDeclarationResult> Parser::externFunctionDeclaration()
{
	EXPECT(Token::KeywordExtern);
	TRY(result, functionDeclaration());
	EXPECT(Token::Newline);
	return std::move(*result);
}

/**
 * functionDeclaration : "fun" templateArguments identifier "("
 * [argDeclaration
 * ("," argDeclaration)*] ")" ["->" "ref"? singleTypeUse]
 */
Expected<Parser::FunctionDeclarationResult> Parser::functionDeclaration(
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

	EXPECT(Token::Identifier);
	auto nm = lIdent;
	TRY(args, functionArguments());
	llvm::SmallVector<mlir::Type> argTypes;
	llvm::SmallVector<llvm::StringRef> argName;
	llvm::SmallVector<mlir::Location> argLocs;

	for (auto& arg : *args)
	{
		argTypes.push_back(std::get<mlir::Type>(arg));
		argName.push_back(std::get<std::string>(arg));
		argLocs.push_back(location);
	}

	mlir::Type retType = mlir::rlc::VoidType::get(ctx);
	if (accept<Token::Arrow>())
	{
		bool isRef = accept<Token::KeywordRef>();
		TRY(actualRetType, singleTypeUse());
		retType = *actualRetType;
		if (isRef)
			retType =
					mlir::rlc::ReferenceType::get(actualRetType->getContext(), retType);
	}

	auto fun = builder.create<mlir::rlc::FunctionOp>(
			location,
			builder.getStringAttr(nm),
			mlir::FunctionType::get(ctx, argTypes, { retType }),
			builder.getStrArrayAttr(argName),
			isMemberFunction,
			templateParameters);

	return FunctionDeclarationResult{ fun, argLocs };
}

/**
 * functionDefinition : functionDeclaration ":\n" statementList
 */
Expected<mlir::rlc::FunctionOp> Parser::functionDefinition(
		bool isMemberFunction)
{
	TRY(result, functionDeclaration(true, isMemberFunction));
	auto fun = result->op;
	auto location = getCurrentSourcePos();
	auto pos = builder.saveInsertionPoint();

	auto* bodyB = builder.createBlock(
			&fun.getBody(), {}, fun.getArgumentTypes(), result->argLocs);
	auto* condB = builder.createBlock(
			&fun.getPrecondition(), {}, fun.getArgumentTypes(), result->argLocs);

	auto onExit = [&, this]() {
		builder.setInsertionPointToEnd(bodyB);
		emitYieldIfNeeded(getCurrentSourcePos());
		builder.restoreInsertionPoint(pos);
	};

	TRY(list, requirementList(), onExit());
	EXPECT(Token::Colons, onExit());
	EXPECT(Token::Newline, onExit());

	builder.setInsertionPointToEnd(bodyB);
	TRY(body, statementList(), onExit());
	onExit();

	return fun;
}

/**
 * actionDeclaration : "act" identifier "(" [argDeclaration (","
 * argDeclaration)*] ")" -> Type
 */
Expected<mlir::rlc::ActionFunction> Parser::actionDeclaration(
		bool needsReturnType)
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordAction);
	EXPECT(Token::Identifier);
	auto nm = lIdent;

	TRY(args, functionArguments());
	llvm::SmallVector<mlir::Type> argTypes;
	llvm::SmallVector<llvm::StringRef> argName;

	for (auto& arg : *args)
	{
		argName.push_back(std::get<std::string>(arg));
		argTypes.push_back(std::get<mlir::Type>(arg));
	}

	std::string name;
	if (needsReturnType)
	{
		EXPECT(Token::Arrow);
		EXPECT(Token::Identifier);
		name = lIdent;
	}
	auto retType = mlir::rlc::EntityType::getIdentified(ctx, name, {});
	return builder.create<mlir::rlc::ActionFunction>(
			location,
			mlir::FunctionType::get(ctx, argTypes, { retType }),
			mlir::rlc::UnknownType::get(builder.getContext()),
			mlir::TypeRange(),
			builder.getStringAttr(nm),
			builder.getStrArrayAttr(argName));
}

/**
 * actionDefinition : actionDeclaration ":\n" statementList
 */
Expected<mlir::rlc::ActionFunction> Parser::actionDefinition()
{
	TRY(decl, actionDeclaration(true));

	auto pos = builder.saveInsertionPoint();
	llvm::SmallVector<mlir::Location> locs;
	for (size_t i = 0; i < decl->getFunctionType().getInputs().size(); i++)
		locs.push_back(decl->getLoc());

	auto* block = builder.createBlock(
			&decl->getBody(), {}, decl->getFunctionType().getInputs(), locs);
	auto* condB = builder.createBlock(
			&decl->getPrecondition(), {}, decl->getFunctionType().getInputs(), locs);

	auto onExit = [&, this]() {
		builder.setInsertionPointToEnd(block);
		emitYieldIfNeeded(getCurrentSourcePos());
		builder.restoreInsertionPoint(pos);
	};

	TRY(list, requirementList(), onExit());
	EXPECT(Token::Colons, onExit());
	EXPECT(Token::Newline, onExit());

	builder.setInsertionPoint(block, block->begin());
	TRY(body, statementList(), onExit());
	onExit();
	return decl;
}

/*
 * enumDeclaration: "enum" ident:\nindent ident(\n indent)*\ndeindent
 */
Expected<mlir::rlc::EnumDeclarationOp> Parser::enumDeclaration()
{
	auto location = getCurrentSourcePos();
	auto pos = builder.saveInsertionPoint();
	EXPECT(Token::KeywordEnum);
	EXPECT(Token::Identifier);
	auto enumName = lIdent;
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	EXPECT(Token::Indent);

	llvm::SmallVector<std::unique_ptr<std::string>, 2> enumFields;
	llvm::SmallVector<llvm::StringRef, 2> enumFieldsRefs;

	mlir::Region region;
	auto* bb = builder.createBlock(&region);
	builder.setInsertionPointToStart(bb);

	const auto onExit = [&]() {
		builder.restoreInsertionPoint(pos);
		auto toReturn = builder.create<mlir::rlc::EnumDeclarationOp>(
				location, enumName, builder.getStrArrayAttr(enumFieldsRefs));

		toReturn.getBody().takeBody(region);
		return toReturn;
	};

	while (not accept<Token::Deindent>())
	{
		if (current == Token::KeywordFun)
		{
			TRY(_, functionDefinition(true), onExit());
		}
		else
		{
			EXPECT(Token::Identifier, onExit());
			auto enumField = lIdent;
			EXPECT(Token::Newline, onExit());
			enumFields.emplace_back(std::make_unique<std::string>(enumField));
			enumFieldsRefs.push_back(*enumFields.back());
		}
		while (accept<Token::Newline>())
			;
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
	EXPECT(Token::Identifier, onExit());
	auto templateParameter = lIdent;
	EXPECT(Token::RAng, onExit());
	EXPECT(Token::Identifier, onExit());
	auto traitName = lIdent;
	auto trait = builder.create<mlir::rlc::UncheckedTraitDefinition>(
			location, traitName, templateParameter);
	builder.createBlock(&trait.getBody());
	builder.setInsertionPoint(
			&trait.getBody().front(), trait.getBody().front().begin());

	EXPECT(Token::Colons, onExit());
	EXPECT(Token::Newline, onExit());
	EXPECT(Token::Indent, onExit());
	while (not accept<Token::Deindent>())
	{
		TRY(fun, functionDeclaration(), onExit());
		EXPECT(Token::Newline, onExit());
	}
	onExit();

	return trait;
}

Expected<mlir::ModuleOp> Parser::system(mlir::ModuleOp destination)
{
	auto location = getCurrentSourcePos();
	std::string name = "unknown";
	if (accept<Token::KeywordSystem>())
	{
		EXPECT(Token::Identifier);
		name = lIdent;
		EXPECT(Token::Newline);
	}

	auto module = destination == nullptr
										? mlir::ModuleOp::create(location, llvm::StringRef(name))
										: destination;
	builder.setInsertionPointToStart(
			&*module.getBodyRegion().getBlocks().begin());

	while (current != Token::End)
	{
		if (accept<Token::Newline>() or accept<Token::Indent>() or
				accept<Token::Deindent>())
			continue;

		if (current == Token::KeywordExtern)
		{
			TRY(f, externFunctionDeclaration());
			continue;
		}

		if (current == Token::KeywordAction)
		{
			TRY(f, actionDefinition());
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

		if (current == Token::KeywordEntity)
		{
			TRY(f, entityDeclaration());
			continue;
		}

		if (accept<Token::KeywordImport>())
		{
			EXPECT(Token::Identifier);

			std::string subFile = lIdent;
			while (accept<Token::Dot>())
			{
				subFile.append("/");
				EXPECT(Token::Identifier);
				subFile.append(lIdent);
			}
			EXPECT(Token::Newline);
			importedFiles.push_back(subFile + ".rl");
			continue;
		}

		if (current == Token::KeywordTrait)
		{
			TRY(f, traitDefinition());
			continue;
		}
		auto location = getCurrentSourcePos();
		return make_error<RlcError>(
				"Expected function, action or entity declaration",
				RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
				location);
	}
	return module;
}

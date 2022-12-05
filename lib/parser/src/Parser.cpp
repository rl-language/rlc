#include "rlc/parser/Parser.hpp"

#include <system_error>

#include "llvm/Support/Error.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/Expression.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/parser/Lexer.hpp"
#include "rlc/utils/Error.hpp"
#include "rlc/utils/SourcePosition.hpp"

using namespace llvm;
using namespace rlc;
using namespace std;

void Parser::next()
{
	pos.setColumn(std::max<int64_t>(1, lexer.getCurrentColumn() - 1));
	pos.setLine(lexer.getCurrentLine());
	if (current == Token::Identifier)
		lIdent = lexer.lastIndent();
	if (current == Token::Int64)
		lInt64 = lexer.lastInt64();
	if (current == Token::Double)
		lDouble = lexer.lastDouble();
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

	std::string errorMessage = "\nunexpected token ";
	errorMessage += tokenToString(current);
	errorMessage += " expected ";
	errorMessage += tokenToString(t);
	return make_error<RlcError>(
			std::move(errorMessage),
			RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
			pos);
}

#define EXPECT(Token)                                                          \
	if (auto e = expect(Token); !e)                                              \
	return e.takeError()

#define TRY(outVar, expression)                                                \
	auto outVar = expression;                                                    \
	if (!outVar)                                                                 \
	return outVar.takeError()

/**
 * primaryExpression : Ident | Double | int64 | "true" | "false" | "("
 * expression ")"
 */
Expected<Expression> Parser::primaryExpression()
{
	auto location = getCurrentSourcePos();
	if (accept<Token::Identifier>())
		return Expression::reference(lIdent, location);

	if (accept<Token::Double>())
		return Expression::scalarConstant(lDouble, location);

	if (accept<Token::Int64>())
		return Expression::scalarConstant(lInt64, location);

	if (accept<Token::KeywordFalse>())
		return Expression::scalarConstant(false, location);

	if (accept<Token::KeywordTrue>())
		return Expression::scalarConstant(true, location);

	if (accept<Token::LPar>())
	{
		TRY(exp, expression());
		EXPECT(Token::RPar);
		return exp;
	}

	return make_error<RlcError>(
			" empty expression",
			RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
			pos);
}

/**
 * argumentExpressionList : (expression",")*expression
 */
Expected<SmallVector<Expression, 3>> Parser::argumentExpressionList()
{
	SmallVector<Expression, 3> exps;
	if (current == Token::RPar)
		return exps;

	do
	{
		TRY(exp, expression());
		exps.emplace_back(std::move(*exp));
	} while (accept<Token::Comma>());

	return exps;
}

/**
 * postFixExpression :
 *			((postFixExpression)*
 *					("["expression"]"
 *					|"("argumentExpressionList ")" |
 *					. identifier ["(" argumentExpressionList")"]))
 *			| primaryExpression
 */
Expected<Expression> Parser::postFixExpression()
{
	TRY(maybeExp, primaryExpression());
	auto exp = std::move(*maybeExp);

	while (true)
	{
		auto location = getCurrentSourcePos();
		if (accept<Token::LPar>())
		{
			TRY(args, argumentExpressionList());
			EXPECT(Token::RPar);
			exp = Expression::call(std::move(exp), std::move(*args), location);
			continue;
		}

		if (accept<Token::LSquare>())
		{
			TRY(accExp, expression());
			EXPECT(Token::RSquare);
			auto expression = (exp)[std::move(*accExp)];
			expression.setPosition(location);
			exp = expression;
			continue;
		}

		if (accept<Token::Dot>())
		{
			EXPECT(Token::Identifier);
			auto memberName = lIdent;
			if (not accept<Token::LPar>())
			{
				exp = Expression::memberAccess(std::move(exp), memberName, location);
				continue;
			}

			TRY(arguments, argumentExpressionList());
			EXPECT(Token::RPar);
			arguments->insert(arguments->begin(), std::move(exp));
			exp = Expression::call(
					Expression::reference(memberName), std::move(*arguments), location);
		}
		break;
	}
	return exp;
}

/**
 * unaryExpression : postFixExpression | unaryOperator unaryExpression
 */
Expected<Expression> Parser::unaryExpression()
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
		return Expression::call(
				Expression::reference(BuiltinFunctions::Minus),
				{ std::move(*exp) },
				location);
	}

	if (accept<Token::ExMark>())
	{
		TRY(exp, unaryExpression());
		return Expression::call(
				Expression::reference(BuiltinFunctions::Not),
				{ std::move(*exp) },
				location);
	}

	return postFixExpression();
}

/**
 * multicativeExpression : unaryExpression | multiplicativeExpresion ('*' | '%'
 * | '/') unaryExpression
 */
Expected<Expression> Parser::multyplicativeExpression()
{
	TRY(exp, unaryExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::Mult>())
	{
		TRY(rhs, multyplicativeExpression());
		auto expresion = (*exp) * (std::move(*rhs));
		expresion.setPosition(location);
		return expresion;
	}
	if (accept<Token::Divide>())
	{
		TRY(rhs, multyplicativeExpression());
		auto expression = (*exp) / (std::move(*rhs));
		expression.setPosition(location);
		return expression;
	}
	if (accept<Token::Module>())
	{
		TRY(rhs, multyplicativeExpression());
		auto expression = Expression::call(
				Expression::reference(BuiltinFunctions::Reminder),
				{ *std::move(exp), std::move(*rhs) },
				location);
		expression.setPosition(location);
		return expression;
	}

	return std::move(*exp);
}

/**
 * additiveExpression : multiplicativeExpression | additiveExpression ('+' |
 * '-')multiplicativeExpression
 */
Expected<Expression> Parser::additiveExpression()
{
	TRY(exp, multyplicativeExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::Plus>())
	{
		TRY(rhs, additiveExpression());
		auto expression = (*exp) + (std::move(*rhs));
		expression.setPosition(location);
		return expression;
	}
	if (accept<Token::Minus>())
	{
		TRY(rhs, additiveExpression());
		auto expression = (*exp) - (std::move(*rhs));
		expression.setPosition(location);
		return expression;
	}
	return std::move(*exp);
}

/**
 * orExpression : additiveExpression (('<' | '>' | '<=' | '>=')
 * additiveExpression)*
 */
Expected<Expression> Parser::relationalExpression()
{
	TRY(exp, additiveExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::LAng>())
	{
		TRY(rhs, relationalExpression());
		auto expression = (*exp) < (std::move(*rhs));
		expression.setPosition(location);
		return expression;
	}
	if (accept<Token::RAng>())
	{
		TRY(rhs, relationalExpression());
		auto expression = (*exp) > (std::move(*rhs));
		expression.setPosition(location);
		return expression;
	}
	if (accept<Token::GEqual>())
	{
		TRY(rhs, relationalExpression());
		auto expression = (*exp) >= (std::move(*rhs));
		expression.setPosition(location);
		return expression;
	}
	if (accept<Token::LEqual>())
	{
		TRY(rhs, relationalExpression());
		auto expression = (*exp) <= (std::move(*rhs));
		expression.setPosition(location);
		return expression;
	}
	return std::move(*exp);
}

/**
 * orExpression : relationalExpression (('==' | '!=') relationalExpression)*
 */
Expected<Expression> Parser::equalityExpression()
{
	TRY(exp, relationalExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::EqualEqual>())
	{
		TRY(rhs, equalityExpression());
		return Expression::call(
				Expression::reference(BuiltinFunctions::Equal),
				{ std::move(*exp), std::move(*rhs) },
				location);
	}
	if (accept<Token::NEqual>())
	{
		TRY(rhs, equalityExpression());
		return Expression::call(
				Expression::reference(BuiltinFunctions::NotEqual),
				{ std::move(*exp), std::move(*rhs) },
				location);
	}
	return std::move(*exp);
}

/**
 * orExpression : equalityExpression ('and' equalityExpression)*
 */
Expected<Expression> Parser::andExpression()
{
	TRY(exp, equalityExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::KeywordAnd>())
	{
		TRY(rhs, andExpression());
		return Expression::call(
				Expression::reference(BuiltinFunctions::And),
				{ std::move(*exp), std::move(*rhs) },
				location);
	}
	return std::move(*exp);
}

/**
 * orExpression : andExpression ('or' andExpression)*
 */
Expected<Expression> Parser::orExpression()
{
	TRY(exp, andExpression());

	auto location = getCurrentSourcePos();
	if (accept<Token::KeywordOr>())
	{
		TRY(rhs, orExpression());
		return Expression::call(
				Expression::reference(BuiltinFunctions::Or),
				{ std::move(*exp), std::move(*rhs) },
				location);
	}
	return std::move(*exp);
}

Expected<Expression> Parser::expression() { return assignmentExpression(); }

/**
 * assigmentExpression : orExpression | orExpression "=" assigmentExpression
 */
Expected<Expression> Parser::assignmentExpression()
{
	TRY(leftHand, orExpression());

	auto location = getCurrentSourcePos();
	if (!accept<Token::Equal>())
		return std::move(*leftHand);

	TRY(rightHand, assignmentExpression());
	return Expression::assign(
			std::move(*leftHand), std::move(*rightHand), location);
}

/**
 * EntityField : TypeUse Identifier
 */
llvm::Expected<EntityField> Parser::entityField()
{
	auto location = getCurrentSourcePos();
	TRY(type, singleTypeUse());
	EXPECT(Token::Identifier);
	return EntityField(std::move(*type), std::move(lIdent), location);
}

/**
 * EntityDeclaration : Ent Identifier Colons Newline Indent (entityField
 * Newline)* Deindent
 */
llvm::Expected<EntityDeclaration> Parser::entityDeclaration()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordEntity);
	EXPECT(Token::Identifier);
	string name = lIdent;
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	EXPECT(Token::Indent);
	SmallVector<EntityField, 3> fields;

	while (!accept<Token::Deindent>())
	{
		TRY(field, entityField());
		fields.emplace_back(std::move(*field));
		EXPECT(Token::Newline);
	}
	auto e = Entity(std::move(name), std::move(fields));
	return EntityDeclaration(std::move(e), std::move(location));
}

/**
 * expressionStatement : expression '\n'
 */
llvm::Expected<Statement> Parser::expressionStatement()
{
	auto location = getCurrentSourcePos();
	TRY(exp, expression());
	EXPECT(Token::Newline);

	return Statement::expStatement(std::move(*exp), std::move(location));
}

/**
 * actionStatement : actionDeclaration '\n'
 */
llvm::Expected<Statement> Parser::actionStatement()
{
	TRY(action, actionDeclaration());
	EXPECT(Token::Newline);

	return Statement::actionStatement(*action);
}

/**
 * ifStatement : if expression ':\nindent statementList [else ':\n'
 * statementList ]
 */
llvm::Expected<Statement> Parser::ifStatement()
{
	SourcePosition location = getCurrentSourcePos();
	EXPECT(Token::KeywordIf);
	TRY(exp, expression());
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	TRY(tBranch, statementList());
	if (!accept<Token::KeywordElse>())
		return Statement::ifStatment(
				std::move(*exp), std::move(*tBranch), std::move(location));

	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	TRY(fBranch, statementList());
	return Statement::ifStatment(
			std::move(*exp),
			std::move(*tBranch),
			std::move(*fBranch),
			std::move(location));
}

/**
 * whileStatement : While exp ':\n' statementList
 */
Expected<Statement> Parser::whileStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordWhile);
	TRY(exp, expression());
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	TRY(statLis, statementList());
	return Statement::whileStatement(
			std::move(*exp), std::move(*statLis), std::move(location));
}

/**
 * returnStatement : return [expression] '\n'
 */
Expected<Statement> Parser::returnStatement()
{
	auto location = getCurrentSourcePos();
	EXPECT(Token::KeywordReturn);
	if (accept(Token::Newline))
		return Statement::returnStatement(std::move(location));

	TRY(exp, expression());
	EXPECT(Token::Newline);
	return Statement::returnStatement(std::move(*exp), std::move(location));
}

Expected<Statement> Parser::statement()
{
	if (current == Token::KeywordAction)
		return actionStatement();

	if (current == Token::KeywordIf)
		return ifStatement();

	if (current == Token::KeywordReturn)
		return returnStatement();

	if (current == Token::KeywordWhile)
		return whileStatement();

	if (current == Token::KeywordLet)
		return declarationStatement();

	return expressionStatement();
}

/**
 * declarationStatement : 'let' identifier ['=' expression | ':' type_use ]
 */
Expected<Statement> Parser::declarationStatement()
{
	EXPECT(Token::KeywordLet);
	EXPECT(Token::Identifier);
	auto location = getCurrentSourcePos();
	auto name = lIdent;

	if (accept<Token::Equal>())
	{
		TRY(exp, expression());
		EXPECT(Token::Newline);
		return Statement::declarationStatement(
				std::move(name), std::move(*exp), std::move(location));
	}

	EXPECT(Token::Colons);
	auto typePosition = getCurrentSourcePos();
	TRY(use, singleTypeUse());
	EXPECT(Token::Newline);
	return Statement::declarationStatement(
			std::move(name),
			Expression::zeroInitializer(std::move(*use), typePosition),
			location);
}

/**
 * statmentList : indent (statement)* deindent
 */
Expected<Statement> Parser::statementList()
{
	auto location = getCurrentSourcePos();

	SmallVector<Statement, 3> stmts;
	EXPECT(Token::Indent);
	while (!accept<Token::Deindent>())
	{
		TRY(s, statement());
		stmts.emplace_back(std::move(*s));
	}

	return Statement::statmentList(std::move(stmts), std::move(location));
}

/**
 * functionTypeUse : [singleTypeUse ("->" singleTypeUse )*]
 */
Expected<SingleTypeUse> Parser::functionTypeUse()
{
	auto location = getCurrentSourcePos();
	SmallVector<SingleTypeUse, 2> tpUse;

	TRY(singleTp, singleTypeUse());
	tpUse.emplace_back(std::move(*singleTp));
	while (accept<Token::Arrow>())
	{
		TRY(singleTp, singleTypeUse());
		tpUse.emplace_back(std::move(*singleTp));
	}

	return FunctionTypeUse::functionType(std::move(tpUse), pos);
}

/**
 * singleTypeUse : "(" functionType ")" | identifier["["int64"]"]
 */
Expected<SingleTypeUse> Parser::singleTypeUse()
{
	auto location = getCurrentSourcePos();
	if (accept<Token::LPar>())
	{
		TRY(fType, functionTypeUse());
		EXPECT(Token::RPar);
		return std::move(*fType);
	}

	EXPECT(Token::Identifier);
	auto nm = lIdent;
	if (!accept<Token::LSquare>())
		return SingleTypeUse::scalarType(std::move(nm), location);

	EXPECT(Token::Int64);
	auto size = lInt64;
	EXPECT(Token::RSquare);
	return SingleTypeUse::arrayType(std::move(nm), size, location);
}

Expected<ArgumentDeclaration> Parser::argDeclaration()
{
	auto location = getCurrentSourcePos();
	TRY(tp, singleTypeUse());
	EXPECT(Token::Identifier);
	auto parName = lIdent;
	return ArgumentDeclaration(
			std::move(parName), std::move(*tp), std::move(location));
}

/**
 * functionDefinition : "(" [argDeclaration ("," argDeclaration)*] ")"
 */
Expected<llvm::SmallVector<ArgumentDeclaration, 3>> Parser::functionArguments()
{
	EXPECT(Token::LPar);

	llvm::SmallVector<ArgumentDeclaration, 3> args;

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
 * functionDefinition : "fun" identifier "(" [argDeclaration (","
 * argDeclaration)*] ")" ["->" singleTypeUse] ":\n" statementList
 */
Expected<FunctionDefinition> Parser::functionDefinition()
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordFun);
	EXPECT(Token::Identifier);
	auto nm = lIdent;

	TRY(args, functionArguments());

	SingleTypeUse retType = SingleTypeUse::scalarType(
			builtinTypeToString(BuiltinType::VOID).str(), location);
	if (accept<Token::Arrow>())
	{
		TRY(actualRetType, singleTypeUse());
		retType = std::move(*actualRetType);
	}

	EXPECT(Token::Colons);
	EXPECT(Token::Newline);

	TRY(body, statementList());
	return FunctionDefinition(
			std::move(nm),
			std::move(*body),
			std::move(retType),
			std::move(*args),
			location);
}

/**
 * actionDeclaration : "act" identifier "(" [argDeclaration (","
 * argDeclaration)*] ")"
 */
Expected<ActionDeclaration> Parser::actionDeclaration()
{
	auto location = getCurrentSourcePos();

	EXPECT(Token::KeywordAction);
	EXPECT(Token::Identifier);
	auto nm = lIdent;

	TRY(args, functionArguments());

	SingleTypeUse retType = SingleTypeUse::scalarType(
			builtinTypeToString(BuiltinType::VOID).str(), location);
	return ActionDeclaration(std::move(nm), std::move(*args), location);
}

/**
 * actionDefinition : actionDeclaration ":\n" statementList
 */
Expected<ActionDefinition> Parser::actionDefinition()
{
	TRY(decl, actionDeclaration());
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);

	TRY(body, statementList());
	return ActionDefinition(std::move(*decl), std::move(*body));
}

Expected<System> Parser::system()
{
	auto location = getCurrentSourcePos();
	System s("anonymous system", location);
	if (accept<Token::KeywordSystem>())
	{
		EXPECT(Token::Identifier);
		s.setName(lIdent);
		EXPECT(Token::Newline);
	}

	while (current != Token::End)
	{
		while (accept<Token::Newline>() or accept<Token::Indent>() or
					 accept<Token::Deindent>())
			;

		if (current == Token::KeywordAction)
		{
			TRY(f, actionDefinition());
			s.addAction(std::move(*f));
			continue;
		}

		if (current == Token::KeywordFun)
		{
			TRY(f, functionDefinition());
			s.addFunction(std::move(*f));
			continue;
		}

		if (current == Token::KeywordEntity)
		{
			TRY(f, entityDeclaration());
			s.addEntity(std::move(*f));
			continue;
		}
		auto location = getCurrentSourcePos();
		return make_error<RlcError>(
				"Expected function, action or entity declaration",
				RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken),
				location);
	}
	return s;
}

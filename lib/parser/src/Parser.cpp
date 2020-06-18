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
	pos.setColumn(lexer.getCurrentColumn());
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

	auto errorMessage = pos.toString();
	errorMessage += "unexpected token ";
	errorMessage += tokenToString(current);
	errorMessage += " expected";
	errorMessage += tokenToString(t);
	return make_error<StringError>(
			move(errorMessage),
			RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken));
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
	if (accept<Token::Identifier>())
		return Expression::reference(lIdent);

	if (accept<Token::Double>())
		return Expression::scalarConstant(lDouble);

	if (accept<Token::Int64>())
		return Expression::scalarConstant(lInt64);

	if (accept<Token::KeywordFalse>())
		return Expression::scalarConstant(false);

	if (accept<Token::KeywordTrue>())
		return Expression::scalarConstant(true);

	if (accept<Token::LPar>())
	{
		TRY(exp, expression());
		EXPECT(Token::RPar);
		return exp;
	}

	return make_error<StringError>(
			pos.toString() + " empty expression",
			RlcErrorCategory::errorCode(RlcErrorCode::unexpectedToken));
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
		exps.emplace_back(move(*exp));
	} while (accept<Token::Comma>());

	return exps;
}

/**
 * postFixExpression :
 *			((postFixExpression)*
 *					("["expression"]"
 *					|"("argumentExpressionList ")" |
 *					. identifier))
 *			| primaryExpression
 */
Expected<Expression> Parser::postFixExpression()
{
	TRY(exp, primaryExpression());

	if (accept<Token::LPar>())
	{
		TRY(args, argumentExpressionList());
		EXPECT(Token::RPar);
		return Expression::call(*exp, move(*args));
	}

	if (accept<Token::LSquare>())
	{
		TRY(accExp, expression());
		EXPECT(Token::RSquare);
		return (*exp)[move(*accExp)];
	}

	if (accept<Token::Dot>())
	{
		EXPECT(Token::Identifier);
		return Expression::memberAccess(move(*exp), lIdent);
	}
	return move(*exp);
}

/**
 * unaryExpression : postFixExpression | unaryOperator unaryExpression
 */
Expected<Expression> Parser::unaryExpression()
{
	if (accept<Token::Plus>())
	{
		TRY(exp, unaryExpression());
		return Expression::call(Expression::reference("plus"), { move(*exp) });
	}

	if (accept<Token::Minus>())
	{
		TRY(exp, unaryExpression());
		return Expression::call(Expression::reference("minus"), { move(*exp) });
	}

	if (accept<Token::ExMark>())
	{
		TRY(exp, unaryExpression());
		return Expression::call(Expression::reference("not"), { move(*exp) });
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

	if (accept<Token::Mult>())
	{
		TRY(rhs, multyplicativeExpression());
		return (*exp) * (move(*rhs));
	}
	if (accept<Token::Divide>())
	{
		TRY(rhs, multyplicativeExpression());
		return (*exp) / (move(*rhs));
	}
	if (accept<Token::Module>())
	{
		TRY(rhs, multyplicativeExpression());
		return Expression::call(
				Expression::reference("module"), { *move(exp), move(*rhs) });
	}

	return move(*exp);
}

/**
 * additiveExpression : multiplicativeExpression | additiveExpression ('+' |
 * '-')multiplicativeExpression
 */
Expected<Expression> Parser::additiveExpression()
{
	TRY(exp, multyplicativeExpression());

	if (accept<Token::Plus>())
	{
		TRY(rhs, additiveExpression());
		return (*exp) + (move(*rhs));
	}
	if (accept<Token::Minus>())
	{
		TRY(rhs, additiveExpression());
		return (*exp) - (move(*rhs));
	}
	return move(*exp);
}

/**
 * orExpression : additiveExpression (('<' | '>' | '<=' | '>=')
 * additiveExpression)*
 */
Expected<Expression> Parser::relationalExpression()
{
	TRY(exp, additiveExpression());

	if (accept<Token::LAng>())
	{
		TRY(rhs, relationalExpression());
		return (*exp) < (move(*rhs));
	}
	if (accept<Token::RAng>())
	{
		TRY(rhs, relationalExpression());
		return (*exp) > (move(*rhs));
	}
	if (accept<Token::GEqual>())
	{
		TRY(rhs, relationalExpression());
		return (*exp) >= (move(*rhs));
	}
	if (accept<Token::LEqual>())
	{
		TRY(rhs, relationalExpression());
		return (*exp) <= (move(*rhs));
	}
	return move(*exp);
}

/**
 * orExpression : relationalExpression (('==' | '!=') relationalExpression)*
 */
Expected<Expression> Parser::equalityExpression()
{
	TRY(exp, relationalExpression());

	if (accept<Token::EqualEqual>())
	{
		TRY(rhs, equalityExpression());
		return Expression::call(
				Expression::reference("equal"), { move(*exp), move(*rhs) });
	}
	if (accept<Token::NEqual>())
	{
		TRY(rhs, equalityExpression());
		return Expression::call(
				Expression::reference("nequal"), { move(*exp), move(*rhs) });
	}
	return move(*exp);
}

/**
 * orExpression : equalityExpression ('and' equalityExpression)*
 */
Expected<Expression> Parser::andExpression()
{
	TRY(exp, equalityExpression());

	if (accept<Token::KeywordAnd>())
	{
		TRY(rhs, andExpression());
		return Expression::call(
				Expression::reference("and"), { move(*exp), move(*rhs) });
	}
	return move(*exp);
}

/**
 * orExpression : andExpression ('or' andExpression)*
 */
Expected<Expression> Parser::orExpression()
{
	TRY(exp, andExpression());

	if (accept<Token::KeywordOr>())
	{
		TRY(rhs, orExpression());
		return Expression::call(
				Expression::reference("or"), { move(*exp), move(*rhs) });
	}
	return move(*exp);
}

Expected<Expression> Parser::expression() { return assignmentExpression(); }

/**
 * assigmentExpression : orExpression | orExpression "=" assigmentExpression
 */
Expected<Expression> Parser::assignmentExpression()
{
	TRY(leftHand, orExpression());

	if (!accept<Token::Equal>())
		return move(*leftHand);

	TRY(rightHand, assignmentExpression());
	return Expression::assign(move(*leftHand), move(*rightHand));
}

/**
 * EntityField : Indetifier Identifier
 */
llvm::Expected<EntityField> Parser::entityField()
{
	EXPECT(Token::Identifier);
	auto typeName = lIdent;
	EXPECT(Token::Identifier);
	return EntityField(move(typeName), move(lIdent));
}

/**
 * EntityDeclaration : Ent Identifier Colons Newline Indent (entityField
 * Newline)* Deindent
 */
llvm::Expected<EntityDeclaration> Parser::entityDeclaration()
{
	auto pos = getCurrentSourcePos();
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
		fields.emplace_back(move(*field));
		EXPECT(Token::Newline);
	}
	auto e = Entity(move(name), move(fields));
	return EntityDeclaration(move(e), move(pos));
}

/**
 * expressionStatement : expression '\n'
 */
llvm::Expected<Statement> Parser::expressionStatement()
{
	auto pos = getCurrentSourcePos();
	TRY(exp, expression());
	EXPECT(Token::Newline);

	return Statement::expStatement(move(*exp), move(pos));
}

/**
 * ifStatement : if expression ':\nindent statementList [else ':\n'
 * statementList ]
 */
llvm::Expected<Statement> Parser::ifStatement()
{
	SourcePosition pos = getCurrentSourcePos();
	EXPECT(Token::KeywordIf);
	TRY(exp, expression());
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	TRY(tBranch, statementList());
	if (!accept<Token::KeywordElse>())
		return Statement::ifStatment(move(*exp), move(*tBranch), move(pos));

	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	TRY(fBranch, statementList());
	return Statement::ifStatment(
			move(*exp), move(*tBranch), move(*fBranch), move(pos));
}

/**
 * whileStatement : While exp ':\n' statementList
 */
Expected<Statement> Parser::whileStatement()
{
	auto pos = getCurrentSourcePos();
	EXPECT(Token::KeywordWhile);
	TRY(exp, expression());
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);
	TRY(statLis, statementList());
	return Statement::whileStatement(move(*exp), move(*statLis), move(pos));
}

/**
 * returnStatement : return [expression] '\n'
 */
Expected<Statement> Parser::returnStatement()
{
	auto pos = getCurrentSourcePos();
	EXPECT(Token::KeywordReturn);
	if (accept(Token::Newline))
		return Statement::returnStatement(move(pos));

	TRY(exp, expression());
	EXPECT(Token::Newline);
	return Statement::returnStatement(move(*exp), move(pos));
}

Expected<Statement> Parser::statement()
{
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
 * declarationStatement : let identifier '=' expression
 */
Expected<Statement> Parser::declarationStatement()
{
	auto pos = getCurrentSourcePos();
	EXPECT(Token::KeywordLet);
	EXPECT(Token::Identifier);
	auto name = lIdent;

	EXPECT(Token::Equal);
	TRY(exp, expression());
	EXPECT(Token::Newline);
	return Statement::declarationStatement(move(name), move(*exp), move(pos));
}

/**
 * statmentList : indent (statement)* deindent
 */
Expected<Statement> Parser::statementList()
{
	auto pos = getCurrentSourcePos();

	SmallVector<Statement, 3> stmts;
	EXPECT(Token::Indent);
	while (!accept<Token::Deindent>())
	{
		TRY(s, statement());
		stmts.emplace_back(move(*s));
	}

	return Statement::statmentList(move(stmts), move(pos));
}

/**
 * functionTypeUse : [singleTypeUse ("->" singleTypeUse )*]
 */
Expected<SingleTypeUse> Parser::functionTypeUse()
{
	SmallVector<SingleTypeUse, 2> tpUse;

	TRY(singleTp, singleTypeUse());
	tpUse.emplace_back(move(*singleTp));
	while (accept<Token::Arrow>())
	{
		TRY(singleTp, singleTypeUse());
		tpUse.emplace_back(move(*singleTp));
	}

	return FunctionTypeUse::functionType(move(tpUse));
}

/**
 * singleTypeUse : "(" functionType ")" | identifier["["int64"]"]
 */
Expected<SingleTypeUse> Parser::singleTypeUse()
{
	if (accept<Token::LPar>())
	{
		TRY(fType, functionTypeUse());
		EXPECT(Token::RPar);
		return move(*fType);
	}

	EXPECT(Token::Identifier);
	auto nm = lIdent;
	if (!accept<Token::LSquare>())
		return SingleTypeUse::scalarType(move(nm));

	EXPECT(Token::Int64);
	auto size = lInt64;
	EXPECT(Token::RSquare);
	return SingleTypeUse::arrayType(move(nm), size);
}

Expected<ArgumentDeclaration> Parser::argDeclaration()
{
	auto pos = getCurrentSourcePos();
	TRY(tp, singleTypeUse());
	EXPECT(Token::Identifier);
	auto parName = lIdent;
	return ArgumentDeclaration(move(parName), move(*tp), move(pos));
}

/**
 * functionDefinition : "fun" identifier "(" [argDeclaration (","
 * argDeclaration)*] ")->" singleTypeUse ":\n" statementList
 */
Expected<FunctionDefinition> Parser::functionDefinition()
{
	auto pos = getCurrentSourcePos();
	EXPECT(Token::KeywordFun);
	EXPECT(Token::Identifier);
	auto nm = lIdent;
	EXPECT(Token::LPar);

	llvm::SmallVector<ArgumentDeclaration, 3> args;

	if (current != Token::RPar)
	{
		do
		{
			TRY(arg, argDeclaration());
			args.emplace_back(move(*arg));
		} while (accept<Token::Comma>());
	}

	EXPECT(Token::RPar);
	EXPECT(Token::Arrow);
	TRY(retType, singleTypeUse());
	EXPECT(Token::Colons);
	EXPECT(Token::Newline);

	TRY(body, statementList());
	return FunctionDefinition(
			move(nm), move(*body), move(*retType), move(args), pos);
}

Expected<System> Parser::system()
{
	EXPECT(Token::KeywordSystem);
	EXPECT(Token::Identifier);
	System s(lIdent);

	EXPECT(Token::Newline);

	while (current != Token::End)
	{
		accept<Token::Newline>();
		accept<Token::Indent>();
		accept<Token::Deindent>();

		if (current == Token::KeywordFun)
		{
			TRY(f, functionDefinition());
			s.addFunction(move(*f));
		}

		if (current == Token::KeywordEntity)
		{
			TRY(f, entityDeclaration());
			s.addEntity(move(*f));
		}
	}
	return s;
}

#include "rlc/parser/Parser.hpp"

#include <system_error>

#include "llvm/Support/Error.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/Expression.hpp"
#include "rlc/parser/Lexer.hpp"
#include "rlc/utils/Error.hpp"

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

Expected<Expression> Parser::expression() { return orExpression(); }

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

#include "gtest/gtest.h"
#include <bits/stdint-intn.h>

#include "rlc/ast/Call.hpp"
#include "rlc/ast/Reference.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/parser/Lexer.hpp"
#include "rlc/parser/Parser.hpp"
#include "rlc/utils/ScopeGuard.hpp"

using namespace rlc;

TEST(ParserTest, testScalarConstantExpressions)
{
	Parser p("3", "fileName");

	auto exp = p.primaryExpression();
	if (!exp)
		FAIL();

	EXPECT_TRUE(exp->isA<ScalarConstant>());
	EXPECT_EQ(3, exp->get<ScalarConstant>().get<int64_t>());
}

TEST(ParserTest, testScalarBoolExpressions)
{
	Parser p("true", "fileName");

	auto exp = p.primaryExpression();
	if (!exp)
		FAIL();

	EXPECT_TRUE(exp->isA<ScalarConstant>());
	EXPECT_EQ(true, exp->get<ScalarConstant>().get<bool>());
}

TEST(ParserTest, testScalarDoubleExpressions)
{
	Parser p("3.14", "fileName");

	auto exp = p.primaryExpression();
	if (!exp)
		FAIL();

	EXPECT_TRUE(exp->isA<ScalarConstant>());
	EXPECT_EQ(3.14, exp->get<ScalarConstant>().get<double>());
}

TEST(ParserTest, testExpression)
{
	Parser p("3.14", "fileName");

	auto exp = p.expression();
	if (!exp)
		FAIL();

	EXPECT_TRUE(exp->isA<ScalarConstant>());
	EXPECT_EQ(3.14, exp->get<ScalarConstant>().get<double>());
}

TEST(ParserTest, testAdditiveExpression)
{
	Parser p("3.14 + 2", "fileName");

	auto exp = p.expression();
	if (!exp)
		FAIL();

	EXPECT_TRUE(exp->isA<Call>());
	EXPECT_EQ(exp->getCall().argsCount(), 2);
}

TEST(ParserTest, testAssigmentExpression)
{
	Parser p("3.14 = 2", "fileName");

	auto exp = p.expression();
	if (!exp)
		FAIL();

	EXPECT_TRUE(exp->isA<Call>());
	EXPECT_EQ(exp->getCall().argsCount(), 2);
	EXPECT_EQ(
			exp->getCall().getFunctionExpression().get<Reference>().getName(),
			"assign");
}

TEST(ParserTest, testEntityDeclaration)
{
	Parser p("ent something:\n\tdouble a\n\tbool b\nsomethingelse", "fileName");

	auto ent = p.entityDeclaration();
	if (!ent)
		FAIL();

	EXPECT_EQ(ent->getEntity().getName(), "something");
	EXPECT_EQ(ent->getEntity().fieldsCount(), 2);
	EXPECT_EQ(ent->getEntity()[0].getName(), "a");
	EXPECT_EQ(ent->getEntity()[1].getName(), "b");
	EXPECT_EQ(ent->getEntity()[1].getTypeName(), "bool");
}

TEST(ParserTest, declarationTest)
{
	Parser p("let a = 5", "fileName");
	auto s = p.statement();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->isDeclarationStatement());
	auto& dcl = s->get<DeclarationStatement>();
	EXPECT_EQ(dcl.getName(), "a");
	EXPECT_TRUE(dcl.getExpression().isA<ScalarConstant>());
}

TEST(ParserTest, ifElseTest)
{
	Parser p("if 1:\n\tcall()\nelse:\n\tbody()\nsomething()", "fileName");
	auto s = p.statement();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->isIfStat());
	auto& ifS = s->get<IfStatement>();
	EXPECT_TRUE(ifS.hasFalseBranch());
	EXPECT_TRUE(ifS.getTrueBranch().isStatmentList());
	EXPECT_TRUE(ifS.getFalseBranch().isStatmentList());
}

TEST(ParserTest, whileStatement)
{
	Parser p("while 1:\n\ta = call()\nsomething()", "fileName");
	auto s = p.statement();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->isWhileStat());
	auto& ifS = s->get<WhileStatement>();
	EXPECT_TRUE(ifS.getBody().isStatmentList());
}

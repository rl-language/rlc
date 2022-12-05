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
	EXPECT_EQ(ent->getEntity()[1].getTypeUse().getName(), "bool");
}

TEST(ParserTest, declarationTest)
{
	Parser p("let a = 5\n", "fileName");
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

TEST(ParserTest, singleTypeTest)
{
	Parser p("int", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->isScalar());
	EXPECT_EQ(s->getName(), "int");
}

TEST(ParserTest, arrayTypeTest)
{
	Parser p("int[10]", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->isArray());
	EXPECT_EQ(s->getName(), "int");
	EXPECT_EQ(s->getDimension(), 10);
}

TEST(ParserTest, functionTypeTest)
{
	Parser p("(int[10])", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->isFunctionType());
	const auto& r = s->getFunctionType().getReturnType();
	EXPECT_TRUE(r.isArray());
	EXPECT_EQ(r.getName(), "int");
	EXPECT_EQ(r.getDimension(), 10);
}

TEST(ParserTest, complexFunctionTypeTest)
{
	Parser p("(int[10] -> int)", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->isFunctionType());
	EXPECT_EQ(s->getFunctionType().argCount(), 1);
	const auto& r = s->getFunctionType().getArgType(0);
	EXPECT_TRUE(r.isArray());
	EXPECT_EQ(r.getName(), "int");
	EXPECT_EQ(r.getDimension(), 10);
}

TEST(ParserTest, functionDefinition)
{
	Parser p("fun a()->void:\n\tb()\nc()", "fileName");
	auto s = p.functionDefinition();
	if (!s)
		FAIL();

	EXPECT_EQ(s->getName(), "a");
	EXPECT_EQ(s->argumentsCount(), 0);
	EXPECT_EQ(s->getTypeUse().getName(), "void");
}

TEST(ParserTest, zeroDefinition)
{
	Parser p("0", "fileName");
	auto s = p.primaryExpression();
	if (!s)
		FAIL();

	auto& constant = s->get<ScalarConstant>();
	EXPECT_TRUE(constant.isA<int64_t>());
	EXPECT_EQ(constant.get<int64_t>(), 0);
}

TEST(ParserTest, voidDeclaration)
{
	Parser p("fun function() -> void:\n\treturn\n", "fileName");
	auto s = p.functionDefinition();
	if (!s)
		FAIL();

	EXPECT_EQ(s->getName(), "function");
	EXPECT_EQ(s->getDeclaration().getName(), "function");
	EXPECT_EQ(s->getDeclaration().getTypeUse().getName(), "void");
}

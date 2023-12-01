#include "gtest/gtest.h"

#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/parser/Parser.hpp"
#include "rlc/utils/ScopeGuard.hpp"

using namespace rlc;

TEST(ParserTest, testScalarConstantExpressions)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "3", "fileName");

	auto exp = p.primaryExpression();
	if (!exp)
		FAIL();

	EXPECT_NE(exp->getDefiningOp<mlir::rlc::Constant>(), nullptr);
	EXPECT_EQ(
			3,
			exp->getDefiningOp<mlir::rlc::Constant>()
					.getValueAttr()
					.cast<mlir::IntegerAttr>()
					.getValue());
}

TEST(ParserTest, testScalarBoolExpressions)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "true", "fileName");

	auto exp = p.primaryExpression();
	if (!exp)
		FAIL();

	EXPECT_NE(exp->getDefiningOp<mlir::rlc::Constant>(), nullptr);
	EXPECT_EQ(
			true,
			exp->getDefiningOp<mlir::rlc::Constant>()
					.getValue()
					.cast<mlir::BoolAttr>()
					.getValue());
}

TEST(ParserTest, testScalarDoubleExpressions)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "3.14", "fileName");

	auto exp = p.primaryExpression();
	if (!exp)
		FAIL();

	EXPECT_NE(exp->getDefiningOp<mlir::rlc::Constant>(), nullptr);
	exp->getDefiningOp<mlir::rlc::Constant>()
			.getValueAttr()
			.cast<mlir::FloatAttr>()
			.getValue();
}

TEST(ParserTest, testExpression)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "3.14", "fileName");

	auto exp = p.expression();
	if (!exp)
		FAIL();

	EXPECT_TRUE(exp->getDefiningOp<mlir::rlc::Constant>());
	exp->getDefiningOp<mlir::rlc::Constant>()
			.getValueAttr()
			.cast<mlir::FloatAttr>()
			.getValue();
}

TEST(ParserTest, testAdditiveExpression)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "3.14 + 2", "fileName");

	auto exp = p.expression();
	if (!exp)
		FAIL();

	EXPECT_NE(exp->getDefiningOp<mlir::rlc::AddOp>(), nullptr);
}

TEST(ParserTest, testAssigmentExpression)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "3.14 = 2", "fileName");

	auto exp = p.expression();
	if (!exp)
		FAIL();

	EXPECT_NE(exp->getDefiningOp<mlir::rlc::AssignOp>(), nullptr);
}

TEST(ParserTest, testEntityDeclaration)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(
			&context,
			"ent something:\n\tdouble a\n\tbool b\nsomethingelse",
			"fileName");

	auto ent = p.entityDeclaration();
	if (!ent)
		FAIL();

	EXPECT_EQ(ent->getName(), "something");
	EXPECT_EQ(ent->getMemberNames().size(), 2);
	EXPECT_EQ(ent->getMemberNames()[0].cast<mlir::StringAttr>().getValue(), "a");
	EXPECT_EQ(ent->getMemberNames()[1].cast<mlir::StringAttr>().getValue(), "b");
}

TEST(ParserTest, declarationTest)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "let a = 5\n", "fileName");
	auto s = p.declarationStatement();
	if (!s)
		FAIL();

	EXPECT_EQ(s->getName(), "a");
}

TEST(ParserTest, ifElseTest)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(
			&context, "if 1:\n\tcall()\nelse:\n\tbody()\nsomething()", "fileName");
	auto s = p.ifStatement();
	if (!s)
		FAIL();

	EXPECT_TRUE(s->getTrueBranch().getBlocks().size() == 1);
	EXPECT_TRUE(s->getElseBranch().getBlocks().size() == 1);
}

TEST(ParserTest, whileStatement)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "while 1:\n\ta = call()\nsomething()", "fileName");
	auto s = p.whileStatement();
	if (!s)
		FAIL();
}

TEST(ParserTest, singleTypeTest)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	Parser p(&context, "int", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();
	auto casted = s->cast<mlir::rlc::ScalarUseType>();

	EXPECT_EQ(casted.getSize(), mlir::rlc::IntegerLiteralType::get(&context, 0));
	EXPECT_EQ(casted.getReadType(), "int");
}

TEST(ParserTest, arrayTypeTest)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "int[10]", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();
	auto casted = s->cast<mlir::rlc::ScalarUseType>();

	EXPECT_EQ(casted.getSize(), mlir::rlc::IntegerLiteralType::get(&context, 10));
	EXPECT_EQ(
			casted.getUnderlying().cast<mlir::rlc::ScalarUseType>().getReadType(),
			"int");
}

TEST(ParserTest, functionTypeTest)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "(int[10])", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();

	auto casted = s->cast<mlir::rlc::ScalarUseType>();
	EXPECT_TRUE(casted.getUnderlying().isa<mlir::rlc::FunctionUseType>());
	auto r = casted.getUnderlying()
							 .cast<mlir::rlc::FunctionUseType>()
							 .getSubTypes()[0]
							 .cast<mlir::rlc::ScalarUseType>();
	EXPECT_EQ(
			r.getUnderlying().cast<mlir::rlc::ScalarUseType>().getReadType(), "int");
	EXPECT_EQ(r.getSize(), mlir::rlc::IntegerLiteralType::get(&context, 10));
}

TEST(ParserTest, complexFunctionTypeTest)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "(int[10] -> int)", "fileName");
	auto s = p.singleTypeUse();
	if (!s)
		FAIL();

	auto casted = s->cast<mlir::rlc::ScalarUseType>();
	EXPECT_TRUE(casted.getUnderlying().isa<mlir::rlc::FunctionUseType>());
	auto r = casted.getUnderlying()
							 .cast<mlir::rlc::FunctionUseType>()
							 .getSubTypes()[0]
							 .cast<mlir::rlc::ScalarUseType>();
	EXPECT_EQ(
			r.getUnderlying().cast<mlir::rlc::ScalarUseType>().getReadType(), "int");
	EXPECT_EQ(r.getSize(), mlir::rlc::IntegerLiteralType::get(&context, 10));
	auto r2 = casted.getUnderlying()
								.cast<mlir::rlc::FunctionUseType>()
								.getSubTypes()[1]
								.cast<mlir::rlc::ScalarUseType>();
	EXPECT_EQ(r2.getReadType(), "int");
	EXPECT_EQ(r2.getSize(), mlir::rlc::IntegerLiteralType::get(&context, 0));
}

TEST(ParserTest, functionDefinition)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "fun a()->void:\n\tb()\nc()", "fileName");
	auto s = p.functionDefinition();
	if (!s)
		FAIL();

	EXPECT_EQ(s->getUnmangledName(), "a");
	EXPECT_EQ(s->getArgNames().size(), 0);
}

TEST(ParserTest, zeroDefinition)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "0", "fileName");
	auto s = p.primaryExpression();
	if (!s)
		FAIL();

	EXPECT_NE(s->getDefiningOp<mlir::rlc::Constant>(), nullptr);
	EXPECT_EQ(
			0,
			s->getDefiningOp<mlir::rlc::Constant>()
					.getValueAttr()
					.cast<mlir::IntegerAttr>()
					.getValue());
}

TEST(ParserTest, voidDeclaration)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(&context, "fun function() -> void:\n\treturn\n", "fileName");
	auto s = p.functionDefinition();
	if (!s)
		FAIL();

	EXPECT_EQ(s->getUnmangledName(), "function");
}

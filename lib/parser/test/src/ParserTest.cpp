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

	auto exp = p.primaryExpression();
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

	auto exp = p.additiveExpression();
	if (!exp)
		FAIL();

	EXPECT_NE(exp->getDefiningOp<mlir::rlc::AddOp>(), nullptr);
}

TEST(ParserTest, testClassDeclaration)
{
	mlir::MLIRContext context;
	context.loadDialect<mlir::rlc::RLCDialect>();
	context.loadAllAvailableDialects();
	Parser p(
			&context,
			"cls something:\n\tdouble a\n\tbool b\nsomethingelse",
			"fileName");

	auto ent = p.classDeclaration();
	if (!ent)
		FAIL();

	EXPECT_EQ(ent->getName(), "something");
	EXPECT_EQ(ent->getMembers().size(), 2);
	EXPECT_EQ(ent->getMemberFields()[0].getName(), "a");
	EXPECT_EQ(ent->getMemberFields()[1].getName(), "b");
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

	EXPECT_EQ(s->getSymName(), "a");
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

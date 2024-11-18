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

#include "any"
#include "rlc/dialect/ActionArgumentAnalysis.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"

TEST(DialectTest, typeTest)
{
	mlir::MLIRContext ctx;
	ctx.loadDialect<mlir::rlc::RLCDialect>();

	auto integer = mlir::rlc::IntegerType::getInt64(&ctx);
	EXPECT_EQ(integer.getMnemonic(), "int");
}

TEST(DialectTest, structTest)
{
	mlir::MLIRContext ctx;
	ctx.loadDialect<mlir::rlc::RLCDialect>();

	auto integer = mlir::rlc::IntegerType::getInt64(&ctx);
	auto floattype = mlir::rlc::FloatType::get(&ctx);
	auto classDecl = mlir::rlc::ClassType::getNewIdentified(
			&ctx,
			"peppino",
			{ mlir::rlc::ClassFieldAttr::get("asdint", integer),
				mlir::rlc::ClassFieldAttr::get("asdfloat", floattype) },
			{});
	EXPECT_EQ(classDecl.getMnemonic(), "class");
	EXPECT_EQ(classDecl.getName(), "peppino");
}

TEST(DialectTest, expressionTest)
{
	mlir::MLIRContext ctx;
	ctx.loadDialect<mlir::rlc::RLCDialect>();
	mlir::OpBuilder builder(&ctx);
	auto op = builder.create<mlir::ModuleOp>(
			mlir::UnknownLoc::get(&ctx), llvm::StringRef("name"));
	auto& bb = op.getBodyRegion().front();
	builder.setInsertionPoint(&bb, bb.begin());
	auto exp = builder.create<mlir::rlc::ExpressionStatement>(
			mlir::UnknownLoc::get(&ctx));
	builder.createBlock(&exp.getBody());
	builder.create<mlir::LLVM::ReturnOp>(
			mlir::UnknownLoc::get(&ctx), mlir::ValueRange{});
}

TEST(ActionArgumentAnalysisTest, integerBoundTest)
{
	mlir::MLIRContext ctx;
	ctx.loadDialect<mlir::rlc::RLCDialect>();
	mlir::OpBuilder builder(&ctx);
	auto op = builder.create<mlir::ModuleOp>(
			mlir::UnknownLoc::get(&ctx), llvm::StringRef("name"));
	auto& bb = op.getBodyRegion().front();
	builder.setInsertionPoint(&bb, bb.begin());
	auto action = builder.create<mlir::rlc::ActionStatement>(
			builder.getUnknownLoc(),
			mlir::TypeRange({ mlir::rlc::IntegerType::getInt64(&ctx) }),
			builder.getStringAttr("dc"),
			mlir::rlc::FunctionInfoAttr::get(builder.getContext(), { "arg" }),
			0,
			0);

	auto* block = builder.createBlock(
			&action.getPrecondition(),
			action.getPrecondition().begin(),
			mlir::TypeRange({ mlir::rlc::IntegerType::getInt64(&ctx) }),
			{ builder.getUnknownLoc() });
	builder.setInsertionPoint(block, block->begin());

	auto minusTen = builder.create<mlir::rlc::Constant>(
			builder.getUnknownLoc(), int64_t(-10));
	auto ten =
			builder.create<mlir::rlc::Constant>(builder.getUnknownLoc(), int64_t(10));

	builder.create<mlir::rlc::LessEqualOp>(
			builder.getUnknownLoc(), minusTen, block->getArgument(0));
	builder.create<mlir::rlc::LessEqualOp>(
			builder.getUnknownLoc(), block->getArgument(0), ten);

	builder.create<mlir::rlc::Yield>(
			builder.getUnknownLoc(), mlir::ValueRange({ minusTen, ten }));

	mlir::rlc::ActionArgumentAnalysis analysis(action);
	EXPECT_EQ(
			analysis.getBoundsOf(action.getPrecondition().front().getArgument(0))
					.getMin(),
			-10);
	EXPECT_EQ(
			analysis.getBoundsOf(action.getPrecondition().front().getArgument(0))
					.getMax(),
			10);
}

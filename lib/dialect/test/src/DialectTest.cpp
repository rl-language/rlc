#include "gtest/gtest.h"

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
	auto entity = mlir::rlc::EntityType::getNewIdentified(
			&ctx, "peppino", { integer, floattype }, { "asdint", "asdfloat" }, {});
	EXPECT_EQ(entity.getMnemonic(), "entity");
	EXPECT_EQ(entity.getName(), "peppino");
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
			"dc",
			builder.getStrArrayAttr({ "arg" }));

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

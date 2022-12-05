#include "gtest/gtest.h"

#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"

TEST(DialectTest, typeTest)
{
	mlir::MLIRContext ctx;
	ctx.loadDialect<mlir::rlc::RLCDialect>();

	auto integer = mlir::rlc::IntegerType::get(&ctx);
	EXPECT_EQ(integer.getMnemonic(), "int");
}

TEST(DialectTest, structTest)
{
	mlir::MLIRContext ctx;
	ctx.loadDialect<mlir::rlc::RLCDialect>();

	auto integer = mlir::rlc::IntegerType::get(&ctx);
	auto floattype = mlir::rlc::FloatType::get(&ctx);
	auto entity = mlir::rlc::EntityType::getNewIdentified(
			&ctx, "peppino", { integer, floattype });
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
	op.dump();
}

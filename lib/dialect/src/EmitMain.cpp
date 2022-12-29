#include "rlc/dialect/EmitMain.hpp"

void rlc::EmitMainPass::runOnOperation()
{
	auto mangeledMainName = mlir::rlc::mangledName(
			"main",
			mlir::FunctionType::get(
					&getContext(),
					mlir::TypeRange(),
					mlir::TypeRange({ mlir::rlc::IntegerType::get(&getContext()) })));
	auto realMain =
			getOperation().lookupSymbol<mlir::LLVM::LLVMFuncOp>(mangeledMainName);
	if (realMain == nullptr)
		return;

	mlir::OpBuilder builder(realMain);

	auto returnType = builder.getI32Type();
	auto op = builder.create<mlir::LLVM::LLVMFuncOp>(
			realMain.getLoc(),
			"main",
			mlir::LLVM::LLVMFunctionType::get(returnType, {}));

	auto* block = op.addEntryBlock();
	builder.setInsertionPoint(block, block->begin());

	auto call = builder.create<mlir::LLVM::CallOp>(
			realMain.getLoc(), realMain, mlir::ValueRange());

	auto res = *call.getResults().begin();
	auto trunchated =
			builder.create<mlir::LLVM::TruncOp>(realMain.getLoc(), returnType, res);
	builder.create<mlir::LLVM::ReturnOp>(
			realMain.getLoc(), mlir::ValueRange({ trunchated }));
}

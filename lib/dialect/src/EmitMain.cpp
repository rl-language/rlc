
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
#define GEN_PASS_DEF_EMITMAINPASS
#include "rlc/dialect/Passes.inc"
	struct EmitMainPass: public impl::EmitMainPassBase<EmitMainPass>
	{
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect, mlir::cf::ControlFlowDialect>();
		}

		void runOnOperation() override
		{
			auto mangeledMainName = mlir::rlc::mangledName(
					"main",
					mlir::FunctionType::get(
							&getContext(),
							mlir::TypeRange(),
							mlir::TypeRange(
									{ mlir::rlc::IntegerType::getInt64(&getContext()) })));
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

			auto count = builder.create<mlir::LLVM::ConstantOp>(
					realMain.getLoc(),
					builder.getI64Type(),
					builder.getI64IntegerAttr(1));
			auto alloca = builder.create<mlir::LLVM::AllocaOp>(
					realMain.getLoc(),
					mlir::LLVM::LLVMPointerType::get(builder.getI64Type()),
					count,
					0);
			auto call = builder.create<mlir::LLVM::CallOp>(
					realMain.getLoc(), realMain, mlir::ValueRange({ alloca }));

			auto aligment =
					mlir::DataLayout::closest(alloca).getTypePreferredAlignment(
							alloca.getType()
									.cast<mlir::LLVM::LLVMPointerType>()
									.getElementType());
			auto loaded = builder.create<mlir::LLVM::LoadOp>(
					realMain.getLoc(), alloca, aligment);

			auto res = *call.getResults().begin();
			auto trunchated = builder.create<mlir::LLVM::TruncOp>(
					realMain.getLoc(), returnType, loaded);
			builder.create<mlir::LLVM::ReturnOp>(
					realMain.getLoc(), mlir::ValueRange({ trunchated }));
		}

		private:
		mlir::TypeConverter converter;
	};

}	 // namespace mlir::rlc

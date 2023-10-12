
#include "llvm/Support/Casting.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"

static void addPreconditionChecks(mlir::rlc::FunctionMetadataOp metadata) {
	auto funcOp = llvm::cast<mlir::rlc::FunctionOp>(metadata.getSourceFunction().getDefiningOp());
	auto &body = funcOp.getBody();
	auto preconditionFunction = metadata.getPreconditionFunction();

	mlir::OpBuilder builder(funcOp);

	// call the precondition function at the beginning of the function body.
	builder.setInsertionPoint(&body.front().front());
	
	auto preconditionsHold = builder.create<mlir::rlc::CallOp>(
		preconditionFunction.getLoc(),
		preconditionFunction,
		body.getBlocks().begin()->getArguments()
	);
	
	// emit an assert
	builder.create<mlir::rlc::AssertOp>(preconditionFunction.getLoc(), preconditionsHold->getResults().front());
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_ADDPRECONDITIONSCHECKPASS
#include "rlc/dialect/Passes.inc"
	struct AddPreconditionsCheckPass: public impl::AddPreconditionsCheckPassBase<AddPreconditionsCheckPass>
	{
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}

		void runOnOperation() override
		{
			ModuleOp module = getOperation();

			llvm::SmallVector<mlir::rlc::FunctionMetadataOp, 4> functionMetadataOps;
			module.walk([&](mlir::rlc::FunctionMetadataOp op){
				functionMetadataOps.emplace_back(op);
			});

			for(auto metadata : functionMetadataOps) {
				addPreconditionChecks(metadata);
			}
		}
	};

}	 // namespace mlir::rlc

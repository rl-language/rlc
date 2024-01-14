/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
*/

#include "llvm/Support/Casting.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"

static void addPreconditionChecks(mlir::rlc::FunctionMetadataOp metadata)
{
	auto funcOp = llvm::cast<mlir::rlc::FunctionOp>(
			metadata.getSourceFunction().getDefiningOp());
	auto& body = funcOp.getBody();
	auto preconditionFunction = metadata.getPreconditionFunction();

	mlir::OpBuilder builder(funcOp);

	// call the precondition function at the beginning of the function body.
	builder.setInsertionPoint(&body.front().front());

	auto preconditionsHold = builder.create<mlir::rlc::CallOp>(
			preconditionFunction.getLoc(),
			preconditionFunction,
			false,
			body.getBlocks().begin()->getArguments());

	// emit an assert
	builder.create<mlir::rlc::AssertOp>(
			preconditionFunction.getLoc(), preconditionsHold->getResults().front());
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_ADDPRECONDITIONSCHECKPASS
#include "rlc/dialect/Passes.inc"
	struct AddPreconditionsCheckPass
			: public impl::AddPreconditionsCheckPassBase<AddPreconditionsCheckPass>
	{
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}

		void runOnOperation() override
		{
			ModuleOp module = getOperation();

			llvm::SmallVector<mlir::rlc::FunctionMetadataOp, 4> functionMetadataOps;
			module.walk([&](mlir::rlc::FunctionMetadataOp op) {
				functionMetadataOps.emplace_back(op);
			});

			for (auto metadata : functionMetadataOps)
			{
				addPreconditionChecks(metadata);
			}
		}
	};

}	 // namespace mlir::rlc

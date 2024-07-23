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

	std::string message;
	llvm::raw_string_ostream OS(message);
	OS << "function " << funcOp.getUnmangledName()
		 << " called without respecting precondition.";
	OS.flush();

	// emit an assert
	builder.create<mlir::rlc::AssertOp>(
			preconditionFunction.getLoc(),
			preconditionsHold->getResults().front(),
			message);
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

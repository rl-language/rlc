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
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/PatternMatch.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

static mlir::rlc::FunctionMetadataOp emitPreconditionFunction(
		mlir::rlc::FunctionOp fun)
{
	mlir::IRRewriter rewriter(fun.getContext());
	rewriter.setInsertionPoint(fun);
	if (fun.getPrecondition().empty())
		return nullptr;

	auto ftype = mlir::FunctionType::get(
			fun.getContext(),
			fun.getFunctionType().getInputs(),
			{ mlir::rlc::BoolType::get(fun.getContext()) });

	auto validityFunction = rewriter.create<mlir::rlc::FunctionOp>(
			fun.getLoc(),
			("can_" + fun.getUnmangledName()).str(),
			ftype,
			mlir::rlc::FunctionInfoAttr::get(
					rewriter.getContext(), fun.getInfo().getArgs(), nullptr, nullptr),
			fun.getIsMemberFunction());
	rewriter.cloneRegionBefore(
			fun.getPrecondition(),
			validityFunction.getBody(),
			validityFunction.getBody().begin());

	auto& yieldedConditions = validityFunction.getBody().front().back();
	rewriter.setInsertionPoint(&yieldedConditions);

	mlir::Value lastOperand =
			rewriter.create<mlir::rlc::Constant>(fun.getLoc(), true);
	for (auto value : yieldedConditions.getOperands())
		lastOperand =
				rewriter.create<mlir::rlc::AndOp>(fun.getLoc(), lastOperand, value);
	rewriter.replaceOpWithNewOp<mlir::rlc::Yield>(
			&yieldedConditions, mlir::ValueRange({ lastOperand }));

	rewriter.setInsertionPoint(validityFunction);
	return rewriter.create<mlir::rlc::FunctionMetadataOp>(
			fun.getLoc(), fun.getResult(), validityFunction.getResult());
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_EXTRACTPRECONDITIONPASS
#include "rlc/dialect/Passes.inc"

	struct ExtractPreconditionPass
			: impl::ExtractPreconditionPassBase<ExtractPreconditionPass>
	{
		using impl::ExtractPreconditionPassBase<
				ExtractPreconditionPass>::ExtractPreconditionPassBase;

		void runOnOperation() override
		{
			auto range = getOperation().getOps<mlir::rlc::FunctionOp>();

			llvm::SmallVector<mlir::rlc::FunctionOp, 2> ops(
					range.begin(), range.end());

			for (auto function : ops)
			{
				auto metadata = emitPreconditionFunction(function);
				if (metadata)
				{
					for (auto* user : metadata.getSourceFunction().getUsers())
					{
						if (auto casted = mlir::dyn_cast<mlir::rlc::CanOp>(user))
							casted.replaceAllUsesWith(metadata.getPreconditionFunction());
					}
				}
			}

			llvm::SmallVector<mlir::rlc::CanOp, 2> canOps;
			getOperation()->walk(
					[&](mlir::rlc::CanOp canOp) { canOps.push_back(canOp); });
			for (auto canOp : canOps)
			{
				if (not canOp->getUses().empty())
				{	 // The CanOp's who still have users are those that referred to an
					 // empty precondition.
					mlir::OpBuilder builder(canOp);
					auto t = builder.create<mlir::rlc::Constant>(canOp->getLoc(), true);
					canOp.replaceAllUsesWith(t.getResult());
				}
				canOp->erase();
			}
		}
	};
}	 // namespace mlir::rlc

namespace mlir::rlc
{
#define GEN_PASS_DEF_STRIPFUNCTIONMETADATAPASS
#include "rlc/dialect/Passes.inc"

	struct StripFunctionMetadataPass
			: impl::StripFunctionMetadataPassBase<StripFunctionMetadataPass>
	{
		using impl::StripFunctionMetadataPassBase<
				StripFunctionMetadataPass>::StripFunctionMetadataPassBase;

		void runOnOperation() override
		{
			auto range = getOperation().getOps<mlir::rlc::FunctionMetadataOp>();

			llvm::SmallVector<mlir::rlc::FunctionMetadataOp, 2> ops(
					range.begin(), range.end());

			for (auto metadata : ops)
				metadata.erase();
		}
	};
}	 // namespace mlir::rlc

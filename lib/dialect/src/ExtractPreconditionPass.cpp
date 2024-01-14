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
			fun.getArgNamesAttr(),
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

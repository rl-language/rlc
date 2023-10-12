#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

static void emitPreconditionFunction(mlir::rlc::FunctionOp fun)
{
	mlir::IRRewriter rewriter(fun.getContext());
	rewriter.setInsertionPoint(fun);
	if (fun.getPrecondition().empty())
		return;

	auto ftype = mlir::FunctionType::get(
			fun.getContext(),
			fun.getFunctionType().getInputs(),
			{ mlir::rlc::BoolType::get(fun.getContext()) });
	auto validityFunction = rewriter.create<mlir::rlc::FunctionOp>(
			fun.getLoc(),
			("can_" + fun.getUnmangledName()).str(),
			ftype,
			fun.getArgNamesAttr());
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
	rewriter.create<mlir::rlc::FunctionMetadataOp>(
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
				emitPreconditionFunction(function);
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

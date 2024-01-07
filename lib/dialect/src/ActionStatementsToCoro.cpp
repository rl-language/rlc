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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

// insert a store of -1 to the resumption index after every yield and every
// return
static void insertTerminatorResumptionIndexStore(
		mlir::rlc::FlatFunctionOp fun,
		mlir::IRRewriter& rewriter,
		mlir::Value resumeIndex)
{
	llvm::SmallVector<mlir::Operation*> ops;
	for (auto& operation : fun.getBody().getOps())
		ops.push_back(&operation);

	for (auto* operation : ops)
	{
		if (not mlir::isa<mlir::rlc::ReturnStatement>(operation) and
				not mlir::isa<mlir::rlc::Yield>(operation))
			continue;

		rewriter.setInsertionPoint(operation);
		auto newResumeIndexValue = rewriter.create<mlir::rlc::Constant>(
				fun.getLoc(), static_cast<int64_t>(-1));

		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				fun.getLoc(), resumeIndex, newResumeIndexValue);
	}
}

static llvm::DenseMap<int64_t, mlir::Block*> splitActionBlocks(
		mlir::rlc::FlatFunctionOp fun,
		mlir::IRRewriter& rewriter,
		mlir::Value resumeIndex)
{
	llvm::DenseMap<int64_t, mlir::Block*> resumePoints;
	llvm::SmallVector<mlir::Block*, 4> blocksToAnalyze;

	for (auto& block : fun.getBlocks())
		blocksToAnalyze.push_back(&block);

	while (not blocksToAnalyze.empty())
	{
		auto& block = blocksToAnalyze.back();
		blocksToAnalyze.pop_back();

		for (auto iter = block->begin(); iter != block->end(); iter++)
		{
			auto casted = mlir::dyn_cast<mlir::rlc::ActionStatement>(*iter);
			if (casted == nullptr)
				continue;

			resumePoints[casted.getId()] =
					rewriter.splitBlock(block, std::next(iter));
			blocksToAnalyze.push_back(resumePoints[casted.getId()]);

			rewriter.setInsertionPoint(casted);

			auto newResumeIndexValue = rewriter.create<mlir::rlc::Constant>(
					casted.getLoc(), (int64_t) casted.getResumptionPoint());
			rewriter.create<mlir::rlc::BuiltinAssignOp>(
					casted.getLoc(), resumeIndex, newResumeIndexValue);
			rewriter.create<mlir::rlc::Yield>(casted.getLoc());
			rewriter.eraseOp(casted);

			break;
		}
	}

	return resumePoints;
}

// rewrite action so that the resume index is loaded from the coroutine frame
// and then jumps to the action with the given resume index
static mlir::LogicalResult actionsToBraches(mlir::rlc::FlatFunctionOp fun)
{
	mlir::IRRewriter rewriter(fun.getContext());

	if (fun.getBody().getOps<mlir::rlc::ActionStatement>().empty())
		return mlir::success();

	mlir::Block& entry = *fun.getBlocks().begin();
	auto* everythingElse = rewriter.splitBlock(&entry, entry.begin());

	rewriter.setInsertionPoint(&entry, entry.begin());
	auto routineIndex = rewriter.create<mlir::rlc::MemberAccess>(
			fun.getLoc(), entry.getArgument(0), 0);

	insertTerminatorResumptionIndexStore(fun, rewriter, routineIndex);
	auto resumePoints = splitActionBlocks(fun, rewriter, routineIndex);
	resumePoints[0] = everythingElse;
	llvm::SmallVector<mlir::Block*, 2> sortedResumePoints(resumePoints.size());
	for (auto& pair : resumePoints)
		sortedResumePoints[pair.first] = pair.second;

	rewriter.setInsertionPoint(&entry, entry.end());
	rewriter.create<mlir::rlc::SelectBranch>(
			fun.getLoc(), routineIndex, sortedResumePoints);
	return mlir::success();
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_ACTIONSTATEMENTSTOCOROPASS
#include "rlc/dialect/Passes.inc"

	struct ActionStatementsToCoroPass
			: impl::ActionStatementsToCoroPassBase<ActionStatementsToCoroPass>
	{
		using impl::ActionStatementsToCoroPassBase<
				ActionStatementsToCoroPass>::ActionStatementsToCoroPassBase;

		void runOnOperation() override
		{
			for (auto f : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
			{
				if (actionsToBraches(f).failed())
				{
					signalPassFailure();
					return;
				}
			}

			mlir::IRRewriter rewriter(getOperation().getContext());
			llvm::SmallVector<mlir::rlc::Yield, 4> yields;
			getOperation().walk(
					[&](mlir::rlc::Yield yield) { yields.push_back(yield); });
			for (auto yield : yields)
			{
				if (yield.getOnEnd().empty())
					continue;

				rewriter.setInsertionPoint(yield);
				size_t i = 0;
				for (auto arg : yield.getOperands())
				{
					auto copy = rewriter.create<mlir::rlc::CopyOp>(
							yield.getLoc(), arg.getType(), arg);
					yield.setOperand(i, copy);
					i++;
				}

				llvm::SmallVector<mlir::Operation*, 2> ops;

				for (auto& op : llvm::drop_end(yield.getOnEnd().front()))
					ops.push_back(&op);

				for (auto* op : ops)
					op->moveBefore(yield);

				while (not yield.getOnEnd().empty())
					rewriter.eraseBlock(&yield.getOnEnd().front());
			}
		}
	};
}	 // namespace mlir::rlc

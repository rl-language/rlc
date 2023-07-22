#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static llvm::SmallVector<mlir::Block*, 4> splitActionBlocks(
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

	llvm::SmallVector<mlir::Block*, 4> resumePoints;
	llvm::SmallVector<mlir::Block*, 4> blocksToAnalyze;

	for (auto& block : fun.getBlocks())
		blocksToAnalyze.push_back(&block);

	int64_t newResumeIndex = 1;
	while (not blocksToAnalyze.empty())
	{
		auto& block = blocksToAnalyze.back();
		blocksToAnalyze.pop_back();

		for (auto iter = block->begin(); iter != block->end(); iter++)
		{
			auto casted = mlir::dyn_cast<mlir::rlc::ActionStatement>(*iter);
			if (casted == nullptr)
				continue;

			resumePoints.push_back(rewriter.splitBlock(block, std::next(iter)));
			blocksToAnalyze.push_back(resumePoints.back());

			rewriter.setInsertionPoint(casted);

			auto newResumeIndexValue =
					rewriter.create<mlir::rlc::Constant>(casted.getLoc(), newResumeIndex);
			newResumeIndex++;
			rewriter.create<mlir::rlc::BuiltinAssignOp>(
					casted.getLoc(), resumeIndex, newResumeIndexValue);
			rewriter.create<mlir::rlc::Yield>(casted.getLoc());
			rewriter.eraseOp(casted);

			break;
		}
	}

	return resumePoints;
}

static mlir::LogicalResult actionsToBraches(mlir::rlc::FlatFunctionOp fun)
{
	mlir::IRRewriter rewriter(fun.getContext());

	if (fun.getBody().getOps<mlir::rlc::ActionStatement>().empty())
		return mlir::success();

	mlir::IRRewriter builder(fun.getContext());
	for (auto op : fun.getBody().getOps<mlir::rlc::ActionStatement>())
	{
		for (const auto& [res, name] :
				 llvm::zip(op.getResults(), op.getDeclaredNames()))
		{
			llvm::SmallVector<mlir::OpOperand*, 4> uses;
			for (auto& use : res.getUses())
			{
				uses.push_back(&use);
			}
			for (auto* use : uses)
			{
				rewriter.setInsertionPoint(use->getOwner());
				auto frame = fun.getBlocks().front().getArgument(0);
				auto entityType = frame.getType().cast<mlir::rlc::EntityType>();

				size_t fieldIndex = std::distance(
						entityType.getFieldNames().begin(),
						llvm::find(
								entityType.getFieldNames(),
								name.cast<mlir::StringAttr>().str()));
				auto ref = rewriter.create<mlir::rlc::MemberAccess>(
						op.getLoc(), frame, fieldIndex);

				use->set(ref);
			}
		}
	}

	mlir::Block& entry = *fun.getBlocks().begin();
	auto* everythingElse = rewriter.splitBlock(&entry, entry.begin());

	rewriter.setInsertionPoint(&entry, entry.begin());
	auto routineIndex = rewriter.create<mlir::rlc::MemberAccess>(
			fun.getLoc(), entry.getArgument(0), 0);

	auto resumePoints = splitActionBlocks(fun, rewriter, routineIndex);
	resumePoints.insert(resumePoints.begin(), everythingElse);

	rewriter.setInsertionPoint(&entry, entry.end());
	rewriter.create<mlir::rlc::SelectBranch>(
			fun.getLoc(), routineIndex, resumePoints);
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

				for (auto& op : yield.getOnEnd().getOps())
					ops.push_back(&op);

				for (auto* op : ops)
					op->moveBefore(yield);

				while (not yield.getOnEnd().empty())
					rewriter.eraseBlock(&yield.getOnEnd().front());
			}
		}
	};
}	 // namespace mlir::rlc

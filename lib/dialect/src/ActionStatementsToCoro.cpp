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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/IRMapping.h"
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

// pattern matches the pattern
// resume_index = member_access(state, 0)
// store resume_index, current
// resume_index = member_access(state, 0)
// switch(resume_index)
static void removeSwitch(
		mlir::IRRewriter& rewriter, mlir::rlc::FlatFunctionOp funOp)
{
	assert(
			std::distance(
					funOp.getBody().getOps<mlir::rlc::SelectBranch>().begin(),
					funOp.getBody().getOps<mlir::rlc::SelectBranch>().end()) == 1);
	auto switchOp = *funOp.getBody().getOps<mlir::rlc::SelectBranch>().begin();
	if (std::distance(
					switchOp->getBlock()
							->getOps<mlir::rlc::CurrentActionResumeIndex>()
							.begin(),
					switchOp->getBlock()
							->getOps<mlir::rlc::CurrentActionResumeIndex>()
							.end()) != 1)
	{
		funOp.dump();
		abort();
	}
	auto index = *switchOp->getBlock()
										->getOps<mlir::rlc::CurrentActionResumeIndex>()
										.begin();

	auto* realSucessor = switchOp.getSuccessor(index.getIndex());

	rewriter.setInsertionPoint(switchOp);
	rewriter.replaceOpWithNewOp<mlir::rlc::Branch>(switchOp, realSucessor);
	mlir::rlc::pruneUnrechableBlocks(funOp.getBody(), rewriter);
	index.erase();
}

static void dropUselessDestructors(
		mlir::IRRewriter& rewriter, mlir::rlc::FlatFunctionOp funOp)
{
	llvm::SmallVector<mlir::rlc::CallOp, 4> ops;
	for (auto call : funOp.getBody().getOps<mlir::rlc::CallOp>())
		ops.push_back(call);

	for (auto op : ops)
	{
		for (auto operand : op.getArgOperands())
		{
			if (operand != nullptr)
				continue;

			op.erase();
			break;
		}
	}
}

static void inlineInCallers(
		mlir::rlc::FlatFunctionOp fun,
		llvm::ArrayRef<mlir::rlc::Reference> references)
{
	mlir::IRRewriter rewriter(fun.getContext());

	assert(
			std::distance(
					fun.getResult().getUses().begin(), fun.getResult().getUses().end()) ==
			0);

	for (auto referenceToGlobal : references)
	{
		mlir::IRMapping mapping;
		rewriter.cloneRegionBefore(
				fun.getBody(),
				*referenceToGlobal->getParentRegion(),
				referenceToGlobal->getParentRegion()->end(),
				mapping);

		assert(
				std::distance(
						referenceToGlobal.getResult().getUses().begin(),
						referenceToGlobal.getResult().getUses().end()) == 1);
		auto call =
				mlir::cast<mlir::rlc::CallOp>(*referenceToGlobal->getUsers().begin());
		assert(call.getResults().size() == 0);

		auto* inlinedEntry = mapping.getBlockMap().at(&fun.getBody().front());

		for (auto [blockArgument, value] :
				 llvm::zip(inlinedEntry->getArguments(), call.getArgOperands()))
			blockArgument.replaceAllUsesWith(value);

		inlinedEntry->eraseArguments(0, inlinedEntry->getNumArguments());

		auto currentBlock = call->getBlock();
		// split the block after the call, so we can jump to the inlined body and
		// then get back when the function is done by jumping into afterBlock
		auto iteratorAfterCall = llvm::find_if(
				call->getBlock()->getOperations(),
				[&](mlir::Operation& op) { return &op == call.getOperation(); });
		iteratorAfterCall++;
		auto* afterBlock = rewriter.splitBlock(currentBlock, iteratorAfterCall);
		rewriter.setInsertionPoint(call);
		call.erase();
		rewriter.mergeBlocks(inlinedEntry, currentBlock);

		fun.walk([&](mlir::rlc::Yield yield) {
			auto otherYield = mapping.getOperationMap().at(yield);
			rewriter.setInsertionPoint(otherYield);
			rewriter.replaceOpWithNewOp<mlir::rlc::Branch>(otherYield, afterBlock);
		});

		auto caller =
				mlir::cast<mlir::rlc::FlatFunctionOp>(referenceToGlobal->getParentOp());
		referenceToGlobal.erase();
		removeSwitch(rewriter, caller);
		dropUselessDestructors(rewriter, caller);
	}
	fun.erase();
}

// rewrite action so that the resume index is loaded from the coroutine frame
// and then jumps to the action with the given resume index
static mlir::LogicalResult actionsToBraches(mlir::rlc::FlatFunctionOp fun)
{
	mlir::IRRewriter rewriter(fun.getContext());

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
			llvm::SmallVector<mlir::rlc::FlatFunctionOp, 4> ops;
			for (auto f : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
				if (not f.getBody().getOps<mlir::rlc::ActionStatement>().empty())
					ops.push_back(f);

			for (auto f : ops)
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
			// at this point the ir is fully flat, except yields that return values,
			// which include the destructors invocations of variables.
			// Therefore we move the destructor invocation before the yield, and if
			// one of those variables was to be returned, we emit the copy first
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

			llvm::StringMap<llvm::SmallVector<mlir::rlc::Reference, 4>> references;
			getOperation().walk([&](mlir::rlc::Reference ref) {
				references[ref.getReferred()].push_back(ref);
			});

			for (auto f : ops)
				inlineInCallers(f, references[f.getMangledName()]);
		}
	};
}	 // namespace mlir::rlc

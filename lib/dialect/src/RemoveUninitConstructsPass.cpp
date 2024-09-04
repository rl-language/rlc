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
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_REMOVEUNINITCONSTRUCTSPASS
#include "rlc/dialect/Passes.inc"

	static bool handleCondBranchTrueBranch(
			mlir::rlc::CondBranch branch,
			mlir::rlc::UninitializedConstruct uninitConstruct,
			mlir::Block& block,
			mlir::rlc::BuiltinAssignOp assign)
	{
		auto trueSuccessor = branch->getSuccessors()[0];
		if (trueSuccessor->getOperations().size() != 1)
			return false;
		auto yield =
				mlir::dyn_cast<mlir::rlc::Yield>(trueSuccessor->getTerminator());
		if (not yield or not yield.getOnEnd().empty() or
				yield.getArguments()[0] != uninitConstruct)
			return false;

		mlir::IRRewriter rewriter(branch.getContext());
		rewriter.setInsertionPointToEnd(&block);
		auto bb = rewriter.createBlock(block.getParent());
		branch.setSuccessor(bb, 0);

		auto constant = rewriter.create<mlir::rlc::Constant>(yield.getLoc(), true);
		rewriter.create<mlir::rlc::Yield>(
				yield.getLoc(), mlir::ValueRange({ constant }));
		return true;
	}

	static bool handleCondBranchFalseBranch(
			mlir::rlc::CondBranch branch,
			mlir::rlc::UninitializedConstruct uninitConstruct,
			mlir::Block& block,
			mlir::rlc::BuiltinAssignOp assign)
	{
		auto falseSuccessor = branch->getSuccessors()[1];
		if (falseSuccessor->getOperations().size() != 1)
			return false;
		auto yield =
				mlir::dyn_cast<mlir::rlc::Yield>(falseSuccessor->getTerminator());
		if (not yield or not yield.getOnEnd().empty() or
				yield.getArguments()[0] != uninitConstruct)
			return false;

		mlir::IRRewriter rewriter(branch.getContext());
		rewriter.setInsertionPointToEnd(&block);
		auto bb = rewriter.createBlock(block.getParent());
		branch.setSuccessor(bb, 1);

		auto constant = rewriter.create<mlir::rlc::Constant>(yield.getLoc(), false);
		rewriter.create<mlir::rlc::Yield>(
				yield.getLoc(), mlir::ValueRange({ constant }));
		return true;
	}

	static bool removeUninitConstructs(mlir::Block& block)
	{
		const auto opsCount = block.getOperations().size();
		if (opsCount <= 1)
			return false;
		auto assign = mlir::dyn_cast<mlir::rlc::BuiltinAssignOp>(
				*(std::next(block.getOperations().begin(), (opsCount - 2))));

		if (not assign)
			return false;
		if (assign.getLhs().getDefiningOp() == nullptr)
			return false;

		auto uninitConstruct = mlir::dyn_cast<mlir::rlc::UninitializedConstruct>(
				assign.getLhs().getDefiningOp());

		if (not uninitConstruct)
			return false;

		if (auto branch = mlir::dyn_cast<mlir::rlc::Branch>(block.getTerminator());
				branch and branch->getSuccessors()[0]->getOperations().size() == 1)
		{
			auto successor = branch->getSuccessors()[0];
			auto yield = mlir::dyn_cast<mlir::rlc::Yield>(successor->getTerminator());
			if (not yield or not yield.getOnEnd().empty() or
					yield.getArguments()[0] != uninitConstruct)
				return false;
			mlir::IRRewriter rewriter(branch.getContext());
			rewriter.setInsertionPointToEnd(&block);
			rewriter.create<mlir::rlc::Yield>(
					yield.getLoc(), mlir::ValueRange(assign.getRhs()));
			assign.erase();
			branch.erase();
			rewriter.setInsertionPointToEnd(&block);
			return true;
		}

		if (auto branch =
						mlir::dyn_cast<mlir::rlc::CondBranch>(block.getTerminator()))
		{
			bool result = false;
			if (handleCondBranchTrueBranch(branch, uninitConstruct, block, assign))
				result = true;
			if (handleCondBranchFalseBranch(branch, uninitConstruct, block, assign))
				result = true;
			return result;
		}

		return false;
	}

	static void removeUninitConstructs(mlir::rlc::FlatFunctionOp op)
	{
		if (op.getFunctionType().getResults().empty())
			return;
		if (not op.getFunctionType().getResults()[0].isa<mlir::rlc::BoolType>())
			return;

		bool changed = true;
		while (changed)
		{
			changed = false;
			llvm::SmallVector<mlir::Block*, 4> frontier;
			for (auto& block : op.getBlocks())
				frontier.push_back(&block);

			for (auto* block : frontier)
			{
				if (block->getPredecessors().empty() and block != &op.getBody().front())
				{
					block->erase();
					continue;
				}

				if (removeUninitConstructs(*block))
				{
					block->dump();
					changed = true;
				}
			}
		}
	}

	struct RemoveUninitConstructPass
			: impl::RemoveUninitConstructsPassBase<RemoveUninitConstructPass>
	{
		using impl::RemoveUninitConstructsPassBase<
				RemoveUninitConstructPass>::RemoveUninitConstructsPassBase;

		void runOnOperation() override
		{
			for (auto op :
					 getOperation().getBodyRegion().getOps<mlir::rlc::FlatFunctionOp>())
				removeUninitConstructs(op);
		}
	};
}	 // namespace mlir::rlc

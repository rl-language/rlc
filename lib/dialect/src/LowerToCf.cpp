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
#include "rlc/dialect/conversion/TypeConverter.h"

static void mergeYieldsIntoSplittedBlock(
		llvm::MutableArrayRef<mlir::rlc::Yield> yields,
		mlir::Block* successor,
		mlir::IRRewriter& rewriter)
{
	if (yields.empty())
	{
		rewriter.eraseBlock(successor);
		return;
	}

	rewriter.eraseOp(&*successor->begin());
	if (yields.size() == 1)
	{
		rewriter.setInsertionPoint(yields.front());
		llvm::SmallVector<mlir::Operation*> toMove;
		for (auto& child : yields.front().getOps())
			toMove.push_back(&child);
		for (auto& child : toMove)
			if (not mlir::isa<mlir::rlc::Yield>(child))
				child->moveBefore(yields.front());
		rewriter.mergeBlocks(
				successor, yields.front()->getBlock(), mlir::ValueRange());
		yields.front().erase();
		return;
	}

	for (mlir::rlc::Yield yield : yields)
	{
		rewriter.setInsertionPoint(yield);
		llvm::SmallVector<mlir::Operation*> toMove;
		for (auto& child : yield.getOps())
			if (not mlir::isa<mlir::rlc::Yield>(child))
				toMove.push_back(&child);
		for (auto* op : toMove)
			op->moveBefore(yield);
		rewriter.create<mlir::rlc::Branch>(yield.getLoc(), successor);
		rewriter.eraseOp(yield);
	}
}

static llvm::SmallVector<mlir::rlc::Yield, 2> getYieldTerminators(auto& op)
{
	llvm::SmallVector<mlir::rlc::Yield, 2> terminators;
	for (auto& block : op.getBody())
	{
		for (auto& entry : block)
		{
			if (auto yield = mlir::dyn_cast<mlir::rlc::Yield>(entry))
				terminators.push_back(yield);
		}
	}
	return terminators;
}

static llvm::SmallVector<mlir::rlc::Yield, 2> getYieldTerminators(
		mlir::Region& region)
{
	llvm::SmallVector<mlir::rlc::Yield, 2> terminators;
	for (auto& block : region)
	{
		for (auto& entry : block)
		{
			if (auto yield = mlir::dyn_cast<mlir::rlc::Yield>(entry))
				terminators.push_back(yield);
		}
	}
	return terminators;
}

static void pruneUnrechableBlocks(mlir::Region& op, mlir::IRRewriter& rewriter)
{
	bool foundOne = true;

	while (foundOne)
	{
		foundOne = false;
		llvm::SmallVector<mlir::Block*, 3> blocks;
		for (auto& block : op)
			blocks.push_back(&block);

		for (auto& block : blocks)
		{
			if (block->hasNoPredecessors() and not block->isEntryBlock())
			{
				rewriter.eraseBlock(block);
				foundOne = true;
			}
		}
	}
}

static void eraseYield(
		mlir::IRRewriter& rewriter,
		mlir::rlc::Yield yield,
		mlir::Operation* newTerminator)
{
	assert(yield.getOnEnd().getBlocks().size() <= 1);
	if (yield.getOnEnd().empty())
	{
		yield.erase();
		return;
	}

	yield.getOnEnd().front().getTerminator()->erase();
	rewriter.inlineBlockBefore(&yield.getOnEnd().front(), newTerminator);
	yield.erase();
}

static mlir::LogicalResult flatten(
		mlir::rlc::WhileStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto condTerminators = getYieldTerminators(op.getCondition());
	auto bodyTerminators = getYieldTerminators(op.getBody());

	auto* previousBlock = rewriter.getBlock();
	auto* afterBlock =
			rewriter.splitBlock(previousBlock, rewriter.getInsertionPoint());
	auto& conditionBlock = op.getCondition().front();
	auto& bodyBlock = op.getBody().front();

	rewriter.inlineRegionBefore(op.getCondition(), afterBlock);

	rewriter.inlineRegionBefore(op.getBody(), afterBlock);

	rewriter.setInsertionPoint(previousBlock, previousBlock->end());
	rewriter.create<mlir::rlc::Branch>(op.getLoc(), &conditionBlock);

	for (auto yield : condTerminators)
	{
		rewriter.setInsertionPoint(yield);
		auto newTerminator = rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), &bodyBlock, afterBlock);
		eraseYield(rewriter, yield, newTerminator);
	}

	for (auto yield : bodyTerminators)
	{
		rewriter.setInsertionPoint(yield);
		auto newTerminator =
				rewriter.create<mlir::rlc::Branch>(yield.getLoc(), &conditionBlock);
		eraseYield(rewriter, yield, newTerminator);
	}

	rewriter.eraseOp(op);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::ActionsStatement op, mlir::IRRewriter& rewriter)
{
	auto parent = op->getParentOfType<mlir::rlc::FunctionOp>();
	rewriter.setInsertionPoint(op);
	auto location = op->getLoc();
	llvm::SmallVector<mlir::rlc::Yield, 2> actionsTerminators;
	llvm::SmallVector<mlir::Block*> actionsBlocks;

	for (auto& region : op.getActions())
	{
		auto terminatos = getYieldTerminators(region);
		for (auto terminator : terminatos)
			actionsTerminators.emplace_back(terminator);
		actionsBlocks.emplace_back(&region.front());
	}

	auto* previousBlock = rewriter.getBlock();
	auto* afterBlock =
			rewriter.splitBlock(previousBlock, rewriter.getInsertionPoint());

	for (auto& region : op.getActions())
		rewriter.inlineRegionBefore(region, afterBlock);

	mergeYieldsIntoSplittedBlock(actionsTerminators, afterBlock, rewriter);

	rewriter.setInsertionPointToEnd(previousBlock);
	rewriter.create<mlir::rlc::FlatActionStatement>(location, actionsBlocks);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::ShortCircuitingOr op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto lhsTerminators = getYieldTerminators(op.getLhs());
	auto rhsTerminators = getYieldTerminators(op.getRhs());

	auto& lhsBlock = op.getLhs().front();
	auto& rhsBlock = op.getRhs().front();
	auto* previousBlock = rewriter.getBlock();
	auto* afterBlock =
			rewriter.splitBlock(previousBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getLhs(), rewriter.getBlock());
	rewriter.inlineRegionBefore(op.getRhs(), afterBlock);

	rewriter.setInsertionPointToStart(&lhsBlock);
	auto boolToReturn = rewriter.create<mlir::rlc::UninitializedConstruct>(
			op.getLoc(), mlir::rlc::BoolType::get(rewriter.getContext()));

	rewriter.mergeBlocks(&lhsBlock, previousBlock, mlir::ValueRange());

	// redirect all rlhs yields to jump to either the exit or the rhs
	for (auto yield : lhsTerminators)
	{
		rewriter.setInsertionPoint(yield);
		// store the computed result of the right and into the actual result
		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				op.getLoc(), boolToReturn, yield.getArguments()[0]);
		auto newTerminator = rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), afterBlock, &rhsBlock);
		eraseYield(rewriter, yield, newTerminator);
	}

	// redirect all rhs yields to jump to the exit
	for (auto yield : rhsTerminators)
	{
		rewriter.setInsertionPoint(yield);
		// store the computed result of the right and into the actual result
		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				op.getLoc(), boolToReturn, yield.getArguments()[0]);
		auto newTerminator =
				rewriter.create<mlir::rlc::Branch>(yield.getLoc(), afterBlock);
		eraseYield(rewriter, yield, newTerminator);
	}
	auto* parent = op->getParentOp();
	op.replaceAllUsesWith(boolToReturn.getResult());
	op.erase();

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::ShortCircuitingAnd op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto lhsTerminators = getYieldTerminators(op.getLhs());
	auto rhsTerminators = getYieldTerminators(op.getRhs());

	auto& lhsBlock = op.getLhs().front();
	auto& rhsBlock = op.getRhs().front();
	auto* previousBlock = rewriter.getBlock();
	auto* afterBlock =
			rewriter.splitBlock(previousBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getLhs(), rewriter.getBlock());
	rewriter.inlineRegionBefore(op.getRhs(), afterBlock);

	rewriter.setInsertionPointToStart(&lhsBlock);
	auto boolToReturn = rewriter.create<mlir::rlc::UninitializedConstruct>(
			op.getLoc(), mlir::rlc::BoolType::get(rewriter.getContext()));

	rewriter.mergeBlocks(&lhsBlock, previousBlock, mlir::ValueRange());

	// redirect all rlhs yields to jump to either the exit or the rhs
	for (auto yield : lhsTerminators)
	{
		rewriter.setInsertionPoint(yield);
		// store the computed result of the right and into the actual result
		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				op.getLoc(), boolToReturn, yield.getArguments()[0]);
		auto newTerminator = rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), &rhsBlock, afterBlock);
		eraseYield(rewriter, yield, newTerminator);
	}

	// redirect all rhs yields to jump to the exit
	for (auto yield : rhsTerminators)
	{
		rewriter.setInsertionPoint(yield);
		// store the computed result of the right and into the actual result
		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				op.getLoc(), boolToReturn, yield.getArguments()[0]);
		auto newTerminator =
				rewriter.create<mlir::rlc::Branch>(yield.getLoc(), afterBlock);
		eraseYield(rewriter, yield, newTerminator);
	}
	auto* parent = op->getParentOp();
	op.replaceAllUsesWith(boolToReturn.getResult());
	op.erase();

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::IfStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto condTerminators = getYieldTerminators(op.getCondition());
	auto trueTerminators = getYieldTerminators(op.getTrueBranch());
	auto falseTerminators = getYieldTerminators(op.getElseBranch());

	auto& conditionBlock = op.getCondition().front();
	auto& trueBlock = op.getTrueBranch().front();
	auto& falseBlock = op.getElseBranch().front();
	auto* previousBlock = rewriter.getBlock();
	auto* afterBlock =
			rewriter.splitBlock(previousBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getCondition(), rewriter.getBlock());

	rewriter.inlineRegionBefore(op.getElseBranch(), afterBlock);
	rewriter.inlineRegionBefore(op.getTrueBranch(), afterBlock);

	rewriter.mergeBlocks(&conditionBlock, previousBlock, mlir::ValueRange());

	for (auto yield : condTerminators)
	{
		rewriter.setInsertionPoint(yield);
		auto newTerminator = rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), &trueBlock, &falseBlock);
		eraseYield(rewriter, yield, newTerminator);
	}

	llvm::copy(falseTerminators, std::back_inserter(trueTerminators));
	mergeYieldsIntoSplittedBlock(trueTerminators, afterBlock, rewriter);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::StatementList op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto terminators = getYieldTerminators(op);

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();
	auto* after = rewriter.splitBlock(parentBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());

	mergeYieldsIntoSplittedBlock(terminators, after, rewriter);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::DeclarationStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto terminators = getYieldTerminators(op);
	assert(terminators.size() == 1);
	auto terminator = terminators.front();

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();
	auto* after = rewriter.splitBlock(parentBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());

	auto alloca = terminator.getArguments();
	rewriter.replaceOp(op, { alloca });

	rewriter.mergeBlocks(after, terminator->getBlock(), mlir::ValueRange());
	eraseYield(rewriter, terminator, terminator->getNextNode());

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::ReturnStatement op, mlir::IRRewriter& rewriter)
{
	if (op.getBody().empty())
	{
		assert(op.getResult() == mlir::rlc::VoidType::get(op.getContext()));
		rewriter.replaceOpWithNewOp<mlir::LLVM::ReturnOp>(op, mlir::ValueRange());
		return mlir::LogicalResult::success();
	}
	rewriter.setInsertionPoint(op);

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());
	rewriter.eraseOp(op);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::ExpressionStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto terminators = getYieldTerminators(op);

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();
	auto* after = rewriter.splitBlock(parentBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());

	mergeYieldsIntoSplittedBlock(terminators, after, rewriter);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult squashCF(
		mlir::rlc::FunctionOp& op, mlir::IRRewriter& rewriter)
{
	bool foundOne = true;
	auto dispatch = [&]<typename T>() -> mlir::LogicalResult {
		for (auto child : op.getBody().getOps<T>())
		{
			foundOne = true;
			return flatten(child, rewriter);
		}
		return mlir::LogicalResult::success();
	};

	while (foundOne)
	{
		foundOne = false;
		if (auto res = dispatch.operator()<mlir::rlc::ReturnStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::IfStatement>(); res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::StatementList>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::ExpressionStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::DeclarationStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::WhileStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::ActionsStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::ShortCircuitingAnd>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::ShortCircuitingOr>();
				res.failed())
			return res;
	}
	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flattenModule(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 2> ops(
			op.getOps<mlir::rlc::FunctionOp>());

	assert(op.verify().succeeded());
	for (auto f : ops)
	{
		assert(f.verify().succeeded());
		if (auto res = squashCF(f, rewriter); res.failed())
			return res;

		rewriter.setInsertionPoint(f);
		auto newF = rewriter.create<mlir::rlc::FlatFunctionOp>(
				op.getLoc(),
				f.getFunctionType(),
				f.getUnmangledName(),
				f.getArgNamesAttr(),
				f.getIsMemberFunction());

		rewriter.cloneRegionBefore(
				f.getBody(), newF.getBody(), newF.getBody().begin());

		pruneUnrechableBlocks(newF.getBody(), rewriter);

		rewriter.setInsertionPoint(f);
		llvm::SmallVector<mlir::OpOperand*> operands;
		for (auto& use : f.getResult().getUses())
			operands.push_back(&use);

		for (auto& use : operands)
		{
			if (mlir::isa<mlir::rlc::FunctionMetadataOp>(use->getOwner()))
			{
				use->set(newF);
			}
			else
			{
				rewriter.setInsertionPoint(use->getOwner());

				auto ref = rewriter.create<mlir::rlc::Reference>(
						use->getOwner()->getLoc(), f.getFunctionType(), f.getMangledName());
				use->set(ref);
			}
		}

		rewriter.eraseOp(f);
	}

	return mlir::LogicalResult::success();
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_LOWERTOCFPASS
#include "rlc/dialect/Passes.inc"

	struct LowerToCfPass: impl::LowerToCfPassBase<LowerToCfPass>
	{
		using impl::LowerToCfPassBase<LowerToCfPass>::LowerToCfPassBase;

		void runOnOperation() override
		{
			if (flattenModule(getOperation()).failed())
				signalPassFailure();
		}
	};
}	 // namespace mlir::rlc

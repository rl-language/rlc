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
		rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), &bodyBlock, afterBlock);
		rewriter.eraseOp(yield);
	}

	for (auto yield : bodyTerminators)
	{
		rewriter.setInsertionPoint(yield);
		rewriter.replaceOpWithNewOp<mlir::rlc::Branch>(yield, &conditionBlock);
	}

	rewriter.eraseOp(op);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::ActionsStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
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
	rewriter.create<mlir::rlc::FlatActionStatement>(op.getLoc(), actionsBlocks);

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
		rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), &trueBlock, &falseBlock);
		rewriter.eraseOp(yield);
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

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();
	auto* after = rewriter.splitBlock(parentBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());

	auto alloca = terminators.front().getArguments();
	rewriter.replaceOp(op, { alloca });
	for (auto& terminator : terminators)
	{
		rewriter.mergeBlocks(after, terminator->getBlock(), mlir::ValueRange());
		rewriter.eraseOp(terminator);
	}

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
	}
	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flattenModule(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 2> ops(
			op.getOps<mlir::rlc::FunctionOp>());

	for (auto f : ops)
	{
		rewriter.setInsertionPoint(f);
		llvm::SmallVector<mlir::OpOperand*> operands;
		for (auto& use : f.getResult().getUses())
			operands.push_back(&use);

		for (auto& use : operands)
		{
			rewriter.setInsertionPoint(use->getOwner());
			auto ref = rewriter.create<mlir::rlc::Reference>(
					use->getOwner()->getLoc(), f.getFunctionType(), f.getMangledName());
			use->set(ref);
		}
	}
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
				f.getArgNamesAttr());

		rewriter.cloneRegionBefore(
				f.getBody(), newF.getBody(), newF.getBody().begin());

		rewriter.eraseOp(f);

		pruneUnrechableBlocks(newF.getBody(), rewriter);
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

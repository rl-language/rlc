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

namespace mlir::rlc
{

#define GEN_PASS_DEF_LOWERINITIALIZERLISTSPASS
#include "rlc/dialect/Passes.inc"

	static bool canBeTurnedIntoAGlobal(mlir::rlc::InitializerListOp list)
	{
		// can be turned into a global if everything inside is a constant except for
		// the yield, and the yield only refers to expressions inside the
		// initializer list
		bool isComposedOfConstantExpressions =
				llvm::all_of(list.getOps(), [](mlir::Operation& op) {
					if (mlir::isa<mlir::rlc::Constant>(op) or
							mlir::isa<mlir::rlc::Yield>(op))
						return true;
					if (auto casted = mlir::dyn_cast<mlir::rlc::InitializerListOp>(op))
						return true;
					return false;
				});

		if (not isComposedOfConstantExpressions)
			return false;

		bool yieldOnlyRefersToCostants =
				mlir::cast<mlir::rlc::Yield>(list.getBody().front().getTerminator())
						.getArguments()
						.size() == (list.getBody().front().getOperations().size() - 1);

		return yieldOnlyRefersToCostants;
	}

	static mlir::Value rewriteAsInitialization(
			mlir::rlc::InitializerListOp initializerList, mlir::IRRewriter& rewriter)
	{
		auto variable = rewriter.create<mlir::rlc::UninitializedConstruct>(
				initializerList.getLoc(), initializerList.getResult().getType());

		auto yield = mlir::cast<mlir::rlc::Yield>(
				initializerList.getBody().front().getTerminator());
		while (initializerList.getBody().front().getOperations().size() != 1)
		{
			auto& op = initializerList.getBody().front().front();
			op.moveBefore(variable);
		}
		for (int64_t i = 0; i < static_cast<int64_t>(yield.getArguments().size());
				 i++)
		{
			auto index =
					rewriter.create<mlir::rlc::Constant>(initializerList.getLoc(), i);
			auto member = rewriter.create<mlir::rlc::ArrayAccess>(
					initializerList.getLoc(), variable, index);
			rewriter.create<mlir::rlc::AssignOp>(
					initializerList.getLoc(), member, yield.getArguments()[i]);
		}
		return variable;
	}

	static mlir::rlc::ConstantGlobalArrayOp rewriteAsGlobal(
			llvm::StringRef name,
			mlir::rlc::InitializerListOp op,
			mlir::IRRewriter& rewriter)
	{
		rewriter.setInsertionPoint(op);
		auto global = rewriter.create<mlir::rlc::ConstantGlobalArrayOp>(
				op.getLoc(), op.getResult().getType(), name);

		global.getBody().takeBody(op.getBody());
		global.getBody().front().getTerminator()->erase();

		llvm::SmallVector<mlir::rlc::InitializerListOp> ops;
		for (auto op :
				 global.getBody().front().getOps<mlir::rlc::InitializerListOp>())
		{
			ops.push_back(op);
		}
		for (auto op : ops)
		{
			rewriteAsGlobal("", op, rewriter);
		}

		for (auto& use : op.getResult().getUses())
		{
			rewriter.setInsertionPoint(use.getOwner());
			use.assign(rewriter.create<mlir::rlc::Reference>(
					op.getLoc(), global.getResult(), global.getName()));
		}

		op.erase();
		return global;
	}

	struct LowerInitializerListsPass
			: impl::LowerInitializerListsPassBase<LowerInitializerListsPass>
	{
		using impl::LowerInitializerListsPassBase<
				LowerInitializerListsPass>::LowerInitializerListsPassBase;

		void runOnOperation() override
		{
			llvm::SmallVector<mlir::rlc::InitializerListOp, 4> toGlobals;
			llvm::SmallVector<mlir::rlc::InitializerListOp, 4> initializerLists;
			mlir::IRRewriter rewriter(&getContext());
			size_t emittedGlobals = 0;

			getOperation().walk<WalkOrder::PreOrder>(
					[&](mlir::rlc::InitializerListOp initializer) {
						if (canBeTurnedIntoAGlobal(initializer))
						{
							toGlobals.push_back(initializer);
							return WalkResult::skip();
						}
						initializerLists.push_back(initializer);
						return WalkResult::interrupt();
					});

			for (auto op : toGlobals)
			{
				auto global = rewriteAsGlobal(
						("constant_array_" + llvm::Twine(emittedGlobals)).str(),
						op,
						rewriter);
				global->moveBefore(
						getOperation().getBody(), getOperation().getBody()->begin());
				emittedGlobals++;
			}

			for (auto op :
					 llvm::make_range(initializerLists.rbegin(), initializerLists.rend()))
			{
				rewriter.setInsertionPoint(op);
				rewriter.replaceOp(op, rewriteAsInitialization(op, rewriter));
			}
		}
	};
}	 // namespace mlir::rlc

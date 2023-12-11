#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
	static mlir::LogicalResult rewriteSubAction(
			mlir::rlc::SubActionStatement action,
			const llvm::StringMap<mlir::rlc::ActionFunction>& nameToAction)
	{
		if (not nameToAction.contains(action.getName()))
			return mlir::failure();

		auto toCall = nameToAction.at(action.getName());

		llvm::SmallVector<mlir::rlc::ActionStatement, 2> actionStatements;
		toCall.walk([&](mlir::rlc::ActionStatement action) {
			actionStatements.push_back(action);
		});

		mlir::IRRewriter rewiter(action.getContext());
		rewiter.setInsertionPointAfter(action);

		auto frameVar = rewiter.create<mlir::rlc::DeclarationStatement>(
				action.getLoc(),
				mlir::rlc::UnknownType::get(action.getContext()),
				action.getFrameName());

		rewiter.createBlock(&frameVar.getBody());

		auto ref = rewiter.create<mlir::rlc::UnresolvedReference>(
				action.getLoc(), action.getName());

		auto frame = rewiter.create<mlir::rlc::CallOp>(
				action.getLoc(),
				mlir::rlc::UnknownType::get(action.getContext()),
				ref,
				action.getArgs());

		rewiter.create<mlir::rlc::Yield>(
				action.getLoc(), mlir::ValueRange({ frame.getResult(0) }));

		rewiter.setInsertionPointAfter(frameVar);
		auto loop = rewiter.create<mlir::rlc::WhileStatement>(frame.getLoc());
		rewiter.createBlock(&loop.getCondition());

		auto isDoneRef = rewiter.create<mlir::rlc::UnresolvedReference>(
				action.getLoc(), "is_done");

		auto isDone = rewiter.create<mlir::rlc::CallOp>(
				action.getLoc(),
				mlir::rlc::UnknownType::get(action.getContext()),
				isDoneRef,
				mlir::ValueRange{ frameVar });

		auto isNotDone = rewiter.create<mlir::rlc::NotOp>(
				action.getLoc(), isDone.getResults()[0]);

		rewiter.create<mlir::rlc::Yield>(
				frame.getLoc(), mlir::ValueRange({ isNotDone }));

		rewiter.createBlock(&loop.getBody());

		auto actions = rewiter.create<mlir::rlc::ActionsStatement>(
				frame.getLoc(), actionStatements.size());

		for (size_t i = 0; i < actionStatements.size(); i++)
		{
			auto* bb = rewiter.createBlock(
					&actions.getActions()[i], actions.getActions()[i].begin());
			rewiter.setInsertionPoint(bb, bb->begin());
			auto cloned = mlir::cast<mlir::rlc::ActionStatement>(
					rewiter.clone(*actionStatements[i]));

			auto ref = rewiter.create<mlir::rlc::UnresolvedReference>(
					cloned.getLoc(), actionStatements[i].getName());
			llvm::SmallVector<mlir::Value, 4> args(
					cloned.getResults().begin(), cloned.getResults().end());
			args.insert(args.begin(), frameVar);

			rewiter.create<mlir::rlc::CallOp>(
					cloned->getLoc(),
					mlir::rlc::UnknownType::get(cloned.getContext()),
					ref,
					args);

			rewiter.create<mlir::rlc::Yield>(actions.getLoc());
		}
		rewiter.setInsertionPointAfter(actions);
		rewiter.create<mlir::rlc::Yield>(frame.getLoc());
		rewiter.eraseOp(action);

		return mlir::success();
	}

#define GEN_PASS_DEF_LOWERSUBACTIONSPASS
#include "rlc/dialect/Passes.inc"

	struct LowerSubActionsPass: impl::LowerSubActionsPassBase<LowerSubActionsPass>
	{
		using impl::LowerSubActionsPassBase<
				LowerSubActionsPass>::LowerSubActionsPassBase;

		void runOnOperation() override
		{
			llvm::StringMap<mlir::rlc::ActionFunction> nameToAction;
			getOperation().walk([&](mlir::rlc::ActionFunction action) {
				nameToAction[action.getUnmangledName()] = action;
			});

			std::vector<mlir::rlc::SubActionStatement> subActions;
			getOperation().walk([&](mlir::rlc::SubActionStatement action) {
				subActions.push_back(action);
			});

			for (auto action : subActions)
			{
				if (rewriteSubAction(action, nameToAction).failed())
				{
					action.emitError("unkown action named " + action.getName());
					signalPassFailure();
					return;
				}
			}
		}
	};
}	 // namespace mlir::rlc

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
#define GEN_PASS_DEF_SORTACTIONSPASS
#include "rlc/dialect/Passes.inc"

	struct SortActionsPass: impl::SortActionsPassBase<SortActionsPass>
	{
		using impl::SortActionsPassBase<SortActionsPass>::SortActionsPassBase;

		void runOnOperation() override
		{
			llvm::StringMap<mlir::rlc::ActionFunction> actions;
			for (auto action : getOperation().getOps<mlir::rlc::ActionFunction>())
				actions[action.getUnmangledName()] = action;

			if (actions.size() < 2)
				return;

			std::
					map<mlir::rlc::ActionFunction, std::vector<mlir::rlc::ActionFunction>>
							dependencies;

			for (auto& action : actions)
			{
				action.second.walk([&](mlir::rlc::UnresolvedReference ref) {
					if (not dependencies.contains(action.second))
						dependencies[action.second] = {};

					if (actions.find(ref.getName()) != actions.end())
						dependencies[action.second].push_back(actions[ref.getName()]);
				});
			}

			llvm::SmallVector<mlir::rlc::ActionFunction, 2> sorted;
			while (not dependencies.empty())
			{
				bool foundOne = false;
				for (auto& element : dependencies)
				{
					// erase from dependecies the ones that are already met
					std::erase_if(
							element.second, [&](const mlir::rlc::ActionFunction& dependency) {
								return not dependencies.contains(dependency);
							});

					if (not element.second.empty())
						continue;

					sorted.push_back(element.first);
					dependencies.erase(element.first);
					foundOne = true;
					break;
				}

				// if we have not managed to remove a lement of the dependency list,
				// there was a loop.
				if (not foundOne)
				{
					dependencies.begin()->first->emitError(
							"Found circular dependency in actions");
					signalPassFailure();
					return;
				}
			}

			for (size_t i = 1; i < sorted.size(); i++)
			{
				sorted[i]->moveAfter(sorted[i - 1]);
			}
		}
	};
}	 // namespace mlir::rlc

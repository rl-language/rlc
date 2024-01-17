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

namespace mlir::rlc
{
#define GEN_PASS_DEF_REMOVEUSELESSALLOCAPASS
#include "rlc/dialect/Passes.inc"

	static void removeUselessAlloca(mlir::ModuleOp op)
	{
		llvm::DenseSet<mlir::LLVM::AllocaOp> allocas;
		op.walk([&](mlir::LLVM::AllocaOp alloca) { allocas.insert(alloca); });

		for (auto alloca : allocas)
		{
			auto result = alloca.getRes();
			if (result.getUses().empty())
				alloca.erase();

			mlir::LLVM::StoreOp store = nullptr;
			mlir::LLVM::LoadOp load = nullptr;
			size_t count = 0;
			for (auto* use : result.getUsers())
			{
				count++;
				if (auto maybeStore = mlir::dyn_cast<mlir::LLVM::StoreOp>(use))
					store = maybeStore;
				if (auto maybeLoad = mlir::dyn_cast<mlir::LLVM::LoadOp>(use))
					load = maybeLoad;
			}

			if (count == 2 and store != nullptr and load != nullptr)
			{
				load.replaceAllUsesWith(store.getValue());
				load.erase();
				store.erase();
				alloca.erase();
			}
		}
	}

	struct RemoveUselessAllocaPass
			: impl::RemoveUselessAllocaPassBase<RemoveUselessAllocaPass>
	{
		using impl::RemoveUselessAllocaPassBase<
				RemoveUselessAllocaPass>::RemoveUselessAllocaPassBase;

		void runOnOperation() override { removeUselessAlloca(getOperation()); }
	};
}	 // namespace mlir::rlc

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

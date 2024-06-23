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
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_HOISTALLOCAPASS
#include "rlc/dialect/Passes.inc"

	struct HoistAllocaPass: impl::HoistAllocaPassBase<HoistAllocaPass>
	{
		using impl::HoistAllocaPassBase<HoistAllocaPass>::HoistAllocaPassBase;

		void runOnOperation() override
		{
			llvm::SmallVector<mlir::LLVM::AllocaOp, 4> toHoist;
			getOperation().walk(
					[&](mlir::LLVM::AllocaOp op) { toHoist.push_back(op); });

			for (auto op : toHoist)
			{
				auto parent = op->getParentOfType<mlir::LLVM::LLVMFuncOp>();
				assert(parent != nullptr);
				op->moveAfter(op.getArraySize().getDefiningOp());
			}
		}
	};
}	 // namespace mlir::rlc

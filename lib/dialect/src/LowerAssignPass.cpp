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

namespace mlir::rlc
{

#define GEN_PASS_DEF_LOWERASSIGNPASS
#include "rlc/dialect/Passes.inc"

	static void resolveAssignOp(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::AssignOp op)
	{
		if (isTemplateType(op.getLhs().getType()).succeeded() or
				isTemplateType(op.getRhs().getType()).succeeded())
			return;

		builder.getRewriter().setInsertionPoint(op);
		auto* actualCall = builder.emitCall(
				op,
				true,
				builtinOperatorName<mlir::rlc::AssignOp>(),
				mlir::ValueRange({ op.getLhs(), op.getRhs() }));
		assert(actualCall != nullptr);
		builder.getRewriter().eraseOp(op);
	}

	void lowerAssignOps(mlir::rlc::ModuleBuilder& builder, mlir::Operation* op)
	{
		llvm::SmallVector<mlir::rlc::AssignOp, 2> ops;
		op->walk([&](mlir::rlc::AssignOp op) { ops.push_back(op); });
		for (auto op : ops)
			resolveAssignOp(builder, op);
	}

	struct LowerAssignPass: impl::LowerAssignPassBase<LowerAssignPass>
	{
		using impl::LowerAssignPassBase<LowerAssignPass>::LowerAssignPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			lowerAssignOps(builder, getOperation());
		}
	};
}	 // namespace mlir::rlc

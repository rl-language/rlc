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
		builder.getRewriter().replaceAllUsesWith(op, actualCall->getResult(0));
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

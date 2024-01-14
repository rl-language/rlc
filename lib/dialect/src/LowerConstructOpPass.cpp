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

#define GEN_PASS_DEF_LOWERCONSTRUCTOPPASS
#include "rlc/dialect/Passes.inc"

	static void resolveConstructOp(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::ConstructOp op)
	{
		if (isTemplateType(op.getType()).succeeded())
			return;

		builder.getRewriter().setInsertionPoint(op);
		OverloadResolver resolver(builder.getSymbolTable());
		auto overload = resolver.instantiateOverload(
				builder.getRewriter(),
				true,
				op.getLoc(),
				builtinOperatorName<mlir::rlc::InitOp>(),
				mlir::TypeRange({ op.getResult().getType() }));

		// if the init method is nto avilable now, delay the resolution until when
		// all implicit init methods have been generated
		if (not overload)
			return;

		assert(isTemplateType(overload.getType()).failed());

		builder.getRewriter().replaceOpWithNewOp<mlir::rlc::ExplicitConstructOp>(
				op, overload);
	}

	void lowerConstructOps(mlir::rlc::ModuleBuilder& builder, mlir::Operation* op)
	{
		llvm::SmallVector<mlir::rlc::ConstructOp, 2> ops;
		op->walk([&](mlir::rlc::ConstructOp op) { ops.push_back(op); });
		for (auto op : ops)
			resolveConstructOp(builder, op);
	}

	struct LowerConstructOpPass
			: impl::LowerConstructOpPassBase<LowerConstructOpPass>
	{
		using impl::LowerConstructOpPassBase<
				LowerConstructOpPass>::LowerConstructOpPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			lowerConstructOps(builder, getOperation());
		}
	};
}	 // namespace mlir::rlc

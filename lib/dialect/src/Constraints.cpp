/*
Copyright 2024 Matteo Cenzato 

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
#include <iostream>

namespace mlir::rlc
{

#define GEN_PASS_DEF_CONSTRAINTSPASS
#include "rlc/dialect/Passes.inc"

	/*
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
	*/

	struct ConstraintsPass
			: impl::ConstraintsPassBase<ConstraintsPass>
	{
		using impl::ConstraintsPassBase<
				ConstraintsPass>::ConstraintsPassBase;

		void runOnOperation() override
		{
			//mlir::rlc::ModuleBuilder builder(getOperation());
			//lowerConstructOps(builder, getOperation());
			std::cout<<"FUNZIA :)"<<std::endl;
		}
	};
}	 // namespace mlir::rlc

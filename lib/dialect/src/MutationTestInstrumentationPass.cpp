
/*
Copyright 2026 Massimo Fioravanti

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
#include "mlir/IR/PatternMatch.h"
#include "rlc/dialect/IRBuilder.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
#define GEN_PASS_DEF_MUTATIONTESTINSTRUMENTATIONPASS
#include "rlc/dialect/Passes.inc"

	struct MutationTestInstrumentationPass
			: impl::MutationTestInstrumentationPassBase<
						MutationTestInstrumentationPass>
	{
		using impl::MutationTestInstrumentationPassBase<
				MutationTestInstrumentationPass>::MutationTestInstrumentationPassBase;

		void runOnOperation() override
		{
			mlir::rlc::IRBuilder builder(getOperation());
			builder.setInsertionPointToStart(&getOperation().getBodyRegion().front());
			auto type = mlir::rlc::IntegerType::getInt64(getOperation().getContext());
			auto extF = builder.createFunctionOp(
					builder.getUnknownLoc(),
					"mutation_check",
					mlir::FunctionType::get(
							getOperation().getContext(), { type }, { type }),
					mlir::rlc::FunctionInfoAttr::get(
							getOperation().getContext(), { "ID" }),
					false,
					{});

			int64_t ID = 0;
			getOperation().walk([&](mlir::rlc::AddOp op) {
				if (op.getResult().getType() != type)
					return;
				builder.setInsertionPoint(op);
				auto idOp =
						builder.createConstant(op.getLoc(), ID++, llvm::Twine(ID).str());
				auto call = builder.createCallOp(op.getLoc(), extF, false, { idOp });
				auto result =
						builder.createAddOp(op.getLoc(), call.getResult(0), op.getLhs());
				op.setOperand(0, result);
			});
		}
	};
}	 // namespace mlir::rlc
	 //

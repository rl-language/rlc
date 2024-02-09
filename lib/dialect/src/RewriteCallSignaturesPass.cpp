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
#define GEN_PASS_DEF_REWRITECALLSIGNATURESPASS
#include "rlc/dialect/Passes.inc"
	struct RewriteCallSignaturesPass
			: impl::RewriteCallSignaturesPassBase<RewriteCallSignaturesPass>
	{
		using impl::RewriteCallSignaturesPassBase<
				RewriteCallSignaturesPass>::RewriteCallSignaturesPassBase;

		void runOnOperation() override
		{
			for (auto fun : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
			{
				if (fun.getResultTypes().empty())
					continue;

				if (not fun.getResultTypes()[0].isa<mlir::rlc::ReferenceType>())
					continue;

				mlir::IRRewriter rewriter(&getContext());
				fun.walk([&](mlir::rlc::Yield yield) {
					// happens with unrechable yields
					if (yield.getArguments().empty())
						return;

					rewriter.setInsertionPoint(yield);
					rewriter.create<mlir::rlc::YieldReference>(
							yield.getLoc(), yield.getArguments()[0]);
					rewriter.eraseOp(yield);
				});
			}
		}
	};
}	 // namespace mlir::rlc

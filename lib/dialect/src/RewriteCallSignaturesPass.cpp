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

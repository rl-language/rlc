
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
					rewriter.setInsertionPoint(yield);
					rewriter.create<mlir::rlc::YieldReference>(
							yield.getLoc(), yield.getArguments()[0]);
					rewriter.eraseOp(yield);
				});
			}
		}
	};
}	 // namespace mlir::rlc

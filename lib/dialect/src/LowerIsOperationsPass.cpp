#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{

#define GEN_PASS_DEF_LOWERISOPERATIONSPASS
#include "rlc/dialect/Passes.inc"

	struct LowerIsOperationsPass
			: impl::LowerIsOperationsPassBase<LowerIsOperationsPass>
	{
		using impl::LowerIsOperationsPassBase<
				LowerIsOperationsPass>::LowerIsOperationsPassBase;

		void runOnOperation() override
		{
			llvm::SmallVector<mlir::rlc::IsOp, 4> toReplace;
			getOperation().walk([&](mlir::rlc::IsOp op) { toReplace.push_back(op); });

			mlir::rlc::ModuleBuilder builder(getOperation());
			mlir::IRRewriter rewriter(getOperation().getContext());
			for (auto op : toReplace)
			{
				bool evalsToTrue = true;
				if (auto casted =
								op.getTypeOrTrait().dyn_cast<mlir::rlc::TraitMetaType>())
				{
					evalsToTrue = casted
														.typeRespectsTrait(
																op.getExpression(), builder.getSymbolTable())
														.succeeded();
				}
				else
				{
					evalsToTrue = op.getExpression() == op.getTypeOrTrait();
				}

				rewriter.setInsertionPoint(op);
				rewriter.replaceOpWithNewOp<mlir::rlc::Constant>(op, evalsToTrue);
			}
		}
	};
}	 // namespace mlir::rlc

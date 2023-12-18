#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/SymbolTable.h"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{

#define GEN_PASS_DEF_LOWERSUBACTIONSPASS
#include "rlc/dialect/Passes.inc"

	struct LowerSubActionsPass: impl::LowerSubActionsPassBase<LowerSubActionsPass>
	{
		using impl::LowerSubActionsPassBase<
				LowerSubActionsPass>::LowerSubActionsPassBase;

		void runOnOperation() override {}
	};
}	 // namespace mlir::rlc

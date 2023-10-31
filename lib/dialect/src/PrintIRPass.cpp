#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_PRINTIRPASS
#include "rlc/dialect/Passes.inc"

	struct PrintIRPass: impl::PrintIRPassBase<PrintIRPass>
	{
		using impl::PrintIRPassBase<PrintIRPass>::PrintIRPassBase;

		void runOnOperation() override
		{
			mlir::OpPrintingFlags flags;
			if (not hide_position)
				flags.enableDebugInfo(true);
			getOperation()->print(*OS, flags);
		}
	};
}	 // namespace mlir::rlc

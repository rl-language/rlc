#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/conversions/RLCToC.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_PRINTCHEADERPASS
#include "rlc/dialect/Passes.inc"

	struct PrintCHeaderPass: impl::PrintCHeaderPassBase<PrintCHeaderPass>
	{
		using impl::PrintCHeaderPassBase<PrintCHeaderPass>::PrintCHeaderPassBase;

		void runOnOperation() override { ::rlc::rlcToCHeader(getOperation(), *OS); }
	};
}	 // namespace mlir::rlc

namespace mlir::rlc
{
#define GEN_PASS_DEF_PRINTGODOTPASS
#include "rlc/dialect/Passes.inc"

	struct PrintGodotPass: impl::PrintGodotPassBase<PrintGodotPass>
	{
		using impl::PrintGodotPassBase<PrintGodotPass>::PrintGodotPassBase;

		void runOnOperation() override { ::rlc::rlcToGodot(getOperation(), *OS); }
	};
}	 // namespace mlir::rlc

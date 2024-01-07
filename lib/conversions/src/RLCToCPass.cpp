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

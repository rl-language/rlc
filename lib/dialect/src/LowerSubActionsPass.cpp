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

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
#include "rlc/parser/MultiFileParser.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Path.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/Pass/Pass.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/utils/Error.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_PARSEFILEPASS
#include "rlc/dialect/Passes.inc"

	struct ParseFilePass: impl::ParseFilePassBase<ParseFilePass>
	{
		using impl::ParseFilePassBase<ParseFilePass>::ParseFilePassBase;

		void runOnOperation() override
		{
			assert(srcManager != nullptr);
			::rlc::MultiFileParser parser(
					&getContext(), *includeDirs, srcManager, getOperation());
			const auto inputFileName = llvm::sys::path::filename(input);

			auto maybeAst = parser.parse(input);
			if (not maybeAst)
			{
				llvm::handleAllErrors(
						maybeAst.takeError(), [&](const ::rlc::RlcError& e) {
							getContext().getDiagEngine().emit(
									e.getPosition(), mlir::DiagnosticSeverity::Error)
									<< e.getText();
						});
				signalPassFailure();
			}
		}
	};
}	 // namespace mlir::rlc

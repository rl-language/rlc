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

							//.emitDiagnostic(
							// e.getPosition(),
							// e.getText(),
							// mlir::DiagnosticSeverity::Error);
						});
				signalPassFailure();
			}
		}
	};
}	 // namespace mlir::rlc

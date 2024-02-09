/*
Copyright 2024 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

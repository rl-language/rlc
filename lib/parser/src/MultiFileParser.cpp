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
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/ToolOutputFile.h"
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

			auto maybeAst = parser.parse(inputs);
			if (not maybeAst)
			{
				llvm::handleAllErrors(
						maybeAst.takeError(),
						[&](const ::rlc::RlcError& e) {
							getContext().getDiagEngine().emit(
									e.getPosition(), mlir::DiagnosticSeverity::Error)
									<< e.getText();
						},
						[&](const llvm::ErrorInfoBase& e) {
							std::string toPrint;
							llvm::raw_string_ostream stream(toPrint);
							e.log(stream);
							stream.flush();
							getContext().getDiagEngine().emit(
									mlir::UnknownLoc::get(&getContext()),
									mlir::DiagnosticSeverity::Error)
									<< toPrint;
						});
				signalPassFailure();
				return;
			}

			if (dependencyFile != "")
			{
				std::error_code EC;

				llvm::ToolOutputFile library(
						dependencyFile + ".dep", EC, llvm::sys::fs::OpenFlags::OF_None);
				if (EC)
				{
					getContext().getDiagEngine().emit(
							mlir::UnknownLoc::get(&getContext()),
							mlir::DiagnosticSeverity::Error)
							<< "failed to create dependency file " + dependencyFile + ".dep";
				}
				library.os() << dependencyFile << ":";
				for (auto file : parser.getImportedFiles())
				{
					llvm::SmallVector<char> absolute;
					llvm::sys::fs::real_path(file, absolute);
					llvm::StringRef ref(absolute.begin(), absolute.size());
					library.os() << " " << ref << " \\\n";
				}
				library.os() << "\n";

				library.keep();
			}
		}
	};
}	 // namespace mlir::rlc

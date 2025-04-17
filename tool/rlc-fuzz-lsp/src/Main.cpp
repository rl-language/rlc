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
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/TargetParser/Host.h"
#include "mlir/Dialect/DLTI/DLTI.h"
#include "mlir/Dialect/Index/IR/IndexDialect.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllTranslations.h"
#include "mlir/Target/LLVMIR/Import.h"
#include "mlir/Tools/lsp-server-support/Transport.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/lsp/LSP.hpp"
#include "rlc/parser/MultiFileParser.hpp"

#define DEBUG_TYPE "rlc-lsp-server"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
	mlir::rlc::lsp::LSPContext context;
	mlir::rlc::lsp::RLCServer server(context);

	auto uri = llvm::cantFail(mlir::lsp::URIForFile::fromFile("/dev/null"));

	std::vector<mlir::lsp::Diagnostic> diagnostics;
	server.addOrUpdateDocument(
			uri, llvm::StringRef((char *) Data, Size), 0, diagnostics);

	for (auto diag : diagnostics)
	{
		auto okMsg = diag.range.start.character != -1 and
								 diag.range.start.line != -1 and
								 diag.range.end.character != -1 and diag.range.end.line != -1;
		if (not okMsg)
		{
			llvm::outs() << diag.message;
			abort();
		}
	}

	mlir::lsp::Position position;

	position.character = 0;
	position.line = 0;

	for (size_t i = 0; i != Size; i++)
	{
		position.character++;
		if (((char *) Data)[i] == '\n')
		{
			position.character = 1;
			position.line++;
		}
		server.getCodeCompletion(uri, position);
		server.findHover(uri, position);
		std::vector<mlir::lsp::Location> locs;
		server.getLocationsOf(uri, position, locs);
	}
	return 0;
}

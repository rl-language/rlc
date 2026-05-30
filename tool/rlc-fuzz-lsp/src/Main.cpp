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


enum class LSPAction : uint8_t
{
	CodeCompletion = 0,
	Hover = 1,
	GoToDefinition = 2,
	FindReferences = 3,
	DocumentSymbols = 4,
	// All values >= 5 fall through to CodeCompletion (default).
};


// Produces a corrupted version of the input by flipping a byte at
// a position determined by the last byte of the input. To explore

static std::string mutateFile(const uint8_t *Data, size_t Size)
{
	if (Size == 0)
		return "";

	std::string mutated(reinterpret_cast<const char *>(Data), Size);
	// Use the last byte to pick the mutation position.
	size_t mutPos = static_cast<size_t>(Data[Size - 1]) % Size;
	// Flip all bits at that position.
	mutated[mutPos] ^= 0xFF;
	return mutated;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
	// Need at least 2 bytes: 1 for the action selector, 1 for the file content.
	if (Size < 2)
		return 0;

	// First byte: selects which LSP action to invoke at each position.
	LSPAction action = static_cast<LSPAction>(Data[0] % 5);

	// Remaining bytes: the .rl file content to load into the server.
	const uint8_t *fileData = Data + 1;
	size_t fileSize = Size - 1;

	
	mlir::rlc::lsp::LSPContext context;
	mlir::rlc::lsp::RLCServer server(context);

	auto uri = llvm::cantFail(mlir::lsp::URIForFile::fromFile("/dev/null"));

	std::vector<mlir::lsp::Diagnostic> diagnostics;
	server.addOrUpdateDocument(
			uri, llvm::StringRef((char *) fileData, fileSize), 0, diagnostics);

	// Validate that all diagnostics have valid ranges. A diagnostic
	// with a -1 position means the server produced an internal error.
	for (auto &diag : diagnostics)
	{
		bool okMsg = diag.range.start.character != -1 &&
								 diag.range.start.line != -1 &&
								 diag.range.end.character != -1 &&
								 diag.range.end.line != -1;
		if (!okMsg)
		{
			llvm::outs() << diag.message;
			abort();
		}
	}

	
	mlir::lsp::Position position;
	position.character = 0;
	position.line = 0;

	for (size_t i = 0; i != fileSize; i++)
	{
		position.character++;
		if (((char *) fileData)[i] == '\n')
		{
			position.character = 1;
			position.line++;
		}

		switch (action)
		{
			case LSPAction::CodeCompletion:
				server.getCodeCompletion(uri, position);
				break;

			case LSPAction::Hover:
				server.findHover(uri, position);
				break;

			case LSPAction::GoToDefinition:
			{
				std::vector<mlir::lsp::Location> locs;
				server.getLocationsOf(uri, position, locs);
				break;
			}

			case LSPAction::FindReferences:
			{
				std::vector<mlir::lsp::Location> refs;
				server.findReferencesOf(uri, position, refs);
				break;
			}

			case LSPAction::DocumentSymbols:
			{
				// Exercise our new findDocumentSymbols implementation.
				std::vector<mlir::lsp::DocumentSymbol> symbols;
				server.findDocumentSymbols(uri, symbols);
				break;
			}
		}
	}

	
	std::string mutated = mutateFile(fileData, fileSize);
	server.addOrUpdateDocument(uri, mutated, 1, diagnostics);

	// Run document symbols on the mutated file to stress the new code.
	std::vector<mlir::lsp::DocumentSymbol> symbols;
	server.findDocumentSymbols(uri, symbols);

	// Run completion at position (0,0) on the mutated file.
	mlir::lsp::Position origin;
	origin.line = 0;
	origin.character = 0;
	server.getCodeCompletion(uri, origin);

	return 0;
}

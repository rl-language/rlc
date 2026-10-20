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
#include <cstdlib>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/MemoryBuffer.h"
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

namespace
{

std::vector<std::pair<std::string, std::string>> gPrograms;

class InputReader
{
	public:
	InputReader(const uint8_t *data, size_t size) : data(data), size(size) {}

	bool empty() const { return cursor >= size; }

	uint8_t next()
	{
		if (cursor >= size)
			return 0;
		return data[cursor++];
	}

	private:
	const uint8_t *data;
	size_t size;
	size_t cursor = 0;
};

mlir::lsp::Position positionFromInput(
		InputReader &reader, llvm::StringRef program)
{
	size_t offset = 0;
	if (not program.empty())
	{
		uint16_t raw = static_cast<uint16_t>(reader.next()) << 8;
		raw |= reader.next();
		offset = raw % program.size();
	}

	mlir::lsp::Position pos;
	pos.line = 0;
	pos.character = 0;
	for (size_t i = 0; i < offset; i++)
	{
		if (program[i] == '\n')
		{
			pos.line++;
			pos.character = 0;
		}
		else
			pos.character++;
	}
	return pos;
}


std::string applyEdit(InputReader &reader, const std::string &program)
{
	if (program.empty())
		return program;

	uint8_t kind = reader.next() % 3;
	uint16_t raw = static_cast<uint16_t>(reader.next()) << 8;
	raw |= reader.next();
	size_t pos = raw % program.size();

	std::string edited = program;
	switch (kind)
	{
		case 0: // replace one character
			edited[pos] = static_cast<char>(reader.next());
			break;
		case 1: // insert one character before pos
			edited.insert(edited.begin() + pos, static_cast<char>(reader.next()));
			break;
		case 2: // delete the character at pos
			edited.erase(edited.begin() + pos);
			break;
	}
	return edited;
}
}	
extern "C" int LLVMFuzzerInitialize(int *, char ***)
{
	const char *dir = getenv("RLC_FUZZ_PROGRAM_DIR");
	if (not dir)
	{
		llvm::errs() << "set RLC_FUZZ_PROGRAM_DIR to a directory of .rl programs\n";
		return 0;
	}

	std::error_code ec;
	for (llvm::sys::fs::directory_iterator it(dir, ec), end;
			 it != end and not ec; it.increment(ec))
	{
		auto buffer = llvm::MemoryBuffer::getFile(it->path());
		if (buffer)
			gPrograms.emplace_back(it->path(), (*buffer)->getBuffer().str());
	}
	return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
	if (gPrograms.empty())
		return 0;

	InputReader reader(Data, Size);

	// First byte selects which of the loaded programs to operate on.
	const auto &chosen = gPrograms[reader.next() % gPrograms.size()];
	std::string program = chosen.second;

	mlir::rlc::lsp::LSPContext context;
	mlir::rlc::lsp::RLCServer server(context);
	auto uri =
			llvm::cantFail(mlir::lsp::URIForFile::fromFile(chosen.first));

	int64_t version = 0;
	std::vector<mlir::lsp::Diagnostic> diagnostics;
	server.addOrUpdateDocument(uri, program, version, diagnostics);

	const auto checkDiagnostics = [&]() {
		for (auto diag : diagnostics)
		{
			auto valid = diag.range.start.character != -1 and
									 diag.range.start.line != -1 and
									 diag.range.end.character != -1 and
									 diag.range.end.line != -1;
			if (not valid)
			{
				llvm::outs() << diag.message;
				abort();
			}
		}
	};
	checkDiagnostics();

	while (not reader.empty())
	{
		switch (reader.next() % 6)
		{
			case 0:
			{
				auto pos = positionFromInput(reader, program);
				server.getCodeCompletion(uri, pos);
				break;
			}
			case 1:
			{
				auto pos = positionFromInput(reader, program);
				server.findHover(uri, pos);
				break;
			}
			case 2:
			{
				auto pos = positionFromInput(reader, program);
				std::vector<mlir::lsp::Location> locations;
				server.getLocationsOf(uri, pos, locations);
				break;
			}
			case 3:
			{
				auto pos = positionFromInput(reader, program);
				std::vector<mlir::lsp::Location> references;
				server.findReferencesOf(uri, pos, references);
				break;
			} 
			

			case 4:
			{
				std::vector<mlir::lsp::DocumentSymbol> symbols;
				server.findDocumentSymbols(uri, symbols);
				break;
			}
				  
			case 5:
			{
				program = applyEdit(reader, program);
				diagnostics.clear();
				server.addOrUpdateDocument(uri, program, ++version, diagnostics);
				checkDiagnostics();
				break;
			}
		}
			

	}

	return 0;
}
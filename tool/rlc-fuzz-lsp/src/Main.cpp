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

static const char *baseProgramPath =
		"/home/emanuele/Desktop/fuzz-corpus/seed1";

namespace
{
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

std::string applyEdit(
		InputReader &reader, const std::string &program)
{
	if (program.empty())
		return program;

	uint16_t raw = static_cast<uint16_t>(reader.next()) << 8;
	raw |= reader.next();
	size_t pos = raw % program.size();
	char replacement = static_cast<char>(reader.next());

	std::string edited = program;
	edited[pos] = replacement;
	return edited;
}
}	 // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
	auto bufferOrErr = llvm::MemoryBuffer::getFile(baseProgramPath);
	if (not bufferOrErr)
		return 0;

	std::string program = (*bufferOrErr)->getBuffer().str();

	mlir::rlc::lsp::LSPContext context;
	mlir::rlc::lsp::RLCServer server(context);
	auto uri = llvm::cantFail(mlir::lsp::URIForFile::fromFile(baseProgramPath));

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

	InputReader reader(Data, Size);
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

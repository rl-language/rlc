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
#include "rlc/backend/BackEnd.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/driver/Driver.hpp"
#include "rlc/parser/MultiFileParser.hpp"

#define DEBUG_TYPE "rlc-lsp-server"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
	int quanity = 1;
	const char *name = "fuzz-llvm";
	const char **nameArray = &name;
	mlir::registerAllTranslations();
	mlir::MLIRContext context(mlir::MLIRContext::Threading::DISABLED);
	llvm::SourceMgr sourceManager;
	mlir::DialectRegistry Registry;
	Registry.insert<
			mlir::BuiltinDialect,
			mlir::memref::MemRefDialect,
			mlir::rlc::RLCDialect,
			mlir::DLTIDialect,
			mlir::index::IndexDialect>();

	context.appendDialectRegistry(Registry);
	context.loadAllAvailableDialects();

	std::string s;
	llvm::raw_string_ostream OS(s);
	mlir::rlc::Driver driver(sourceManager, { "-" }, "-", OS);
	driver.setRequest(mlir::rlc::Driver::Request::dumpMLIR);
	driver.setEmitPreconditionChecks(true);
	driver.setSkipParsing();

	auto module = mlir::ModuleOp::create(
			mlir::FileLineColLoc::get(&context, "-", 0, 0), "-");

	::rlc::MultiFileParser parser(&context, {}, &sourceManager, module);
	auto res = parser.parseFromBuffer(llvm::StringRef((char *) Data, Size), "-");
	if (not res)
		llvm::consumeError(res.takeError());

	mlir::PassManager manager(&context);
	driver.configurePassManager(manager);
	auto _ = manager.run(module);

	if (module.verify().failed())
		abort();

	return 0;
}

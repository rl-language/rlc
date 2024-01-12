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
	mlir::rlc::Driver driver(sourceManager, "-", "-", OS);
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

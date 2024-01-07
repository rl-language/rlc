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
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "mlir/InitAllDialects.h"
#include "mlir/Tools/mlir-translate/MlirTranslateMain.h"
#include "mlir/Tools/mlir-translate/Translation.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/parser/Parser.hpp"
#include "rlc/utils/IRange.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

static ExitOnError exitOnErr;

static mlir::OwningOpRef<mlir::ModuleOp> rlcToMlir(
		llvm::SourceMgr& sourceMgr, mlir::MLIRContext* context)
{
	mlir::DialectRegistry Registry;
	Registry.insert<mlir::LLVM::LLVMDialect, mlir::rlc::RLCDialect>();
	context->appendDialectRegistry(Registry);
	context->loadAllAvailableDialects();

	Parser parser(
			context, sourceMgr.getMemoryBuffer(1)->getBuffer().data(), "random_name");

	return exitOnErr(parser.system());
}

namespace mlir
{
	void registerFromRLCTranslation()
	{
		mlir::TranslateToMLIRRegistration fromRLC(
				"import-rlc",
				"parse a rlc file",
				[](llvm::SourceMgr& sourceMgr, MLIRContext* context) {
					return ::rlcToMlir(sourceMgr, context);
				});
	}
}	 // namespace mlir

int main(int argc, char* argv[])
{
	mlir::registerFromRLCTranslation();
	return mlir::mlirTranslateMain(argc, argv, "rlc-to-mlir").failed();
}

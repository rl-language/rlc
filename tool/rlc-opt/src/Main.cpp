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
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Passes.hpp"
#include "rlc/parser/Parser.hpp"
#include "rlc/utils/IRange.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

int main(int argc, char* argv[])
{
	mlir::rlc::registerdialectPasses();
	mlir::DialectRegistry registry;
	registry.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
	auto result = mlir::MlirOptMain(argc, argv, "rlc-opt", registry);
	return mlir::asMainReturnCode(result);
}

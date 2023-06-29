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

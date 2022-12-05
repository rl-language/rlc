#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "mlir/InitAllDialects.h"
#include "mlir/Tools/mlir-translate/MlirTranslateMain.h"
#include "mlir/Tools/mlir-translate/Translation.h"
#include "rlc/ast/BuiltinEntities.hpp"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/lowerer/Lowerer.hpp"
#include "rlc/parser/Parser.hpp"
#include "rlc/utils/IRange.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

static ExitOnError exitOnErr;

static mlir::OwningOpRef<mlir::ModuleOp> rlcToMlir(
		llvm::SourceMgr& sourceMgr, mlir::MLIRContext* context)
{
	TypeDB db;
	Lowerer lowerer(*context, db);
	mlir::DialectRegistry Registry;
	Registry.insert<mlir::LLVM::LLVMDialect>();
	lowerer.getContext().appendDialectRegistry(Registry);
	lowerer.getContext().loadAllAvailableDialects();

	for (auto i : irange(sourceMgr.getNumBuffers()))
	{
		Parser parser(
				sourceMgr.getMemoryBuffer(i + 1)->getBuffer().data(), "random_name");

		auto ast = exitOnErr(parser.system());
		rlc::addBuilints(ast);
		rlc::addBuilintsEntities(ast);
		exitOnErr(ast.typeCheck(SymbolTable(), db));

		exitOnErr(lowerer.lowerSystem(ast));
	}
	return lowerer.getModule(0);
}

namespace mlir
{
	void registerFromRLCTranslation()
	{
		mlir::TranslateToMLIRRegistration fromRLC(
				"import-rlc", [](llvm::SourceMgr& sourceMgr, MLIRContext* context) {
					return ::rlcToMlir(sourceMgr, context);
				});
	}
}	 // namespace mlir

int main(int argc, char* argv[])
{
	mlir::registerFromRLCTranslation();
	return mlir::mlirTranslateMain(argc, argv, "rlc-to-mlir").failed();
}

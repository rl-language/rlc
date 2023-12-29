#include "rlc/lsp/LSPContext.hpp"

#include "llvm/Support/Error.h"
#include "mlir/Pass/PassManager.h"
#include "rlc/dialect/Passes.hpp"
#include "rlc/parser/MultiFileParser.hpp"

using namespace mlir::rlc::lsp;

LSPContext::LSPContext(): context(mlir::MLIRContext::Threading::DISABLED)
{
	mlir::SourceMgrDiagnosticHandler diagnostic(
			sourceManager, &context, [](mlir::Location) { return true; });
	Registry.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
	context.appendDialectRegistry(Registry);
	context.loadAllAvailableDialects();
}

llvm::Expected<mlir::ModuleOp> LSPContext::loadFile(
		llvm::StringRef path,
		llvm::StringRef contents,
		int64_t version,
		mlir::ModuleOp op)
{
	::rlc::MultiFileParser parser(&context, includes, &sourceManager, op);
	auto res = parser.parseFromBuffer(contents);

	mlir::PassManager manager(&context);
	manager.addPass(mlir::rlc::createSortActionsPass());
	manager.addPass(mlir::rlc::createEmitEnumEntitiesPass());
	manager.addPass(mlir::rlc::createTypeCheckEntitiesPass());
	manager.addPass(mlir::rlc::createTypeCheckPass());
	manager.enableVerifier(false);
	auto typeCheckResult = manager.run(op);

	if (!res)
		return res.takeError();

	if (typeCheckResult.failed())
		return llvm::createStringError(
				llvm::inconvertibleErrorCode(), "failed to parse");
	return op;
}

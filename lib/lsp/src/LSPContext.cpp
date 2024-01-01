#include "rlc/lsp/LSPContext.hpp"

#include "llvm/Support/Error.h"
#include "mlir/Pass/PassManager.h"
#include "rlc/dialect/Passes.hpp"
#include "rlc/parser/MultiFileParser.hpp"
#include "rlc/utils/Error.hpp"

using namespace mlir::rlc::lsp;

LSPContext::LSPContext()
		: context(mlir::MLIRContext::Threading::DISABLED),
			diagnosticHandler(&context, [this](mlir::Diagnostic &diagnostic) {
				std::string text;
				llvm::raw_string_ostream stream(text);
				diagnostic.print(stream);
				stream.flush();
				diagnostics.emplace_back(Diagnostic{
						text, diagnostic.getLocation(), diagnostic.getSeverity() });
			})
{
	Registry.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
	context.appendDialectRegistry(Registry);
	context.loadAllAvailableDialects();
}

void LSPContext::loadFile(
		llvm::StringRef path,
		llvm::StringRef contents,
		int64_t version,
		mlir::ModuleOp op)
{
	::rlc::MultiFileParser parser(&context, includes, &sourceManager, op);
	auto res = parser.parseFromBuffer(contents, path);

	if (!res)
	{
		auto error = llvm::handleErrors(
				res.takeError(),
				[&](const llvm::StringError &error) {
					diagnostics.emplace_back(
							error.getMessage(),
							mlir::FileLineColLoc::get(&context, "-", 1, 1),
							mlir::DiagnosticSeverity::Error);
				},
				[&](const ::rlc::RlcError &error) {
					diagnostics.emplace_back(
							error.getText(),
							error.getPosition(),
							mlir::DiagnosticSeverity::Error);
				});
		if (error)
		{
			diagnostics.emplace_back(
					"lsp: unkown llvm error",
					mlir::FileLineColLoc::get(&context, "-", 1, 1),
					mlir::DiagnosticSeverity::Error);
			llvm::consumeError(std::move(error));
		}
	}

	mlir::PassManager manager(&context);
	manager.addPass(mlir::rlc::createSortActionsPass());
	manager.addPass(mlir::rlc::createEmitEnumEntitiesPass());
	manager.addPass(mlir::rlc::createTypeCheckEntitiesPass());
	manager.addPass(mlir::rlc::createTypeCheckPass());
	auto typeCheckResult = manager.run(op);
}

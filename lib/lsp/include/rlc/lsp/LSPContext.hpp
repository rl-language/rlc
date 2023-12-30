#pragma once

#include "llvm/Support/SourceMgr.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/MLIRContext.h"
#include "rlc/dialect/Dialect.h"

namespace mlir::rlc::lsp
{
	class LSPContext
	{
		public:
		struct Diagnostic
		{
			std::string text;
			mlir::Location location;
			mlir::DiagnosticSeverity severity;
		};

		LSPContext();

		void addInclude(llvm::StringRef include)
		{
			includes.push_back(include.str());
		}

		void loadFile(
				llvm::StringRef path,
				llvm::StringRef contents,
				int64_t version,
				mlir::ModuleOp op);

		mlir::MLIRContext* getContext() { return &context; }

		llvm::ArrayRef<Diagnostic> getDiagnostics() const { return diagnostics; }

		void clearDiagnostics() { diagnostics.clear(); }

		private:
		llvm::SmallVector<Diagnostic> diagnostics;
		mlir::MLIRContext context;
		mlir::ScopedDiagnosticHandler diagnosticHandler;
		llvm::SourceMgr sourceManager;
		mlir::DialectRegistry Registry;
		llvm::SmallVector<std::string, 4> includes;
	};

}	 // namespace mlir::rlc::lsp

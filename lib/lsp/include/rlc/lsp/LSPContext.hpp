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
		LSPContext();

		void addInclude(llvm::StringRef include)
		{
			includes.push_back(include.str());
		}

		llvm::Expected<mlir::ModuleOp> loadFile(
				llvm::StringRef path,
				llvm::StringRef contents,
				int64_t version,
				mlir::ModuleOp op);

		mlir::MLIRContext* getContext() { return &context; }

		private:
		mlir::MLIRContext context;
		llvm::SourceMgr sourceManager;
		mlir::DialectRegistry Registry;
		llvm::SmallVector<std::string, 4> includes;
	};

}	 // namespace mlir::rlc::lsp

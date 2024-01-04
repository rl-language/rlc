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

		llvm::SourceMgr& getSourceManager() { return sourceManager; }
		llvm::ArrayRef<std::string> getIncludePaths() { return includes; }

		private:
		llvm::SourceMgr sourceManager;
		llvm::SmallVector<std::string, 4> includes;
	};

}	 // namespace mlir::rlc::lsp

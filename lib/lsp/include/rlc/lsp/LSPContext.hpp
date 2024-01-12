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
		LSPContext() {}

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

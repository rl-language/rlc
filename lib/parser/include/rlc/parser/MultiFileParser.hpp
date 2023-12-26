#pragma once

#include <set>

#include "llvm/Support/SourceMgr.h"
#include "rlc/parser/Parser.hpp"

namespace rlc
{

	class MultiFileParser
	{
		public:
		MultiFileParser(
				mlir::MLIRContext* context,
				llvm::ArrayRef<std::string> includeDirs,
				llvm::SourceMgr* srcManager,
				mlir::ModuleOp module = nullptr)
				: sourceManager(srcManager), context(context), module(module)
		{
			sourceManager->setIncludeDirs(includeDirs);
			if (module == nullptr)
				this->module =
						mlir::ModuleOp::create(mlir::UnknownLoc::get(context), "unknown");
		}

		llvm::Expected<mlir::ModuleOp> parse(const std::string& fileName)
		{
			mlir::IRRewriter rewriter(context);
			std::set<std::string> alreadyLoaded;
			llvm::SmallVector<std::string> fileToLoad = { fileName };

			while (not fileToLoad.empty())
			{
				std::string current = fileToLoad.back();
				fileToLoad.pop_back();

				std::string AbslutePath;
				auto id =
						sourceManager->AddIncludeFile(current, llvm::SMLoc(), AbslutePath);
				if (id == 0)
					return llvm::createStringError(
							llvm::inconvertibleErrorCode(),
							std::string("cannot find file ") + current);

				if (alreadyLoaded.contains(AbslutePath))
					continue;

				alreadyLoaded.insert(AbslutePath);
				Parser parser(
						context,
						sourceManager->getMemoryBuffer(id)->getBuffer().str(),
						AbslutePath);
				auto maybeAst = parser.system(module);
				if (not maybeAst)
					return maybeAst.takeError();
				for (auto file : parser.getImportedFiles())
					fileToLoad.push_back(file);
			}
			return module;
		}

		private:
		llvm::SourceMgr* sourceManager;
		mlir::MLIRContext* context;
		mlir::ModuleOp module;
	};

}	 // namespace rlc

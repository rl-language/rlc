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

		llvm::Error parseOneFile(
				llvm::StringRef content,
				llvm::StringRef fileName,
				llvm::SmallVector<std::string>& fileToLoad)
		{
			Parser parser(context, content.str(), fileName.str());
			auto maybeAst = parser.system(module);
			if (not maybeAst)
				return maybeAst.takeError();
			for (auto file : parser.getImportedFiles())
				fileToLoad.push_back(file);
			return llvm::Error::success();
		}

		llvm::Expected<mlir::ModuleOp> parseFromBuffer(
				llvm::StringRef content, llvm::StringRef fileName)
		{
			mlir::IRRewriter rewriter(context);
			std::set<std::string> alreadyLoaded;
			llvm::SmallVector<std::string> fileToLoad;

			if (llvm::Error error = parseOneFile(content, fileName, fileToLoad))
				return std::move(error);

			if (llvm::Error error = recursiveParseFile(alreadyLoaded, fileToLoad))
				return std::move(error);

			return module;
		}

		llvm::Error recursiveParseFile(
				std::set<std::string>& alreadyLoaded,
				llvm::SmallVector<std::string>& fileToLoad)
		{
			mlir::IRRewriter rewriter(context);
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
				if (auto error = parseOneFile(
								sourceManager->getMemoryBuffer(id)->getBuffer().str(),
								AbslutePath,
								fileToLoad))
					return error;
			}
			return llvm::Error::success();
		}

		llvm::Expected<mlir::ModuleOp> parse(const std::string& fileName)
		{
			mlir::IRRewriter rewriter(context);
			std::set<std::string> alreadyLoaded;
			llvm::SmallVector<std::string> fileToLoad = { fileName };
			if (auto error = recursiveParseFile(alreadyLoaded, fileToLoad))
				return std::move(error);
			return module;
		}

		private:
		llvm::SourceMgr* sourceManager;
		mlir::MLIRContext* context;
		mlir::ModuleOp module;
	};

}	 // namespace rlc

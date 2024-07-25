/*
Copyright 2024 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
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
			for (auto file : parser.getImportedFiles())
				fileToLoad.push_back(file);
			if (not maybeAst)
				return maybeAst.takeError();
			return llvm::Error::success();
		}

		llvm::Expected<mlir::ModuleOp> parseFromBuffer(
				llvm::StringRef content, llvm::StringRef fileName)
		{
			mlir::IRRewriter rewriter(context);
			std::set<std::string> alreadyLoaded;
			llvm::SmallVector<std::string> fileToLoad;

			if (llvm::Error error = parseOneFile(content, fileName, fileToLoad))
			{
				// if we fail to parse the first file, keep parsing the othe imported
				// files so we can autocompleate types
				if (llvm::Error error2 = recursiveParseFile(alreadyLoaded, fileToLoad))
					llvm::consumeError(std::move(error2));
				return std::move(error);
			}

			if (llvm::Error error = recursiveParseFile(alreadyLoaded, fileToLoad))
				return std::move(error);

			return module;
		}

		llvm::Error recursiveParseFile(
				std::set<std::string>& alreadyLoaded,
				llvm::SmallVector<std::string>& fileToLoad)
		{
			mlir::IRRewriter rewriter(context);
			llvm::Error maybeError = llvm::Error::success();
			while (not fileToLoad.empty())
			{
				std::string current = fileToLoad.back();
				fileToLoad.pop_back();

				std::string AbslutePath;
				auto id =
						sourceManager->AddIncludeFile(current, llvm::SMLoc(), AbslutePath);
				// could not find the file
				if (id == 0)
				{
					if (!maybeError)
						maybeError = llvm::createStringError(
								llvm::inconvertibleErrorCode(),
								std::string("cannot find file ") + current);
					return maybeError;
				}

				if (alreadyLoaded.contains(AbslutePath))
					continue;

				alreadyLoaded.insert(AbslutePath);
				if (auto error = parseOneFile(
								sourceManager->getMemoryBuffer(id)->getBuffer().str(),
								AbslutePath,
								fileToLoad))
				{
					// instead of stopping at the first error in a file
					// try parse them all, and return the first error
					// this way we allow the LSP to print any type from
					// any file, even if they contain errors
					if (maybeError)
						llvm::consumeError(std::move(error));
					else
						maybeError = std::move(error);
				}
			}
			return maybeError;
		}

		llvm::Expected<mlir::ModuleOp> parse(llvm::ArrayRef<std::string> fileNames)
		{
			mlir::IRRewriter rewriter(context);
			std::set<std::string> alreadyLoaded;
			llvm::SmallVector<std::string> fileToLoad(
					fileNames.begin(), fileNames.end());
			std::reverse(fileToLoad.begin(), fileToLoad.end());
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

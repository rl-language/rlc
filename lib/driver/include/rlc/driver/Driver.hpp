#pragma once

#include "mlir/Pass/PassManager.h"
#include "rlc/backend/BackEnd.hpp"

namespace mlir::rlc
{

	class Driver
	{
		public:
		enum class Request
		{
			printIncludedFiles,
			dumpUncheckedAST,
			dumpDot,
			dumpParsableGraph,
			dumpCheckedAST,
			dumpCWrapper,
			dumpCSharp,
			dumpBeforeTemplate,
			dumpGodotWrapper,
			dumpPythonWrapper,
			dumpRLC,
			dumpAfterImplicit,
			dumpFlatIR,
			dumpMLIR,
			compile,
			executable,
			format
		};

		public:
		void configurePassManager(mlir::PassManager &manager) const;

		void setRequest(Request newRequest) { request = newRequest; }
		void setHidePosition(bool doHide) { hidePosition = doHide; }
		void setEmitPreconditionChecks(bool doEmit)
		{
			emitPreconditionChecks = doEmit;
		}

		void setDumpIR(bool doDump) { dumpIR = doDump; }
		void setClangPath(std::string newPath) { clangPath = newPath; }
		void setAbortSymbol(std::string abortSym) { abortSymbol = abortSym; }
		void setExtraObjectFile(std::vector<std::string> newExtraObjectFiles)
		{
			extraObjectFiles = newExtraObjectFiles;
		}
		void setRPath(std::vector<std::string> newRPath) { rPath = newRPath; }

		void setEmitFuzzer(bool newEmitFuzzer) { emitFuzzer = newEmitFuzzer; }

		void setIncludeDirs(llvm::SmallVector<std::string, 4> newIncludeDirs)
		{
			includeDirs = newIncludeDirs;
		}

		void setTargetInfo(const TargetInfo *newTargetInfo)
		{
			targetInfo = newTargetInfo;
		}

		void setEmitBoundChecks(bool doEmit) { emitBoundChecks = doEmit; }
		void setEmitSanitizer(bool doEmit) { emitSanitizer = doEmit; }
		void setEmitDependencyFile(bool doEmit) { emitDependencyFile = doEmit; }

		void setSkipParsing(bool doIt = true) { skipParsing = doIt; }
		void setDebug(bool doIt = true) { debug = doIt; }
		void setVerbose(bool doIt = true) { verbose = doIt; }
		void setHideStandardLibFiles(bool doIt = true)
		{
			hideStandardLibFiles = doIt;
		}
		void setGraphInlineCalls(bool doIt) { graphInlineCalls = doIt; }
		void setGraphKeepOnlyActions(bool doIt) { graphKeepOnlyActions = doIt; }
		void setGraphRegexFilter(std::string s) { graphRegexFilter = s; }

		Driver(
				llvm::SourceMgr &srcManager,
				llvm::ArrayRef<std::string> inputFile,
				std::string outputFile,
				llvm::raw_ostream &outputStream)
				: srcManager(&srcManager),
					inputFile(inputFile),
					outputFile(outputFile),
					OS(&outputStream)
		{
		}

		private:
		bool hidePosition = false;
		Request request = Request::executable;
		bool emitPreconditionChecks = true;
		bool emitBoundChecks = true;
		bool hideStandardLibFiles = true;
		bool emitFuzzer = false;
		bool emitSanitizer = false;
		bool emitDependencyFile = false;
		bool skipParsing = false;
		bool debug = false;

		std::string clangPath = "clang";

		std::string abortSymbol = "";

		llvm::SourceMgr *srcManager;
		llvm::SmallVector<std::string, 2> inputFile;
		std::string outputFile;

		std::vector<std::string> extraObjectFiles = {};
		std::vector<std::string> rPath = {};
		llvm::SmallVector<std::string, 4> includeDirs = {};
		llvm::raw_ostream *OS;
		bool dumpIR = false;
		bool verbose = false;

		bool graphInlineCalls = false;
		bool graphKeepOnlyActions = false;
		std::string graphRegexFilter = ".*";

		const mlir::rlc::TargetInfo *targetInfo = nullptr;
	};
}	 // namespace mlir::rlc

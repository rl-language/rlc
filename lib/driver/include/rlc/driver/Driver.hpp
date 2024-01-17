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
			dumpUncheckedAST,
			dumpDot,
			dumpCheckedAST,
			dumpCWrapper,
			dumpGodotWrapper,
			dumpPythonAST,
			dumpPythonWrapper,
			dumpRLC,
			dumpAfterImplicit,
			dumpFlatIR,
			dumpMLIR,
			compile,
			executable
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
		void setExtraObjectFile(std::vector<std::string> newExtraObjectFiles)
		{
			extraObjectFiles = newExtraObjectFiles;
		}

		void setIncludeDirs(llvm::SmallVector<std::string, 4> newIncludeDirs)
		{
			includeDirs = newIncludeDirs;
		}

		void setTargetInfo(const TargetInfo *newTargetInfo)
		{
			targetInfo = newTargetInfo;
		}

		void setEmitBoundChecks(bool doEmit) { emitBoundChecks = doEmit; }

		void setSkipParsing(bool doIt = true) { skipParsing = doIt; }

		Driver(
				llvm::SourceMgr &srcManager,
				std::string inputFile,
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
		bool skipParsing = false;

		std::string clangPath = "clang";

		llvm::SourceMgr *srcManager;
		std::string inputFile;
		std::string outputFile;

		std::vector<std::string> extraObjectFiles = {};
		llvm::SmallVector<std::string, 4> includeDirs = {};
		llvm::raw_ostream *OS;
		bool dumpIR = false;

		const mlir::rlc::TargetInfo *targetInfo = nullptr;
	};
}	 // namespace mlir::rlc

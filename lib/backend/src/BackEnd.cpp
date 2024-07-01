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
#include "rlc/backend/BackEnd.hpp"

#include <clang/Driver/Job.h>
#include <string>
#include <vector>

#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/AutoUpgrade.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Transforms/Instrumentation.h"
#include "llvm/Transforms/Instrumentation/SanitizerCoverage.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h"
#include "mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Export.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
using namespace llvm;
using namespace std;

namespace mlir::rlc
{

	void initLLVM()
	{
		using namespace llvm;
		InitializeAllTargets();
		InitializeAllTargetMCs();
		InitializeAllAsmPrinters();
		InitializeAllAsmParsers();

		PassRegistry &Registry = *llvm::PassRegistry::getPassRegistry();
		initializeCore(Registry);

		initializeScalarOpts(Registry);
		initializeVectorization(Registry);
		initializeIPO(Registry);
		initializeAnalysis(Registry);
		initializeTransformUtils(Registry);
		initializeInstCombine(Registry);
		initializeTarget(Registry);
		// For codegen passes, only passes that do IR to IR transformation are
		// supported.
		initializeExpandMemCmpLegacyPassPass(Registry);
		initializeAtomicExpandPass(Registry);
		initializeWinEHPreparePass(Registry);
		initializeSafeStackLegacyPassPass(Registry);
		initializeSjLjEHPreparePass(Registry);
		initializePreISelIntrinsicLoweringLegacyPassPass(Registry);
		initializeGlobalMergePass(Registry);
		initializeIndirectBrExpandLegacyPassPass(Registry);
		initializeInterleavedLoadCombinePass(Registry);
		initializeInterleavedAccessPass(Registry);
		initializeUnreachableBlockElimLegacyPassPass(Registry);
		initializeExpandReductionsPass(Registry);
		initializeWasmEHPreparePass(Registry);
		initializeWriteBitcodePassPass(Registry);
	}
}	 // namespace mlir::rlc

static void addFuzzerInstrumentationPass(llvm::ModulePassManager &MPM)
{
	SanitizerCoverageOptions opts;
	opts.CoverageType = llvm::SanitizerCoverageOptions::SCK_Edge;
	opts.IndirectCalls = true;
	opts.Inline8bitCounters = true;
	opts.TraceCmp = true;
	opts.PCTable = true;
	MPM.addPass(SanitizerCoveragePass(opts));
}

static void runOptimizer(
		llvm::Module &M,
		bool optimize,
		bool emitSanitizerInstrumentation,
		bool linkAgainsFuzzer)
{
	// Create the analysis managers.
	LoopAnalysisManager LAM;
	FunctionAnalysisManager FAM;
	CGSCCAnalysisManager CGAM;
	ModuleAnalysisManager MAM;

	// Create the new pass manager builder.
	// Take a look at the PassBuilder constructor parameters for more
	// customization, e.g. specifying a TargetMachine or various debugging
	// options.
	PassBuilder PB;

	// Register all the basic analyses with the managers.
	PB.registerModuleAnalyses(MAM);
	PB.registerCGSCCAnalyses(CGAM);
	PB.registerFunctionAnalyses(FAM);
	PB.registerLoopAnalyses(LAM);
	PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

	// Create the pass manager.
	// This one corresponds to a typical -O2 optimization pipeline.
	if (optimize)
	{
		ModulePassManager passManager;
		FunctionPassManager functionPassManager;
		functionPassManager.addPass(llvm::PromotePass());
		passManager.addPass(
				createModuleToFunctionPassAdaptor(std::move(functionPassManager)));
		if (emitSanitizerInstrumentation)
			addFuzzerInstrumentationPass(passManager);
		passManager.run(M, MAM);

		PB.buildModuleSimplificationPipeline(
					OptimizationLevel::O2, llvm::ThinOrFullLTOPhase::ThinLTOPreLink)
				.run(M, MAM);
		PB.buildModuleInlinerPipeline(
					OptimizationLevel::O2, llvm::ThinOrFullLTOPhase::ThinLTOPreLink)
				.run(M, MAM);
		PB.buildModuleSimplificationPipeline(
					OptimizationLevel::O2, llvm::ThinOrFullLTOPhase::ThinLTOPreLink)
				.run(M, MAM);
		PB.buildModuleOptimizationPipeline(
					OptimizationLevel::O2, llvm::ThinOrFullLTOPhase::ThinLTOPreLink)
				.run(M, MAM);
	}
	else
	{
		ModulePassManager MPM =
				PB.buildO0DefaultPipeline(OptimizationLevel::O0, true);
		if (emitSanitizerInstrumentation)
			addFuzzerInstrumentationPass(MPM);
		MPM.run(M, MAM);
	}
}

struct mlir::rlc::TargetInfoImpl
{
	public:
	TargetInfoImpl(std::string triple, bool shared, bool optimize)
			: triple(triple),
				optimize(
						optimize ? CodeGenOptLevel::Aggressive : CodeGenOptLevel::Default),
				reloc(shared ? llvm::Reloc::PIC_ : llvm::Reloc::Static)
	{
		std::string Error;
		target = llvm::TargetRegistry::lookupTarget("", this->triple, Error);
		assert(target);
		options = llvm::codegen::InitTargetOptionsFromCodeGenFlags(this->triple);

		auto *Ptr = target->createTargetMachine(
				this->triple.getTriple(),
				"",
				"",
				options,
				reloc,
				llvm::CodeModel::Large,
				this->optimize);
		targetMachine = unique_ptr<TargetMachine>(Ptr);

		datalayout =
				std::make_unique<llvm::DataLayout>(targetMachine->createDataLayout());
	}

	llvm::Triple triple;
	llvm::CodeModel::Model model;
	llvm::CodeGenOptLevel optimize;
	llvm::Reloc::Model reloc;
	const llvm::Target *target;
	llvm::TargetOptions options;
	std::unique_ptr<llvm::TargetMachine> targetMachine;
	std::unique_ptr<llvm::DataLayout> datalayout;
};

mlir::rlc::TargetInfo::TargetInfo(
		std::string triple, bool shared, bool optimize)
{
	pimpl = new TargetInfoImpl(triple, shared, optimize);
}

mlir::rlc::TargetInfo &mlir::rlc::TargetInfo::operator==(TargetInfo &&other)
{
	if (this == &other)
		return *this;
	delete pimpl;
	pimpl = other.pimpl;
	other.pimpl = nullptr;
	return *this;
}

mlir::rlc::TargetInfo::~TargetInfo() { delete pimpl; }

bool mlir::rlc::TargetInfo::isMacOS() const
{
	return pimpl->triple.isOSDarwin();
}

bool mlir::rlc::TargetInfo::isWindows() const
{
	return pimpl->triple.isOSWindows();
}

const llvm::DataLayout &mlir::rlc::TargetInfo::getDataLayout() const
{
	return *pimpl->datalayout;
}

bool mlir::rlc::TargetInfo::optimize() const
{
	return pimpl->optimize != llvm::CodeGenOptLevel::Default;
}

bool mlir::rlc::TargetInfo::isShared() const
{
	return pimpl->reloc != llvm::Reloc::Static;
}

static void compile(
		const mlir::rlc::TargetInfo &info,
		std::unique_ptr<llvm::Module> M,
		llvm::raw_pwrite_stream &OS)
{
	std::string Error;

	M->setDataLayout(*info.pimpl->datalayout);
	llvm::UpgradeDebugInfo(*M);

	auto &LLVMTM = static_cast<LLVMTargetMachine &>(*info.pimpl->targetMachine);
	auto *MMIWP = new MachineModuleInfoWrapperPass(&LLVMTM);

	llvm::legacy::PassManager manager;
	llvm::TargetLibraryInfoImpl TLII(Triple(M->getTargetTriple()));

	manager.add(new TargetLibraryInfoWrapperPass(TLII));
	manager.add(new TargetLibraryInfoWrapperPass(TLII));

	bool Err = LLVMTM.addPassesToEmitFile(
			manager, OS, nullptr, llvm::CodeGenFileType::ObjectFile, true, MMIWP);
	assert(not Err);
	manager.run(*M);
	OS.flush();
}

static mlir::LogicalResult getLinkerInvocation(
		llvm::StringRef clangPath,
		llvm::ArrayRef<string> clangInvocation,
		llvm::StringRef triple)
{
	llvm::SmallVector<const char *, 4> args;
	for (auto &arg : clangInvocation)
		args.push_back(arg.data());
	llvm::IntrusiveRefCntPtr<llvm::vfs::OverlayFileSystem> VFS =
			new llvm::vfs::OverlayFileSystem(llvm::vfs::getRealFileSystem());
	llvm::IntrusiveRefCntPtr diagIds(new clang::DiagnosticIDs());
	llvm::IntrusiveRefCntPtr opts(new clang::DiagnosticOptions());
	clang::DiagnosticsEngine engine(diagIds, opts);
	clang::driver::Driver driver(
			clangInvocation[0], triple, engine, "clang LLVM Compiler", VFS);
	driver.setCheckInputsExist(false);
	auto binDir = llvm::sys::path::parent_path(clangPath);
	auto installDir = llvm::sys::path::parent_path(binDir);
	llvm::SmallVector<char, 4> clangResourceDir;
	llvm::sys::path::append(clangResourceDir, installDir, "lib", "clang", "18");
	driver.ResourceDir =
			llvm::StringRef(clangResourceDir.data(), clangResourceDir.size());

	std::unique_ptr<clang::driver::Compilation> C(driver.BuildCompilation(args));
	if (!C)
	{
		llvm::errs()
				<< "Interal compiler error, could not create clang compilation\n";
		return mlir::failure();
	}

	const clang::driver::JobList &Jobs = C->getJobs();
	const clang::driver::Command *LinkCommand = nullptr;

	for (const auto &Job : Jobs)
	{
		if (llvm::StringRef(Job.getCreator().getName()).contains("Linker"))
		{
			LinkCommand = &Job;
			break;
		}
	}

	if (!LinkCommand)
	{
		llvm::errs()
				<< "Interal compiler error, could not deduce proper linker command\n";
		return mlir::failure();
	}

	std::string Error;
	bool failed = false;
	LinkCommand->Execute({}, &Error, &failed);
	if (failed)
	{
		llvm::errs() << Error;
		return mlir::failure();
	}

	return mlir::success();
}

static int linkLibraries(
		llvm::ToolOutputFile &library,
		llvm::StringRef clangPath,
		llvm::StringRef outputFile,
		bool shared,
		bool emitSanitizerInstrumentation,
		bool linkAgainstFuzzer,
		const std::vector<std::string> &extraObjectFiles,
		const std::vector<std::string> &rpaths,
		const mlir::rlc::TargetInfo &info)
{
	auto failedToFindClang = false;
	auto maybeRealPath =
			llvm::errorOrToExpected(llvm::sys::findProgramByName(clangPath));

	if (!maybeRealPath and (linkAgainstFuzzer or emitSanitizerInstrumentation))
	{
		llvm::consumeError(maybeRealPath.takeError());
		llvm::errs()
				<< "could not find clang, it is mandatory when using the fuzzer or the "
					 "sanitizer, install it or specify it which to "
					 "use --clang \n";
		return -1;
	}

	if (!maybeRealPath)
	{
		llvm::consumeError(maybeRealPath.takeError());
		maybeRealPath = "clang";
	}

	auto realPath = *maybeRealPath;

	std::string Errors;
	llvm::SmallVector<std::string, 4> argSource;
	argSource.push_back("clang");
	argSource.push_back(library.getFilename().str());
	if (info.isWindows())
	{
		argSource.push_back("-fuse-ld=lld");
		argSource.push_back("-Wl,-subsystem:console");
	}
	argSource.push_back("-o");
	argSource.push_back(outputFile.str());
	if (not info.isMacOS())
		argSource.push_back("-lm");
	if (shared)
	{
		argSource.push_back("--shared");
		if (not info.isWindows())
			argSource.push_back("-fPIE");
	}
	else
	{
		if (not info.isWindows())
			argSource.push_back("-no-pie");
	}
	if (emitSanitizerInstrumentation or linkAgainstFuzzer)
	{
		std::string arg("-fsanitize=");
		if (emitSanitizerInstrumentation)
			arg += "address";
		if (emitSanitizerInstrumentation and linkAgainstFuzzer)
			arg += ",";
		if (linkAgainstFuzzer)
			arg += "fuzzer";
		argSource.push_back(arg);
	}

	for (auto rpathEntry : rpaths)
		argSource.push_back("-Wl,-rpath," + rpathEntry);

	for (auto extraObject : extraObjectFiles)
		argSource.push_back(extraObject);

	if (getLinkerInvocation(*maybeRealPath, argSource, info.pimpl->triple.str())
					.failed())
		return -1;

	Errors.clear();
	auto perms = llvm::cantFail(
			llvm::errorOrToExpected(llvm::sys::fs::getPermissions(outputFile)));
	llvm::sys::fs::setPermissions(
			outputFile, llvm::sys::fs::perms::owner_exe | perms);
	return 0;
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_RLCBACKENDPASS
#include "rlc/dialect/Passes.inc"

	struct RLCBackEndPass: impl::RLCBackEndPassBase<RLCBackEndPass>
	{
		using impl::RLCBackEndPassBase<RLCBackEndPass>::RLCBackEndPassBase;
		void runOnOperation()
		{
			LLVMContext LLVMcontext;
			auto Module = mlir::translateModuleToLLVMIR(
					getOperation(), LLVMcontext, getOperation().getName().value());
			assert(Module);
			Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

			runOptimizer(*Module, targetInfo->optimize(), emitSanitizer, emitFuzzer);

			if (dumpIR)
			{
				Module->print(*OS, nullptr);
				return;
			}

			error_code errorCompile;
			std::string realOut = outputFile;
			if (realOut != "-")
				realOut = compileOnly ? outputFile : outputFile + ".o";
			llvm::ToolOutputFile library(
					realOut, errorCompile, llvm::sys::fs::OpenFlags::OF_None);

			error_code error;
			if (errorCompile)
			{
				errs() << error.message();
				signalPassFailure();
				return;
			}

			compile(*targetInfo, std::move(Module), library.os());
			library.os().close();

			if (compileOnly)
			{
				library.keep();
				return;
			}

			if (linkLibraries(
							library,
							clangPath,
							outputFile,
							targetInfo->isShared(),
							emitSanitizer,
							emitFuzzer,
							*extraObjectFiles,
							*rpaths,
							*targetInfo) != 0)
				signalPassFailure();
		}
	};
}	 // namespace mlir::rlc

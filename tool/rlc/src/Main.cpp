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
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h"
#include "mlir/IR/Verifier.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllTranslations.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Export.h"
#include "rlc/dialect/Conversion.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/EmitMain.hpp"
#include "rlc/dialect/TypeCheck.hpp"
#include "rlc/parser/Parser.hpp"
#include "rlc/utils/Error.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

static llvm::codegen::RegisterCodeGenFlags Flags;
static cl::OptionCategory astDumperCategory("rlc options");
static cl::opt<string> InputFilePath(
		cl::Positional,
		cl::desc("<input-file>"),
		cl::init("-"),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpTokens(
		"token",
		cl::desc("dumps the tokens and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpUncheckedAST(
		"unchecked",
		cl::desc("dumps the unchcked ast and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> compileOnly(
		"compile",
		cl::desc("compile but do not link"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<std::string> clangPath(
		"clang",
		cl::desc("clang to used as a linker"),
		cl::init("clang"),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpRLC(
		"rlc",
		cl::desc("dumps the ast and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpAST(
		"ast",
		cl::desc("dumps the ast and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> hidePosition(
		"hide-position",
		cl::desc("does not print source file position in ast"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpMLIR(
		"mlir",
		cl::desc("dumps the mlir and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> Optimize(
		"O2", cl::desc("Optimize"), cl::init(false), cl::cat(astDumperCategory));

static cl::opt<bool> dumpIR(
		"ir",
		cl::desc("dumps the llvm-ir and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<string> outputFile(
		"o", cl::desc("<output-file>"), cl::init("-"), cl::cat(astDumperCategory));

static void initLLVM()
{
	using namespace llvm;
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmPrinters();
	InitializeAllAsmParsers();

	PassRegistry &Registry = *llvm::PassRegistry::getPassRegistry();
	initializeCore(Registry);

	initializeScalarOpts(Registry);
	initializeObjCARCOpts(Registry);
	initializeVectorization(Registry);
	initializeIPO(Registry);
	initializeAnalysis(Registry);
	initializeTransformUtils(Registry);
	initializeInstCombine(Registry);
	initializeAggressiveInstCombine(Registry);
	initializeInstrumentation(Registry);
	initializeTarget(Registry);
	// For codegen passes, only passes that do IR to IR transformation are
	// supported.
	initializeExpandMemCmpPassPass(Registry);
	initializeCodeGenPreparePass(Registry);
	initializeAtomicExpandPass(Registry);
	initializeRewriteSymbolsLegacyPassPass(Registry);
	initializeWinEHPreparePass(Registry);
	initializeSafeStackLegacyPassPass(Registry);
	initializeSjLjEHPreparePass(Registry);
	initializePreISelIntrinsicLoweringLegacyPassPass(Registry);
	initializeGlobalMergePass(Registry);
	initializeIndirectBrExpandPassPass(Registry);
	initializeInterleavedLoadCombinePass(Registry);
	initializeInterleavedAccessPass(Registry);
	initializeUnreachableBlockElimLegacyPassPass(Registry);
	initializeExpandReductionsPass(Registry);
	initializeWasmEHPreparePass(Registry);
	initializeWriteBitcodePassPass(Registry);
	initializeHardwareLoopsPass(Registry);
}

static void optimize(llvm::Module &M)
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
	if (Optimize)
	{
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
		MPM.run(M, MAM);
	}
}

static void compile(
		std::unique_ptr<llvm::Module> M, llvm::raw_pwrite_stream &OS)
{
	std::string Error;
	llvm::Triple triple(M->getTargetTriple());
	const auto *TheTarget = llvm::TargetRegistry::lookupTarget("", triple, Error);
	TargetOptions options =
			llvm::codegen::InitTargetOptionsFromCodeGenFlags(triple);

	CodeGenOpt::Level OLvl =
			Optimize ? CodeGenOpt::Aggressive : CodeGenOpt::Default;
	auto *Ptr = TheTarget->createTargetMachine(
			triple.getTriple(),
			"",
			"",
			options,
			llvm::codegen::getRelocModel(),
			M->getCodeModel(),
			OLvl);
	unique_ptr<TargetMachine> Target(Ptr);

	M->setDataLayout(Target->createDataLayout());
	llvm::UpgradeDebugInfo(*M);

	auto &LLVMTM = static_cast<LLVMTargetMachine &>(*Target);
	auto *MMIWP = new MachineModuleInfoWrapperPass(&LLVMTM);

	llvm::legacy::PassManager manager;
	llvm::TargetLibraryInfoImpl TLII(Triple(M->getTargetTriple()));

	manager.add(new TargetLibraryInfoWrapperPass(TLII));
	manager.add(new TargetLibraryInfoWrapperPass(TLII));

	bool Err = Target->addPassesToEmitFile(
			manager, OS, nullptr, CGFT_ObjectFile, true, MMIWP);
	assert(not Err);
	manager.run(*M);
}

class RlcExitOnError
{
	public:
	RlcExitOnError(mlir::SourceMgrDiagnosticHandler &handler): handler(&handler)
	{
	}

	void operator()(llvm::Error error)
	{
		auto otherErrors =
				llvm::handleErrors(std::move(error), [this](const rlc::RlcError &e) {
					handler->emitDiagnostic(
							e.getPosition(), e.getText(), mlir::DiagnosticSeverity::Error);
					exit(-1);
				});

		exitOnErrBase(std::move(otherErrors));
	}

	template<typename T>
	T operator()(llvm::Expected<T> maybeObj)
	{
		if (not maybeObj)
			(*this)(maybeObj.takeError());

		return std::move(*maybeObj);
	}

	private:
	static ExitOnError exitOnErrBase;
	mlir::SourceMgrDiagnosticHandler *handler = nullptr;
};

ExitOnError RlcExitOnError::exitOnErrBase;

int main(int argc, char *argv[])
{
	llvm::cl::HideUnrelatedOptions(astDumperCategory);
	cl::ParseCommandLineOptions(argc, argv);
	InitLLVM X(argc, argv);
	initLLVM();
	mlir::registerAllTranslations();

	SourceMgr sourceManager;
	std::string AbslutePath;
	sourceManager.AddIncludeFile(InputFilePath, SMLoc(), AbslutePath);

	mlir::MLIRContext context;
	mlir::SourceMgrDiagnosticHandler diagnostic(sourceManager, &context);
	RlcExitOnError exitOnErr(diagnostic);

	error_code error;
	raw_fd_ostream OS(outputFile, error);
	if (error)
	{
		errs() << error.message();
		return -1;
	}

	const auto inputFileName = llvm::sys::path::filename(InputFilePath);
	if (dumpTokens)
	{
		Lexer lexer(sourceManager.getMemoryBuffer(1)->getBuffer().data());
		lexer.print(OS);
		return 0;
	}

	mlir::DialectRegistry Registry;
	Registry.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
	mlir::registerLLVMDialectTranslation(Registry);
	context.appendDialectRegistry(Registry);
	context.loadAllAvailableDialects();

	Parser parser(
			&context,
			sourceManager.getMemoryBuffer(1)->getBuffer().str(),
			AbslutePath);
	auto ast = exitOnErr(parser.system());

	if (dumpUncheckedAST)
	{
		mlir::OpPrintingFlags flags;
		if (not hidePosition)
			flags.enableDebugInfo(true);
		ast->print(OS, flags);
		return 0;
	}

	mlir::PassManager typeChecker(&context);
	typeChecker.addPass(rlc::createRLCTypeCheck());
	if (typeChecker.run(ast).failed())
		return -1;

	if (dumpAST)
	{
		mlir::OpPrintingFlags flags;
		if (not hidePosition)
			flags.enableDebugInfo(true);
		ast.print(OS, flags);
		if (mlir::verify(ast).failed())
			return -1;
		return 0;
	}

	if (dumpRLC)
	{
		mlir::OpPrintingFlags flags;
		if (not hidePosition)
			flags.enableDebugInfo(true);
		ast->print(OS, flags);

		if (mlir::verify(ast).failed())
			return -1;
		return 0;
	}

	mlir::PassManager manager(&context);
	manager.addPass(rlc::createRLCLowerArrayCalls());
	manager.addPass(rlc::createRLCToCfLoweringPass());
	manager.addPass(rlc::createRLCLowerActions());
	manager.addPass(rlc::createRLCToLLVMLoweringPass());
	if (ast.lookupSymbol("main() -> !rlc.int") != nullptr)
	{
		manager.addPass(createEmitMainPass());
	}
	if (manager.run(ast).failed())
		return -1;

	if (dumpMLIR)
	{
		mlir::OpPrintingFlags flags;
		if (not hidePosition)
			flags.enableDebugInfo(true);
		ast.print(OS, flags);
		if (mlir::verify(ast).failed())
			return -1;
		return 0;
	}

	LLVMContext LLVMcontext;
	auto Module = mlir::translateModuleToLLVMIR(ast, LLVMcontext, inputFileName);
	Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

	assert(Module);
	optimize(*Module);
	if (dumpIR)
	{
		Module->print(OS, nullptr);
		return 0;
	}

	error_code errorCompile;
	llvm::ToolOutputFile library(
			compileOnly ? outputFile : outputFile + ".o",
			errorCompile,
			llvm::sys::fs::OpenFlags::OF_None);

	if (errorCompile)
	{
		errs() << error.message();
		return -1;
	}

	compile(std::move(Module), library.os());

	if (compileOnly)
	{
		library.keep();
		return 0;
	}

	auto realPath = exitOnErr(
			llvm::errorOrToExpected(llvm::sys::findProgramByName(clangPath)));
	std::string Errors;
	auto res = llvm::sys::ExecuteAndWait(
			realPath,
			{ "clang", library.getFilename(), "-o", outputFile, "-lm" },
			llvm::None,
			{},
			0,
			0,
			&Errors);
	llvm::errs() << Errors;

	Errors.clear();
	auto perms = exitOnErr(
			llvm::errorOrToExpected(llvm::sys::fs::getPermissions(outputFile)));
	llvm::sys::fs::setPermissions(
			outputFile, llvm::sys::fs::perms::owner_exe | perms);

	return res;
}

#include "llvm/CodeGen/CommandFlags.h"
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
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "mlir/IR/Verifier.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllTranslations.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Import.h"
#include "mlir/Target/LLVMIR/ModuleTranslation.h"
#include "mlir/Transforms/Passes.h"
#include "rlc/backend/BackEnd.hpp"
#include "rlc/conversions/RLCToC.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Passes.hpp"
#include "rlc/parser/MultiFileParser.hpp"
#include "rlc/python/Interfaces.hpp"
#include "rlc/python/Passes.hpp"
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

cl::list<std::string> ExtraObjectFiles(
		cl::Positional, cl::desc("<extra-object-files>"));
cl::list<std::string> IncludeDirs("i", cl::desc("<include dirs>"));

static cl::opt<bool> dumpTokens(
		"token",
		cl::desc("dumps the tokens and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpDot(
		"dot",
		cl::desc("dump dot of actions"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpUncheckedAST(
		"unchecked",
		cl::desc("dumps the unchcked ast and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpFlatIR(
		"flattened",
		cl::desc("dump ir before lowering to llvm dialect"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpCheckedAST(
		"type-checked",
		cl::desc("dumps the type checked ast before template expansion and exits"),
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

static cl::opt<bool> dumpPythonAST(
		"python-ast",
		cl::desc("dumps the ast of python-ast and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpGodotWrapper(
		"godot",
		cl::desc("dumps the godot wrapper and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpCWrapper(
		"header",
		cl::desc("dumps the c wrapper and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpPythonWrapper(
		"python",
		cl::desc("dumps the ast of python and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpAfterImplicit(
		"after-implicit",
		cl::desc("dumps the ast after implicit function expansions and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpRLC(
		"rlc",
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

static cl::opt<bool> shared(
		"shared",
		cl::desc("compile as shared lib"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> dumpIR(
		"ir",
		cl::desc("dumps the llvm-ir and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<string> outputFile(
		"o", cl::desc("<output-file>"), cl::init("-"), cl::cat(astDumperCategory));

static cl::opt<bool> emitPreconditionChecks(
		"emit-precondition-checks",
		cl::desc("emits precondition checks for functions"),
		cl::init(true),
		cl::cat(astDumperCategory));

static cl::opt<bool> Optimize(
		"O2",
		cl::desc("Optimize"),
		cl::callback([](const bool &value) {
			emitPreconditionChecks.setInitialValue(!value);
		}),
		cl::init(false),
		cl::cat(astDumperCategory));

static void configurePassManager(
		const llvm::SmallVector<std::string, 4> &includeDirs,
		const std::string &inputFile,
		llvm::raw_ostream &OS,
		mlir::PassManager &manager,
		mlir::rlc::TargetInfo &targetInfo)
{
	manager.addPass(mlir::rlc::createParseFilePass({ &includeDirs, inputFile }));
	if (dumpUncheckedAST)
	{
		manager.addPass(mlir::rlc::createPrintIRPass({ &OS, hidePosition }));
		return;
	}

	if (dumpDot)
	{
		manager.addPass(mlir::rlc::createUncheckedAstToDotPass({ &OS }));
		return;
	}

	manager.addPass(mlir::rlc::createSortActionsPass());
	manager.addPass(mlir::rlc::createEmitEnumEntitiesPass());
	manager.addPass(mlir::rlc::createTypeCheckEntitiesPass());
	manager.addPass(mlir::rlc::createTypeCheckPass());

	if (dumpCheckedAST)
	{
		manager.addPass(mlir::rlc::createPrintIRPass({ &OS, hidePosition }));
		return;
	}

	manager.addPass(mlir::rlc::createEmitImplicitDestructorInvocationsPass());
	manager.addPass(mlir::rlc::createEmitImplicitDestructorsPass());
	manager.addPass(mlir::rlc::createLowerForFieldOpPass());
	manager.addPass(mlir::rlc::createLowerIsOperationsPass());
	manager.addPass(mlir::rlc::createLowerAssignPass());
	manager.addPass(mlir::rlc::createLowerConstructOpPass());
	manager.addPass(mlir::rlc::createLowerDestructorsPass());
	manager.addPass(mlir::rlc::createInstantiateTemplatesPass());

	manager.addPass(mlir::rlc::createLowerConstructOpPass());
	manager.addPass(mlir::rlc::createLowerAssignPass());
	manager.addPass(mlir::rlc::createEmitImplicitDestructorsPass());
	manager.addPass(mlir::rlc::createLowerDestructorsPass());

	manager.addPass(mlir::rlc::createEmitImplicitAssignPass());
	manager.addPass(mlir::rlc::createEmitImplicitInitPass());
	manager.addPass(mlir::rlc::createLowerArrayCallsPass());
	manager.addPass(mlir::rlc::createAddOutOfBoundsCheckPass());

	if (dumpCWrapper)
	{
		manager.addPass(mlir::rlc::createPrintCHeaderPass({ &OS }));
		return;
	}

	if (dumpGodotWrapper)
	{
		manager.addPass(mlir::rlc::createPrintGodotPass({ &OS }));
		return;
	}

	if (dumpPythonWrapper or dumpPythonAST)
	{
		manager.addPass(mlir::rlc::createSortTypeDeclarationsPass());
		manager.addPass(mlir::python::createRLCTypesToPythonTypesPass());
		manager.addPass(mlir::python::createRLCToPythonPass());
		if (dumpPythonAST)
			manager.addPass(mlir::rlc::createPrintIRPass({ &OS, hidePosition }));
		else
			manager.addPass(mlir::python::createPrintPythonPass({ &OS }));
		return;
	}

	if (dumpRLC)
	{
		manager.addPass(mlir::rlc::createPrintIRPass({ &OS, hidePosition }));
		return;
	}

	manager.addPass(mlir::rlc::createLowerActionPass());
	manager.addPass(mlir::rlc::createLowerAssignPass());

	if (dumpAfterImplicit)
	{
		manager.addPass(mlir::rlc::createPrintIRPass({ &OS, hidePosition }));
		return;
	}

	manager.addPass(mlir::rlc::createExtractPreconditionPass());

	if (emitPreconditionChecks)
	{
		manager.addPass(mlir::rlc::createAddPreconditionsCheckPass());
	}

	manager.addPass(mlir::rlc::createLowerAssertsPass());
	manager.addPass(mlir::rlc::createLowerToCfPass());
	manager.addPass(mlir::rlc::createActionStatementsToCoroPass());
	manager.addPass(mlir::rlc::createStripFunctionMetadataPass());
	manager.addPass(mlir::rlc::createRewriteCallSignaturesPass());
	if (dumpFlatIR)
	{
		manager.addPass(mlir::rlc::createPrintIRPass({ &OS, hidePosition }));
		return;
	}
	manager.addPass(mlir::rlc::createLowerToLLVMPass());
	if (not compileOnly)
		manager.addPass(mlir::rlc::createEmitMainPass());
	manager.addPass(mlir::createCanonicalizerPass());

	if (dumpMLIR)
	{
		manager.addPass(mlir::rlc::createPrintIRPass({ &OS, hidePosition }));
		return;
	}
	manager.addPass(mlir::rlc::createRLCBackEndPass(
			mlir::rlc::RLCBackEndPassOptions{ &OS,
																				clangPath,
																				outputFile,
																				&ExtraObjectFiles,
																				dumpIR,
																				compileOnly,
																				&targetInfo }));
}

static int run(
		mlir::MLIRContext &context,
		const llvm::SmallVector<std::string, 4> &includeDirs,
		const std::string &inputFile,
		llvm::raw_ostream &OS,
		mlir::rlc::TargetInfo &info)
{
	mlir::PassManager manager(&context);
	configurePassManager(includeDirs, inputFile, OS, manager, info);

	auto ast = mlir::ModuleOp::create(
			mlir::FileLineColLoc::get(&context, inputFile, 0, 0), inputFile);

	auto mlirDl = mlir::translateDataLayout(info.getDataLayout(), &context);
	ast->setAttr("rlc.target_datalayout", mlirDl);

	if (manager.run(ast).failed())
	{
		mlir::OpPrintingFlags flags;
		if (not hidePosition)
			flags.enableDebugInfo(true);
		ast.print(llvm::errs(), flags);

		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	llvm::cl::HideUnrelatedOptions(astDumperCategory);
	cl::ParseCommandLineOptions(argc, argv);
	InitLLVM X(argc, argv);
	mlir::rlc::initLLVM();
	mlir::registerAllTranslations();

	mlir::MLIRContext context(mlir::MLIRContext::Threading::DISABLED);
	mlir::registerBuiltinDialectTranslation(context);
	mlir::DialectRegistry Registry;
	Registry.insert<
			mlir::BuiltinDialect,
			mlir::memref::MemRefDialect,
			mlir::rlc::RLCDialect,
			mlir::DLTIDialect,
			mlir::index::IndexDialect>();
	std::string targetTriple = llvm::sys::getDefaultTargetTriple();
	mlir::rlc::TargetInfo targetInfo(targetTriple, shared, Optimize);

	mlir::registerLLVMDialectTranslation(Registry);
	context.appendDialectRegistry(Registry);
	context.loadAllAvailableDialects();
	auto pathToRlc = llvm::sys::fs::getMainExecutable(argv[0], (void *) &main);
	auto rlcDirectory =
			llvm::sys::path::parent_path(pathToRlc).str() + "/../lib/rlc/stdlib";
	llvm::SmallVector<std::string, 4> includes(
			IncludeDirs.begin(), IncludeDirs.end());
	includes.push_back(rlcDirectory);

	error_code error;
	raw_fd_ostream OS(outputFile, error);
	if (error)
	{
		errs() << error.message();
		return -1;
	}

	if (dumpTokens)
	{
		std::string fullPath;
		::rlc::MultiFileParser parser(&context, includes);
		parser.getSourceMgr().AddIncludeFile(InputFilePath, SMLoc(), fullPath);
		Lexer lexer(parser.getSourceMgr().getMemoryBuffer(1)->getBuffer().data());
		lexer.print(OS);
		return 0;
	}

	return run(context, includes, InputFilePath, OS, targetInfo);
}

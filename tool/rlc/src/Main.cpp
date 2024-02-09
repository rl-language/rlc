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
#include "rlc/driver/Driver.hpp"
#include "rlc/parser/MultiFileParser.hpp"
#include "rlc/python/Interfaces.hpp"
#include "rlc/python/Passes.hpp"

#if NDEBUG
static constexpr const bool isDebug = false;
#else
static constexpr const bool isDebug = true;
#endif

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

static cl::list<std::string> ExtraObjectFiles(
		cl::Positional, cl::desc("<extra-object-files>"));
static cl::list<std::string> IncludeDirs("i", cl::desc("<include dirs>"));

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

static cl::opt<bool> timing(
		"timing",
		cl::desc("mesure time"),
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

static cl::opt<bool> printIROnFailure(
		"print-ir-on-failure",
		cl::desc("prints ir on failure"),
		cl::init(isDebug),
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

static cl::opt<bool> debugInfo(
		"g",
		cl::desc("Add debug info to the output"),
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

static cl::opt<bool> emitBoundChecks(
		"emit-bound-checks",
		cl::desc("emit bound checks for array accesses"),
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

static cl::opt<bool> ExpectFail(
		"expect-fail",
		cl::desc("return error code 0 on failure and error code 1 on success"),
		cl::init(false),
		cl::cat(astDumperCategory));

int main(int argc, char *argv[]);

static mlir::rlc::Driver::Request getRequest()
{
	using namespace mlir::rlc;
	if (dumpUncheckedAST)
		return Driver::Request::dumpUncheckedAST;
	if (dumpDot)
		return Driver::Request::dumpDot;
	if (dumpCheckedAST)
		return Driver::Request::dumpCheckedAST;
	if (dumpCWrapper)
		return Driver::Request::dumpCWrapper;
	if (dumpGodotWrapper)
		return Driver::Request::dumpGodotWrapper;
	if (dumpPythonAST)
		return Driver::Request::dumpPythonAST;
	if (dumpPythonWrapper)
		return Driver::Request::dumpPythonWrapper;
	if (dumpRLC)
		return Driver::Request::dumpRLC;
	if (dumpAfterImplicit)
		return Driver::Request::dumpAfterImplicit;
	if (dumpFlatIR)
		return Driver::Request::dumpFlatIR;
	if (dumpMLIR)
		return Driver::Request::dumpMLIR;
	if (compileOnly)
		return Driver::Request::compile;
	return Driver::Request::executable;
}

static mlir::rlc::Driver configureDriver(
		char *argv[],
		llvm::SourceMgr &srcManager,
		llvm::raw_ostream &OS,
		mlir::rlc::TargetInfo &info)
{
	using namespace mlir::rlc;
	auto pathToRlc = llvm::sys::fs::getMainExecutable(argv[0], (void *) &main);
	auto rlcDirectory =
			llvm::sys::path::parent_path(pathToRlc).str() + "/../lib/rlc/stdlib";
	llvm::SmallVector<std::string, 4> includes(
			IncludeDirs.begin(), IncludeDirs.end());
	auto directory = llvm::sys::path::parent_path(InputFilePath);
	includes.push_back(directory.str());
	includes.push_back(rlcDirectory);

	Driver driver(srcManager, InputFilePath, outputFile, OS);
	driver.setRequest(getRequest());
	driver.setDebug(debugInfo);
	driver.setHidePosition(hidePosition);
	driver.setEmitPreconditionChecks(emitPreconditionChecks);
	driver.setDumpIR(dumpIR);
	driver.setClangPath(clangPath);
	driver.setIncludeDirs(includes);
	driver.setExtraObjectFile(ExtraObjectFiles);
	driver.setTargetInfo(&info);
	driver.setEmitBoundChecks(emitBoundChecks);

	return driver;
}

static int run(
		mlir::MLIRContext &context,
		const mlir::rlc::Driver &driver,
		const std::string &inputFile,
		const mlir::rlc::TargetInfo &info)
{
	mlir::PassManager manager(&context);
	driver.configurePassManager(manager);

	if (timing)
		manager.enableTiming();

	auto ast = mlir::ModuleOp::create(
			mlir::FileLineColLoc::get(&context, inputFile, 0, 0), inputFile);

	auto mlirDl = mlir::translateDataLayout(info.getDataLayout(), &context);
	ast->setAttr("rlc.target_datalayout", mlirDl);

	if (manager.run(ast).failed())
	{
		if (printIROnFailure)
		{
			mlir::OpPrintingFlags flags;
			if (not hidePosition)
				flags.enableDebugInfo(true);
			ast.print(llvm::errs(), flags);
		}
		return -1;
	}
	return 0;
}

static int errorCode(int error_code)
{
	if (ExpectFail and error_code != 0)
		return 0;
	if (ExpectFail and error_code == 0)
		return -1;
	return error_code;
}

int main(int argc, char *argv[])
{
	llvm::cl::HideUnrelatedOptions(astDumperCategory);
	cl::ParseCommandLineOptions(argc, argv);
	InitLLVM X(argc, argv);
	mlir::rlc::initLLVM();
	mlir::registerAllTranslations();

	mlir::MLIRContext context(mlir::MLIRContext::Threading::DISABLED);
	llvm::SourceMgr sourceManager;
	mlir::SourceMgrDiagnosticHandler diagnostic(
			sourceManager, &context, [](mlir::Location) { return true; });
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

	if (outputFile == "-" and
			(getRequest() == mlir::rlc::Driver::Request::executable or
			 getRequest() == mlir::rlc::Driver::Request::compile) and
			not dumpIR)
	{
		errs()
				<< "cannot write on a executable on stdout, specify a path with -o\n";
		return errorCode(-1);
	}

	error_code error;
	raw_fd_ostream OS(outputFile, error);
	if (error)
	{
		errs() << error.message();
		return errorCode(-1);
	}

	if (dumpTokens)
	{
		std::string fullPath;
		sourceManager.AddIncludeFile(InputFilePath, SMLoc(), fullPath);
		Lexer lexer(sourceManager.getMemoryBuffer(1)->getBuffer().data());
		lexer.print(OS);
		return errorCode(0);
	}

	auto driver = configureDriver(argv, sourceManager, OS, targetInfo);
	return errorCode(run(context, driver, InputFilePath, targetInfo));
}

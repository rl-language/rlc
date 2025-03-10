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
#include "llvm/Support/Process.h"
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
#include "rlc/LibNames.hpp"
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

static cl::opt<std::string> abortSymbol(
		"abort-symbol",
		cl::desc("abort symbol called by assertions"),
		cl::init(""),
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

static cl::opt<bool> dumpBeforeTemplate(
		"before-template",
		cl::desc("dumps ir before template expansion"),
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

static cl::opt<bool> hideDataLayout(
		"hide-dl",
		cl::desc("does not the datalayout of the file"),
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

static cl::opt<bool> verbose(
		"verbose",
		cl::desc("compile as shared lib"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::alias v1("v", cl::aliasopt(verbose));

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
			emitBoundChecks.setInitialValue(!value);
		}),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> ExpectFail(
		"expect-fail",
		cl::desc("return error code 0 on failure and error code 1 on success"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<std::string> customTargetTriple(
		"target",
		cl::desc("specify a target triple, if empty the default target triple will "
						 "be used instead"),
		cl::init(""),
		cl::cat(astDumperCategory));

static cl::opt<std::string> customFuzzerLibPath(
		"fuzzer-lib",
		cl::desc("path to the fuzzer library."),
		cl::init(""),
		cl::cat(astDumperCategory));

static cl::opt<bool> formatFile(
		"format",
		cl::desc("print the file formatted"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> generateDependecyFile(
		"MD",
		cl::desc("creates a dependency file called OUTPUT.dep"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<std::string> customRuntime(
		"runtime-lib",
		cl::desc("path to runtime"),
		cl::init(""),
		cl::cat(astDumperCategory));

static cl::opt<bool> sanitize(
		"sanitize",
		cl::desc("emit the sanitizer instrumentation"),
		cl::init(false),
		cl::cat(astDumperCategory));

static cl::opt<bool> pylib(
		"pylib",
		cl::desc("link against python interpreter"),
		cl::init(false),
		cl::cat(astDumperCategory),
		cl::callback([](const bool &value) {
			abortSymbol.setInitialValue("rl_py_abort");
		}));

static cl::opt<std::string> customPythonLibPath(
		"pyrlc-lib",
		cl::desc("path to the pyrlc library."),
		cl::init(""),
		cl::cat(astDumperCategory),
		cl::callback([](const std::string &value) {
			pylib.setInitialValue(true);
		}));

static cl::opt<bool> emitFuzzer(
		"fuzzer",
		cl::desc("emit a fuzzer."),	 // TODO consider passing the action name here.
		cl::init(false),
		cl::cat(astDumperCategory),
		cl::callback([](const bool &value) { sanitize.setInitialValue(value); }));

cl::list<std::string> RPath("rpath", cl::desc("<rpath>"));

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
	if (dumpBeforeTemplate)
		return Driver::Request::dumpBeforeTemplate;
	if (formatFile)
		return Driver::Request::format;
	return Driver::Request::executable;
}

static std::string toNative(llvm::StringRef path)
{
	SmallVector<char> converted;
	llvm::sys::path::native(Twine(path), converted);
	std::string out(converted.begin(), converted.end());
	return out;
}

static mlir::rlc::Driver configureDriver(
		char *argv[],
		llvm::SourceMgr &srcManager,
		llvm::raw_ostream &OS,
		mlir::rlc::TargetInfo &info)
{
	using namespace mlir::rlc;
	llvm::SmallVector<std::string, 4> inputs = { InputFilePath };
	std::vector<std::string> objectFiles;
	for (auto &file : ExtraObjectFiles)
	{
		if (llvm::StringRef(file).ends_with(".rl"))
			inputs.push_back(file);
		else
			objectFiles.push_back(file);
	}

	auto pathToRlc = llvm::sys::fs::getMainExecutable(argv[0], (void *) &main);
	auto rlcDirectory =
			llvm::sys::path::parent_path(pathToRlc).str() + "/../lib/rlc/stdlib";
	rlcDirectory = toNative(rlcDirectory);
	llvm::SmallVector<std::string, 4> includes(
			IncludeDirs.begin(), IncludeDirs.end());
	for (auto &file : inputs)
	{
		auto directory = llvm::sys::path::parent_path(file);
		includes.push_back(directory.str());
	}
	includes.push_back(rlcDirectory);

	if (pylib)
	{
		auto envVal = llvm::sys::Process::GetEnv("RLC_PYTHON_LIB_PATH");
		if (envVal.has_value() and customPythonLibPath.empty())
			customPythonLibPath = *envVal;

		string pyrlcLibPath = customPythonLibPath.empty()
															? llvm::sys::path::parent_path(pathToRlc).str() +
																		"/../lib/" + PYRLC_LIBRARY_FILENAME
															: customPythonLibPath.getValue();
		pyrlcLibPath = toNative(pyrlcLibPath);

		objectFiles.push_back(pyrlcLibPath);
		RPath.addValue(llvm::sys::path::parent_path(pyrlcLibPath).str());
	}

	if (emitFuzzer)
	{
		string fuzzerLibPath = customFuzzerLibPath.empty()
															 ? llvm::sys::path::parent_path(pathToRlc).str() +
																		 "/../lib/" + FUZZER_LIBRARY_FILENAME
															 : customFuzzerLibPath.getValue();
		fuzzerLibPath = toNative(fuzzerLibPath);

		objectFiles.push_back(fuzzerLibPath);
		RPath.addValue(llvm::sys::path::parent_path(fuzzerLibPath).str());
	}

	string runtimeLibPath = customRuntime;
	auto envVal = llvm::sys::Process::GetEnv("RLC_RUNTIME_LIB_PATH");
	if (runtimeLibPath.empty())
	{
		if (envVal.has_value())
			runtimeLibPath = toNative(*envVal);
		else
			runtimeLibPath = toNative(
					llvm::sys::path::parent_path(pathToRlc).str() + "/../lib/" +
					RUNTIME_LIBRARY_FILENAME);
	}

	objectFiles.push_back(runtimeLibPath);

	Driver driver(srcManager, inputs, outputFile, OS);
	driver.setRequest(getRequest());
	driver.setDebug(debugInfo);
	driver.setEmitDependencyFile(generateDependecyFile);
	driver.setHidePosition(hidePosition);
	driver.setEmitPreconditionChecks(emitPreconditionChecks);
	driver.setDumpIR(dumpIR);
	driver.setClangPath(clangPath);
	driver.setIncludeDirs(includes);
	driver.setExtraObjectFile(objectFiles);
	driver.setRPath(RPath);
	driver.setEmitFuzzer(emitFuzzer);
	driver.setEmitSanitizer(sanitize);
	driver.setTargetInfo(&info);
	driver.setEmitBoundChecks(emitBoundChecks);
	driver.setVerbose(verbose);
	driver.setAbortSymbol(abortSymbol);

	return driver;
}

static int run(
		mlir::MLIRContext &context,
		const mlir::rlc::Driver &driver,
		const std::string &inputFile,
		const mlir::rlc::TargetInfo &info)
{
	mlir::PassManager manager(&context);
	manager.enableVerifier(isDebug);
	driver.configurePassManager(manager);

	if (timing)
		manager.enableTiming();

	auto ast = mlir::ModuleOp::create(
			mlir::FileLineColLoc::get(&context, inputFile, 0, 0), inputFile);

	if (not hideDataLayout)
	{
		auto mlirDl = mlir::translateDataLayout(info.getDataLayout(), &context);
		ast->setAttr("rlc.target_datalayout", mlirDl);
	}

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
	if (customTargetTriple != "")
		targetTriple = customTargetTriple;
	mlir::rlc::TargetInfo targetInfo(targetTriple, shared, Optimize);

	mlir::registerLLVMDialectTranslation(Registry);
	context.appendDialectRegistry(Registry);
	context.loadAllAvailableDialects();

	if (outputFile == "-" and
			(getRequest() == mlir::rlc::Driver::Request::executable or
			 getRequest() == mlir::rlc::Driver::Request::compile) and
			not dumpIR and not dumpTokens)
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

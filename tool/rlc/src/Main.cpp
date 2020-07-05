#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/BuiltinEntities.hpp"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/lowerer/Lowerer.hpp"
#include "rlc/parser/Parser.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

cl::OptionCategory astDumperCategory("rlc options");
cl::opt<string> InputFileName(
		cl::Positional,
		cl::desc("<input-file>"),
		cl::init("-"),
		cl::cat(astDumperCategory));

cl::opt<bool> dumpAST(
		"ast",
		cl::desc("dumps the ast and exits"),
		cl::init(false),
		cl::cat(astDumperCategory));

cl::opt<string> outputFile(
		"o", cl::desc("<output-file>"), cl::init("-"), cl::cat(astDumperCategory));

ExitOnError exitOnErr;
int main(int argc, char* argv[])
{
	cl::ParseCommandLineOptions(argc, argv);
	auto errorOrBuffer = MemoryBuffer::getFileOrSTDIN(InputFileName);
	auto buffer = exitOnErr(errorOrToExpected(move(errorOrBuffer)));

	error_code error;
	raw_fd_ostream OS(outputFile, error, sys::fs::F_None);
	if (error)
	{
		errs() << error.message();
		return -1;
	}

	Parser parser(buffer->getBufferStart(), InputFileName);
	auto ast = exitOnErr(parser.system());
	rlc::addBuilints(ast);
	rlc::addBuilintsEntities(ast);
	TypeDB db;
	exitOnErr(ast.typeCheck(SymbolTable(), db));
	if (dumpAST)
	{
		ast.print(OS);
		return 0;
	}

	Lowerer lowerer;
	exitOnErr(lowerer.lowerSystem(ast));
	if (!lowerer.verify(errs()))
		return -1;

	WriteBitcodeToFile(lowerer.getModule(0), OS);

	return 0;
}

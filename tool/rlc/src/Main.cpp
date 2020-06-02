#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
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

ExitOnError exitOnErr;
int main(int argc, char* argv[])
{
	cl::ParseCommandLineOptions(argc, argv);
	auto errorOrBuffer = MemoryBuffer::getFileOrSTDIN(InputFileName);
	auto buffer = exitOnErr(errorOrToExpected(move(errorOrBuffer)));
	Parser parser(buffer->getBufferStart(), InputFileName);
	auto ast = exitOnErr(parser.system());
	ast.dump();

	return 0;
}

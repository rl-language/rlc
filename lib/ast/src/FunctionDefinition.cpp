#include "rlc/ast/FunctionDefinition.hpp"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;
using namespace rlc;

void FunctionDefinition::print(
		raw_ostream& OS, size_t indents, bool dumpPosition) const
{
	declaration.print(OS, indents, dumpPosition);
	OS.indent(indents);
	OS << "body:\n";
	body.print(OS, indents + 1);
}
void FunctionDefinition::dump() const { print(outs()); }

#include "rlc/ast/EntityDeclaration.hpp"

using namespace rlc;
using namespace std;
using namespace llvm;

void EntityDeclaration::print(
		raw_ostream& OS, size_t indents, bool printLocation) const
{
	OS.indent(indents);
	if (printLocation)
		OS << pos.toString() << "\n";
	OS.indent(indents);
	entity.print(OS, indents);
}

void EntityDeclaration::dump() const { print(outs()); }

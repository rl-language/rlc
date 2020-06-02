#include "rlc/ast/EntityDeclaration.hpp"

using namespace rlc;
using namespace std;
using namespace llvm;

void EntityDeclaration::print(
		raw_ostream& OS, size_t indents, bool printLocation) const
{
	if (printLocation)
	{
		OS.indent(indents);
		OS << pos.toString() << "\n";
	}
	entity.print(OS, indents);
}

void EntityDeclaration::dump() const { print(outs()); }

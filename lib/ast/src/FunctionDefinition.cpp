#include "rlc/ast/FunctionDefinition.hpp"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;
using namespace rlc;

void ArgumentDeclaration::print(
		raw_ostream& OS, size_t indents, bool dumpPosition) const
{
	OS.indent(indents);
	if (dumpPosition)
		OS << getSourcePosition().toString();
	OS << "arg declaration " << name << " : ";
	getTypeUse().print(OS);
}

void ArgumentDeclaration::dump() const { print(outs()); }

void FunctionDefinition::print(
		raw_ostream& OS, size_t indents, bool dumpPosition) const
{
	OS.indent(indents);
	if (dumpPosition)
		OS << getSourcePosition().toString();
	OS << "function definition " << name << " return type ";
	getTypeUse().print(OS);
	OS << "\n";
	for (const auto& arg : *this)
	{
		arg.print(OS, indents + 1, false);
		OS << "\n";
	}

	OS.indent(indents);
	OS << "body:\n";
	body.print(OS, indents + 1);
}
void FunctionDefinition::dump() const { print(outs()); }

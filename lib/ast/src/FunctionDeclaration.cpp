#include "rlc/ast/FunctionDeclaration.hpp"

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

void FunctionDeclaration::print(
		raw_ostream& OS, size_t indents, bool dumpPosition) const
{
	OS.indent(indents);
	if (dumpPosition)
		OS << getSourcePosition().toString();
	OS << "function " << name << " return type ";
	getTypeUse().print(OS);
	OS << "\n";
	for (const auto& arg : *this)
	{
		arg.print(OS, indents + 1, false);
		OS << "\n";
	}
}
void FunctionDeclaration::dump() const { print(outs()); }

string FunctionDeclaration::canonicalName() const
{
	string toReturn;
	raw_string_ostream s(toReturn);
	s << getName() << "-";
	for (auto a : arguments)
	{
		a.getTypeUse().print(s);
		s << "->";
	}

	s << toReturn;
	s.flush();
	return toReturn;
}

#include "rlc/ast/FunctionDeclaration.hpp"

#include <algorithm>
#include <iterator>

#include "llvm/ADT/Twine.h"
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

Error ArgumentDeclaration::deduceType(const SymbolTable& tb, TypeDB& db)
{
	return typeName.deduceType(tb, db);
}

Error FunctionDeclaration::deduceType(const SymbolTable& tb, TypeDB& db)
{
	for (auto& tu : *this)
		if (auto e = tu.deduceType(tb, db); e)
			return e;

	if (auto e = returnTypeName.deduceType(tb, db); e)
		return e;

	SmallVector<Type*, 3> argTypes(argumentsCount());
	transform(
			begin(), end(), argTypes.begin(), [](auto& l) { return l.getType(); });

	type = db.getFunctionType(getReturnType(), argTypes);
	return Error::success();
}

string FunctionDeclaration::mangledName() const
{
	return string("rlc_") + getName() + getType()->mangledName();
}

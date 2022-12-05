#include "rlc/ast/ActionDeclaration.hpp"

#include <algorithm>
#include <iterator>

#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;
using namespace rlc;

void ActionDeclaration::print(
		raw_ostream& OS, size_t indents, bool dumpPosition) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<ActionDeclaration*>(this);
}
void ActionDeclaration::dump() const { print(outs()); }

string ActionDeclaration::canonicalName() const
{
	string toReturn;
	raw_string_ostream s(toReturn);
	s << getName() << "-";
	for (auto a : arguments)
	{
		a.getTypeUse().print(s);
		s << "->";
	}

	s.flush();
	return toReturn;
}

Error ActionDeclaration::deduceType(const SymbolTable& tb, TypeDB& db)
{
	for (auto& tu : *this)
		if (auto e = tu.deduceType(tb, db); e)
			return e;

	SmallVector<Type*, 3> argTypes(argumentsCount());
	transform(
			begin(), end(), argTypes.begin(), [](auto& l) { return l.getType(); });

	type = db.getFunctionType(db.getVoidType(), argTypes);
	return Error::success();
}

string ActionDeclaration::mangledName() const
{
	return string("rlc_") + getName() + getType()->mangledName();
}

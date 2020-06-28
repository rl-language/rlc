#include "rlc/ast/FunctionDefinition.hpp"

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/SymbolTable.hpp"

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

Error FunctionDefinition::deduceTypes(const SymbolTable& tb, TypeDB& db)
{
	if (auto e = getDeclaration().deduceType(tb, db); e)
		return e;

	SymbolTable t(&tb);
	for (const auto& arg : *this)
		t.insert(arg);
	return getBody().deduceTypes(t, db);
}

#include "rlc/ast/ArgumentDeclaration.hpp"

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
	llvm::yaml::Output output(OS);
	output << *const_cast<ArgumentDeclaration*>(this);
}

void ArgumentDeclaration::dump() const { print(outs()); }

Error ArgumentDeclaration::deduceType(const SymbolTable& tb, TypeDB& db)
{
	return typeName.deduceType(tb, db);
}

#include "rlc/ast/SymbolTable.hpp"

#include "llvm/ADT/StringRef.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/System.hpp"

using namespace std;
using namespace llvm;
using namespace rlc;

StringRef Symbol::getName() const
{
	return visit([](const auto& s) { return s.getName(); });
}

[[nodiscard]] bool SymbolTable::contains(llvm::StringRef name) const
{
	if (auto s = symbols.find(name); s != symbols.end())
		return true;
	if (parent == nullptr)
		return false;
	return parent->contains(name);
}

const Symbol& SymbolTable::get(llvm::StringRef name) const
{
	assert(contains(name));
	if (auto s = symbols.find(name); s != symbols.end())
		return s->getValue();
	return parent->get(name);
}

template<typename Symb>
Type* typeOfSymbol(const Symb& symbol)
{
	assert(false && "trying to take of a symbol that has no type");
	return nullptr;
}

template<>
Type* typeOfSymbol<FunctionDefinition>(const FunctionDefinition& symbol)
{
	return symbol.getType();
}

template<>
Type* typeOfSymbol<DeclarationStatement>(const DeclarationStatement& symbol)
{
	return symbol.getType();
}

[[nodiscard]] Type* Symbol::getType() const
{
	return visit([](const auto& decl) { return typeOfSymbol(decl); });
}

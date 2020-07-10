#include "rlc/ast/SymbolTable.hpp"

#include "llvm/ADT/StringRef.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
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

iterator_range<SymbolTable::Map::const_iterator> SymbolTable::range(
		StringRef name) const
{
	if (!directContain(name) and parent != nullptr)
		return parent->range(name);

	return make_range(symbols.equal_range(name));
}

template<typename Symb>
Type* typeOfSymbol(const Symb& symbol)
{
	assert(false && "trying to take of a symbol that has no type");
	return nullptr;
}

template<>
Type* typeOfSymbol<FunctionDeclaration>(const FunctionDeclaration& symbol)
{
	return symbol.getType();
}

template<>
Type* typeOfSymbol<DeclarationStatement>(const DeclarationStatement& symbol)
{
	return symbol.getType();
}

template<>
Type* typeOfSymbol<ArgumentDeclaration>(const ArgumentDeclaration& symbol)
{
	return symbol.getType();
}

[[nodiscard]] Type* Symbol::getType() const
{
	return visit([](const auto& decl) { return typeOfSymbol(decl); });
}

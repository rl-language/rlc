#include "rlc/ast/BuiltinEntities.hpp"

#include "rlc/ast/Entity.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"

using namespace std;
using namespace rlc;
using namespace llvm;

constexpr auto builtinTypes = { "Int", "Float", "Bool", "Void" };
void rlc::addBuilintsEntities(System& s)
{
	for (const auto& t : builtinTypes)
		s.addEntity(Entity(t));
}

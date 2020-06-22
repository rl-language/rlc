#include "rlc/ast/BuiltinFunctions.hpp"

#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"

using namespace std;
using namespace rlc;
using namespace llvm;

constexpr auto logicOp = { "and",		"or",					"greater",
													 "less",	"less_equal", "greater_equal",
													 "equal", "nequal",			"not" };
constexpr auto numeralTypes = { "int", "float" };
constexpr auto builtinTypes = { "int", "float", "bool" };
constexpr auto internalOperations = {
	"add", "subtract", "divide", "multiply", "module"
};

void rlc::addBuilints(System& s)
{
	for (auto tName : numeralTypes)
		for (auto op : internalOperations)
			s.addFunDeclaration(op, tName, { tName, tName });

	for (auto tName : builtinTypes)
		for (auto op : logicOp)
			s.addFunDeclaration(op, "bool", { tName, tName });
}

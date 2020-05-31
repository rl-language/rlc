#include "rlc/ast/Entity.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

void EntityField::print(llvm::raw_ostream& OS) const
{
	OS << "field " << fieldName << " : " << fieldTypeName << " ";
	if (fieldType == nullptr)
		return;

	OS << "(";
	fieldType->print(OS);
	OS << ") ";
}

void EntityField::dump() const { print(llvm::outs()); }

void Entity::print(llvm::raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	OS << "entity " << nm;
	for (const auto& field : fields)
	{
		OS << "\n";
		OS.indent(indents + 1);
		field.print(OS);
	}
}

void Entity::dump() const { print(llvm::outs()); }

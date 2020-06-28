#include "rlc/ast/Entity.hpp"

#include <iterator>

#include "rlc/utils/Error.hpp"

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

size_t Entity::indexOfField(StringRef name) const
{
	auto accessed = find_if(*this, [&name](const EntityField& child) {
		return name == child.getName();
	});

	return distance(begin(), accessed);
}

Error Entity::createType(SymbolTable& tb, TypeDB& db)
{
	if (tb.contains(getName()))
		return make_error<StringError>(
				"Already declared entity " + getName(),
				RlcErrorCategory::errorCode(RlcErrorCode::alreadyDeclaredType));
	tb.insert(*this);
	type = db.createUserDefinedType(getName());
	return Error::success();
}

Error Entity::deduceType(const SymbolTable& tb, TypeDB& db)
{
	for (auto& field : *this)
		if (auto e = field.deduceType(tb, db); e)
			return e;

	for (auto& field : *this)
		getType()->addSubType(field.getType());

	return Error::success();
}

Error EntityField::deduceType(const SymbolTable& tb, TypeDB& db)
{
	if (!tb.contains(getTypeName()))
		return make_error<StringError>(
				"Unkown type " + getTypeName(),
				RlcErrorCategory::errorCode(RlcErrorCode::unknownReference));

	const auto& ent = tb.getUnique<Entity>(getTypeName());
	fieldType = ent.getType();
	return Error::success();
}

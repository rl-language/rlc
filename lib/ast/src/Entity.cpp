#include "rlc/ast/Entity.hpp"

#include <iterator>

#include "rlc/utils/Error.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

void EntityField::print(llvm::raw_ostream& OS) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<EntityField*>(this);
}

void EntityField::dump() const { print(llvm::outs()); }

void Entity::print(llvm::raw_ostream& OS, size_t indents) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<Entity*>(this);
}

void Entity::dump() const { print(llvm::outs()); }

size_t Entity::indexOfField(StringRef name) const
{
	const auto* accessed = find_if(*this, [&name](const EntityField& child) {
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
		getType()->addSubType(field.getType(), field.getName());

	return Error::success();
}

Error EntityField::deduceType(const SymbolTable& tb, TypeDB& db)
{
	return fieldTypeName.deduceType(tb, db);
}

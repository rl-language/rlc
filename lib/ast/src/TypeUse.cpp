#include "rlc/ast/TypeUse.hpp"

#include <iterator>
#include <memory>
#include <sstream>
#include <string>

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/utils/Error.hpp"

using namespace llvm;
using namespace std;
using namespace rlc;

SingleTypeUse::SingleTypeUse(std::unique_ptr<FunctionTypeUse> f)
		: fType(std::move(f))
{
}

const FunctionTypeUse& SingleTypeUse::getFunctionType() const { return *fType; }
FunctionTypeUse& SingleTypeUse::getFunctionType() { return *fType; }

void SingleTypeUse::print(llvm::raw_ostream& OS) const
{
	if (isFunctionType())
	{
		getFunctionType().print(OS);
		return;
	}

	OS << name;
	if (isArray())
		OS << "[" << to_string(dim) << "]";
}
void SingleTypeUse::dump() const { print(outs()); }

void FunctionTypeUse::print(llvm::raw_ostream& OS) const
{
	OS << "(";
	for (const auto& t : make_range(begin(), end()))
	{
		t.print(OS);
		if (&t != &(*(end() - 1)))
			OS << "->";
	}

	OS << ")";
}

void FunctionTypeUse::dump() const { print(outs()); }

SingleTypeUse::SingleTypeUse(const SingleTypeUse& other)
		: name(other.name), array(other.array), dim(other.dim)
{
	if (other.fType != nullptr)
		fType = std::make_unique<FunctionTypeUse>(*other.fType);
}
SingleTypeUse& SingleTypeUse::operator=(const SingleTypeUse& other)
{
	if (this == &other)
		return *this;
	name = other.name;
	array = other.array;
	dim = other.dim;
	if (other.fType != nullptr)
		fType = std::make_unique<FunctionTypeUse>(*other.fType);

	return *this;
}

string SingleTypeUse::toString() const
{
	string toReturn;
	raw_string_ostream s(toReturn);
	print(s);
	s.flush();
	return toReturn;
}

static Expected<Type*> typeFromName(
		llvm::StringRef name, const SymbolTable& tb, TypeDB& db)
{
	if (!tb.contains(name))
		return make_error<StringError>(
				name.str() + " does not name a type",
				RlcErrorCategory::errorCode(RlcErrorCode::unknownReference));

	return tb.getUnique<Entity>(name).getType();
}

Error SingleTypeUse::deduceType(const SymbolTable& tb, TypeDB& db)
{
	if (isFunctionType())
	{
		if (auto e = getFunctionType().deduceType(tb, db); e)
			return e;

		type = getFunctionType().getType();
		return Error::success();
	}

	Expected<Type*> t = typeFromName(getName(), tb, db);
	if (!t)
		return t.takeError();

	type = *t;
	return Error::success();
}

Error FunctionTypeUse::deduceType(const SymbolTable& tb, TypeDB& db)
{
	for (auto& typeUse : *this)
		if (auto e = typeUse.deduceType(tb, db); e)
			return e;

	SmallVector<Type*, 2> types(argCount());

	transform(argsRange(), types.begin(), [](const SingleTypeUse& use) {
		return use.getType();
	});

	type = db.getFunctionType(getReturnType().getType(), types);
	return Error::success();
}

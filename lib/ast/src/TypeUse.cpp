#include "rlc/ast/TypeUse.hpp"

#include <memory>
#include <sstream>
#include <string>

#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Type.hpp"

using namespace llvm;
using namespace std;
using namespace rlc;

SingleTypeUse::SingleTypeUse(std::unique_ptr<FunctionTypeUse> f)
		: fType(std::move(f))
{
}

const FunctionTypeUse& SingleTypeUse::getFunctionType() const { return *fType; }

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

#include "rlc/ast/MemberAccess.hpp"

#include <memory>

#include "rlc/ast/Expression.hpp"

using namespace rlc;
using namespace std;

MemberAccess::MemberAccess(const Expression& exp, string accessedField)
		: accessedField(move(accessedField)),
			exp({ std::make_unique<Expression>(exp) })
{
}

void MemberAccess::print(llvm::raw_ostream& OS, size_t indent) const
{
	OS.indent(indent);
	OS << "accessing member " << getFieldName() << " in exp ";
	getExp().print(OS, indent + 1);
}
void MemberAccess::dump() const { print(); }

bool MemberAccess::operator==(const MemberAccess& other) const
{
	return accessedField == other.accessedField and getExp() == other.getExp();
}

MemberAccess& MemberAccess::operator=(const MemberAccess& other)
{
	if (this == &other)
		return *this;
	accessedField = other.accessedField;
	exp[0] = make_unique<Expression>(other.getExp());
	return *this;
}
MemberAccess::MemberAccess(const MemberAccess& other)
		: accessedField(other.accessedField),
			exp({ make_unique<Expression>(other.getExp()) })
{
}

template<>
Expression& SimpleIterator<MemberAccess&, Expression>::operator*() const
{
	return type.getExp();
}

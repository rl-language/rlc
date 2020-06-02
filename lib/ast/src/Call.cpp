#include "rlc/ast/Call.hpp"

#include <initializer_list>
#include <memory>

#include "rlc/ast/Expression.hpp"
#include "rlc/utils/IRange.hpp"

using namespace std;
using namespace llvm;
using namespace rlc;

Call::Call(
		unique_ptr<Expression> f, SmallVector<std::unique_ptr<Expression>, 3> args)
		: args(move(args))
{
	this->args.emplace_back(move(f));
}

template<>
Expression& SimpleIterator<Call&, Expression>::operator*() const
{
	return type.getSubExpression(index);
}

template<>
const Expression& SimpleIterator<const Call&, const Expression>::operator*()
		const
{
	return type.getSubExpression(index);
}

void Call::print(raw_ostream& OS, size_t indents) const
{
	OS << "call\n";
	getFunctionExpression().print(OS, indents + 1);
	OS << "\n";
	OS.indent(indents);
	OS << "call args\n";
	for (const auto& a : argsRange())
	{
		a.print(OS, indents + 1);
		if (&a != &(*(argsRange().end() - 1)))
			OS << "\n";
	}
}

Call::Call(const Call& other)
{
	for (const auto& exp : other)
		args.emplace_back(std::make_unique<Expression>(exp));
}

Call& Call::operator=(const Call& other)
{
	if (this == &other)
		return *this;
	args.clear();
	for (const auto& exp : other)
		args.emplace_back(std::make_unique<Expression>(exp));
	return *this;
}

void Call::dump() const { print(); }

[[nodiscard]] bool Call::operator==(const Call& other) const
{
	for (auto i : irange(subExpCount()))
		if (getSubExpression(i) != other.getSubExpression(i))
			return false;

	return true;
}

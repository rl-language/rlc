#include "rlc/ast/ArrayAccess.hpp"

#include "rlc/ast/Expression.hpp"

using namespace rlc;

ArrayAccess::ArrayAccess(
		std::unique_ptr<Expression> lhs,
		std::unique_ptr<Expression> rhs,
		SourcePosition position)
		: position(std::move(position))
{
	args.emplace_back(std::move(lhs));
	args.emplace_back(std::move(rhs));
}

ArrayAccess::ArrayAccess(const ArrayAccess& other): position(other.position)
{
	args.emplace_back(std::make_unique<Expression>(*other.args[0]));
	args.emplace_back(std::make_unique<Expression>(*other.args[1]));
}

ArrayAccess& ArrayAccess::operator=(const ArrayAccess& other)
{
	if (this != &other)
		return *this;
	args.emplace_back(std::make_unique<Expression>(*other.args[0]));
	args.emplace_back(std::make_unique<Expression>(*other.args[1]));
	position = other.position;
	return *this;
}

[[nodiscard]] bool ArrayAccess::operator==(const ArrayAccess& other) const
{
	return std::tie(args) == std::tie(other.args);
}

void rlc::ArrayAccess::print(llvm::raw_ostream& OS) const
{
	llvm::yaml::Output output(OS);
	output << *const_cast<ArrayAccess*>(this);
}

void rlc::ArrayAccess::dump() const { print(llvm::outs()); }

template<>
Expression& SimpleIterator<ArrayAccess&, Expression>::operator*() const
{
	return type.getSubExpression(index);
}

template<>
const Expression&
SimpleIterator<const ArrayAccess&, const Expression>::operator*() const
{
	return type.getSubExpression(index);
}

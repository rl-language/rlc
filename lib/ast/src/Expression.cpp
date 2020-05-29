#include "rlc/ast/Expression.hpp"

#include <variant>

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Constant.hpp"
#include "rlc/ast/Reference.hpp"

using namespace std;
using namespace llvm;
using namespace rlc;

static void printOut(const ScalarConstant& c, raw_ostream& OS, size_t indents)
{
	OS << " Value: ";
	c.print(OS);
}

static void printOut(const Call& call, raw_ostream& OS, size_t indents)
{
	OS << " Call: \n";
	call.print(OS, indents);
}

static void printOut(const Reference& ref, raw_ostream& OS, size_t indents)
{
	OS << " Reference ";
	ref.print(OS);
}

void Expression::print(raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	OS << "exp of type ";
	if (getType() != nullptr)
		getType()->print(OS);
	else
		OS << " unkown type ";

	visit([&](const auto& t) { printOut(t, OS, indents); });
}

void Expression::dump() const { print(); }

template<>
Expression& SimpleBiIterator<Expression&, Expression>::operator*() const
{
	return type.subExpression(index);
}

template<>
const Expression&
		SimpleBiIterator<const Expression&, const Expression>::operator*() const
{
	return type.subExpression(index);
}

static size_t subExpCount(const Call& call) { return call.subExpCount(); }
static size_t subExpCount(const ScalarConstant& call) { return 0; }
static size_t subExpCount(const Reference& call) { return 0; }

size_t Expression::subExpressionCount() const
{
	return visit([](const auto& c) { return subExpCount(c); });
}

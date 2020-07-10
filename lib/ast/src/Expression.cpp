#include "rlc/ast/Expression.hpp"

#include <iterator>
#include <string>
#include <variant>

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Constant.hpp"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/MemberAccess.hpp"
#include "rlc/ast/Reference.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/utils/Error.hpp"
#include "rlc/utils/IRange.hpp"

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
	call.print(OS, indents);
}

static void printOut(const Reference& ref, raw_ostream& OS, size_t indents)
{
	OS << " ref ";
	ref.print(OS);
}

static void printOut(const MemberAccess& acc, raw_ostream& OS, size_t indents)
{
	OS << " member access ";
	acc.print(OS);
}

void Expression::print(raw_ostream& OS, size_t indents) const
{
	OS.indent(indents);
	if (getType() != nullptr)
	{
		OS << "type ";
		getType()->print(OS);
	}

	visit([&](const auto& t) { printOut(t, OS, indents); });
}

void Expression::dump() const { print(); }

template<>
Expression& SimpleIterator<Expression&, Expression>::operator*() const
{
	return type.subExpression(index);
}

template<>
const Expression&
SimpleIterator<const Expression&, const Expression>::operator*() const
{
	return type.subExpression(index);
}

static size_t subExpCount(const Call& call) { return call.subExpCount(); }
static size_t subExpCount(const ScalarConstant& call) { return 0; }
static size_t subExpCount(const Reference& call) { return 0; }
static size_t subExpCount(const MemberAccess& call) { return 1; }

size_t Expression::subExpressionCount() const
{
	return visit([](const auto& c) { return subExpCount(c); });
}

Expression Expression::call(Expression call, SmallVector<Expression, 3> args)
{
	Call::Container newArgs;
	for (auto& arg : args)
		newArgs.emplace_back(std::make_unique<Expression>(std::move(arg)));

	return Expression(
			Call(std::make_unique<Expression>(std::move(call)), std::move(newArgs)),
			nullptr);
}

static Expected<Type*> tpOfExp(
		const ScalarConstant& cst, const SymbolTable& tb, TypeDB& db)
{
	return cst.type(db);
}

static Expected<Type*> tpOfExp(
		const Reference& ref, const SymbolTable& tb, TypeDB& db)
{
	if (!tb.contains(ref.getName()))
		return make_error<StringError>(
				"Unkown Reference " + ref.getName(),
				RlcErrorCategory::errorCode(RlcErrorCode::unknownReference));

	SmallVector<Symbol*, 3> sym;
	for (auto e : tb.range(ref.getName()))
		if (e.second.getType() != nullptr)
			sym.emplace_back(&(e.second));

	assert(
			sym.size() <= 1 &&
			"somehow there are two typed symbols with the same name");

	if (sym.empty())
		return make_error<StringError>(
				ref.getName() + " is not a typed symbol",
				RlcErrorCategory::errorCode(RlcErrorCode::unknownReference));
	return sym[0]->getType();
}

static Error checkArguments(const Call& call)
{
	const auto& fExp = call.getFunctionExpression();
	const auto& fType = fExp.getType();
	assert(fType != nullptr);
	assert(fType->isFunctionType());
	if (call.argsCount() != fType->getArgCount())
		return make_error<StringError>(
				"argument count missmatch, expected" + to_string(fType->getArgCount()) +
						", received " + to_string(call.argsCount()),
				RlcErrorCategory::errorCode(RlcErrorCode::argumentCountMissmatch));

	for (auto i : irange(call.argsCount()))
	{
		if (fType->getArgumentType(i) != call.getArg(i).getType())
			return make_error<StringError>(
					"argument type missmatch for arg number: " + to_string(i),
					RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch));
	}
	return Error::success();
}

static bool argTypeMatching(Type* tp, const Call& call)
{
	if (call.argsCount() != tp->getArgCount())
		return false;

	for (auto i : irange(call.argsCount()))
		if (tp->getArgumentType(i) != call.getArg(i).getType())
			return false;
	return true;
}

static Error deduceOverload(Call& call, const SymbolTable& tb, TypeDB& db)
{
	auto& fExp = call.getFunctionExpression();
	assert(fExp.isA<Reference>());
	const auto& fName = fExp.get<Reference>().getName();

	// get all functions with the requrested name
	auto overloadSet = tb.range<FunctionDeclaration>(fName);

	// if we saw no oveloads then assume it a variable
	if (distance(overloadSet.begin(), overloadSet.end()) == 0)
		return fExp.deduceType(tb, db);

	// get the first that matches with the arguments types
	auto matching = find_if(overloadSet, [&call](const auto& overload) {
		return argTypeMatching(overload.getType(), call);
	});

	// if one matches then we found out the type
	if (matching != overloadSet.end())
	{
		fExp.setType(matching->getType());
		return Error::success();
	}

	// else throw a error
	return make_error<StringError>(
			"no matching function " + fName,
			RlcErrorCategory::errorCode(RlcErrorCode::noMatchingFunction));
}

static bool isSpecialFunction(const Reference& ref)
{
	if (ref.getName() == "assign")
		return true;
	return ref.getName() == "array_access";
}

static Error deduceAssignType(Call& call, const SymbolTable& tb, TypeDB& db)
{
	if (call.argsCount() != 2)
		return make_error<StringError>(
				"assign operation called with wrong argument count",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentCountMissmatch));

	if (call.getArg(0).getType() != call.getArg(1).getType())
		return make_error<StringError>(
				"assign operation called with missmatched types",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch));

	auto t = call.getArg(0).getType();

	auto& fExp = call.getFunctionExpression();
	fExp.setType(db.getFunctionType(t, t, t));

	return Error::success();
}

static Error deduceArrayAccessType(
		Call& call, const SymbolTable& tb, TypeDB& db)
{
	if (call.argsCount() != 2)
		return make_error<StringError>(
				"array subscripting operation called with wrong argument count",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentCountMissmatch));

	if (call.getArg(1).getType() != db.getLongType())
		return make_error<StringError>(
				"array subscripting  operation called with missmatched types",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch));

	if (call.getArg(0).getType()->isArray())
		return make_error<StringError>(
				"array subscripting  operation called with missmatched types",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch));

	auto t = call.getArg(0).getType();

	auto& fExp = call.getFunctionExpression();
	fExp.setType(db.getFunctionType(t, db.getLongType(), t->getBaseType()));

	return Error::success();
}

static Error deduceSpecialFunction(
		Call& call, const SymbolTable& tb, TypeDB& db)
{
	auto& fExp = call.getFunctionExpression();
	assert(fExp.isA<Reference>());
	auto& ref = fExp.get<Reference>();
	if (ref.getName() == "assign")
		return deduceAssignType(call, tb, db);

	if (ref.getName() == "array_access")
		return deduceArrayAccessType(call, tb, db);

	assert(false && "unrechable");
	return Error::success();
}

static Error deduceFunctionType(Call& call, const SymbolTable& tb, TypeDB& db)
{
	auto& fExp = call.getFunctionExpression();
	if (!fExp.isA<Reference>())
		return fExp.deduceType(tb, db);

	if (isSpecialFunction(fExp.get<Reference>()))
		return deduceSpecialFunction(call, tb, db);

	return deduceOverload(call, tb, db);
}

static Expected<Type*> tpOfExp(Call& call, const SymbolTable& tb, TypeDB& db)
{
	auto& fExp = call.getFunctionExpression();

	for (auto& c : call.argsRange())
		if (auto e = c.deduceType(tb, db); e)
			return Expected<Type*>(move(e));

	if (auto e = deduceFunctionType(call, tb, db); e)
		return Expected<Type*>(move(e));

	if (!fExp.getType()->isFunctionType())
		return make_error<StringError>(
				"object called  was not a function",
				RlcErrorCategory::errorCode(RlcErrorCode::nonFunctionCalled));

	if (auto e = checkArguments(call); e)
		return Expected<Type*>(move(e));

	return fExp.getType()->getReturnType();
}

static Expected<Type*> tpOfExp(
		MemberAccess& member, const SymbolTable& tb, TypeDB& db)
{
	for (auto& sub : member)
		if (auto e = sub.deduceType(tb, db); e)
			return Expected<Type*>(move(e));

	auto tp = member.getExp().getType();
	if (!tp->isUserDefined())
		return make_error<StringError>(
				"accessing element of a non user defined type",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch));

	auto entity = tb.getUnique<Entity>(tp->getName());
	auto index = entity.indexOfField(member.getFieldName());
	if (index <= tp->containedTypesCount())
		return tp->getContainedType(index);

	return make_error<StringError>(
			"No known field named " + member.getFieldName() + " in entity " +
					entity.getName(),
			RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch));
}

Error Expression::deduceType(const SymbolTable& tb, TypeDB& db)
{
	auto expectedType =
			visit([&](auto& exp) -> Expected<Type*> { return tpOfExp(exp, tb, db); });
	if (!expectedType)
		return expectedType.takeError();

	type = *expectedType;
	return Error::success();
}

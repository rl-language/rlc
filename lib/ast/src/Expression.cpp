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

void Expression::print(
		raw_ostream& OS, size_t indents, bool printLocation) const
{
	llvm::yaml::Output Output(OS);
	Output << *const_cast<Expression*>(this);
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
static size_t subExpCount(const ArrayAccess& call) { return 2; }
static size_t subExpCount(const Reference& call) { return 0; }
static size_t subExpCount(const ZeroInitializer& call) { return 0; }
static size_t subExpCount(const MemberAccess& call) { return 1; }

size_t Expression::subExpressionCount() const
{
	return visit([](const auto& c) { return subExpCount(c); });
}

Expression Expression::call(
		Expression call, SmallVector<Expression, 3> args, SourcePosition position)
{
	Call::Container newArgs;
	for (auto& arg : args)
		newArgs.emplace_back(std::make_unique<Expression>(std::move(arg)));

	return Expression(
			Call(
					std::make_unique<Expression>(std::move(call)),
					std::move(newArgs),
					std::move(position)),
			nullptr);
}

static Expected<Type*> tpOfExp(
		const ScalarConstant& cst, const SymbolTable& tb, TypeDB& db)
{
	return cst.type(db);
}

static Expected<Type*> tpOfExp(
		Reference& ref, const SymbolTable& tb, TypeDB& db)
{
	if (!tb.contains(ref.getName()))
		return make_error<RlcError>(
				"Unkown Reference " + ref.getName(),
				RlcErrorCategory::errorCode(RlcErrorCode::unknownReference),
				ref.getPosition());

	SmallVector<Symbol*, 3> sym;
	for (auto e : tb.range(ref.getName()))
		if (e.second.getType() != nullptr)
			sym.emplace_back(&(e.second));

	assert(
			sym.size() <= 1 &&
			"somehow there are two typed symbols with the same name");

	if (sym.empty())
		return make_error<RlcError>(
				ref.getName() + " is not a typed symbol",
				RlcErrorCategory::errorCode(RlcErrorCode::unknownReference),
				ref.getPosition());

	ref.setReferred(*sym[0]);
	return sym[0]->getType();
}

static Error checkArguments(const Call& call)
{
	const auto& fExp = call.getFunctionExpression();
	const auto& fType = fExp.getType();
	assert(fType != nullptr);
	assert(fType->isFunctionType());
	if (call.argsCount() != fType->getArgCount())
		return make_error<RlcError>(
				"argument count missmatch, expected" + to_string(fType->getArgCount()) +
						", received " + to_string(call.argsCount()),
				RlcErrorCategory::errorCode(RlcErrorCode::argumentCountMissmatch),
				call.getPosition());

	for (auto i : irange(call.argsCount()))
	{
		if (fType->getArgumentType(i) != call.getArg(i).getType())
			return make_error<RlcError>(
					"argument type missmatch for arg number: " + to_string(i),
					RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
					call.getPosition());
	}
	return Error::success();
}

static bool argTypeMatching(Type* tp, const Call& call)
{
	assert(tp != nullptr);
	if (not tp->isFunctionType())
		return false;

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
	return make_error<RlcError>(
			"no matching function " + fName,
			RlcErrorCategory::errorCode(RlcErrorCode::noMatchingFunction),
			call.getPosition());
}

static bool isSpecialFunction(const Reference& ref)
{
	if (ref.getName() == "assign")
		return true;
	if (ref.getName() == "init")
		return true;
	return ref.getName() == "array_access";
}

static Error deduceInitType(Call& call, const SymbolTable& tb, TypeDB& db)
{
	if (call.argsCount() != 1)
		return make_error<RlcError>(
				"init operation called with more than 1 argument",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentCountMissmatch),
				call.getPosition());

	auto t = call.getArg(0).getType();

	auto& fExp = call.getFunctionExpression();
	fExp.setType(db.getFunctionType(db.getVoidType(), t));

	return Error::success();
}

static Error deduceAssignType(Call& call, const SymbolTable& tb, TypeDB& db)
{
	if (call.argsCount() != 2)
		return make_error<RlcError>(
				"assign operation called with wrong argument count",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentCountMissmatch),
				call.getPosition());

	if (call.getArg(0).getType() != call.getArg(1).getType())
		return make_error<RlcError>(
				"assign operation called with missmatched types",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				call.getPosition());

	auto t = call.getArg(0).getType();

	auto& fExp = call.getFunctionExpression();
	fExp.setType(db.getFunctionType(t, t, t));

	return Error::success();
}

static Error deduceArrayAccessType(
		ArrayAccess& call, const SymbolTable& tb, TypeDB& db)
{
	if (call.getIndexing().getType() != db.getLongType())
		return make_error<RlcError>(
				"array subscripting  operation called with non int index",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				call.getIndexing().getPosition());

	if (not call.getArray().getType()->isArray())
		return make_error<RlcError>(
				"array subscripting operation invoked on a non array type",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				call.getArray().getPosition());

	return Error::success();
}

static Error deduceSpecialFunction(
		Call& call, const SymbolTable& tb, TypeDB& db)
{
	auto& fExp = call.getFunctionExpression();
	assert(fExp.isA<Reference>());
	auto& ref = fExp.get<Reference>();
	if (ref.getName() == builtinFunctionsToString(BuiltinFunctions::Assign))
		return deduceAssignType(call, tb, db);

	if (ref.getName() == builtinFunctionsToString(BuiltinFunctions::Init))
		return deduceInitType(call, tb, db);

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

static Expected<Type*> tpOfExp(
		ArrayAccess& access, const SymbolTable& tb, TypeDB& db)
{
	for (auto& c : access)
		if (auto e = c.deduceType(tb, db); e)
			return Expected<Type*>(std::move(e));

	if (auto Error = deduceArrayAccessType(access, tb, db); Error)
		return Error;

	auto t = access.getArray().getType();
	return t->getBaseType();
}

static Expected<Type*> tpOfExp(Call& call, const SymbolTable& tb, TypeDB& db)
{
	auto& fExp = call.getFunctionExpression();

	for (auto& c : call.argsRange())
		if (auto e = c.deduceType(tb, db); e)
			return Expected<Type*>(std::move(e));

	if (auto e = deduceFunctionType(call, tb, db); e)
		return Expected<Type*>(std::move(e));

	if (!fExp.getType()->isFunctionType())
		return make_error<RlcError>(
				"object called  was not a function",
				RlcErrorCategory::errorCode(RlcErrorCode::nonFunctionCalled),
				call.getPosition());

	if (auto e = checkArguments(call); e)
		return Expected<Type*>(std::move(e));

	return fExp.getType()->getReturnType();
}

static Expected<Type*> tpOfExp(
		ZeroInitializer& initializer, const SymbolTable& tb, TypeDB& db)
{
	if (auto Error = initializer.getTypeUse().deduceType(tb, db); Error)
		return Error;
	return initializer.getTypeUse().getType();
}

static Expected<Type*> tpOfExp(
		MemberAccess& member, const SymbolTable& tb, TypeDB& db)
{
	for (auto& sub : member)
		if (auto e = sub.deduceType(tb, db); e)
			return Expected<Type*>(std::move(e));

	auto tp = member.getExp().getType();
	if (!tp->isUserDefined())
		return make_error<RlcError>(
				"accessing element of a non user defined type",
				RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
				member.getPosition());

	auto entity = tb.getUnique<Entity>(tp->getName());
	auto index = entity.indexOfField(member.getFieldName());
	if (index < tp->containedTypesCount())
		return tp->getContainedType(index);

	return make_error<RlcError>(
			"No known field named " + member.getFieldName() + " in entity " +
					entity.getName(),
			RlcErrorCategory::errorCode(RlcErrorCode::argumentTypeMissmatch),
			member.getPosition());
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

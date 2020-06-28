#include "rlc/ast/Type.hpp"

#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

using namespace rlc;
using namespace std;
using namespace llvm;

StringRef rlc::builtinTypeToString(BuiltinType t)
{
	switch (t)
	{
		case BuiltinType::VOID:
			return "void";
		case BuiltinType::BOOL:
			return "bool";
		case BuiltinType::DOUBLE:
			return "double";
		case BuiltinType::LONG:
			return "long";
	}
	assert(false && "unrechable");
	return "";
}

template<typename T>
constexpr bool isArrayType()
{
	return std::is_same<std::decay<T>, ArrayType>::value;
}

template<typename T>
constexpr bool isUserDefinedType()
{
	return std::is_same<std::decay<T>, UserDefinedType>::value;
}

template<typename T>
constexpr bool isBuiltinType()
{
	return std::is_same<std::decay<T>, BuiltinType>::value;
}

template<typename T>
string name(const T& tp)
{
	return tp.getName();
}

template<>
string name(const BuiltinType& tp)
{
	return builtinTypeToString(tp);
}

string FunctionType::getName() const
{
	string toReturn;
	for (auto t : argumentsRange())
		toReturn += t->getName() + " -> ";

	toReturn += getReturnType()->getName();
	return toReturn;
}

[[nodiscard]] string Type::getName() const
{
	return visit([](const auto& t) { return name(t); });
}

string ArrayType::getName() const
{
	return innerType[0]->getName() + "[" + to_string(siz) + "]";
}

static size_t containedTypes(BuiltinType) { return 0; }

static size_t containedTypes(const ArrayType&) { return 1; }

static size_t containedTypes(const UserDefinedType& t)
{
	return t.childrenCount();
}

static size_t containedTypes(const FunctionType& t)
{
	return t.containedTypesCount();
}

size_t Type::containedTypesCount() const
{
	return visit([](const auto& tp) { return containedTypes(tp); });
}

static Type* containedType(const BuiltinType&, size_t) { return nullptr; }

static Type* containedType(const UserDefinedType& t, size_t i)
{
	return t.getContainedType(i);
}

static Type* containedType(const ArrayType& t, size_t)
{
	return t.getBaseType();
}

static Type* containedType(const FunctionType& t, size_t index)
{
	return t.getContainedType(index);
}

Type* Type::getContainedType(size_t index) const
{
	assert(index < containedTypesCount());

	return visit([index](const auto& c) { return containedType(c, index); });
}

void Type::print(llvm::raw_ostream& out) { out << getName(); }
void Type::dump() { print(llvm::outs()); }

Type* TypeDB::getBuiltin(BuiltinType b)
{
	if (auto f = builtinTypes.find(b); f != builtinTypes.end())
		return f->second.get();
	builtinTypes.try_emplace(b, std::make_unique<Type>(b));
	return builtinTypes[b].get();
}

Type* TypeDB::getUserDefined(llvm::StringRef name)
{
	if (auto t = nameToBuiltinType(name); t != nullptr)
		return t;
	if (auto f = userDefinedTypes.find(name); f != userDefinedTypes.end())
		return f->second.get();

	return nullptr;
}

Type* TypeDB::createUserDefinedType(
		std::string name, llvm::ArrayRef<Type*> members)
{
	if (auto t = nameToBuiltinType(name); t != nullptr)
		return t;

	if (userDefinedTypes.find(name) != userDefinedTypes.end())
		return nullptr;

	auto s = new (allocator.Allocate<string>()) string(std::move(name));

	userDefinedTypes[*s] = std::make_unique<Type>(*s, members);
	return userDefinedTypes[*s].get();
}

Type* TypeDB::getArrayType(Type* tp, size_t size)
{
	auto p = make_pair(tp, size);
	if (auto f = arrayTypes.find(p); f != arrayTypes.end())
		return f->second.get();

	arrayTypes[p] = std::make_unique<Type>(tp, size);
	return arrayTypes[p].get();
}

Type* TypeDB::getFunctionType(Type* returnType, llvm::ArrayRef<Type*> args)
{
	llvm::SmallVector<Type*, 3> t({ args.begin(), args.end() });
	t.push_back(returnType);

	if (auto f = functionTypes.find(t); f != functionTypes.end())
		return f->second.get();

	functionTypes[t] = std::make_unique<Type>(returnType, args);
	return functionTypes[t].get();
}

template<>
const Type* SimpleIterator<const Type*, const Type*, const Type*>::operator*()
		const
{
	return type->getContainedType(index);
}

Type* TypeDB::nameToBuiltinType(llvm::StringRef name)
{
	if (name == "int")
		return getLongType();

	if (name == "bool")
		return getBoolType();

	if (name == "float")
		return getFloatType();

	if (name == "void")
		return getVoidType();
	return nullptr;
}

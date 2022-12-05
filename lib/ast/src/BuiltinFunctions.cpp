#include "rlc/ast/BuiltinFunctions.hpp"

#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/utils/SourcePosition.hpp"

using namespace std;
using namespace rlc;
using namespace llvm;

const auto numeralTypes = { builtinTypeToString(BuiltinType::DOUBLE).str(),
														builtinTypeToString(BuiltinType::LONG).str() };
const auto builtinTypes = { builtinTypeToString(BuiltinType::BOOL).str(),
														builtinTypeToString(BuiltinType::DOUBLE).str(),
														builtinTypeToString(BuiltinType::LONG).str() };

llvm::StringRef rlc::builtinFunctionsToString(BuiltinFunctions fun)
{
	switch (fun)
	{
		case rlc::BuiltinFunctions::And:
			return "and";
		case rlc::BuiltinFunctions::Or:
			return "or";
		case rlc::BuiltinFunctions::Greater:
			return "greater";
		case rlc::BuiltinFunctions::Less:
			return "less";
		case rlc::BuiltinFunctions::GrearEqual:
			return "greater_equal";
		case rlc::BuiltinFunctions::LessEqual:
			return "less_equal";
		case rlc::BuiltinFunctions::Equal:
			return "equal";
		case rlc::BuiltinFunctions::NotEqual:
			return "not_equal";
		case rlc::BuiltinFunctions::Add:
			return "add";
		case rlc::BuiltinFunctions::Subtract:
			return "subtract";
		case rlc::BuiltinFunctions::Divide:
			return "divide";
		case rlc::BuiltinFunctions::Multiply:
			return "multiply";
		case rlc::BuiltinFunctions::Reminder:
			return "reminder";
		case rlc::BuiltinFunctions::Not:
			return "not";
		case rlc::BuiltinFunctions::Int:
			return "int";
		case rlc::BuiltinFunctions::Bool:
			return "bool";
		case rlc::BuiltinFunctions::Float:
			return "float";
		case rlc::BuiltinFunctions::Assign:
			return "assign";
		case rlc::BuiltinFunctions::Minus:
			return "minus";
		case rlc::BuiltinFunctions::Init:
			return "init";
	}
	assert(false && "unrechable");
	return "not";
}

BuiltinFunctions rlc::stringToBuiltinFunction(llvm::StringRef name)
{
	if ("and" == name)
		return rlc::BuiltinFunctions::And;
	if ("or" == name)
		return rlc::BuiltinFunctions::Or;
	if ("greater" == name)
		return rlc::BuiltinFunctions::Greater;
	if ("less" == name)
		return rlc::BuiltinFunctions::Less;
	if ("greater_equal" == name)
		return rlc::BuiltinFunctions::GrearEqual;
	if ("less_equal" == name)
		return rlc::BuiltinFunctions::LessEqual;
	if ("equal" == name)
		return rlc::BuiltinFunctions::Equal;
	if ("not_equal" == name)
		return rlc::BuiltinFunctions::NotEqual;
	if ("add" == name)
		return rlc::BuiltinFunctions::Add;
	if ("subtract" == name)
		return rlc::BuiltinFunctions::Subtract;
	if ("divide" == name)
		return rlc::BuiltinFunctions::Divide;
	if ("multiply" == name)
		return rlc::BuiltinFunctions::Multiply;
	if ("reminder" == name)
		return rlc::BuiltinFunctions::Reminder;
	if ("not" == name)
		return rlc::BuiltinFunctions::Not;
	if ("int" == name)
		return rlc::BuiltinFunctions::Int;
	if ("float" == name)
		return rlc::BuiltinFunctions::Float;
	if ("bool" == name)
		return rlc::BuiltinFunctions::Bool;
	if ("assign" == name)
		return rlc::BuiltinFunctions::Assign;
	if ("minus" == name)
		return rlc::BuiltinFunctions::Minus;
	if ("init" == name)
		return rlc::BuiltinFunctions::Init;

	assert(false && "unrechable");
	return rlc::BuiltinFunctions::Add;
}

constexpr auto internalOperations = {
	rlc::BuiltinFunctions::Add,			 rlc::BuiltinFunctions::Subtract,
	rlc::BuiltinFunctions::Multiply, rlc::BuiltinFunctions::Divide,
	rlc::BuiltinFunctions::Reminder,
};

constexpr auto logicOp = {
	rlc::BuiltinFunctions::Greater, rlc::BuiltinFunctions::LessEqual,
	rlc::BuiltinFunctions::Less,		rlc::BuiltinFunctions::GrearEqual,
	rlc::BuiltinFunctions::Equal,		rlc::BuiltinFunctions::NotEqual
};

void rlc::addBuilints(System& s)
{
	auto BoolName = builtinTypeToString(BuiltinType::BOOL).str();
	auto FloatName = builtinTypeToString(BuiltinType::DOUBLE).str();
	auto IntName = builtinTypeToString(BuiltinType::LONG).str();
	auto VoidName = builtinTypeToString(BuiltinType::VOID).str();
	SourcePosition pos(BuiltinFileName);
	for (const auto& tName : numeralTypes)
	{
		for (auto op : internalOperations)
			s.addFunDeclaration(op, tName, { tName, tName }, pos);
	}

	for (const auto& tName : builtinTypes)
	{
		s.addFunDeclaration(
				rlc::BuiltinFunctions::Assign, tName, { tName, tName }, pos);
		s.addFunDeclaration(rlc::BuiltinFunctions::Minus, tName, { tName }, pos);
		s.addFunDeclaration(rlc::BuiltinFunctions::Init, VoidName, { tName }, pos);
	}

	for (const auto& tName : builtinTypes)
		for (const auto op : logicOp)
			s.addFunDeclaration(op, BoolName, { tName, tName }, pos);

	s.addFunDeclaration(rlc::BuiltinFunctions::Not, BoolName, { BoolName }, pos);
	s.addFunDeclaration(
			rlc::BuiltinFunctions::Or, BoolName, { BoolName, BoolName }, pos);
	s.addFunDeclaration(
			rlc::BuiltinFunctions::And, BoolName, { BoolName, BoolName }, pos);
	s.addFunDeclaration(rlc::BuiltinFunctions::Bool, BoolName, { IntName }, pos);
	s.addFunDeclaration(
			rlc::BuiltinFunctions::Bool, BoolName, { FloatName }, pos);

	s.addFunDeclaration(rlc::BuiltinFunctions::Int, IntName, { BoolName }, pos);
	s.addFunDeclaration(rlc::BuiltinFunctions::Int, IntName, { FloatName }, pos);

	s.addFunDeclaration(
			rlc::BuiltinFunctions::Float, FloatName, { BoolName }, pos);
	s.addFunDeclaration(
			rlc::BuiltinFunctions::Float, FloatName, { IntName }, pos);
}

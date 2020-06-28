#include "rlc/ast/System.hpp"

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/Error.hpp"

using namespace llvm;
using namespace rlc;
using namespace std;

void System::print(llvm::raw_ostream& OS) const
{
	OS << "system: " << name << "\n";
	for (const auto& pair : entities)
	{
		pair.second.print(OS, 1);
		OS << "\n";
	}

	OS << "declarations: "
		 << "\n";
	for (const auto& pair : funDecl)
		pair.second.print(OS, 1);

	OS << "definitions: "
		 << "\n";
	for (const auto& pair : funDef)
		pair.second.print(OS, 1);
}

void System::dump() const { print(outs()); }

void System::addFunDeclaration(FunctionDeclaration fun)
{
	auto name = fun.canonicalName();
	funDecl.try_emplace(std::move(name), std::move(fun));
}

void System::addFunction(FunctionDefinition fun)
{
	auto name = fun.canonicalName();
	funDef.try_emplace(std::move(name), std::move(fun));
}

void System::addFunDeclaration(
		std::string name,
		std::string returnType,
		llvm::SmallVector<std::string, 3> argTypes)
{
	SmallVector<ArgumentDeclaration, 3> args;
	auto u = SingleTypeUse::scalarType(move(returnType));
	for (auto a : argTypes)
		args.emplace_back("", SingleTypeUse::scalarType(a));

	addFunDeclaration(FunctionDeclaration(move(name), move(u), move(args)));
}

Error System::deduceEntitiesTypes(const SymbolTable& tb, TypeDB& db)
{
	for (auto& ep : entities)
		if (auto e = ep.second.getEntity().deduceType(tb, db); e)
			return e;

	return Error::success();
}

Error System::collectEntityTypes(SymbolTable& tb, TypeDB& db)
{
	for (auto& ep : entities)
	{
		auto& entity = ep.second.getEntity();
		if (auto e = entity.createType(tb, db); e)
			return e;
	}

	return Error::success();
}

Error System::deduceFunctionTypes(SymbolTable& tb, TypeDB& db)
{
	for (const auto& fDecl : funDecl)
		tb.insert(fDecl.second);

	for (const auto& fDef : funDef)
		tb.insert(fDef.second.getDeclaration());

	for (auto& fDecl : funDecl)
		if (auto e = fDecl.second.deduceType(tb, db); e)
			return e;

	for (auto& fDef : funDef)
		if (auto e = fDef.second.deduceTypes(tb, db); e)
			return e;

	return Error::success();
}

Error System::typeCheck(const SymbolTable& tb, TypeDB& db)
{
	SymbolTable table(&tb);
	if (auto e = collectEntityTypes(table, db); e)
		return e;

	if (auto e = deduceEntitiesTypes(table, db); e)
		return e;

	if (auto e = deduceFunctionTypes(table, db); e)
		return e;

	return Error::success();
}

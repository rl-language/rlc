#include "rlc/ast/System.hpp"

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/TypeUse.hpp"

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

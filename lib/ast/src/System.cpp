#include "rlc/ast/System.hpp"

#include "llvm/Support/raw_ostream.h"

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

	for (const auto& pair : funDef)
		pair.second.print(OS, 1);
}

void System::dump() const { print(outs()); }

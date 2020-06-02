#pragma once

#include "llvm/ADT/StringMap.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
namespace rlc
{
	class System
	{
		public:
		void print(llvm::raw_ostream& OS) const;
		void dump() const;

		void addFunction(FunctionDefinition fun)
		{
			auto name = fun.getName();
			funDef.try_emplace(std::move(name), std::move(fun));
		}

		void addEntity(EntityDeclaration ent)
		{
			auto name = ent.getEntity().getName();
			entities.try_emplace(std::move(name), std::move(ent));
		}

		System(std::string name): name(std::move(name)) {}

		private:
		std::string name;
		llvm::StringMap<FunctionDefinition> funDef;
		llvm::StringMap<EntityDeclaration> entities;
	};
}	 // namespace rlc

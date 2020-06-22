#pragma once

#include <utility>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
namespace rlc
{
	class System
	{
		public:
		void print(llvm::raw_ostream& OS) const;
		void dump() const;

		void addFunDeclaration(
				std::string name,
				std::string returnType,
				llvm::SmallVector<std::string, 3> argTypes);

		void addFunDeclaration(FunctionDeclaration fun);

		void addFunction(FunctionDefinition fun);

		void addEntity(EntityDeclaration ent)
		{
			auto name = ent.getEntity().getName();
			entities.try_emplace(std::move(name), std::move(ent));
		}

		[[nodiscard]] const std::string& getName() const { return name; }
		System(std::string name): name(std::move(name)) {}

		private:
		std::string name;
		llvm::StringMap<FunctionDeclaration> funDecl;
		llvm::StringMap<FunctionDefinition> funDef;
		llvm::StringMap<EntityDeclaration> entities;
	};
}	 // namespace rlc

#pragma once

#include <utility>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
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
		llvm::Error typeCheck(const SymbolTable& tb, TypeDB& db);

		[[nodiscard]] auto declarationsRange() const
		{
			return llvm::make_range(funDecl.begin(), funDecl.end());
		}

		[[nodiscard]] auto definitionsRange() const
		{
			return llvm::make_range(funDef.begin(), funDef.end());
		}

		[[nodiscard]] auto entitiesRange() const
		{
			return llvm::make_range(entities.begin(), entities.end());
		}

		private:
		llvm::Error deduceEntitiesTypes(const SymbolTable& tb, TypeDB& db);
		llvm::Error collectEntityTypes(SymbolTable& tb, TypeDB& db);
		llvm::Error deduceFunctionTypes(SymbolTable& tb, TypeDB& db);
		std::string name;
		llvm::StringMap<FunctionDeclaration> funDecl;
		llvm::StringMap<FunctionDefinition> funDef;
		llvm::StringMap<EntityDeclaration> entities;
	};
}	 // namespace rlc

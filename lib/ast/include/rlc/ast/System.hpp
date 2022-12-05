#pragma once

#include <utility>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/ActionDefinition.hpp"
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
		friend llvm::yaml::MappingTraits<rlc::System>;
		void print(llvm::raw_ostream& OS, bool printPosition = false) const;
		void dump() const;

		void addFunDeclaration(
				std::string name,
				std::string returnType,
				llvm::SmallVector<std::string, 3> argTypes,
				SourcePosition pos = SourcePosition());

		void addFunDeclaration(
				BuiltinFunctions fun,
				std::string returnType,
				llvm::SmallVector<std::string, 3> argTypes,
				SourcePosition pos = SourcePosition());

		FunctionDeclaration& addFunDeclaration(FunctionDeclaration fun);

		FunctionDefinition& addFunction(FunctionDefinition fun);
		ActionDefinition& addAction(ActionDefinition act);

		EntityDeclaration& addEntity(EntityDeclaration ent)
		{
			auto name = ent.getEntity().getName();
			auto res = entities.try_emplace(std::move(name), std::move(ent));
			return res.first->second;
		}

		[[nodiscard]] const std::string& getName() const { return name; }
		System(std::string name, SourcePosition position = SourcePosition())
				: name(std::move(name)), position(std::move(position))
		{
		}
		llvm::Error typeCheck(const SymbolTable& tb, TypeDB& db);

		[[nodiscard]] bool hasMain() const
		{
			return llvm::find_if(funDef, [](auto& f) -> bool {
							 return f->getName() == "main";
						 }) != funDef.end();
		}

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

		void setName(std::string newName) { name = std::move(newName); }

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }

		private:
		llvm::Error defineImplicitInitArrayFunction(
				Type* type, SymbolTable& tb, TypeDB& db);
		llvm::Error defineImplicitAssigmentArrayFunction(
				Type* type, SymbolTable& tb, TypeDB& db);
		llvm::Error defineImplicitAssigmentFunction(
				const EntityDeclaration& entity, SymbolTable& tb, TypeDB& db);
		llvm::Error deduceEntitiesTypes(const SymbolTable& tb, TypeDB& db);
		llvm::Error collectEntityTypes(SymbolTable& tb, TypeDB& db);
		llvm::Error deduceFunctionTypes(SymbolTable& tb, TypeDB& db);
		llvm::Error deduceFunctionDeclarationType(SymbolTable& tb, TypeDB& db);
		llvm::Error deduceActionsEntities(SymbolTable& tb, TypeDB& db);
		llvm::Error deduceCreatorActionFunction(
				const ActionDefinition& outermost, SymbolTable& tb, TypeDB& db);
		llvm::Error deduceWrapperActionFunction(
				const ActionDefinition& outermost,
				const ActionDeclaration& decl,
				SymbolTable& tb,
				TypeDB& db);
		llvm::Error deduceActionsFunctions(SymbolTable& tb, TypeDB& db);
		llvm::Error deduceImplicitFunctions(SymbolTable& tb, TypeDB& db);
		llvm::Error declareImplicitFunction(SymbolTable& tb, TypeDB& db);
		llvm::Error deduceImplicitArrayFunctions(SymbolTable& tb, TypeDB& db);

		llvm::Error defineImplicitInitFunction(
				const EntityDeclaration& entity, SymbolTable& tb, TypeDB& db);
		std::string name;
		llvm::StringMap<FunctionDeclaration> funDecl;
		std::vector<std::unique_ptr<FunctionDefinition>> funDef;
		llvm::StringMap<EntityDeclaration> entities;
		llvm::StringMap<ActionDefinition> actions;
		SourcePosition position;
	};
}	 // namespace rlc
template<>
struct llvm::yaml::SequenceTraits<llvm::StringMap<rlc::FunctionDeclaration>>
{
	static size_t size(IO& io, llvm::StringMap<rlc::FunctionDeclaration>& list)
	{
		return list.size();
	}
	static rlc::FunctionDeclaration& element(
			IO& io, llvm::StringMap<rlc::FunctionDeclaration>& list, size_t index)
	{
		assert(io.outputting());
		return std::next(list.begin(), index)->getValue();
	}
};

template<>
struct llvm::yaml::SequenceTraits<
		std::vector<std::unique_ptr<rlc::FunctionDefinition>>>
{
	static size_t size(
			IO& io, std::vector<std::unique_ptr<rlc::FunctionDefinition>>& list)
	{
		return list.size();
	}
	static rlc::FunctionDefinition& element(
			IO& io,
			std::vector<std::unique_ptr<rlc::FunctionDefinition>>& list,
			size_t index)
	{
		assert(io.outputting());
		return **std::next(list.begin(), index);
	}
};

template<>
struct llvm::yaml::SequenceTraits<llvm::StringMap<rlc::EntityDeclaration>>
{
	static size_t size(IO& io, llvm::StringMap<rlc::EntityDeclaration>& list)
	{
		return list.size();
	}
	static rlc::EntityDeclaration& element(
			IO& io, llvm::StringMap<rlc::EntityDeclaration>& list, size_t index)
	{
		assert(io.outputting());
		return std::next(list.begin(), index)->getValue();
	}
};

template<>
struct llvm::yaml::MappingTraits<rlc::System>
{
	static void mapping(IO& io, rlc::System& value)
	{
		assert(io.outputting());

		io.mapRequired("name", value.name);
		io.mapRequired("function_declarations", value.funDecl);
		io.mapRequired("function_definitions", value.funDef);
		io.mapRequired("entity_declaration", value.entities);
		if (not value.position.isMissing())
			io.mapRequired("poisition", value.position);
	}
};

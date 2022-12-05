#pragma once

#include <string>

#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/ActionDeclaration.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class FunctionDefinition;
	class ActionDefinition
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::ActionDefinition>;
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool dumpPosition = false) const;
		void dump() const;

		using iterator = ActionDeclaration::iterator;
		using const_iterator = ActionDeclaration::const_iterator;

		[[nodiscard]] const_iterator begin() const { return declaration.begin(); }
		[[nodiscard]] iterator begin() { return declaration.begin(); }
		[[nodiscard]] const_iterator end() const { return declaration.end(); }
		[[nodiscard]] iterator end() { return declaration.end(); }
		[[nodiscard]] size_t argumentsCount() const
		{
			return declaration.argumentsCount();
		}
		[[nodiscard]] const ArgumentDeclaration& operator[](size_t index) const
		{
			assert(index < argumentsCount());
			return declaration[index];
		}
		[[nodiscard]] ArgumentDeclaration& operator[](size_t index)
		{
			assert(index < argumentsCount());
			return declaration[index];
		}

		[[nodiscard]] Type* getType() const { return declaration.getType(); }

		[[nodiscard]] const std::string& getName() const
		{
			return declaration.getName();
		}

		[[nodiscard]] std::string canonicalName() const
		{
			return declaration.canonicalName();
		}

		[[nodiscard]] FunctionDefinition asFunction(SymbolTable& table) const;

		[[nodiscard]] std::string implementatioName() const
		{
			return "_" + getName() + "_impl";
		}

		[[nodiscard]] const Statement& getBody() const { return body; }
		[[nodiscard]] Statement& getBody() { return body; }

		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return declaration.getSourcePosition();
		}

		ActionDefinition(ActionDeclaration decl, Statement body)
				: declaration(std::move(decl)), body(std::move(body))
		{
		}

		[[nodiscard]] llvm::SmallVector<const ActionDeclaration*, 4>
		allInnerActions() const;

		ActionDefinition(
				BuiltinFunctions name,
				Statement body,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: declaration(std::move(name), std::move(arguments), pos),
					body(std::move(body))
		{
		}

		ActionDefinition(
				std::string name,
				Statement body,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: declaration(std::move(name), std::move(arguments), pos),
					body(std::move(body))
		{
		}

		ActionDefinition(std::string name, Statement body, SourcePosition pos)
				: declaration(std::move(name), std::move(pos)), body(std::move(body))
		{
		}

		[[nodiscard]] const ActionDeclaration& getDeclaration() const
		{
			return declaration;
		}
		[[nodiscard]] ActionDeclaration& getDeclaration() { return declaration; }
		llvm::Error deduceTypes(const SymbolTable& tb, TypeDB& db);

		llvm::Error checkReturnedExpressionTypesAreCorrect() const;

		private:
		ActionDeclaration declaration;
		Statement body;
	};

	class ActionDefinitionGraph
	{
		public:
		ActionDefinition* graph;
	};
}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::ActionDefinition>
{
	static void mapping(IO& io, rlc::ActionDefinition& value)
	{
		assert(io.outputting());
		io.mapRequired("declaration", value.getDeclaration());
		io.mapRequired("body", value.getBody());
	}
};

template<>
struct llvm::GraphTraits<rlc::ActionDefinitionGraph>
{
	struct Node
	{
		Node(rlc::Statement& stat): statement(&stat) {}

		[[nodiscard]] bool operator==(const Node&) const = default;
		rlc::Statement* statement;
	};
	using NodeRef = Node;
	using ChildIteratorType = rlc::Statement::StatIterator;

	static NodeRef getEntryNode(const rlc::ActionDefinitionGraph& function)
	{
		return function.graph->getBody();
	}
	static ChildIteratorType child_begin(NodeRef current)
	{
		return current.statement->begin();
	}
	static ChildIteratorType child_end(NodeRef current)
	{
		return current.statement->end();
	}
};

template<>
struct llvm::GraphTraits<rlc::ActionDefinition>
{
	struct Node
	{
		Node(const rlc::Statement& stat): statement(&stat) {}

		[[nodiscard]] bool operator==(const Node&) const = default;
		const rlc::Statement* statement;
	};
	using NodeRef = Node;
	using ChildIteratorType = rlc::Statement::ConstStatIterator;

	static NodeRef getEntryNode(const rlc::ActionDefinition& function)
	{
		return function.getBody();
	}
	static ChildIteratorType child_begin(NodeRef current)
	{
		return current.statement->begin();
	}
	static ChildIteratorType child_end(NodeRef current)
	{
		return current.statement->end();
	}
};

template<>
struct llvm::PointerLikeTypeTraits<
		llvm::GraphTraits<rlc::ActionDefinition>::Node>
{
	static void* getAsVoidPointer(
			llvm::GraphTraits<rlc::ActionDefinition>::Node node)
	{
		return (void*) node.statement;
	}

	static llvm::GraphTraits<rlc::ActionDefinition>::Node getFromVoidPtr(
			void* node)
	{
		auto* stm = static_cast<rlc::Statement*>(node);
		return *stm;
	}
};

template<>
struct llvm::PointerLikeTypeTraits<
		llvm::GraphTraits<rlc::ActionDefinitionGraph>::Node>
{
	static void* getAsVoidPointer(
			llvm::GraphTraits<rlc::ActionDefinitionGraph>::Node node)
	{
		return (void*) node.statement;
	}

	static llvm::GraphTraits<rlc::ActionDefinitionGraph>::Node getFromVoidPtr(
			void* node)
	{
		auto* stm = static_cast<rlc::Statement*>(node);
		return *stm;
	}
};

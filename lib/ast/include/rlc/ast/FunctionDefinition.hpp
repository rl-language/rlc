#pragma once

#include <string>

#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class FunctionDefinition
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::FunctionDefinition>;
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool dumpPosition = false) const;
		void dump() const;

		using iterator = FunctionDeclaration::iterator;
		using const_iterator = FunctionDeclaration::const_iterator;

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

		[[nodiscard]] const Statement& getBody() const { return body; }
		[[nodiscard]] Statement& getBody() { return body; }
		[[nodiscard]] const SingleTypeUse& getTypeUse() const
		{
			return declaration.getTypeUse();
		}

		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return declaration.getSourcePosition();
		}

		FunctionDefinition(
				BuiltinFunctions name,
				Statement body,
				SingleTypeUse returnTypeName,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: declaration(
							std::move(name),
							std::move(returnTypeName),
							std::move(arguments),
							pos),
					body(std::move(body))
		{
		}

		FunctionDefinition(
				std::string name,
				Statement body,
				SingleTypeUse returnTypeName,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: declaration(
							std::move(name),
							std::move(returnTypeName),
							std::move(arguments),
							pos),
					body(std::move(body))
		{
		}

		FunctionDefinition(
				std::string name,
				Statement body,
				SingleTypeUse returnTypeName,
				SourcePosition pos)
				: declaration(
							std::move(name), std::move(returnTypeName), std::move(pos)),
					body(std::move(body))
		{
		}

		[[nodiscard]] const FunctionDeclaration& getDeclaration() const
		{
			return declaration;
		}
		[[nodiscard]] FunctionDeclaration& getDeclaration() { return declaration; }
		llvm::Error deduceTypes(const SymbolTable& tb, TypeDB& db);

		llvm::Error checkReturnedExpressionTypesAreCorrect() const;

		[[nodiscard]] bool isAction() const;

		private:
		FunctionDeclaration declaration;
		Statement body;
	};

	struct FunctionStatementGraph
	{
		FunctionDefinition* definition;
	};
}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::FunctionDefinition>
{
	static void mapping(IO& io, rlc::FunctionDefinition& value)
	{
		assert(io.outputting());
		io.mapRequired("declaration", value.getDeclaration());
		io.mapRequired("body", value.getBody());
	}
};

template<>
struct llvm::GraphTraits<rlc::FunctionStatementGraph>
{
	struct Node
	{
		Node(rlc::Statement& stat): statement(&stat) {}

		[[nodiscard]] bool operator==(const Node&) const = default;
		rlc::Statement* statement;
	};
	using NodeRef = Node;
	using ChildIteratorType = rlc::Statement::StatIterator;

	static NodeRef getEntryNode(const rlc::FunctionStatementGraph& function)
	{
		return function.definition->getBody();
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
struct llvm::GraphTraits<rlc::FunctionDefinition>
{
	struct Node
	{
		Node(const rlc::Statement& stat): statement(&stat) {}

		[[nodiscard]] bool operator==(const Node&) const = default;
		const rlc::Statement* statement;
	};
	using NodeRef = Node;
	using ChildIteratorType = rlc::Statement::ConstStatIterator;

	static NodeRef getEntryNode(const rlc::FunctionDefinition& function)
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
		llvm::GraphTraits<rlc::FunctionStatementGraph>::Node>
{
	static void* getAsVoidPointer(
			llvm::GraphTraits<rlc::FunctionStatementGraph>::Node node)
	{
		return (void*) node.statement;
	}

	static llvm::GraphTraits<rlc::FunctionStatementGraph>::Node getFromVoidPtr(
			void* node)
	{
		auto* stm = static_cast<rlc::Statement*>(node);
		return *stm;
	}
};

template<>
struct llvm::PointerLikeTypeTraits<
		llvm::GraphTraits<rlc::FunctionDefinition>::Node>
{
	static void* getAsVoidPointer(
			llvm::GraphTraits<rlc::FunctionDefinition>::Node node)
	{
		return (void*) node.statement;
	}

	static llvm::GraphTraits<rlc::FunctionDefinition>::Node getFromVoidPtr(
			void* node)
	{
		auto* stm = static_cast<rlc::Statement*>(node);
		return *stm;
	}
};

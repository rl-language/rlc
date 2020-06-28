#pragma once

#include <string>

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
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool dumpPosition = false) const;
		void dump() const;

		[[nodiscard]] auto begin() const { return declaration.begin(); }
		[[nodiscard]] auto begin() { return declaration.begin(); }
		[[nodiscard]] auto end() const { return declaration.end(); }
		[[nodiscard]] auto end() { return declaration.end(); }
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
				std::string name,
				Statement body,
				SingleTypeUse returnTypeName,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: declaration(
							std::move(name), std::move(returnTypeName), std::move(arguments)),
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

		private:
		FunctionDeclaration declaration;
		Statement body;
	};
}	 // namespace rlc

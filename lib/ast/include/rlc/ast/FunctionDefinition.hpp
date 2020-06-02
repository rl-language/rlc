#pragma once

#include <string>

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class ArgumentDeclaration
	{
		public:
		[[nodiscard]] Type* getType() const { return tp; }
		[[nodiscard]] const std::string& getName() const { return name; }
		[[nodiscard]] const SingleTypeUse& getTypeUse() const { return typeName; }
		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return sourcePosition;
		}

		ArgumentDeclaration(
				std::string nm,
				SingleTypeUse typeUse,
				SourcePosition pos = SourcePosition())
				: name(std::move(nm)),
					typeName(std::move(typeUse)),
					sourcePosition(std::move(pos))
		{
		}

		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool dumpPosition = false) const;
		void dump() const;

		private:
		std::string name;
		SingleTypeUse typeName;
		Type* tp{ nullptr };
		SourcePosition sourcePosition;
	};

	class FunctionDefinition
	{
		public:
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool dumpPosition = false) const;
		void dump() const;

		[[nodiscard]] auto begin() const { return arguments.begin(); }
		[[nodiscard]] auto begin() { return arguments.begin(); }
		[[nodiscard]] auto end() const { return arguments.end(); }
		[[nodiscard]] auto end() { return arguments.end(); }
		[[nodiscard]] size_t argumentsCount() const { return arguments.size(); }
		[[nodiscard]] const ArgumentDeclaration& operator[](size_t index) const
		{
			assert(index < argumentsCount());
			return arguments[index];
		}
		[[nodiscard]] ArgumentDeclaration& operator[](size_t index)
		{
			assert(index < argumentsCount());
			return arguments[index];
		}

		[[nodiscard]] Type* getType() const { return type; }

		[[nodiscard]] const std::string& getName() const { return name; }

		[[nodiscard]] const Statement& getBody() const { return body; }
		[[nodiscard]] const SingleTypeUse& getTypeUse() const
		{
			return returnTypeName;
		}

		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return position;
		}

		FunctionDefinition(
				std::string name,
				Statement body,
				SingleTypeUse returnTypeName,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: name(std::move(name)),
					body(std::move(body)),
					arguments(std::move(arguments)),
					returnTypeName(std::move(returnTypeName)),
					position(std::move(pos))
		{
		}

		FunctionDefinition(
				std::string name,
				Statement body,
				SingleTypeUse returnTypeName,
				SourcePosition pos)
				: name(std::move(name)),
					body(std::move(body)),
					arguments(),
					returnTypeName(std::move(returnTypeName)),
					position(std::move(pos))
		{
		}

		private:
		std::string name;
		Statement body;
		Type* type{ nullptr };
		llvm::SmallVector<ArgumentDeclaration, 3> arguments;
		SingleTypeUse returnTypeName;
		SourcePosition position;
	};
}	 // namespace rlc

#pragma once
#include <string>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class ArgumentDeclaration
	{
		public:
		[[nodiscard]] Type* getType() const { return typeName.getType(); }
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
		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);

		private:
		std::string name;
		SingleTypeUse typeName;
		SourcePosition sourcePosition;
	};

	class FunctionDeclaration
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
		[[nodiscard]] Type* getReturnType() const
		{
			return returnTypeName.getType();
		}

		[[nodiscard]] const std::string& getName() const { return name; }

		[[nodiscard]] const SingleTypeUse& getTypeUse() const
		{
			return returnTypeName;
		}

		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return position;
		}

		FunctionDeclaration(
				std::string name,
				SingleTypeUse returnTypeName,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: name(std::move(name)),
					arguments(std::move(arguments)),
					returnTypeName(std::move(returnTypeName)),
					position(std::move(pos))
		{
		}

		FunctionDeclaration(
				std::string name, SingleTypeUse returnTypeName, SourcePosition pos)
				: name(std::move(name)),
					arguments(),
					returnTypeName(std::move(returnTypeName)),
					position(std::move(pos))
		{
		}

		[[nodiscard]] std::string mangledName() const;

		[[nodiscard]] std::string canonicalName() const;
		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);

		private:
		std::string name;
		Type* type{ nullptr };
		llvm::SmallVector<ArgumentDeclaration, 3> arguments;
		SingleTypeUse returnTypeName;
		SourcePosition position;
	};
}	 // namespace rlc

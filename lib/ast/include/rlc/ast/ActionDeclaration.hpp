#pragma once
#include <string>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/ArgumentDeclaration.hpp"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{

	class ActionDeclaration
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::ActionDeclaration>;
		void print(
				llvm::raw_ostream& OS,
				size_t indents = 0,
				bool dumpPosition = false) const;
		void dump() const;

		using iterator = llvm::SmallVector<ArgumentDeclaration, 3>::iterator;
		using const_iterator =
				llvm::SmallVector<ArgumentDeclaration, 3>::const_iterator;

		[[nodiscard]] const_iterator begin() const { return arguments.begin(); }
		[[nodiscard]] iterator begin() { return arguments.begin(); }
		[[nodiscard]] const_iterator end() const { return arguments.end(); }
		[[nodiscard]] iterator end() { return arguments.end(); }
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

		[[nodiscard]] std::string getEntityName() const
		{
			return getName() + "Entity";
		}

		[[nodiscard]] Type* getType() const { return type; }

		[[nodiscard]] const std::string& getName() const { return name; }

		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return position;
		}

		ActionDeclaration(
				std::string name,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: name(std::move(name)),
					arguments(std::move(arguments)),
					position(std::move(pos))
		{
		}

		ActionDeclaration(
				BuiltinFunctions builtin,
				llvm::SmallVector<ArgumentDeclaration, 3> arguments = {},
				SourcePosition pos = SourcePosition())
				: name(builtinFunctionsToString(builtin)),
					arguments(std::move(arguments)),
					position(std::move(pos))
		{
		}

		ActionDeclaration(std::string name, SourcePosition pos)
				: name(std::move(name)), position(std::move(pos))
		{
		}

		[[nodiscard]] std::string mangledName() const;

		[[nodiscard]] std::string canonicalName() const;
		llvm::Error deduceType(const SymbolTable& tb, TypeDB& db);

		[[nodiscard]] bool isBuiltin() const
		{
			return position.getFileName() == BuiltinFileName;
		}

		private:
		std::string name;
		Type* type{ nullptr };
		llvm::SmallVector<ArgumentDeclaration, 3> arguments;
		SourcePosition position;
	};
}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::ActionDeclaration>
{
	static void mapping(IO& io, rlc::ActionDeclaration& value)
	{
		assert(io.outputting());
		io.mapRequired("name", value.name);
		if (value.type != nullptr)
			io.mapRequired("type", *value.type);
		io.mapRequired("args", value.arguments);
		if (not value.getSourcePosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

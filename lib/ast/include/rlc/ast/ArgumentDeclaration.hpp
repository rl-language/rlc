#pragma once
#include <string>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class ArgumentDeclaration
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::ArgumentDeclaration>;
		[[nodiscard]] Type* getType() const { return typeName.getType(); }
		[[nodiscard]] const std::string& getName() const { return name; }
		[[nodiscard]] const SingleTypeUse& getTypeUse() const { return typeName; }
		[[nodiscard]] const SourcePosition& getSourcePosition() const
		{
			return sourcePosition;
		}
		[[nodiscard]] bool isPrivate() const { return getName().starts_with("_"); }

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

}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::ArgumentDeclaration>
{
	static void mapping(IO& io, rlc::ArgumentDeclaration& value)
	{
		assert(io.outputting());
		io.mapRequired("name", value.name);
		io.mapRequired("type", value.typeName);
		if (not value.getSourcePosition().isMissing())
			io.mapRequired("position", value.sourcePosition);
	}
};

template<>
struct llvm::yaml::SequenceTraits<
		llvm::SmallVector<rlc::ArgumentDeclaration, 3>>
{
	static size_t size(
			IO& io, llvm::SmallVector<rlc::ArgumentDeclaration, 3>& list)
	{
		return list.size();
	}
	static rlc::ArgumentDeclaration& element(
			IO& io,
			llvm::SmallVector<rlc::ArgumentDeclaration, 3>& list,
			size_t index)
	{
		assert(io.outputting());
		return list[index];
	}
};

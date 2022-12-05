#pragma once

#include <optional>
#include <string>

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class Reference
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::Reference>;
		static constexpr const char* name = "reference";
		explicit Reference(std::string name, SourcePosition position)
				: refName(std::move(name)), position(std::move(position))
		{
		}
		explicit Reference(BuiltinFunctions fun, SourcePosition position)
				: refName(builtinFunctionsToString(fun)), position(std::move(position))
		{
		}
		[[nodiscard]] const std::string& getName() const { return refName; }

		void print(llvm::raw_ostream& OS) const { OS << refName; }

		[[nodiscard]] bool operator==(const Reference& other) const
		{
			return refName == other.refName;
		}

		[[nodiscard]] bool operator!=(const Reference& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }
		void setPosition(const SourcePosition& newPoisition)
		{
			position = newPoisition;
		}

		void setReferred(Symbol s) { referred = s; }
		bool hasReference() { return referred.has_value(); }

		Symbol getReferred()
		{
			assert(referred.has_value());
			return referred.value();
		}

		private:
		std::string refName;
		std::optional<Symbol> referred;
		SourcePosition position;
	};
}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::Reference>
{
	static void mapping(IO& io, rlc::Reference& value)
	{
		assert(io.outputting());
		io.mapRequired("name", value.refName);
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

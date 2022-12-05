#pragma once

#include <string>

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/BuiltinFunctions.hpp"
#include "rlc/ast/TypeUse.hpp"
#include "rlc/utils/SourcePosition.hpp"

namespace rlc
{
	class ZeroInitializer
	{
		public:
		friend llvm::yaml::MappingTraits<rlc::ZeroInitializer>;
		static constexpr const char* name = "zero_initializer";
		explicit ZeroInitializer(SingleTypeUse typeName, SourcePosition position)
				: refName(std::move(typeName)), position(std::move(position))
		{
		}

		void print(llvm::raw_ostream& OS) const { refName.print(OS); }

		[[nodiscard]] bool operator==(const ZeroInitializer& other) const
		{
			return refName == other.refName;
		}

		[[nodiscard]] bool operator!=(const ZeroInitializer& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] const SourcePosition& getPosition() const { return position; }
		void setPosition(const SourcePosition& newPoisition)
		{
			position = newPoisition;
		}

		SingleTypeUse& getTypeUse() { return refName; }

		[[nodiscard]] const SingleTypeUse& getTypeUse() const { return refName; }

		private:
		SingleTypeUse refName;
		SourcePosition position;
	};
}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::ZeroInitializer>
{
	static void mapping(IO& io, rlc::ZeroInitializer& value)
	{
		assert(io.outputting());
		io.mapRequired("type_use", value.refName);
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};

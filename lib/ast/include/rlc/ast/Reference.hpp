#pragma once

#include <string>

#include "llvm/Support/raw_ostream.h"

namespace rlc
{
	class Reference
	{
		public:
		explicit Reference(std::string name): name(std::move(name)) {}
		[[nodiscard]] const std::string& getName() const { return name; }

		void print(llvm::raw_ostream& OS) const { OS << name; }

		[[nodiscard]] bool operator==(const Reference& other) const
		{
			return name == other.name;
		}

		[[nodiscard]] bool operator!=(const Reference& other) const
		{
			return !(*this == other);
		}

		private:
		std::string name;
	};
}	 // namespace rlc

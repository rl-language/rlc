#pragma once

#include <memory>
#include <string>

#include "llvm/ADT/StringRef.h"
namespace rlc
{
	class SourcePosition
	{
		public:
		SourcePosition(std::string fileName, size_t l = 0, size_t c = 0)
				: sourceFileName(std::make_shared<std::string>(fileName)),
					line(l),
					column(c)
		{
		}
		[[nodiscard]] size_t getLine() const { return line; }
		[[nodiscard]] size_t getColumn() const { return column; }
		[[nodiscard]] llvm::StringRef getFileName() const
		{
			return *sourceFileName;
		}
		void setLine(size_t l) { line = l; }
		void setColumn(size_t c) { column = c; }

		[[nodiscard]] std::string toString() const
		{
			std::string toReturn = *sourceFileName;
			toReturn += "[";
			toReturn += std::to_string(line);
			toReturn += ",";
			toReturn += std::to_string(column);
			toReturn += "]";

			return toReturn;
		}

		private:
		std::shared_ptr<std::string> sourceFileName;
		size_t line;
		size_t column;
	};
}	 // namespace rlc

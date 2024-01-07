/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "mlir/Transforms/DialectConversion.h"
#include "rlc/python/Dialect.h"
#include "rlc/python/Interfaces.hpp"
#include "rlc/python/Types.hpp"

namespace mlir
{
	class IRRewriter;
}
namespace mlir::rlc::python
{
	class SerializationContext
	{
		public:
		[[nodiscard]] size_t getIndent() const { return currentIndent; }
		void indent() { currentIndent++; }
		void deindent() { currentIndent--; }
		[[nodiscard]] std::string getUniqueName()
		{
			std::string s;
			llvm::raw_string_ostream OS(s);
			OS << "var" << currentVarNameIndex++;
			OS.flush();
			return s;
		}

		llvm::StringRef registerValue(mlir::Value value, std::string name = "")
		{
			if (name.empty())
				name = getUniqueName();
			assert(varName.count(value) == 0);
			varName[value] = name;
			return varName[value];
		}

		llvm::StringRef nameOf(mlir::Value value) const
		{
			return varName.find(value)->second;
		}

		private:
		size_t currentIndent = 0;
		size_t currentVarNameIndex = 0;
		mlir::DenseMap<mlir::Value, std::string> varName;
	};

	::mlir::LogicalResult serializePython(
			llvm::raw_ostream& OS,
			mlir::Operation& op,
			SerializationContext& context);

	::mlir::LogicalResult serializePythonModule(
			llvm::raw_ostream& OS,
			mlir::Operation& op,
			SerializationContext& context);

	inline ::mlir::LogicalResult serializePythonModule(
			llvm::raw_ostream& OS, mlir::Operation& op)
	{
		SerializationContext context;
		return serializePythonModule(OS, op, context);
	}

}	 // namespace mlir::rlc::python
#include "rlc/python/Interfaces.inc"

namespace mlir::rlc
{

}	 // namespace mlir::rlc

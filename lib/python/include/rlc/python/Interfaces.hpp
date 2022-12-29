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

	inline ::mlir::LogicalResult serializePython(
			llvm::raw_ostream& OS, mlir::Operation& op)
	{
		SerializationContext context;
		return serializePython(OS, op, context);
	}

}	 // namespace mlir::rlc::python
#include "rlc/python/Interfaces.inc"

namespace mlir::rlc
{

}	 // namespace mlir::rlc

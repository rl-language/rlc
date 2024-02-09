/*
Copyright 2024 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

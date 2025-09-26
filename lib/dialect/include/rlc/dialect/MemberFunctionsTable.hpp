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

#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/ActionArgumentAnalysis.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"
#include "rlc/utils/PatternMatcher.hpp"

namespace mlir::rlc
{
	// a table that keeps track of which member functions belong to which type
	// You can query it to check if a type has a the init, drop and assign
	// methods, and to get all member functions of a type.
	class MemberFunctionsTable
	{
		public:
		MemberFunctionsTable(mlir::ModuleOp mod)
		{
			for (auto op : mod.getOps<mlir::rlc::FunctionOp>())
				if (op.getIsMemberFunction() and not op.isInternal() and
						(mlir::isa<mlir::rlc::ClassType>(op.getArgumentTypes()[0]) or
						 mlir::isa<mlir::rlc::AlternativeType>(op.getArgumentTypes()[0])))
				{
					auto selfType = op.getArgumentTypes()[0];
					auto key = selfType.getAsOpaquePointer();
					if (isInitFunction(selfType, op))
						initFunction[key] = op;
					else if (isDropFunction(selfType, op))
						dropFunction[key] = op;
					else if (isAssignFunction(selfType, op))
						assignFunction[key] = op;
					else
						typeToMethods[key].insert(op);
				}
		}

		bool isInitFunction(mlir::Type t, mlir::rlc::FunctionOp method)
		{
			return (
					method.getUnmangledName() == "init" and
					returnsVoid(method.getType()).succeeded() and
					method.getType().getNumInputs() == 1 and
					method.getType().getInput(0) == t);
		}

		bool isTriviallyInitializable(mlir::Type t)
		{
			return initFunction.count(t.getAsOpaquePointer()) == 0;
		}

		bool isDropFunction(mlir::Type t, mlir::rlc::FunctionOp method)
		{
			return (
					method.getUnmangledName() == "drop" and
					returnsVoid(method.getType()).succeeded() and
					method.getType().getNumInputs() == 1 and
					method.getType().getInput(0) == t);
		}

		bool isTriviallyDestructible(mlir::Type t)
		{
			return dropFunction.count(t.getAsOpaquePointer()) == 0;
		}

		bool isAssignFunction(mlir::Type t, mlir::rlc::FunctionOp method)
		{
			return (
					method.getUnmangledName() == "assign" and
					returnsVoid(method.getType()).succeeded() and
					method.getType().getNumInputs() == 2 and
					method.getType().getInput(0) == t and
					method.getType().getInput(1) == t);
		}

		bool isTriviallyCopiable(mlir::Type t)
		{
			return assignFunction.count(t.getAsOpaquePointer()) == 0;
		}

		llvm::DenseSet<mlir::rlc::FunctionOp> getMemberFunctionsOf(mlir::Type type)
		{
			return typeToMethods[type.getAsOpaquePointer()];
		}

		private:
		std::map<const void*, llvm::DenseSet<mlir::rlc::FunctionOp>> typeToMethods;
		std::map<const void*, mlir::rlc::FunctionOp> initFunction;
		std::map<const void*, mlir::rlc::FunctionOp> dropFunction;
		std::map<const void*, mlir::rlc::FunctionOp> assignFunction;
	};
}	 // namespace mlir::rlc

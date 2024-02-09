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
#include "rlc/python/Dialect.h"

#include "Dialect.inc"
#include "rlc/python/Interfaces.hpp"
#include "rlc/python/Operations.hpp"
#include "rlc/python/Types.hpp"

class PythonTypeAliasASMInterface: public mlir::OpAsmDialectInterface
{
	public:
	using mlir::OpAsmDialectInterface::OpAsmDialectInterface;

	AliasResult getAlias(mlir::Type type, llvm::raw_ostream& OS) const final
	{
		if (auto casted = type.dyn_cast<mlir::rlc::python::CTypeStructType>())
		{
			OS << casted.getName();
			return AliasResult::FinalAlias;
		}

		return AliasResult::NoAlias;
	}
};

void mlir::rlc::python::RLCPython::initialize()
{
	registerTypes();
	registerOperations();
	addInterfaces<PythonTypeAliasASMInterface>();
}

namespace mlir::rlc::python
{

	static void emitCallToOverload(
			llvm::raw_ostream& OS, mlir::rlc::python::PythonFun fun)
	{
		OS << "return ";
		OS << fun.getSymName();
		OS << "(*args)";
		if (not fun.getResultTypes().empty() and
				fun.getResultTypes().front().isa<mlir::rlc::python::FloatType>())
		{
			OS << ".value";
		}
		OS << "\n\n";
	}

	static void emitOverloads(
			llvm::raw_ostream& OS,
			llvm::StringMap<std::vector<mlir::rlc::python::PythonFun>>& overloads)
	{
		OS << "class functions:\n";
		for (auto& overload : overloads)
		{
			OS.indent(4 * 1);
			OS << "def " << overload.getKey() << "(*args):\n";
			for (auto& candidate : overload.second)
			{
				OS.indent(4 * 2);
				OS << "if len(args) == " << candidate.getArgumentTypes().size();
				if (not candidate.getArgumentTypes().empty())
					OS << " and ";
				for (const auto& pair : llvm::enumerate(candidate.getArgumentTypes()))
				{
					OS << "isinstance(args[" << pair.index() << "], "
						 << typeToString(pair.value()) << ")";

					if (pair.index() != candidate.getArgumentTypes().size() - 1)
						OS << " and ";
				}

				OS << ":\n";
				OS.indent(4 * 3);
				emitCallToOverload(OS, candidate);
				OS << "\n";
			}
			OS.indent(4 * 2);
			OS << "assert False, \"no correct overload to invoke " << overload.first()
				 << "\"";
			OS << "\n\n";
		}
	}

	template<typename Filter>
	mlir::LogicalResult serializeAllChidlrenIf(
			llvm::raw_ostream& OS,
			mlir::Operation& op,
			SerializationContext& context,
			Filter&& filter)
	{
		for (auto& region : op.getRegions())
		{
			for (auto& block : region.getBlocks())
			{
				for (auto& subOp : block.getOperations())
				{
					assert(subOp.hasTrait<mlir::rlc::python::EmitPython::Trait>());
					if (not filter(subOp))
						continue;
					if (mlir::cast<mlir::rlc::python::EmitPython>(subOp)
									.emit(OS, context)
									.failed())
						return mlir::failure();
				}
			}
		}

		return mlir::success();
	}

	mlir::LogicalResult serializePython(
			llvm::raw_ostream& OS, mlir::Operation& op, SerializationContext& context)
	{
		if (serializeAllChidlrenIf(OS, op, context, [](const mlir::Operation& Op) {
					return mlir::isa<mlir::rlc::python::CTypesLoad>(Op);
				}).failed())

			return mlir::failure();
		if (serializeAllChidlrenIf(OS, op, context, [](const mlir::Operation& Op) {
					return mlir::isa<mlir::rlc::python::CTypeStructDecl>(Op);
				}).failed())
			return mlir::failure();

		if (serializeAllChidlrenIf(OS, op, context, [](const mlir::Operation& Op) {
					return not mlir::isa<mlir::rlc::python::CTypeStructDecl>(Op) and
								 not mlir::isa<mlir::rlc::python::CTypesLoad>(Op);
				}).failed())
			return mlir::failure();

		return mlir::success();
	}

	mlir::LogicalResult serializePythonModule(
			llvm::raw_ostream& OS, mlir::Operation& op, SerializationContext& context)
	{
		if (serializePython(OS, op, context).failed())
			return mlir::failure();

		llvm::StringMap<std::vector<mlir::rlc::python::PythonFun>> overloads;
		for (auto& region : op.getRegions())
		{
			for (auto& block : region.getBlocks())
			{
				for (auto& subOp : block.getOperations())
				{
					if (auto casted = mlir::dyn_cast<mlir::rlc::python::PythonFun>(subOp);
							casted != nullptr)
						overloads[casted.getOverloadName()].push_back(casted);
				}
			}
		}

		emitOverloads(OS, overloads);
		return mlir::success();
	}
}	 // namespace mlir::rlc::python

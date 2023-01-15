#include "rlc/python/Dialect.h"

#include "Dialect.inc"
#include "rlc/python/Interfaces.hpp"
#include "rlc/python/Operations.hpp"
#include "rlc/python/Types.hpp"

void mlir::rlc::python::RLCPython::initialize()
{
	registerTypes();
	registerOperations();
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
				for (auto& pair : llvm::enumerate(candidate.getArgumentTypes()))
				{
					OS << "isinstance(args[" << pair.index() << "], "
						 << typeToString(pair.value()) << ")";

					if (pair.index() != candidate.getArgumentTypes().size() - 1)
						OS << " and ";
				}

				OS << ":\n";
				OS.indent(4 * 3);
				emitCallToOverload(OS, candidate);
				OS.indent(4 * 2);
				OS << "assert False, \"no correct overload to invoke "
					 << overload.first() << "\"";
				OS << "\n\n";
			}
		}
	}

	mlir::LogicalResult serializePython(
			llvm::raw_ostream& OS, mlir::Operation& op, SerializationContext& context)
	{
		for (auto& region : op.getRegions())
		{
			for (auto& block : region.getBlocks())
			{
				for (auto& subOp : block.getOperations())
				{
					assert(subOp.hasTrait<mlir::rlc::python::EmitPython::Trait>());
					if (mlir::cast<mlir::rlc::python::EmitPython>(subOp)
									.emit(OS, context)
									.failed())
						return mlir::failure();
				}
			}
		}

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

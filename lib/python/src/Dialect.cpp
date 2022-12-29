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
	static void emitArgCast(
			llvm::raw_ostream& OS, mlir::rlc::python::PythonFun fun)
	{
		for (auto& pair : llvm::enumerate(fun.getArgumentTypes()))
		{
			mlir::Type type = pair.value();
			if (type.isa<mlir::rlc::python::PythonBoolType>())
			{
				OS << "c_bool(args[" << pair.index() << "])";
			}
			else if (type.isa<mlir::rlc::python::PythonIntType>())
			{
				OS << "c_longlong(args[" << pair.index() << "])";
			}
			else if (type.isa<mlir::rlc::python::PythonFloatType>())
			{
				OS << "c_double(args[" << pair.index() << "])";
			}
			else
			{
				OS << "args[" << pair.index() << "]";
			}
			OS << ", ";
		}
	}

	static void emitCallToOverload(
			llvm::raw_ostream& OS, mlir::rlc::python::PythonFun fun)
	{
		OS << "return ";
		OS << fun.getSymName();
		OS << "(";
		emitArgCast(OS, fun);
		OS << ")\n\n";
	}

	static void emitOverloads(
			llvm::raw_ostream& OS,
			llvm::StringMap<std::vector<mlir::rlc::python::PythonFun>> overloads)
	{
		for (auto& overload : overloads)
		{
			OS << "def " << overload.getKey() << "(*args):\n";
			if (overload.second.size() == 1)
			{
				OS.indent(4);
				emitCallToOverload(OS, overload.second.front());
				continue;
			}

			for (auto& candidate : overload.second)
			{
				OS.indent(4);
				OS << "if ";
				for (auto& pair : llvm::enumerate(candidate.getArgumentTypes()))
				{
					OS << "isinstance(args[" << pair.index() << "], "
						 << builtinTypeToString(pair.value()) << ")";

					if (pair.index() != candidate.getArgumentTypes().size() - 1)
						OS << " and ";
				}

				OS << ":\n";
				OS.indent(8);
				emitCallToOverload(OS, candidate);
				OS << "\n";
			}
		}
	}
	mlir::LogicalResult serializePython(
			llvm::raw_ostream& OS, mlir::Operation& op, SerializationContext& context)
	{
		llvm::StringMap<std::vector<mlir::rlc::python::PythonFun>> overloads;
		for (auto& region : op.getRegions())
			for (auto& block : region.getBlocks())
				for (auto& subOp : block.getOperations())
				{
					assert(subOp.hasTrait<mlir::rlc::python::EmitPython::Trait>());
					if (mlir::cast<mlir::rlc::python::EmitPython>(subOp)
									.emit(OS, context)
									.failed())
						return mlir::failure();

					if (auto casted = mlir::dyn_cast<mlir::rlc::python::PythonFun>(subOp);
							casted != nullptr)
						overloads[casted.getOverloadName()].push_back(casted);
				}
		emitOverloads(OS, overloads);

		return mlir::success();
	}
}	 // namespace mlir::rlc::python

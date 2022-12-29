#pragma once

#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#define GET_TYPEDEF_CLASSES
#include "rlc/python/Types.inc"

namespace mlir::rlc
{
	inline void writeBuiltinTypeName(llvm::raw_ostream& OS, mlir::Type t)
	{
		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonIntType>())
		{
			OS << "int";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonFloatType>())
		{
			OS << "float";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonBoolType>())
		{
			OS << "bool";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonNoneType>())
		{
			OS << "None";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CArrayType>())
		{
			OS << " list ";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CTypeStructType>())
		{
			OS << maybeType.getName();
			return;
		}

		assert(false && "unrechable");
	}
	inline void writeTypeName(llvm::raw_ostream& OS, mlir::Type t)
	{
		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonIntType>())
		{
			OS << "c_longlong";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonFloatType>())
		{
			OS << "c_double";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonBoolType>())
		{
			OS << "c_bool";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::PythonNoneType>())
		{
			OS << "None";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CArrayType>())
		{
			writeTypeName(OS, maybeType.getSubType());
			OS << " * " << maybeType.getSize();
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CTypeStructType>())
		{
			OS << maybeType.getName();
			return;
		}

		assert(false && "unrechable");
	}

	inline std::string typeToString(mlir::Type t)
	{
		std::string toReturn;
		llvm::raw_string_ostream OS(toReturn);
		writeTypeName(OS, t);
		OS.flush();
		return toReturn;
	}

	inline std::string builtinTypeToString(mlir::Type t)
	{
		std::string toReturn;
		llvm::raw_string_ostream OS(toReturn);
		writeBuiltinTypeName(OS, t);
		OS.flush();
		return toReturn;
	}

}	 // namespace mlir::rlc

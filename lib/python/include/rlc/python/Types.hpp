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
	inline void writeTypeName(
			llvm::raw_ostream& OS, mlir::Type t, bool ctypesSyntax = false)
	{
		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CTypesIntType>())
		{
			if (maybeType.getSize() == 8)
			{
				OS << "c_byte";
			}
			else if (maybeType.getSize() == 64)
			{
				OS << "c_longlong";
			}
			else
			{
				llvm_unreachable("unhandled");
			}
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CTypesFloatType>())
		{
			OS << "c_double";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CTypesBoolType>())
		{
			OS << "c_bool";
			return;
		}
		if (auto maybeType = t.dyn_cast<mlir::rlc::python::IntType>())
		{
			OS << "builtins.int";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::FloatType>())
		{
			OS << "builtins.float";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::BoolType>())
		{
			OS << "builtins.bool";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::NoneType>())
		{
			OS << "None";
			return;
		}

		if (auto maybeType = t.dyn_cast<mlir::rlc::python::CArrayType>())
		{
			if (ctypesSyntax)
			{
				writeTypeName(OS, maybeType.getSubType(), ctypesSyntax);
				OS << " * ";
				OS << maybeType.getSize();
			}
			else
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

	inline std::string typeToString(mlir::Type t, bool ctypesSyntax = false)
	{
		std::string toReturn;
		llvm::raw_string_ostream OS(toReturn);
		writeTypeName(OS, t, ctypesSyntax);
		OS.flush();
		return toReturn;
	}

	inline bool isBuiltinType(mlir::Type t)
	{
		return t.isa<mlir::rlc::python::BoolType>() or
					 t.isa<mlir::rlc::python::IntType>() or
					 t.isa<mlir::rlc::python::FloatType>();
	}

	inline mlir::Type pythonCTypesToBuiltin(mlir::Type t)
	{
		if (t.isa<mlir::rlc::python::CTypesBoolType>())
			return mlir::rlc::python::BoolType::get(t.getContext());
		if (t.isa<mlir::rlc::python::CTypesFloatType>())
			return mlir::rlc::python::FloatType::get(t.getContext());
		if (auto casted = t.dyn_cast<mlir::rlc::python::CTypesIntType>())
			return mlir::rlc::python::IntType::get(t.getContext(), casted.getSize());

		return t;
	}

	inline mlir::Type pythonBuiltinToCTypes(mlir::Type t)
	{
		if (t.isa<mlir::rlc::python::BoolType>())
			return mlir::rlc::python::CTypesBoolType::get(t.getContext());
		if (t.isa<mlir::rlc::python::FloatType>())
			return mlir::rlc::python::CTypesFloatType::get(t.getContext());
		if (auto casted = t.dyn_cast<mlir::rlc::python::IntType>())
			return mlir::rlc::python::CTypesIntType::get(
					t.getContext(), casted.getSize());
		return t;
	}

}	 // namespace mlir::rlc

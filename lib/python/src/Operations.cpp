#include "rlc/python/Operations.hpp"

#include "rlc/python/Dialect.h"
#define GET_OP_CLASSES
#include "./Operations.inc"

void mlir::rlc::python::RLCPython::registerOperations()
{
	addOperations<
#define GET_OP_LIST
#include "./Operations.inc"
			>();
}

mlir::LogicalResult mlir::rlc::python::PythonFun::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	context.registerValue(getResult(), getSymName().str());
	for (const auto& [operand, name] :
			 llvm::zip(getBlocks().front().getArguments(), getArgs()))
	{
		context.registerValue(operand, name.cast<mlir::StringAttr>().str());
	}

	OS << "def " << getSymName() << "(";
	const auto* typeIter = getFunctionType().getInputs().begin();
	for (const auto* iter = getArgs().begin(); iter != getArgs().end(); iter++)
	{
		OS << iter->cast<mlir::StringAttr>().getValue();
		OS << ": ";
		writeTypeName(OS, *typeIter);
		if (iter + 1 != getArgs().end())
			OS << ", ";
		typeIter++;
	}
	OS << ")";
	if (not getResultTypes().empty())
	{
		OS << " -> ";
		writeTypeName(OS, getResultTypes().front());
	}
	OS << ":\n";
	OS.indent(context.getIndent() * 4);
	context.indent();
	auto res = mlir::rlc::python::serializePython(OS, *getOperation(), context);
	context.deindent();
	if (res.failed())
		return res;

	OS << "overloads[\"" << getOverloadName() << "\"].append(" << getSymName()
		 << ")\n";
	OS << "\n";

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::CTypeStructDecl::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	auto type = getType().cast<mlir::rlc::python::CTypeStructType>();
	OS << "class " << type.getName() << "(Structure)";

	OS << ":\n";
	OS.indent((context.getIndent() + 1) * 4);
	OS << "_fields_ = [";
	for (const auto& [type, name] :
			 llvm::zip(type.getSubTypes(), getFieldNames()))
	{
		OS << "(" << name << ", ";
		writeTypeName(OS, type);
		OS << "), ";
	}
	OS << "]\n\n";

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::PythonAccess::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	OS << context.registerValue(getResult()) << " = ";
	OS << context.nameOf(getLhs());
	OS << ".";
	OS << getFieldName();
	OS << "\n";
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::PythonReturn::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	if (getLhs().empty())
	{
		OS << "return\n";
		return mlir::success();
	}

	OS << "return " << context.nameOf(getLhs().front()) << "\n";
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::AssignResultType::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	OS << context.nameOf(getLhs()) << ".restype = ";
	writeTypeName(OS, getReturnType());
	OS << "\n";

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::CTypesLoad::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS << "from ctypes import *\n";
	OS << "from collections import defaultdict\n\n";
	OS << "lib = CDLL(\"" << getLibName() << "\")\n";
	context.registerValue(getResult(), "lib");

	OS << "overloads = defaultdict(list)\n";

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::PythonCast::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	assert(false);
}

mlir::LogicalResult mlir::rlc::python::PythonCall::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	assert(getNumResults() <= 1);
	if (getNumResults() != 0)
	{
		OS << context.registerValue(getResults().front()) << " = ";
	}

	OS << context.nameOf(getCallee()) << "(";

	for (auto arg : getArgs())
	{
		OS << "pointer(" << context.nameOf(arg) << "), ";
	}

	OS << ")\n";
	return mlir::success();
}

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

static void emitFunctionSignature(
		llvm::raw_ostream& OS,
		mlir::rlc::python::SerializationContext& context,
		llvm::StringRef name,
		mlir::FunctionType functionType,
		mlir::ArrayAttr args,
		mlir::TypeRange resultTypes)
{
	OS.indent(context.getIndent() * 4);
	OS << "def " << name << "(";
	const auto* typeIter = functionType.getInputs().begin();
	for (const auto* iter = args.begin(); iter != args.end(); iter++)
	{
		OS << (*iter).cast<mlir::StringAttr>().getValue();
		OS << ": ";
		mlir::rlc::writeTypeName(OS, *typeIter);
		if (iter + 1 != args.end())
			OS << ", ";
		typeIter++;
	}
	OS << ")";
	if (not resultTypes.empty())
	{
		OS << " -> ";
		mlir::rlc::writeTypeName(OS, resultTypes.front());
	}
	OS << ":\n";
	context.indent();
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

	OS.indent(context.getIndent() * 4);
	OS << "@overload\n";
	emitFunctionSignature(
			OS,
			context,
			getOverloadName(),
			getFunctionType(),
			getArgs(),
			getResultTypes());
	OS.indent(context.getIndent() * 4);
	OS << "pass\n\n";
	context.deindent();

	emitFunctionSignature(
			OS,
			context,
			getSymName(),
			getFunctionType(),
			getArgs(),
			getResultTypes());
	auto res = mlir::rlc::python::serializePython(OS, *getOperation(), context);
	context.deindent();
	if (res.failed())
		return res;

	OS << "\n";

	OS << "signatures[" << getSymName() << "] = [";
	if (getFunctionType().getNumResults() != 0)
		writeTypeName(OS, getFunctionType().getResult(0));
	else
		OS << "None";
	OS << ", ";
	for (mlir::Type type : getFunctionType().getInputs())
	{
		writeTypeName(OS, type);
		OS << ", ";
	}
	OS << "]\n";

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
		OS << "(\"_" << name.cast<mlir::StringAttr>().str() << "\", ";
		writeTypeName(OS, type, true);
		OS << "), ";
	}
	OS << "]\n\n";

	for (const auto& [type, name] :
			 llvm::zip(type.getSubTypes(), getFieldNames()))
	{
		OS.indent((context.getIndent() + 1) * 4);
		OS << "@property\n";
		OS.indent((context.getIndent() + 1) * 4);
		OS << "def " << name.cast<mlir::StringAttr>().str() << "(self) -> "
			 << typeToString(pythonCTypesToBuiltin(type), true) << ":\n";
		OS.indent((context.getIndent() + 2) * 4);
		OS << "return self._" << name.cast<mlir::StringAttr>().str();
		if (type.isa<mlir::rlc::python::CTypesFloatType>() or
				type.isa<mlir::rlc::python::CTypesIntType>())
		{
			OS << ".value";
		}
		OS << "\n\n";
	}

	OS << "\n\n";

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
	OS << "from typing import overload\n";
	OS << "from pathlib import Path\n";
	OS << "import builtins\n";
	OS << "from collections import defaultdict\n\n";
	OS << "lib = CDLL(Path(__file__).resolve().parent / \"" << getLibName()
		 << "\")\n";
	context.registerValue(getResult(), "lib");

	OS << "actions = defaultdict(list)\n";
	OS << "args_info = {}\n";
	OS << "signatures = {}\n";

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::PythonCast::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	OS << context.registerValue(getResult()) << " = ";

	writeTypeName(OS, getResult().getType());

	OS << "(" << context.nameOf(getLhs()) << ")\n";
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::PythonArgumentConstraint::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::python::PythonActionInfo::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	auto f = getAction().getDefiningOp<mlir::rlc::python::PythonFun>();
	OS << "actions[\"" << f.getOverloadName() << "\"].append("
		 << context.nameOf(getAction()) << ")\n";

	OS << "args_info[" << context.nameOf(getAction()) << "] = [";
	for (auto& arg : getBody().getArguments())
	{
		assert(std::distance(arg.getUses().begin(), arg.getUses().end()) <= 1);
		if (not arg.getUses().empty())
		{
			auto& Use = *arg.getUses().begin();
			auto argConstraint =
					mlir::cast<mlir::rlc::python::PythonArgumentConstraint>(
							Use.getOwner());
			OS << "(" << argConstraint.getMin() << ", " << argConstraint.getMax()
				 << ")";
		}
		else
		{
			OS << "None";
		}
		OS << ", ";
	}
	OS << "]\n\n";

	return mlir::success();
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

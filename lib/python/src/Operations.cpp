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

	OS << "wrappers[\"" << getOverloadName() << "\"].append(" << getName()
		 << ")\n";
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

static void emitStructShort(
		mlir::rlc::python::SerializationContext& context,
		llvm::raw_ostream& OS,
		bool isUnion,
		llvm::StringRef name,
		llvm::ArrayRef<llvm::StringRef> fieldNames,
		llvm::ArrayRef<llvm::StringRef> fieldTypes)
{
	OS << "class " << name << "(" << (isUnion ? "Union" : "Structure") << ")";

	OS << ":\n";
	OS.indent((context.getIndent() + 1) * 4);
	OS << "_fields_ = [";
	for (const auto& [type, name] : llvm::zip(fieldTypes, fieldNames))
	{
		OS << "(\"_" << name << "\", ";
		OS << type;
		OS << "), ";
	}
	OS << "]\n\n";
}

static void emitStructShort(
		mlir::rlc::python::SerializationContext& context,
		llvm::raw_ostream& OS,
		bool isUnion,
		llvm::StringRef name,
		llvm::ArrayRef<mlir::StringRef> fieldNames,
		mlir::TypeRange fieldTypes)
{
	llvm::SmallVector<std::string, 2> fieldTypesStrings;
	for (const auto& type : fieldTypes)
	{
		std::string s;
		llvm::raw_string_ostream OS(s);
		mlir::rlc::writeTypeName(OS, type, true);
		OS.flush();
		fieldTypesStrings.push_back(s);
	}
	llvm::SmallVector<llvm::StringRef, 2> args;
	for (auto& s : fieldTypesStrings)
		args.push_back(s);
	emitStructShort(context, OS, isUnion, name, fieldNames, args);
}

static void emitStructImplicitMethods(
		mlir::rlc::python::SerializationContext& context,
		llvm::raw_ostream& OS,
		llvm::StringRef name)
{
	OS.indent((context.getIndent() + 1) * 4);
	OS << "def __init__(self):\n";
	OS.indent((context.getIndent() + 2) * 4);
	OS << "functions.init(self)\n";
	OS << "\n";

	OS.indent((context.getIndent() + 1) * 4);
	OS << "def copy(self):\n";
	OS.indent((context.getIndent() + 2) * 4);
	OS << "return functions.assign(" << name << "(), self)\n";
	OS << "\n";

	OS.indent((context.getIndent() + 1) * 4);
	OS << "def __drop__(self):\n";
	OS.indent((context.getIndent() + 2) * 4);
	OS << "return functions.drop(" << name << "(), self)\n";
	OS << "\n";
}

static void emitStructGetter(
		mlir::rlc::python::SerializationContext& context,
		llvm::raw_ostream& OS,
		mlir::StringRef fieldName,
		mlir::StringRef fieldType)
{
	OS.indent((context.getIndent() + 1) * 4);
	OS << "@property\n";
	OS.indent((context.getIndent() + 1) * 4);
	OS << "def " << fieldName << "(self) -> " << fieldType << ":\n";
	OS.indent((context.getIndent() + 2) * 4);
	OS << "return self._" << fieldName;
	OS << "\n\n";
}

static void emitStructGetters(
		mlir::rlc::python::SerializationContext& context,
		llvm::raw_ostream& OS,
		llvm::ArrayRef<mlir::StringRef> fieldNames,
		mlir::TypeRange fieldTypes)
{
	for (const auto& [type, name] : llvm::zip(fieldTypes, fieldNames))
	{
		emitStructGetter(
				context,
				OS,
				name,
				mlir::rlc::typeToString(mlir::rlc::pythonCTypesToBuiltin(type), true));
	}
}

static void emitStruct(
		mlir::rlc::python::SerializationContext& context,
		llvm::raw_ostream& OS,
		bool isUnion,
		llvm::StringRef name,
		llvm::ArrayRef<mlir::StringRef> fieldNames,
		mlir::TypeRange fieldTypes)
{
	emitStructShort(context, OS, isUnion, name, fieldNames, fieldTypes);
	emitStructImplicitMethods(context, OS, name);
	emitStructGetters(context, OS, fieldNames, fieldTypes);

	OS << "\n\n";
}

mlir::LogicalResult mlir::rlc::python::CTypeStructDecl::emit(
		llvm::raw_ostream& OS, SerializationContext& context)
{
	OS.indent(context.getIndent() * 4);
	if (getType().isa<mlir::rlc::python::CTypeStructType>())
	{
		auto type = getType().cast<mlir::rlc::python::CTypeStructType>();
		llvm::SmallVector<llvm::StringRef, 2> fieldNames;
		for (auto name : getFieldNames())
			fieldNames.push_back(name.cast<mlir::StringAttr>());
		emitStruct(
				context, OS, false, type.getName(), fieldNames, type.getSubTypes());
	}
	else
	{
		auto type = getType().cast<python::CTypeUnionType>();
		auto name = typeToString(type, true);

		llvm::SmallVector<llvm::StringRef, 2> fieldNames;
		for (auto name : getFieldNames())
			fieldNames.push_back(name.cast<mlir::StringAttr>());
		emitStructShort(
				context, OS, true, "_" + name, fieldNames, type.getSubTypes());
		emitStructGetters(context, OS, fieldNames, type.getSubTypes());
		emitStructShort(
				context,
				OS,
				false,
				name,
				{ "content", "active_index" },
				{ "_" + name, "c_longlong" });
		emitStructImplicitMethods(context, OS, name);
		emitStructGetter(context, OS, "content", "_" + name);
		emitStructGetter(context, OS, "active_index", "c_longlong");
	}

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
	OS << "wrappers = defaultdict(list)\n";
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

	if (getResult().getType().isa<mlir::rlc::python::CTypesCharPType>())
		OS << "(" << context.nameOf(getLhs()) << ".encode(\"utf-8\"))\n";
	else
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
	if (getNumResults() != 0 and
			not getResults()[0].getType().isa<mlir::rlc::python::NoneType>())
	{
		OS << context.registerValue(getResults().front()) << " = " << "("
			 << typeToString(getResults().front().getType(), true) << ")()\n";
		OS.indent(context.getIndent() * 4);
	}

	OS << context.nameOf(getCallee()) << "(";

	if (getNumResults() != 0 and
			not getResults()[0].getType().isa<mlir::rlc::python::NoneType>())
	{
		OS << "byref(" << context.nameOf(getResults().front()) << "), ";
	}

	for (auto arg : getArgs())
	{
		if (arg.getType().isa<mlir::rlc::python::CTypesCharPType>())
			OS << context.nameOf(arg) << ", ";
		else
			OS << "byref(" << context.nameOf(arg) << "), ";
	}

	OS << ")\n";
	return mlir::success();
}

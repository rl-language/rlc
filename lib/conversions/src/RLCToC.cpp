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
#include "rlc/conversions/RLCToC.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Format.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"

using namespace rlc;

static std::string nonArrayTypeToString(mlir::Type type)
{
	std::string O;
	llvm::raw_string_ostream OS(O);
	llvm::TypeSwitch<mlir::Type>(type)
			.Case([&](mlir::rlc::ClassType Class) { OS << Class.mangledName(); })
			.Case([&](mlir::rlc::AlternativeType alternative) {
				OS << alternative.getMangledName();
			})
			.Case<mlir::rlc::FloatType>([&](mlir::rlc::FloatType) { OS << "double"; })
			.Case<mlir::rlc::StringLiteralType>(
					[&](mlir::rlc::StringLiteralType) { OS << "char*"; })
			.Case<mlir::rlc::BoolType>([&](mlir::rlc::BoolType) { OS << "bool"; })
			.Case<mlir::rlc::IntegerType>([&](mlir::rlc::IntegerType Type) {
				OS << "int" << Type.getSize() << "_t";
			})
			.Case<mlir::rlc::VoidType>([&](mlir::rlc::VoidType) { OS << "void"; })
			.Case<mlir::rlc::IntegerLiteralType>(
					[&](mlir::rlc::IntegerLiteralType t) { OS << t.getValue(); })
			.Default([](auto type) {
				type.dump();
				llvm_unreachable(
						"while emitting c header, recieved  a unexpected type");
			});
	OS.flush();
	return O;
}

static std::string typeToString(mlir::Type type)
{
	if (auto casted = type.dyn_cast<mlir::rlc::FrameType>())
		type = casted.getUnderlying();
	else if (auto casted = type.dyn_cast<mlir::rlc::ContextType>())
		type = casted.getUnderlying();

	std::string O;
	llvm::raw_string_ostream OS(O);
	llvm::TypeSwitch<mlir::Type>(type)
			.Case<mlir::rlc::ArrayType>([&](mlir::rlc::ArrayType array) {
				OS << typeToString(array.getUnderlying());
				OS << "["
					 << array.getSize().cast<mlir::rlc::IntegerLiteralType>().getValue()
					 << "]";
			})
			.Case<mlir::rlc::OwningPtrType>([&](mlir::rlc::OwningPtrType ptr) {
				OS << typeToString(ptr.getUnderlying());
				OS << "*";
			})
			.Case<mlir::rlc::ReferenceType>([&](mlir::rlc::ReferenceType ptr) {
				OS << typeToString(ptr.getUnderlying());
				OS << "*";
			})
			.Default([&](auto type) { OS << nonArrayTypeToString(type); });
	OS.flush();
	return O;
}

static void printTypeField(
		llvm::StringRef fieldName,
		mlir::Type type,
		llvm::raw_ostream& OS,
		bool isRef = false)
{
	if (auto casted = type.dyn_cast<mlir::rlc::FrameType>())
		type = casted.getUnderlying();
	else if (auto casted = type.dyn_cast<mlir::rlc::ContextType>())
		type = casted.getUnderlying();

	llvm::TypeSwitch<mlir::Type>(type)
			.Case<mlir::rlc::ArrayType>([&](mlir::rlc::ArrayType array) {
				printTypeField(fieldName, array.getUnderlying(), OS, isRef);
				OS << "["
					 << array.getSize().cast<mlir::rlc::IntegerLiteralType>().getValue()
					 << "]";
			})
			.Case<mlir::rlc::OwningPtrType>([&](mlir::rlc::OwningPtrType ptr) {
				OS << typeToString(ptr.getUnderlying());
				OS << "*";
			})
			.Case<mlir::rlc::ReferenceType>([&](mlir::rlc::ReferenceType ptr) {
				OS << typeToString(ptr.getUnderlying());
				OS << "*";
			})
			.Default([&](auto type) { OS << nonArrayTypeToString(type); });

	if (not type.isa<mlir::rlc::ArrayType>())
	{
		if (isRef)
			OS << "(&";
		OS << " " << fieldName;
		if (isRef)
			OS << ")";
	}
}

static bool isFunctionSelfAssign(
		llvm::StringRef name, mlir::FunctionType type, mlir::Type self)
{
	return name == "assign" and type.getNumInputs() == 2 and
				 type.getInputs()[0] == self and type.getInputs()[1] == self;
}

static void printMethodOfType(
		llvm::raw_ostream& OS,
		mlir::Type self,
		mlir::FunctionType type,
		llvm::StringRef name,
		llvm::StringRef mangledName,
		mlir::ArrayAttr argNames,
		mlir::rlc::ModuleBuilder& builder,
		bool isDecl)
{
	bool isSelfAssign = isFunctionSelfAssign(name, type, self);
	bool returnsVoid = type.getResults().empty() or
										 type.getResults()[0].isa<mlir::rlc::VoidType>();
	if (not isDecl)
		OS << "inline ";

	bool needReturnType = name != "drop" and name != "init";

	if (not needReturnType)
	{
		// nothing to print
	}
	else if (isSelfAssign)
		OS << mlir::rlc::typeToMangled(self) << "& ";
	else if (returnsVoid)
		OS << "void ";
	else
		OS << typeToString(type.getResults()[0]) << " ";

	if (not isDecl)
		OS << mlir::rlc::typeToMangled(self) << "::";

	if (name == "drop")
	{
		OS << "~" << mlir::rlc::typeToMangled(self) << "(";
	}
	else if (name == "init")
	{
		OS << mlir::rlc::typeToMangled(self) << "(";
	}
	else if (name == "assign")
	{
		OS << "operator=(";
	}
	else if (name == "equal")
	{
		OS << "operator==(";
	}
	else
	{
		OS << name << "(";
	}

	for (size_t i = 1; i < argNames.size(); i++)
	{
		printTypeField(
				argNames[i].cast<mlir::StringAttr>().strref(),
				type.getInput(i),
				OS,
				true);
		if (i + 1 != argNames.size())
			OS << ", ";
	}
	OS << ")";
	if (isDecl)
	{
		OS << ";\n";
		return;
	}
	OS << " {\n";
	if (not returnsVoid)
	{
		bool needsToInvokeDestructor =
				type.getResult(0).isa<mlir::rlc::ClassType>() or
				type.getResult(0).isa<mlir::rlc::AlternativeType>();
		OS.indent(1);
		OS << "union ToReturn { " << typeToString(type.getResult(0))
			 << " payload; ToReturn() {}; ~ToReturn() {";
		if (needsToInvokeDestructor)
			OS << " payload.~" << typeToString(type.getResult(0)) << "();";
		OS << " } }	_rl__result;\n";
	}
	OS.indent(1);
	OS << mangledName << "(";
	if (not returnsVoid)
	{
		OS << "&_rl__result.payload, ";
	}
	OS << "this";
	for (size_t i = 1; i < argNames.size(); i++)
		OS << ", &" << argNames[i].cast<mlir::StringAttr>().strref();

	OS << ");\n";
	if (not returnsVoid)
	{
		OS.indent(1);
		OS << "return _rl__result.payload;\n";
	}

	if (isSelfAssign)
	{
		OS << "return *this;\n";
	}

	OS << "}\n";
}

static void printMethodsOfType(
		mlir::Type type,
		llvm::raw_ostream& OS,
		llvm::DenseSet<mlir::Value>& methods,
		mlir::rlc::ModuleBuilder& builder,
		bool isDecl)
{
	if (methods.empty())
		return;
	for (auto method : methods)
	{
		if (auto f = method.getDefiningOp<mlir::rlc::FunctionOp>())
		{
			if (f.isDeclaration())
				continue;
			printMethodOfType(
					OS,
					type,
					f.getFunctionType(),
					f.getUnmangledName(),
					f.getMangledName(),
					f.getArgNames(),
					builder,
					isDecl);
		}
		if (auto f = method.getDefiningOp<mlir::rlc::ActionFunction>())
		{
			printMethodOfType(
					OS,
					type,
					f.getIsDoneFunctionType(),
					"is_done",
					mlir::rlc::mangledName("is_done", true, f.getIsDoneFunctionType()),
					builder.getRewriter().getStrArrayAttr({ "self" }),
					builder,
					isDecl);

			for (auto value : f.getActions())
			{
				auto action = mlir::cast<mlir::rlc::ActionStatement>(
						builder.actionFunctionValueToActionStatement(value).front());

				llvm::SmallVector<mlir::Attribute, 2> attrs(
						{ mlir::StringAttr::get(action.getContext(), "self") });
				for (auto attr :
						 action.getDeclaredNames().getAsRange<mlir::StringAttr>())
					attrs.push_back(attr);
				printMethodOfType(
						OS,
						type,
						value.getType().cast<mlir::FunctionType>(),
						action.getName(),
						mlir::rlc::mangledName(
								action.getName(),
								true,
								value.getType().cast<mlir::FunctionType>()),
						builder.getRewriter().getArrayAttr(attrs),
						builder,
						isDecl);
			}
		}
	}
}

static void printCPPOverload(
		llvm::raw_ostream& OS,
		mlir::FunctionType type,
		llvm::StringRef name,
		llvm::StringRef mangledName,
		mlir::ArrayAttr argNames,
		mlir::rlc::ModuleBuilder& builder)
{
	// do not emit a proper wrapper for arrays, it is not clear how to do that
	for (auto t : type.getInputs())
		if (t.isa<mlir::rlc::ArrayType>())
			return;

	if (name == "main")
		return;

	bool returnsVoid = type.getResults().empty() or
										 type.getResults()[0].isa<mlir::rlc::VoidType>();
	OS << "inline ";

	if (returnsVoid)
		OS << "void ";
	else
		OS << typeToString(type.getResults()[0]) << " ";

	OS << name << "(";

	for (size_t i = 0; i < argNames.size(); i++)
	{
		printTypeField(
				argNames[i].cast<mlir::StringAttr>().strref(),
				type.getInput(i),
				OS,
				true);
		if (i + 1 != argNames.size())
			OS << ", ";
	}
	OS << ")";
	OS << " {\n";
	if (not returnsVoid)
	{
		bool needsToInvokeDestructor =
				type.getResult(0).isa<mlir::rlc::ClassType>() or
				type.getResult(0).isa<mlir::rlc::AlternativeType>();
		OS.indent(1);
		OS << "union ToReturn { " << typeToString(type.getResult(0))
			 << " payload; ToReturn() {}; ~ToReturn() {";
		if (needsToInvokeDestructor)
			OS << " payload.~" << typeToString(type.getResult(0)) << "();";
		OS << " } }	_rl__result;\n";
	}
	OS.indent(1);
	OS << mangledName << "(";
	if (not returnsVoid)
	{
		OS << "&_rl__result.payload ";
		if (0 != argNames.size())
			OS << ", ";
	}
	for (size_t i = 0; i < argNames.size(); i++)
	{
		OS << "&" << argNames[i].cast<mlir::StringAttr>().strref();
		if (i + 1 != argNames.size())
			OS << ", ";
	}

	OS << ");\n";
	if (not returnsVoid)
	{
		OS.indent(1);
		OS << "return _rl__result.payload;\n";
	}

	OS << "}\n";
}

static void printTypeDecl(mlir::Type type, llvm::raw_ostream& OS)
{
	llvm::TypeSwitch<mlir::Type>(type)
			.Case([&](mlir::rlc::AlternativeType alternative) {
				OS << "struct " << alternative.getMangledName() << ";\n";
			})
			.Case([&](mlir::rlc::ClassType Class) {
				OS << "typedef union " << Class.mangledName() << " "
					 << Class.mangledName() << ";\n";
			})
			.Case<
					mlir::rlc::IntegerType,
					mlir::rlc::BoolType,
					mlir::rlc::FloatType,
					mlir::rlc::StringLiteralType,
					mlir::rlc::OwningPtrType,
					mlir::rlc::ReferenceType,
					mlir::rlc::IntegerLiteralType,
					mlir::rlc::ArrayType,
					mlir::rlc::VoidType,
					mlir::FunctionType>([&](auto) {	 // Pass, already defined by C
			})
			.Default([](auto type) {
				type.dump();
				llvm_unreachable(
						"while emitting c header, recieved  a unexpected type");
			});
}

static void printSpecialFunctions(
		llvm::StringRef typeName,
		llvm::raw_ostream& OS,
		llvm::DenseSet<mlir::Value>& methods)
{
	if (llvm::count_if(methods, [&](mlir::Value value) -> bool {
				auto casted = value.getDefiningOp<mlir::rlc::FunctionOp>();
				if (casted == nullptr)
					return false;
				if (casted.getFunctionType().getNumInputs() == 0)
					return false;
				return isFunctionSelfAssign(
						casted.getUnmangledName(),
						casted.getFunctionType(),
						casted.getFunctionType().getInput(0));
			}) == 0)
	{
		OS << typeName << "& operator=(const " << typeName
			 << "& other) = delete;\n";
	}
	else
		OS << typeName << "(const " << typeName << "& other) : " << typeName
			 << "() {*this = const_cast<" << typeName << "&>(other);}\n";

	OS << typeName << "(" << typeName << "&& other) = delete;\n";
	OS << typeName << "& operator=(" << typeName << "&& other) = delete;\n";

	if (llvm::count_if(methods, [&](mlir::Value value) -> bool {
				auto casted = value.getDefiningOp<mlir::rlc::FunctionOp>();
				if (casted == nullptr)
					return false;
				return casted.getUnmangledName() == "init";
			}) == 0)
	{
		OS << typeName << "() {}\n";
	}
	if (llvm::count_if(methods, [&](mlir::Value value) -> bool {
				auto casted = value.getDefiningOp<mlir::rlc::FunctionOp>();
				if (casted == nullptr)
					return false;
				return casted.getUnmangledName() == "drop";
			}) == 0)
	{
		OS << "~" << typeName << "() {}\n";
	}
}

static void printTypeDefinition(
		mlir::Type type,
		llvm::raw_ostream& OS,
		llvm::DenseSet<mlir::Value>& methods,
		mlir::rlc::ModuleBuilder& builder)
{
	llvm::TypeSwitch<mlir::Type>(type)
			.Case([&](mlir::rlc::AlternativeType alternative) {
				OS << "struct " << alternative.getMangledName() << " {\n";

				OS.indent(2);
				OS << "union _Content" << alternative.getMangledName() << "{\n";

				OS.indent(4);
				OS << "#ifdef __cplusplus\n";
				OS << "_Content" << alternative.getMangledName() << "() {};\n";
				OS << "~_Content" << alternative.getMangledName() << "() {};\n";
				OS << "#endif\n";
				for (auto field : llvm::enumerate(alternative.getUnderlying()))
				{
					OS.indent(4);
					printTypeField(
							(llvm::Twine("field") + llvm::Twine(field.index())).str(),
							field.value(),
							OS);
					OS << ";\n";
				}
				OS.indent(2);
				OS << "} content;\n";
				OS.indent(2);
				OS << "int64_t active_index;\n";

				OS << "#ifdef __cplusplus\n";
				printMethodsOfType(type, OS, methods, builder, true);
				printSpecialFunctions(alternative.getMangledName(), OS, methods);
				OS << "#endif\n";

				OS << "};\n";
			})
			.Case([&](mlir::rlc::ClassType Class) {
				OS << "typedef union " << Class.mangledName() << " {\n";

				OS << "struct _Content" << Class.mangledName() << " {\n";
				for (const auto& [name, fieldType] :
						 llvm::zip(Class.getFieldNames(), Class.getBody()))
				{
					OS.indent(4);
					printTypeField(name, fieldType, OS);
					OS << ";\n";
				}
				OS.indent(2);
				OS << "} content;\n";

				OS << "#ifdef __cplusplus\n";
				printMethodsOfType(type, OS, methods, builder, true);
				printSpecialFunctions(Class.mangledName(), OS, methods);
				OS << "#endif\n";

				OS << "} " << Class.mangledName() << ";\n";
			})
			.Case<
					mlir::rlc::IntegerType,
					mlir::rlc::BoolType,
					mlir::rlc::FloatType,
					mlir::rlc::StringLiteralType,
					mlir::rlc::OwningPtrType,
					mlir::rlc::ReferenceType,
					mlir::rlc::IntegerLiteralType,
					mlir::rlc::ArrayType,
					mlir::rlc::VoidType,
					mlir::FunctionType>([&](auto) {	 // Pass, already defined by C
			})
			.Default([](auto type) {
				type.dump();
				llvm_unreachable(
						"while emitting c header, recieved  a unexpected type");
			});
}

static void printFunctionSignature(
		llvm::StringRef originalName,
		bool isMemberFunction,
		llvm::StringRef cShortName,
		mlir::FunctionType type,
		mlir::ArrayAttr fieldNames,
		llvm::raw_ostream& OS)
{
	std::string name =
			mlir::rlc::mangledName(originalName, isMemberFunction, type);
	OS << "#ifdef RLC_GET_FUNCTION_DECLS\n";
	OS << "void ";
	OS << name << "(";

	if (not type.getResults().empty() and
			not type.getResults()[0].isa<mlir::rlc::VoidType>())
	{
		printTypeField("* __result", type.getResults().front(), OS);

		if (fieldNames.size() != 0)
			OS << ", ";
	}

	for (size_t index = 0; index < fieldNames.size(); index++)
	{
		printTypeField(
				("* " + fieldNames[index].cast<mlir::StringAttr>().getValue()).str(),
				type.getInput(index),
				OS);

		if (index + 1 < fieldNames.size())
			OS << ", ";
	}
	OS << ");\n";
	OS << "#endif\n\n";

	OS << "#ifdef RLC_VISIT_FUNCTION\n";

	OS << "#define RLC_ARGUMENTS_COUNT " << type.getInputs().size() << "\n";

	OS << "#define RLC_ARGUMENTS ";
	size_t I = 0;
	for (auto [type, name] : llvm::zip(type.getInputs(), fieldNames))
	{
		I++;
		printTypeField("*", type, OS);
		OS << " " << name.cast<mlir::StringAttr>().getValue();
		if (I != fieldNames.size())
			OS << ", ";
	}
	OS << "\n";

	OS << "RLC_VISIT_FUNCTION(" << originalName << ", " << name << ", ";
	OS << cShortName;
	OS << ", ";
	if (type.getResults().empty())
		OS << "void ";
	else
		printTypeField("", type.getResults().front(), OS);
	OS << ", ";

	I = 0;
	for (auto name : fieldNames)
	{
		I++;
		OS << " " << name.cast<mlir::StringAttr>().getValue();
		if (I != fieldNames.size())
			OS << ", ";
	}
	OS << ")\n";
	OS << "#undef RLC_ARGUMENTS_COUNT\n";
	OS << "#undef RLC_ARGUMENTS\n";
	OS << "#endif\n\n";
}

static void printFunctionAndCanFunctionSignature(
		llvm::StringRef originalName,
		bool isMemberFunction,
		llvm::StringRef cShortName,
		mlir::FunctionType type,
		mlir::ArrayAttr fieldNames,
		llvm::raw_ostream& OS)
{
	printFunctionSignature(
			originalName, isMemberFunction, cShortName, type, fieldNames, OS);
	auto canType = mlir::FunctionType::get(
			type.getContext(),
			type.getInputs(),
			{ mlir::rlc::BoolType::get(type.getContext()) });
	printFunctionSignature(
			("can_" + originalName).str(),
			isMemberFunction,
			("can_" + cShortName).str(),
			canType,
			fieldNames,
			OS);
}

void rlc::rlcToCHeader(mlir::ModuleOp Module, llvm::raw_ostream& OS)
{
	mlir::rlc::ModuleBuilder builder(Module);
	OS << "#ifdef __cplusplus\n";
	OS << "extern \"C\"{\n";
	OS << "#endif\n";
	OS << "#ifndef RLC_HEADER\n";
	OS << "#ifdef RLC_C_HEADER\n#undef RLC_C_HEADER\n#define RLC_HEADER\n";
	OS << R""""(
#include "stddef.h"
#include "stdint.h"
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_VISIT_FUNCTION(name, mangled_name, cShortName, return_type, ...)   \
	static inline return_type cShortName(RLC_ARGUMENTS)                         \
	{                                                                            \
		return_type ret_value;
		mangled_name(&ret_value, __VA_ARGS__);                                          \
		return ret_value;
	} 
#endif
)"""";
	OS << "#endif\n";

	OS << "#ifdef RLC_GET_TYPE_DECLS\n";
	for (auto type : postOrderTypes(Module))
		printTypeDecl(type, OS);

	OS << "#undef RLC_GET_TYPE_DECLS\n";
	OS << "#endif\n\n";

	OS << "#ifdef RLC_TYPE\n";
	for (auto type : postOrderTypes(Module))
		if (auto casted = type.dyn_cast<mlir::rlc::ClassType>())
			OS << "RLC_TYPE(" << casted.getName() << ")\n";
	OS << "#undef RLC_TYPE\n";
	OS << "#endif\n\n";

	for (auto fun : Module.getOps<mlir::rlc::FunctionOp>())
	{
		if (fun.isInternal())
			continue;
		if (fun.getUnmangledName() == "main")
			continue;
		if (fun.isDeclaration())
			continue;
		std::string cShortName =
				((not fun.getFunctionType().getInputs().empty() and
					fun.getFunctionType().getInputs().front().isa<mlir::rlc::ClassType>())
						 ? fun.getFunctionType()
											 .getInputs()
											 .front()
											 .cast<mlir::rlc::ClassType>()
											 .getName() +
									 "_" + fun.getUnmangledName()
						 : fun.getUnmangledName())
						.str();

		printFunctionAndCanFunctionSignature(
				fun.getUnmangledName(),
				fun.getIsMemberFunction(),
				cShortName,
				fun.getFunctionType(),
				fun.getArgNames(),
				OS);
	}

	for (auto fun : Module.getOps<mlir::rlc::ActionFunction>())
	{
		printFunctionAndCanFunctionSignature(
				fun.getUnmangledName(),
				false,
				(fun.getClassType().getName() + "_" + fun.getUnmangledName()).str(),
				fun.getFunctionType(),
				fun.getArgNames(),
				OS);

		using ActionKey = std::pair<std::string, const void*>;
		std::set<ActionKey> alreadyAdded;
		for (const auto type : fun.getActions())
		{
			auto* action = builder.actionFunctionValueToActionStatement(type).front();
			auto casted = mlir::cast<mlir::rlc::ActionStatement>(action);
			ActionKey key(casted.getName(), type.getAsOpaquePointer());
			if (alreadyAdded.contains(key))
				continue;

			alreadyAdded.insert(key);

			auto castedType = type.getType().cast<mlir::FunctionType>();
			llvm::SmallVector<mlir::Attribute, 2> attrs(
					{ mlir::StringAttr::get(casted.getContext(), "self") });
			for (auto attr : casted.getDeclaredNames().getAsRange<mlir::StringAttr>())
				attrs.push_back(attr);

			printFunctionAndCanFunctionSignature(
					casted.getName(),
					true,
					(fun.getClassType().getName() + "_" + casted.getName()).str(),
					castedType,
					mlir::ArrayAttr::get(casted.getContext(), attrs),
					OS);
		}

		printFunctionAndCanFunctionSignature(
				"is_done",
				true,
				(fun.getClassType().getName() + "_is_done").str(),
				fun.getIsDoneFunctionType(),
				mlir::ArrayAttr::get(
						Module.getContext(),
						{ mlir::StringAttr::get(Module.getContext(), "arg0") }),
				OS);
	}

	OS << "#ifdef RLC_GET_FUNCTION_DECLS\n";
	OS << "#undef RLC_GET_FUNCTION_DECLS\n";
	OS << "#endif\n\n";

	OS << "#ifdef RLC_VISIT_FUNCTION\n";
	OS << "#undef RLC_VISIT_FUNCTION\n";
	OS << "#endif\n";

	std::map<const void*, llvm::DenseSet<mlir::Value>> typeToMethods;
	for (auto op : Module.getOps<mlir::rlc::FunctionOp>())
		if (op.getIsMemberFunction() and not op.isInternal() and
				(op.getArgumentTypes()[0].isa<mlir::rlc::ClassType>() or
				 op.getArgumentTypes()[0].isa<mlir::rlc::AlternativeType>()))
		{
			typeToMethods[op.getArgumentTypes()[0].getAsOpaquePointer()].insert(op);
		}

	for (auto op : Module.getOps<mlir::rlc::ActionFunction>())
		typeToMethods[op.getClassType().getAsOpaquePointer()].insert(
				op.getResult());

	OS << "#ifdef RLC_GET_TYPE_DEFS\n";
	for (auto type : postOrderTypes(Module))
		printTypeDefinition(
				type, OS, typeToMethods[type.getAsOpaquePointer()], builder);

	OS << "#ifdef __cplusplus\n";
	OS << "}\n";
	OS << "#endif\n";

	OS << "#ifdef __cplusplus\n";
	for (auto& pair : typeToMethods)
		printMethodsOfType(
				mlir::Type::getFromOpaquePointer(pair.first),
				OS,
				pair.second,
				builder,
				false);
	OS << "#endif\n";

	for (auto alias : Module.getOps<mlir::rlc::TypeAliasOp>())
	{
		OS << "typedef struct " << typeToString(alias.getAliased()) << " "
			 << alias.getName() << ";\n";
	}

	OS << "#ifdef __cplusplus\n";
	for (auto op : Module.getOps<mlir::rlc::FunctionOp>())
		if (not op.isDeclaration() and not op.isInternal() and
				(not op.getIsMemberFunction() or
				 (not op.getArgumentTypes()[0].isa<mlir::rlc::ClassType>() and
					not op.getArgumentTypes()[0].isa<mlir::rlc::AlternativeType>())))
			printCPPOverload(
					OS,
					op.getFunctionType(),
					op.getUnmangledName(),
					op.getMangledName(),
					op.getArgNames(),
					builder);

	for (auto op : Module.getOps<mlir::rlc::ActionFunction>())
		printCPPOverload(
				OS,
				op.getFunctionType(),
				op.getUnmangledName(),
				op.getMangledName(),
				op.getArgNames(),
				builder);
	OS << "#endif\n";

	OS << "#undef RLC_GET_TYPE_DEFS\n";
	OS << "#endif\n\n";
}

static bool needsToBeEmitted(mlir::Type type)
{
	return not(
			type.isa<mlir::rlc::ArrayType>() or type.isa<mlir::FunctionType>() or
			type.isa<mlir::rlc::OwningPtrType>() or
			type.isa<mlir::rlc::IntegerType>() or type.isa<mlir::rlc::BoolType>() or
			type.isa<mlir::rlc::StringLiteralType>() or
			type.isa<mlir::rlc::FloatType>() or type.isa<mlir::rlc::VoidType>() or
			mlir::rlc::isTemplateType(type).succeeded());
}

static std::string unmangledName(
		mlir::Value op, mlir::rlc::ModuleBuilder& builder)
{
	if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionOp>(op.getDefiningOp()))
	{
		return casted.getUnmangledName().str();
	}
	if (auto casted =
					mlir::dyn_cast<mlir::rlc::ActionFunction>(op.getDefiningOp()))
	{
		if (casted.getIsDoneFunction() == op)
			return "is_done";

		if (casted.getResult() == op)
			return casted.getUnmangledName().str();

		if (auto maybeStatement =
						mlir::dyn_cast_or_null<mlir::rlc::ActionStatement>(
								builder.actionFunctionValueToActionStatement(op).front());
				maybeStatement)
			return maybeStatement.getName().str();
	}
	llvm_unreachable("unrechable");
	return "";
}

static bool isMemberFunction(mlir::Value op, mlir::rlc::ModuleBuilder& builder)
{
	if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionOp>(op.getDefiningOp()))
	{
		return casted.getIsMemberFunction();
	}
	if (auto casted =
					mlir::dyn_cast<mlir::rlc::ActionFunction>(op.getDefiningOp()))
	{
		// they are all member functions except the entry point
		return casted.getResult() != op;
	}
	llvm_unreachable("unrechable");
	return "";
}

static bool unhandledType(mlir::Type t)
{
	return t.isa<mlir::rlc::OwningPtrType>() or t.isa<mlir::rlc::ArrayType>();
};

static void emitGodotFunction(
		llvm::ArrayRef<mlir::FunctionType> overloadTypes,
		llvm::StringRef name,
		llvm::raw_ostream& OS,
		mlir::rlc::ModuleBuilder& builder,
		llvm::ArrayRef<bool> memberFunctions)
{
	if (name.starts_with("_") or name == "main")
		return;
	OS << "godot::Variant RLCLib::RLC_" << name
		 << "(const godot::Variant **args, GDExtensionInt arg_count, "
				"GDExtensionCallError "
				"&error){\n";

	for (auto [overload, isMemberFunction] :
			 llvm::zip(overloadTypes, memberFunctions))
	{
		if (overload == nullptr)
			continue;
		auto fType = overload;
		if (llvm::any_of(fType.getInputs(), unhandledType))
			continue;
		bool isVoid = fType.getNumResults() == 0 or
									fType.getResults().front().isa<mlir::rlc::VoidType>();
		if (not isVoid and unhandledType(fType.getResult(0)))
			continue;
		OS << "if (arg_count == " << fType.getNumInputs();

		for (size_t I = 0; I < fType.getNumInputs(); I++)
		{
			auto type = mlir::rlc::decayCtxFrmType(fType.getInput(I));
			if (type.isa<mlir::rlc::ClassType>() or
					type.isa<mlir::rlc::AlternativeType>())
				OS << " && godot::Object::cast_to<RLC"
					 << typeToString(fType.getInput(I)) << ">(*((godot::Ref<RLC"
					 << typeToString(fType.getInput(I)) << ">)(*(args[" << I << "]))))";
			else if (type.isa<mlir::rlc::StringLiteralType>())
				OS << " && (*(args[" << I << "])).get_type() == godot::Variant::STRING";
		}

		OS << ") {\n";

		for (size_t I = 0; I < fType.getNumInputs(); I++)
		{
			auto type = mlir::rlc::decayCtxFrmType(fType.getInput(I));
			if (type.isa<mlir::rlc::ClassType>() or
					type.isa<mlir::rlc::AlternativeType>())
				continue;

			if (type.isa<mlir::rlc::StringLiteralType>())
			{
				OS << "auto tmp" << I << "= ";
				OS << "(*(args[" << I << "])).operator godot::String()\n;";
				OS << "auto tmp2" << I << "= tmp" << I << ".utf8();\n";
				OS << "char* arg" << I << "= const_cast<char*>(tmp2" << I
					 << ".get_data());\n";
				continue;
			}
			OS << "auto arg" << I << "= ";
			OS << "((" << typeToString(fType.getInput(I)) << ")(*(args[" << I
				 << "])));\n ";
		}

		if (not isVoid)
		{
			if (fType.getResult(0).isa<mlir::rlc::ClassType>() or
					fType.getResult(0).isa<mlir::rlc::AlternativeType>())
			{
				OS << typeToString(fType.getResult(0)) << "* mallocated = ("
					 << typeToString(fType.getResult(0)) << "*) malloc(sizeof("
					 << typeToString(fType.getResult(0)) << "));\n";
				OS << "godot::Ref<RLC" << typeToString(fType.getResult(0))
					 << "> to_return;\n";
				OS << "to_return.instantiate();\n";
				OS << "godot::Object::cast_to<RLC" << typeToString(fType.getResult(0))
					 << ">(*to_return)->setNonOwning(mallocated);\n";
			}
			else
			{
				OS << typeToString(fType.getResult(0)) << " to_return;\n";
			}
		}

		OS << mlir::rlc::mangledName(name, isMemberFunction, overload) << "(";
		if (not isVoid)
		{
			if (not fType.getResult(0).isa<mlir::rlc::ClassType>() and
					not fType.getResult(0).isa<mlir::rlc::AlternativeType>())
				OS << "&to_return";
			else
				OS << "mallocated";
			if (fType.getNumInputs() != 0)
				OS << ",";
		}

		for (size_t I = 0; I < fType.getNumInputs(); I++)
		{
			auto type = mlir::rlc::decayCtxFrmType(fType.getInput(I));
			if (type.isa<mlir::rlc::ClassType>() or
					type.isa<mlir::rlc::AlternativeType>())
				OS << "(godot::Object::cast_to<RLC" << typeToString(fType.getInput(I))
					 << ">(*((godot::Ref<RLC" << typeToString(fType.getInput(I))
					 << ">)(*(args[" << I << "]))))->content)";
			else
				OS << "&arg" << I;
			if (I + 1 < fType.getNumInputs())
				OS << ", ";
		}

		OS << ");\n";
		if (isVoid)
			OS << "return godot::Variant();\n";
		else if (
				auto casted =
						llvm::dyn_cast<mlir::rlc::ReferenceType>(fType.getResult(0)))
		{
			auto underlying = casted.getUnderlying();

			if (not underlying.isa<mlir::rlc::ClassType>() and
					not underlying.isa<mlir::rlc::AlternativeType>())
			{
				OS << "return *to_return;\n";
			}
			else
			{
				OS << "godot::Ref<RLC" << typeToString(underlying)
					 << "> real_return;\n";
				OS << "real_return.instantiate();\n";
				OS << "godot::Object::cast_to<RLC" << typeToString(underlying)
					 << ">(*real_return)->setNonOwning(to_return);\n";
				OS << "return real_return;\n";
			}
		}
		else if (
				auto casted =
						llvm::dyn_cast<mlir::rlc::StringLiteralType>(fType.getResult(0)))
		{
			OS << "return godot::String(to_return);\n";
		}
		else
		{
			auto type = fType.getResult(0);
			OS << "return to_return;\n";
		}
		OS << "}\n";
	}

	OS << "return godot::Variant();";
	OS << "}\n";
}

void rlc::rlcToGodot(mlir::ModuleOp Module, llvm::raw_ostream& OS)
{
	OS << "#define RLC_GET_TYPE_DECLS\n";
	OS << "#define RLC_GET_FUNCTION_DECLS\n";

	rlcToCHeader(Module, OS);
	mlir::rlc::ModuleBuilder builder(Module);

	OS << "#ifdef RLC_GODOT\n";
	OS << "#undef RLC_GODOT\n";
	for (auto type : postOrderTypes(Module))
	{
		if (not needsToBeEmitted(type) or type.isa<mlir::rlc::ReferenceType>())
			continue;
		auto wrapperNameString = "RLC" + nonArrayTypeToString(type);
		const auto* wrapperName = wrapperNameString.c_str();
		OS << llvm::format(
				R"""""(
class %s: public godot::RefCounted {
  GDCLASS(%s, godot::RefCounted);

public:
  using ContentType = %s;
  ContentType* content;
  bool isOwning;
  
  void setOwning(ContentType* newPtr) {
    isOwning = true;
    content = newPtr;
}

  void setNonOwning(ContentType* newPtr) {
    isOwning = false;
    content = newPtr;
}

  %s() {}
  ~%s() { if (isOwning) delete content; }

  static void _bind_methods() {
)""""",
				wrapperName,
				wrapperName,
				nonArrayTypeToString(type).c_str(),
				wrapperName,
				wrapperName,
				wrapperName);

		OS << "godot::ClassDB::bind_static_method(\"RLC"
			 << nonArrayTypeToString(type).c_str()
			 << "\",godot::D_METHOD(\"make\"), &" << wrapperName << "::make);\n";

		if (auto casted = type.dyn_cast<mlir::rlc::ClassType>())
		{
			for (auto [name, type] :
					 llvm::zip(casted.getFieldNames(), casted.getBody()))
			{
				if (unhandledType(type) or name.starts_with("_"))
					continue;

				OS << "godot::ClassDB::bind_method(godot::D_METHOD(\"get_" << name
					 << "\"), &" << wrapperName << "::get_" << name << ");\n";

				if (type.isa<mlir::rlc::ClassType>() or
						type.isa<mlir::rlc::AlternativeType>())
					continue;
				OS << "godot::ClassDB::bind_method(godot::D_METHOD(\"set_" << name
					 << "\"), &" << wrapperName << "::set_" << name << ");\n";
			}
		}
		else if (auto casted = type.dyn_cast<mlir::rlc::AlternativeType>())
		{
			for (auto type : casted.getUnderlying())
			{
				if (unhandledType(type))
					continue;

				auto name = mlir::rlc::typeToMangled(type);
				OS << "godot::ClassDB::bind_method(godot::D_METHOD(\"get_" << name
					 << "\"), &" << wrapperName << "::get_" << name << ");\n";

				if (type.isa<mlir::rlc::ClassType>() or
						type.isa<mlir::rlc::AlternativeType>())
					continue;
				OS << "godot::ClassDB::bind_method(godot::D_METHOD(\"set_" << name
					 << "\"), &" << wrapperName << "::set_" << name << ");\n";
			}
		}

		OS << "}\n";

		OS << "static godot::Ref<" << wrapperName << ">" << " make() {\n";

		OS << "godot::Ref<" << wrapperName << "> to_return;\n";
		OS << "to_return.instantiate();\n";
		OS << "godot::Object::cast_to<RLC" << typeToString(type)
			 << ">(*to_return)->setOwning(new " << nonArrayTypeToString(type).c_str()
			 << ");\n";
		OS << "return to_return;\n}\n";

		if (auto casted = type.dyn_cast<mlir::rlc::ClassType>())
		{
			for (auto [name, type] :
					 llvm::zip(casted.getFieldNames(), casted.getBody()))
			{
				if (unhandledType(type) or name.starts_with("_"))
					continue;

				if (type.isa<mlir::rlc::ClassType>() or
						type.isa<mlir::rlc::AlternativeType>())
				{
					OS << "godot::Ref<RLC" << typeToString(type) << ">" << " get_" << name
						 << "() {\n";

					OS << "godot::Ref<RLC" << typeToString(type) << "> to_return;\n";
					OS << "to_return.instantiate();\n";
					OS << "godot::Object::cast_to<RLC" << typeToString(type)
						 << ">(*to_return)->setNonOwning(&content->content." << name
						 << ");\n";
					OS << "return to_return;\n}\n";
				}
				else
				{
					OS << typeToString(type) << " get_" << name << "() {\n";
					OS << "return content->content." << name << ";\n}\n";

					OS << "void set_" << name << "(" << typeToString(type)
						 << " newVal) {\n";
					OS << "content->content." << name << " =newVal;\n}\n";
				}
			}
		}

		if (auto casted = type.dyn_cast<mlir::rlc::AlternativeType>())
		{
			for (auto pair : llvm::enumerate(casted.getUnderlying()))
			{
				auto type = pair.value();
				if (unhandledType(type))
					continue;

				auto index = pair.index();
				auto name = mlir::rlc::typeToMangled(type);
				if (type.isa<mlir::rlc::ClassType>() or
						type.isa<mlir::rlc::AlternativeType>())
				{
					OS << "godot::Variant get_" << name << "() {\n";

					OS << "if (content->active_index != " << index
						 << ") return nullptr;\n";
					OS << "godot::Ref<RLC" << typeToString(type) << "> to_return;\n";
					OS << "to_return.instantiate();\n";
					OS << "godot::Object::cast_to<RLC" << typeToString(type)
						 << ">(*to_return)->setNonOwning(&content->content.field" << index
						 << ");\n";
					OS << "return to_return;\n}\n";
				}
				else
				{
					OS << "godot::Variant  get_" << name << "() {\n";

					OS << "if (content->active_index != " << index
						 << ") return nullptr;\n";
					OS << "return content->content.field" << index << ";\n}\n";

					OS << "void set_" << name << "(" << typeToString(type)
						 << " newVal) {\n";
					OS << "if (content->active_index == " << index << ") \n";
					OS << "content->content.field" << index << " =newVal;\n}\n";
				}
			}
		}

		OS << "};\n";
	}

	OS << R"""""(
class RLCLib: public godot::Object {
  GDCLASS(RLCLib, godot::Object);

public:
  static void _bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("_init"), &RLCLib::_init);
)""""";

	for (const auto& pair : builder.getSymbolTable().allDirectValues())
	{
		if (pair.first().starts_with("_") or pair.first() == "main")
			continue;
		if (llvm::all_of(pair.second, [](mlir::Value v) {
					auto casted = v.getDefiningOp<mlir::rlc::FunctionOp>();
					return casted and casted.isDeclaration();
				}))
			continue;
		OS << "godot::ClassDB::bind_vararg_method(godot::METHOD_FLAGS_DEFAULT, \""
			 << pair.first() << "\", &RLCLib::RLC_" << pair.first() << ");\n";
		OS << "godot::ClassDB::bind_vararg_method(godot::METHOD_FLAGS_DEFAULT, "
					"\"can_"
			 << pair.first() << "\", &RLCLib::RLC_can_" << pair.first() << ");\n";
	}

	OS << R"""""(
  }

  RLCLib() {}

  void _init() {}
)""""";

	for (const auto& pair : builder.getSymbolTable().allDirectValues())
	{
		if (pair.first().starts_with("_") or pair.first() == "main")
			continue;
		if (llvm::all_of(pair.second, [](mlir::Value v) {
					auto casted = v.getDefiningOp<mlir::rlc::FunctionOp>();
					return casted and casted.isDeclaration();
				}))
			continue;
		OS << "godot::Variant RLC_" << pair.first()
			 << "(const godot::Variant **args, GDExtensionInt arg_count, "
					"GDExtensionCallError &error);\n";
		OS << "godot::Variant RLC_can_" << pair.first()
			 << "(const godot::Variant **args, GDExtensionInt arg_count, "
					"GDExtensionCallError &error);\n";
	}

	OS << "};\n";

	auto* ctx = builder.getRewriter().getContext();
	for (const auto& pair : builder.getSymbolTable().allDirectValues())
	{
		llvm::SmallVector<bool, 4> memberFunctions;
		llvm::SmallVector<mlir::FunctionType, 4> overloads;
		llvm::SmallVector<mlir::FunctionType, 4> canOverloads;
		for (auto overload : pair.second)
		{
			if (auto casted = overload.getDefiningOp<mlir::rlc::FunctionOp>();
					casted and
					(casted.isInternal() or casted.getUnmangledName() == "main" or
					 casted.isDeclaration()))
				continue;

			memberFunctions.push_back(isMemberFunction(overload, builder));
			if (auto casted = overload.getDefiningOp<mlir::rlc::FunctionOp>();
					casted and casted.getPrecondition().empty())
			{
				canOverloads.push_back(nullptr);
				continue;
			}
			if (pair.first() == "is_done")
			{
				canOverloads.push_back(nullptr);
				continue;
			}
			overloads.push_back(overload.getType().cast<mlir::FunctionType>());
			canOverloads.push_back(mlir::FunctionType::get(
					ctx,
					overloads.back().getInputs(),
					{ mlir::rlc::BoolType::get(ctx) }));
		}

		if (memberFunctions.empty())
			continue;

		emitGodotFunction(overloads, pair.first(), OS, builder, memberFunctions);
		emitGodotFunction(
				canOverloads,
				("can_" + pair.first()).str(),
				OS,
				builder,
				memberFunctions);
	}

	OS << R"""""(
/** GDNative Terminate **/
static void 
godot_gdnative_terminate() {
}

static void godot_nativescript_init() {
)""""";

	for (auto type : postOrderTypes(Module))
	{
		if (needsToBeEmitted(type) and not type.isa<mlir::rlc::ReferenceType>())
			OS << "godot::ClassDB::register_class<RLC" << typeToString(type)
				 << ">();\n";
	}

	OS << R"""""(
  godot::ClassDB::register_class<RLCLib>();
}
)""""";

	OS << "#endif\n";
}

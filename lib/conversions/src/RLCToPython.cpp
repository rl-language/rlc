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
	class MemberFunctionsTable
	{
		public:
		MemberFunctionsTable(mlir::ModuleOp mod)
		{
			for (auto op : mod.getOps<mlir::rlc::FunctionOp>())
				if (op.getIsMemberFunction() and not op.isInternal() and
						(op.getArgumentTypes()[0].isa<mlir::rlc::ClassType>() or
						 op.getArgumentTypes()[0].isa<mlir::rlc::AlternativeType>()))
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
	static void printPrelude(StreamWriter& writer, bool isMac, bool isWindows)
	{
		writer.writenl("import ctypes");
		writer.writenl("import os");
		writer.writenl("from typing import overload");
		writer.writenl("from pathlib import Path");
		writer.writenl("import builtins");
		writer.writenl("from collections import defaultdict");

		std::string libName = "lib.so";
		if (isMac)
			libName = "lib.dylib";
		if (isWindows)
			libName = "lib.dll";
		writer.writenl(
				"lib = ctypes.CDLL(os.path.join(Path(__file__).resolve().parent, "
				"\"",
				libName,
				"\"))");
		writer.writenl("actions = defaultdict(list)");
		writer.writenl("wrappers = defaultdict(list)");
		writer.writenl("signatures = {}");
		writer.writenl("actionToAnyFunctionType = {}");
		writer.endLine();
	}

	static bool builtinCType(mlir::Type type)
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::FrameType>(type))
			return builtinCType(casted.getUnderlying());
		if (auto casted = mlir::dyn_cast<mlir::rlc::ContextType>(type))
			return builtinCType(casted.getUnderlying());
		return mlir::isa<mlir::rlc::IntegerType>(type) or
					 mlir::isa<mlir::rlc::BoolType>(type) or
					 mlir::isa<mlir::rlc::FloatType>(type) or
					 mlir::isa<mlir::rlc::StringLiteralType>(type);
	}

	static bool needsUnwrapping(mlir::FunctionType type)
	{
		if (type.getNumResults() == 0)
			return false;
		return builtinCType(type.getResult(0));
	}

	static void printFunctionDecl(
			TypeRange typeRange,
			llvm::ArrayRef<llvm::StringRef> infoRange,
			StreamWriter& writer,
			mlir::Type resultType,
			bool isMemberFunction)
	{
		assert(typeRange.size() == infoRange.size());
		writer.write("(");
		for (size_t i = 0; i != typeRange.size(); i++)
		{
			writer.write(infoRange[i]);
			writer.write(": '");
			writer.writeType(typeRange[i]);
			writer.write("',");
		}
		writer.write(")");

		if (not mlir::isa<mlir::rlc::VoidType>(resultType))
		{
			writer.write(" -> '");
			writer.writeType(resultType);
			writer.write("'");
		}

		writer.writenl(":");
	}

	static void printArg(
			mlir::Type type, llvm::StringRef name, StreamWriter& writer)
	{
		bool typeIsPyobject = false;
		if (auto casted = mlir::dyn_cast_or_null<mlir::rlc::ClassType>(type);
				casted and casted.getName() == "PyObject")
			typeIsPyobject = true;

		bool isStringArg =
				mlir::isa_and_nonnull<mlir::rlc::StringLiteralType>(type);
		writer.write("ctypes.byref(");
		if (typeIsPyobject)
			writer.write("ctypes.py_object(");
		if (type != nullptr and builtinCType(type))
		{
			writer.writeType(type, 1);
			writer.write("(");
		}
		writer.write(name);
		if (isStringArg)
		{
			writer.write(".encode(\"utf-8\")");
		}
		if (type != nullptr and builtinCType(type))
		{
			writer.write(")");
		}
		if (typeIsPyobject)
			writer.write(")");
		writer.write(")");
		writer.write(", ");
	}

	static void printCallArgs(
			TypeRange typeRange,
			llvm::ArrayRef<llvm::StringRef> infoRange,
			StreamWriter& writer,
			mlir::Type resultType)
	{
		assert(typeRange.size() == infoRange.size());
		writer.write("(");

		if (not mlir::isa<mlir::rlc::VoidType>(resultType))
		{
			printArg(nullptr, "__result", writer);
		}
		for (size_t i = 0; i != typeRange.size(); i++)
			printArg(typeRange[i], infoRange[i], writer);
		writer.writenl(")");
	}

	static void printMangledWrapper(
			llvm::StringRef unmangledName,
			llvm::StringRef mangledName,
			llvm::ArrayRef<llvm::StringRef> argsInfo,
			mlir::FunctionType type,
			StreamWriter& w,
			mlir::Type resultType,
			bool isMemberFunction)
	{
		w.write("def ", mangledName);
		printFunctionDecl(
				type.getInputs(), argsInfo, w, resultType, isMemberFunction);
		auto _ = w.indent();

		if (returnsVoid(type).failed())
		{
			w.write("__result = ");
			w.writeType(resultType, 1);
			w.writenl("()");
		}

		w.write("lib.", mangledName);
		printCallArgs(type.getInputs(), argsInfo, w, resultType);

		// for functions that return something emit
		// return result
		// and if they return a builtin ctype type, add .value to
		// extract to convert it to a python builtin type instead
		if (returnsVoid(type).failed())
			w.write("return __result");
		if (needsUnwrapping(type))
			w.writenl(".value").endLine();
		else
			w.endLine().endLine();
	}

	static void declareOverload(
			llvm::StringRef unmangledName,
			llvm::ArrayRef<llvm::StringRef> argsInfo,
			mlir::FunctionType type,
			StreamWriter& w,
			bool isMemberFunction)
	{
		mlir::Type resultType = type.getNumResults() == 0
																? mlir::rlc::VoidType::get(type.getContext())
																: type.getResult(0);
		w.writenl("@overload");
		w.write("def ", unmangledName);
		printFunctionDecl(
				type.getInputs(), argsInfo, w, resultType, isMemberFunction);
		w.indentOnce(1).writenl("return").endLine();
	}

	static void declarePythonFunction(
			llvm::StringRef unmangledName,
			llvm::ArrayRef<llvm::StringRef> argsInfo,
			mlir::FunctionType type,
			StreamWriter& w,
			bool isMemberFunction,
			bool declareOveralods = true)
	{
		mlir::Type resultType = type.getNumResults() == 0
																? mlir::rlc::VoidType::get(type.getContext())
																: type.getResult(0);

		if (declareOveralods)
			declareOverload(unmangledName, argsInfo, type, w, isMemberFunction);
		auto mangledName =
				mlir::rlc::mangledName(unmangledName, isMemberFunction, type);

		// mangled wrapper
		printMangledWrapper(
				unmangledName,
				mangledName,
				argsInfo,
				type,
				w,
				resultType,
				isMemberFunction);

		// register the overload
		w.writenl("wrappers[\"", unmangledName, "\"].append(", mangledName, ")");
		w.write("signatures[", mangledName, "] = [");
		if (returnsVoid(type).succeeded())
			w.write("None, ");
		else
			w.writeType(type.getResults()[0]).write(", ");

		for (auto type : type.getInputs())
			w.writeType(type).write(", ");
		w.writenl("]").endLine();
	}

	void emitSpecialFunctions(
			mlir::Type type, mlir::rlc::StreamWriter& w, MemberFunctionsTable& table)
	{
		if (not table.isTriviallyInitializable(type))
		{
			w.writenl("def __init__(self):");
			auto _ = w.indent();
			w.writenl("self.to_erase = True");
			auto mangled_init_function_name = mangledName(
					"init",
					true,
					mlir::FunctionType::get(type.getContext(), { type }, {}));
			w.writenl("lib.", mangled_init_function_name, "(ctypes.byref(self))")
					.endLine();
		}

		if (not table.isTriviallyDestructible(type))
		{
			w.writenl("def __del__(self):");
			auto _ = w.indent();
			w.writenl("if hasattr(self, \"to_erase\") and self.to_erase:");
			auto _2 = w.indent();
			auto mangled_init_function_name = mangledName(
					"drop",
					true,
					mlir::FunctionType::get(type.getContext(), { type }, {}));
			w.writenl("lib.", mangled_init_function_name, "(ctypes.byref(self))")
					.endLine();
		}

		if (not table.isTriviallyCopiable(type))
		{
			w.writenl("def clone(self):");
			auto _2 = w.indent();

			auto mangled_assign_name = mangledName(
					"assign",
					true,
					mlir::FunctionType::get(type.getContext(), { type, type }, {}));
			w.write("new_one = ");
			w.writeType(type);
			w.writenl("()");
			w.writenl(
					"lib.",
					mangled_assign_name,
					"(ctypes.byref(new_one), ctypes.byref(self))");
			w.writenl("return new_one").endLine();
		}
	}

	void emitMembers(
			llvm::ArrayRef<mlir::Type> types,
			llvm::ArrayRef<llvm::StringRef> memberNames,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		w.write("_fields_ = [");
		for (auto [type, name] : llvm::zip(types, memberNames))
		{
			w.write("(\"", name, "\", ");
			w.writeType(type, 1);
			w.write("), ");
		}
		w.writenl("]").endLine();
	}

	void emitMembers(
			llvm::ArrayRef<mlir::Type> types,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		w.write("_fields_ = [");
		for (auto iter : llvm::enumerate(types))
		{
			w.write("(\"_alternative", iter.index(), "\", ");
			w.writeType(iter.value(), 1);
			w.write("), ");
		}
		w.writenl("]").endLine();
	}

	void emitHinting(
			llvm::ArrayRef<mlir::Type> types,
			llvm::ArrayRef<llvm::StringRef> memberNames,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		for (auto [type, name] : llvm::zip(types, memberNames))
		{
			if (name.starts_with("_"))
				continue;

			w.write(name, ": ");
			w.writeType(type);
			w.endLine();
		}
		w.endLine();
	}

	void emitOverloadDispatcher(
			llvm::StringRef name,
			llvm::ArrayRef<mlir::FunctionType> overloads,
			mlir::rlc::StreamWriter& w,
			bool isMethod)
	{
		w.writenl("def ", name, "(", isMethod ? "self, " : "", "*args):");
		auto _ = w.indent();
		for (auto overload : overloads)
		{
			w.write("if len(args) == ", overload.getNumInputs() - isMethod);
			for (auto argument :
					 llvm::drop_begin(llvm::enumerate(overload.getInputs()), isMethod))
			{
				w.write(" and isinstance(args[", argument.index() - isMethod, "], ");
				w.writeType(argument.value());
				w.write(")");
			}
			w.writenl(":");
			auto _ = w.indent();
			w.writenl(
					"return ",
					mangledName(name, isMethod, overload),
					"(",
					isMethod ? "self, " : "",
					"*args)");
		}

		w.writenl(
				 "raise TypeError(\"",
				 name,
				 " invoked with incorrect arguments types)\")")
				.endLine();
	}

	void emitMemberFunctions(
			mlir::Type type, mlir::rlc::StreamWriter& w, MemberFunctionsTable& table)
	{
		llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
		for (auto memberFunction : table.getMemberFunctionsOf(type))
		{
			declareOverload(
					memberFunction.getUnmangledName(),
					memberFunction.getInfo().getArgNames(),
					memberFunction.getType(),
					w,
					true);
			sortedOverloads[memberFunction.getUnmangledName()].push_back(
					memberFunction.getType());
		}

		for (auto& pair : sortedOverloads)
			emitOverloadDispatcher(pair.first(), pair.second, w, true);
	}

	void emitDeclaration(
			mlir::rlc::ClassType type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		w.write("class ");
		w.writeType(type);
		w.writenl("(ctypes.Structure):");
		auto _ = w.indent();

		emitMembers(type.getMemberTypes(), type.getMemberNames(), w, table);
		emitSpecialFunctions(type, w, table);
		emitHinting(type.getMemberTypes(), type.getMemberNames(), w, table);
		emitMemberFunctions(type, w, table);
	}

	void emitDeclaration(
			mlir::rlc::AlternativeType type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		w.write("class _");
		w.writeType(type);
		w.writenl("(ctypes.Union):");
		{
			auto _ = w.indent();
			emitMembers(type.getUnderlying(), w, table);
			w.endLine();
		}
		w.write("class ");
		w.writeType(type);
		w.writenl("(ctypes.Structure):");
		auto _ = w.indent();

		w.write("_fields_ = [(\"_content\", _");
		w.writeType(type);
		w.writenl("), (\"resume_index\", ctypes.c_longlong)]");

		emitSpecialFunctions(type, w, table);

		w.writenl("def __getitem__(self, key):");
		{
			auto _2 = w.indent();
			for (auto enumeration : llvm::enumerate(type.getUnderlying()))
			{
				w.write("if key == ");
				w.writeType(enumeration.value());
				w.writenl(" and self.resume_index == ", enumeration.index(), ":");
				auto _ = w.indent();
				w.writenl("return self._content._alternative", enumeration.index());
			}
			w.writenl("return None");
		}
		w.endLine();
		emitMemberFunctions(type, w, table);
	}

	class AliasToPythonAlias
	{
		public:
		void apply(mlir::rlc::TypeAliasOp op, mlir::rlc::StreamWriter& w)
		{
			w.write(op.getName(), " = ");
			w.writeType(mlir::cast<mlir::rlc::AliasType>(op.getDeclaredType())
											.getUnderlying());
			w.endLine();
			w.endLine();
		}
	};

	class ActionToPythonFunction
	{
		private:
		mlir::rlc::ModuleBuilder& builder;

		public:
		ActionToPythonFunction(mlir::rlc::ModuleBuilder& builder): builder(builder)
		{
		}
		void apply(mlir::rlc::ActionFunction op, mlir::rlc::StreamWriter& w)
		{
			declarePythonFunction(
					op.getUnmangledName(),
					op.getInfo().getArgNames(),
					op.getMainActionType(),
					w,
					op.getIsMemberFunction());
			if (not op.getPrecondition().empty())
				declarePythonFunction(
						"can_" + op.getUnmangledName().str(),
						op.getInfo().getArgNames(),
						mlir::FunctionType::get(
								op.getContext(),
								op.getMainActionType().getInputs(),
								{ mlir::rlc::BoolType::get(op.getContext()) }),
						w,
						op.getIsMemberFunction());

			for (auto value : op.getActions())
			{
				mlir::Operation* statement =
						builder.actionFunctionValueToActionStatement(value).front();
				auto actionStatement =
						mlir::cast<mlir::rlc::ActionStatement>(statement);
				llvm::SmallVector<llvm::StringRef> argNames = { "self" };
				for (auto arg : actionStatement.getInfo().getArguments())
					argNames.push_back(arg.getName());

				auto fType = mlir::cast<mlir::FunctionType>(value.getType());
				auto mangled = mangledName(actionStatement.getName(), true, fType);

				declarePythonFunction(
						actionStatement.getName(), argNames, fType, w, true, false);

				auto canDoType = mlir::FunctionType::get(
						fType.getContext(),
						fType.getInputs(),
						{ mlir::rlc::BoolType::get(fType.getContext()) });
				declarePythonFunction(
						"can_" + actionStatement.getName().str(),
						argNames,
						canDoType,
						w,
						true,
						false);

				w.writenl(
						"actions[\"",
						actionStatement.getName(),
						"\"].append(",
						mangled,
						")");
			}

			declarePythonFunction(
					"is_done",
					{ "self" },
					mlir::FunctionType::get(
							op.getContext(),
							{ op.getClassType() },
							{ mlir::rlc::BoolType::get(op.getContext()) }),
					w,
					true,
					false);
		}
	};

	class FunctionToPythonFunction
	{
		public:
		void apply(mlir::rlc::FunctionOp op, mlir::rlc::StreamWriter& w)
		{
			declarePythonFunction(
					op.getUnmangledName(),
					op.getInfo().getArgNames(),
					op.getFunctionType(),
					w,
					op.getIsMemberFunction(),
					not op.getIsMemberFunction());
			if (not op.getPrecondition().empty())
				declarePythonFunction(
						"can_" + op.getUnmangledName().str(),
						op.getInfo().getArgNames(),
						mlir::FunctionType::get(
								op.getContext(),
								op.getType().getInputs(),
								{ mlir::rlc::BoolType::get(op.getContext()) }),
						w,
						op.getIsMemberFunction(),
						not op.getIsMemberFunction());
		}
	};

	static void registerCommonTypeConversion(TypeSerializer& matcher)
	{
		matcher.add([](mlir::rlc::IntegerLiteralType type,
									 llvm::raw_string_ostream& OS) { OS << type.getValue(); });
		matcher.add([](mlir::rlc::VoidType type, llvm::raw_string_ostream& OS) {
			OS << "None";
		});
		matcher.add([&](mlir::rlc::AliasType type, llvm::raw_string_ostream& OS) {
			OS << type.getName();
		});
		matcher.add([&](mlir::rlc::FrameType type, llvm::raw_string_ostream& OS) {
			OS << matcher.convert(type.getUnderlying());
		});
		matcher.add([&](mlir::rlc::ContextType type, llvm::raw_string_ostream& OS) {
			OS << matcher.convert(type.getUnderlying());
		});
		matcher.add(
				[&](mlir::rlc::AlternativeType type, llvm::raw_string_ostream& OS) {
					OS << type.getMangledName();
				});
	}

	static void registerTypeConversions(
			TypeSerializer& matcher, TypeSerializer& ctypesSerializer)
	{
		matcher.add([](mlir::rlc::IntegerType type, llvm::raw_string_ostream& OS) {
			OS << "builtins.int";
		});
		matcher.add([](mlir::rlc::BoolType type, llvm::raw_string_ostream& OS) {
			OS << "builtins.bool";
		});
		matcher.add([](mlir::rlc::FloatType type, llvm::raw_string_ostream& OS) {
			OS << "builtins.float";
		});
		matcher.add([&](mlir::rlc::ArrayType type, llvm::raw_string_ostream& OS) {
			OS << "list";
		});
		matcher.add([&](mlir::rlc::ClassType type, llvm::raw_string_ostream& OS) {
			if (type.getName() == "PyObject")
				OS << "builtins.object";
			else
				OS << type.mangledName();
		});
		matcher.add(
				[&](mlir::rlc::OwningPtrType type, llvm::raw_string_ostream& OS) {
					OS << "ctypes.POINTER("
						 << ctypesSerializer.convert(type.getUnderlying()) << ")";
				});
		matcher.add(
				[&](mlir::rlc::ReferenceType type, llvm::raw_string_ostream& OS) {
					OS << "ctypes.POINTER("
						 << ctypesSerializer.convert(type.getUnderlying()) << ")";
				});
		matcher.add([](mlir::rlc::StringLiteralType type,
									 llvm::raw_string_ostream& OS) { OS << "builtins.str"; });
		registerCommonTypeConversion(matcher);
	}

	static void registerCTypesConversions(TypeSerializer& ser)
	{
		ser.add([](mlir::rlc::IntegerType type, llvm::raw_string_ostream& OS) {
			if (type.getSize() == 64)
				OS << "ctypes.c_longlong";
			else
				OS << "ctypes.c_byte";
		});
		ser.add([](mlir::rlc::FloatType type, llvm::raw_string_ostream& OS) {
			OS << "ctypes.c_double";
		});
		ser.add([](mlir::rlc::BoolType type, llvm::raw_string_ostream& OS) {
			OS << "ctypes.c_bool";
		});
		ser.add([&](mlir::rlc::ClassType type, llvm::raw_string_ostream& OS) {
			if (type.getName() == "PyObject")
				OS << "ctypes.py_object";
			else
				OS << type.mangledName();
		});
		ser.add([](mlir::rlc::StringLiteralType type,
							 llvm::raw_string_ostream& OS) { OS << "ctypes.c_char_p"; });
		ser.add([&](mlir::rlc::ArrayType type, llvm::raw_string_ostream& OS) {
			OS << ser.convert(type.getUnderlying()) << " * "
				 << ser.convert(type.getSize());
		});
		ser.add([&](mlir::rlc::OwningPtrType type, llvm::raw_string_ostream& OS) {
			OS << "ctypes.POINTER(" << ser.convert(type.getUnderlying()) << ")";
		});
		ser.add([&](mlir::rlc::ReferenceType type, llvm::raw_string_ostream& OS) {
			OS << "ctypes.POINTER(" << ser.convert(type.getUnderlying()) << ")";
		});
		registerCommonTypeConversion(ser);
	}

	static std::string emitActionFunction(
			mlir::rlc::ClassType frameType,
			llvm::StringRef actionName,
			mlir::TypeRange argTypes,
			mlir::Type resultType,
			llvm::ArrayRef<FunctionArgumentAttr> argsInfo,
			mlir::rlc::ModuleBuilder& builder,
			StreamWriter& OS)
	{
		const bool returnVoid = mlir::isa<mlir::rlc::VoidType>(resultType);
		OS.write("def ", actionName, "(self, ");
		for (auto [info, type] : llvm::zip(argsInfo, argTypes))
		{
			OS.write(info.getName(), ": '");
			OS.writeType(type);
			OS.write("', ");
		}
		OS.write(")");
		if (not returnVoid)
		{
			OS.write(" -> ");
			OS.writeType(resultType);
		}
		OS.writenl(":");
		auto _ = OS.indent();

		for (auto [info, type] : llvm::zip(argsInfo, argTypes))
		{
			OS.write("if ");
			OS.write("not isinstance(", info.getName(), ",");
			OS.writeType(type);
			OS.writenl("):");
			OS.indentOnce(1);
			OS.write(
					"raise TypeError(f\"",
					actionName,
					" invoked with incorrect argument type for argument ",
					info.getName(),
					". Expected ");
			OS.writeType(type);
			OS.writenl(" but got {type(", info.getName(), ")})\")").endLine();
		}

		llvm::SmallVector<mlir::Type> args = { frameType };
		for (auto arg : argTypes)
			args.push_back(arg);
		auto fType =
				mlir::FunctionType::get(resultType.getContext(), args, { resultType });
		std::string mangled = mangledName(actionName, true, fType);

		if (not returnVoid)
		{
			OS.write("__result = ");
			OS.writeType(resultType, 1);
			OS.writenl("()");
		}

		OS.write(
				"lib.",
				mangled,
				"(",
				returnVoid ? "" : "ctypes.byref(__result), ",
				"ctypes.byref(self), ");

		for (auto [info, type] : llvm::zip(argsInfo, argTypes))
		{
			printArg(type, info.getName(), OS);
		}
		OS.writenl(")");

		if (not returnVoid)
			OS.writenl("return __result", needsUnwrapping(fType) ? ".value" : "");

		OS.endLine();
		return mangled;
	}

	static void emitActionFunctions(
			mlir::rlc::ActionFunction action,
			mlir::rlc::ModuleBuilder& builder,
			StreamWriter& OS)
	{
		for (auto value : action.getActions())
		{
			auto _ = OS.indent();
			mlir::Operation* statement =
					builder.actionFunctionValueToActionStatement(value).front();
			auto actionStatement = mlir::cast<mlir::rlc::ActionStatement>(statement);
			emitActionFunction(
					action.getClassType(),
					actionStatement.getName(),
					actionStatement.getResultTypes(),
					mlir::rlc::VoidType::get(action.getContext()),
					actionStatement.getInfo().getArguments(),
					builder,
					OS);
			emitActionFunction(
					action.getClassType(),
					("can_" + actionStatement.getName()).str(),
					actionStatement.getResultTypes(),
					mlir::rlc::BoolType::get(action.getContext()),
					actionStatement.getInfo().getArguments(),
					builder,
					OS);
		}

		{
			auto _ = OS.indent();
			emitActionFunction(
					action.getClassType(),
					"is_done",
					{},
					mlir::rlc::BoolType::get(action.getContext()),
					{},
					builder,
					OS);
		}

		OS.endLine();
	}

#define GEN_PASS_DEF_PRINTPYTHONPASS
#include "rlc/dialect/Passes.inc"
	struct PrintPythonPass: impl::PrintPythonPassBase<PrintPythonPass>
	{
		using impl::PrintPythonPassBase<PrintPythonPass>::PrintPythonPassBase;

		void runOnOperation() override
		{
			rlc::PatternMatcher matcher(*OS);
			MemberFunctionsTable table(getOperation());
			mlir::rlc::ModuleBuilder builder(getOperation());

			matcher.addTypeSerializer();
			registerTypeConversions(
					matcher.getWriter().getTypeSerializer(),
					matcher.getWriter().getTypeSerializer(1));
			registerCTypesConversions(matcher.getWriter().getTypeSerializer(1));

			matcher.add<FunctionToPythonFunction>();
			matcher.add<ActionToPythonFunction>(builder);
			matcher.add<AliasToPythonAlias>();

			// emit includes
			printPrelude(matcher.getWriter(), isMac, isWindows);

			// emit declarations of types
			for (auto t : ::rlc::postOrderTypes(getOperation()))
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(t))
				{
					if (casted.getName() == "PyObject")
						continue;
					emitDeclaration(casted, matcher.getWriter(), table);
					if (builder.isClassOfAction(casted))
					{
						auto action = mlir::cast<mlir::rlc::ActionFunction>(
								builder.getActionOf(casted).getDefiningOp());
						emitActionFunctions(action, builder, matcher.getWriter());
					}
				}
				if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(t))
					emitDeclaration(casted, matcher.getWriter(), table);
			}

			// emit declarations of free functions
			matcher.apply(getOperation());

			// emit dispatcher of free functions
			llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
			for (auto op : getOperation().getOps<mlir::rlc::FunctionOp>())
			{
				if (op.getIsMemberFunction())
					continue;
				sortedOverloads[op.getUnmangledName()].push_back(op.getType());
				if (not op.getPrecondition().empty())
					sortedOverloads["can_" + op.getUnmangledName().str()].push_back(
							mlir::FunctionType::get(
									op.getContext(),
									op.getType().getInputs(),
									{ mlir::rlc::BoolType::get(op.getContext()) }));
			}
			for (auto op : getOperation().getOps<mlir::rlc::ActionFunction>())
			{
				if (op.getIsMemberFunction())
					continue;
				sortedOverloads[op.getUnmangledName()].push_back(
						op.getMainActionType());
				if (not op.getPrecondition().empty())
					sortedOverloads["can_" + op.getUnmangledName().str()].push_back(
							mlir::FunctionType::get(
									op.getContext(),
									op.getMainActionType().getInputs(),
									{ mlir::rlc::BoolType::get(op.getContext()) }));

				auto types = builder.getConverter().getTypes().get(
						("Any" + op.getClassType().getName() + "Action").str());
				if (not types.empty())
					matcher.getWriter().writenl(
							"actionToAnyFunctionType[\"",
							op.getUnmangledName(),
							"\"] = Any",
							op.getClassType().getName(),
							"Action");
			}

			for (auto& pair : sortedOverloads)
				emitOverloadDispatcher(
						pair.first(), pair.second, matcher.getWriter(), false);
		}
	};

}	 // namespace mlir::rlc

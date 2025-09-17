/*
Copyright 2025 Massimo Fioravanti

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

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/ActionArgumentAnalysis.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/MemberFunctionsTable.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"
#include "rlc/utils/PatternMatcher.hpp"
namespace mlir::rlc
{
	static void printCFunDecl(
			StreamWriter& writer, mlir::FunctionType type, llvm::StringRef name)
	{
		mlir::Type result = type.getNumResults() == 0
														? mlir::rlc::VoidType::get(type.getContext())
														: type.getResults()[0];
		writer.write("extern '");
		writer.write("void ");
		writer.write(name);
		writer.write("(");
		if (not result.isa<mlir::rlc::VoidType>())
		{
			writer.write("void* result");
			if (not type.getInputs().empty())
			{
				writer.write(", ");
			}
		}
		size_t i = 1;
		for (auto arg : type.getInputs())
		{
			writer.write("void*");
			if (i++ != type.getNumInputs())
				writer.write(",");
		}
		writer.write(")");
		writer.writenl("'");
	}

	static void printCFunDecls(
			StreamWriter& writer,
			mlir::ModuleOp op,
			mlir::rlc::ModuleBuilder& builder)
	{
		for (auto f : op.getBodyRegion().getOps<mlir::rlc::FunctionOp>())
		{
			if (f.isInternal())
				continue;
			printCFunDecl(writer, f.getType(), f.getMangledName());
			if (not f.getPrecondition().empty())
			{
				printCFunDecl(
						writer,
						mlir::FunctionType::get(
								f.getContext(),
								f.getArgumentTypes(),
								mlir::rlc::BoolType::get(f.getContext())),
						f.getCanFunctionMangledName());
			}
		}

		for (auto f : op.getBodyRegion().getOps<mlir::rlc::ActionFunction>())
		{
			if (not f.isInternal())
			{
				printCFunDecl(writer, f.getType(), f.getMangledName());
				if (not f.getPrecondition().empty())
				{
					printCFunDecl(
							writer,
							mlir::FunctionType::get(
									f.getContext(),
									f.getArgumentTypes(),
									mlir::rlc::BoolType::get(f.getContext())),
							f.getCanFunctionMangledName());
				}
			}
			for (auto value : f.getActions())
			{
				auto action = builder.actionFunctionValueToActionStatement(value);
				if (f.getUnmangledName().starts_with("_"))
				{
					continue;
				}
				auto actionStatement =
						mlir::cast<mlir::rlc::ActionStatement>(action[0]);

				auto fType = mlir::cast<mlir::FunctionType>(value.getType());
				auto mangled = mangledName(actionStatement.getName(), true, fType);
				printCFunDecl(writer, fType, mangled);

				auto canfType = mlir::FunctionType::get(
						f.getContext(),
						fType.getInputs(),
						{ mlir::rlc::BoolType::get(f.getContext()) });
				auto canMangled = mangledName(
						"can_" + actionStatement.getName().str(), true, canfType);
				printCFunDecl(writer, canfType, canMangled);
			}

			printCFunDecl(
					writer,
					f.getIsDoneFunctionType(),
					mangledName("is_done", true, f.getIsDoneFunctionType()));
		}
	}

	// returns true if `type` has a immediate mapping onto a ctypes
	// type.
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

	// returns true if the require holds the real content
	// in the `.value` member
	static bool needsUnwrapping(mlir::FunctionType type)
	{
		if (type.getNumResults() == 0)
			return false;
		return builtinCType(type.getResult(0));
	}

	static void printArgName(StreamWriter& writer, llvm::StringRef name)
	{
		if (name == "self")
		{
			writer.write("__self");
		}
		else
			writer.write(name);
	}

	static void printFunctionDecl(
			TypeRange typeRange,
			llvm::ArrayRef<llvm::StringRef> infoRange,
			StreamWriter& writer,
			mlir::Type resultType,
			bool isMemberFunction)
	{
		assert(typeRange.size() == infoRange.size());
		if (typeRange.empty())
			return;
		for (size_t i = 0; i != typeRange.size(); i++)
		{
			printArgName(writer, infoRange[i]);
			if (i != typeRange.size() - 1)
				writer.write(", ");
		}
	}

	static void printTypeHintingFunctionDecl(
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
			writer.writeType(typeRange[i]);
			writer.write(" ");
			writer.write(infoRange[i]);
			writer.write("',");
		}
		writer.write(")");

		if (not mlir::isa<mlir::rlc::VoidType>(resultType))
		{
			writer.write(" -> ");
			writer.writeType(resultType);
		}
	}

	static void printArg(
			mlir::Type type, llvm::StringRef name, StreamWriter& writer)
	{
		bool isUserDefined = false;
		if (type and (mlir::isa<mlir::rlc::ClassType>(type) or
									mlir::isa<mlir::rlc::AlternativeType>(type)))
			isUserDefined = true;

		bool isStringArg =
				mlir::isa_and_nonnull<mlir::rlc::StringLiteralType>(type);
		if (type != nullptr and builtinCType(type))
		{
			writer.write("rlc_convert_to_");
			writer.writeType(type, 0);
			writer.write("(");
		}
		printArgName(writer, name);
		if (isStringArg)
		{
			writer.write(".encode(\"utf-8\")");
		}
		if (type != nullptr and builtinCType(type))
		{
			writer.write(")");
		}
		if (isUserDefined)
			writer.write("._data");
		writer.write(" ");
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
			writer.write(", ");
		}
		for (size_t i = 0; i != typeRange.size(); i++)
		{
			printArg(typeRange[i], infoRange[i], writer);
			writer.write(", ");
		}
		writer.write(")");
	}

	static void printTypeUnpack(StreamWriter& w, mlir::Type resultType)
	{
		llvm::TypeSwitch<mlir::Type>(resultType)
				.Case([&](mlir::rlc::IntegerType t) {
					if (t.getSize() == 64)
						w.write("q");
					if (t.getSize() == 8)
						w.write("c");
				})
				.Case([&](mlir::rlc::BoolType t) { w.write("c"); })
				.Case([&](mlir::rlc::ReferenceType t) { w.write("J"); })
				.Default([](mlir::Type t) {
					t.dump();
					abort();
				});
	}

	static void printTypeSize(StreamWriter& w, mlir::Type resultType)
	{
		llvm::TypeSwitch<mlir::Type>(resultType)
				.Case([&](mlir::rlc::IntegerType t) {
					if (t.getSize() == 64)
						w.write("Fiddle::SIZEOF_LONG_LONG");
					if (t.getSize() == 8)
						w.write("Fiddle::SIZEOF_CHAR");
				})
				.Case([&](mlir::rlc::BoolType t) { w.write("Fiddle::SIZEOF_CHAR"); })
				.Case([&](mlir::rlc::BoolType t) { w.write("Fiddle::SIZEOF_DOUBLE"); })
				.Case([&](mlir::rlc::ReferenceType t) {
					w.write("Fiddle::SIZEOF_VOIDP");
				})
				.Case(
						[&](mlir::rlc::ClassType t) { w.write(t.mangledName(), ".size"); })
				.Case([&](mlir::rlc::AliasType t) {
					w.write(rlc::typeToMangled(t), ".size");
				})
				.Default([](mlir::Type t) {
					t.dump();
					abort();
				});
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
		w.write("def _", mangledName, " ");
		printFunctionDecl(
				type.getInputs(), argsInfo, w, resultType, isMemberFunction);
		w.endLine();

		{
			auto _ = w.indent();
			bool isUserDefined = resultType.isa<mlir::rlc::ClassType>() or
													 resultType.isa<mlir::rlc::AlternativeType>();

			if (returnsVoid(type).failed())
			{
				if (isUserDefined)
				{
					w.write("__to_return = ");
					w.writeType(resultType);
					w.writenl(".new");
					w.writenl("__result = __to_return._data.to_ptr");
				}
				else
				{
					w.write("__result = Fiddle::Pointer.malloc(");
					printTypeSize(w, resultType);
					w.writenl(")");
				}
			}

			w.write("MyLib::", mangledName);
			printCallArgs(type.getInputs(), argsInfo, w, resultType);
			w.endLine();

			// for functions that return something emit
			// return __result
			// and if they return a builtin ctype type, add .value to
			// extract to convert it to a python builtin type instead
			if (returnsVoid(type).failed())
			{
				if (isUserDefined)
				{
					w.writenl("__to_return");
				}
				else
				{
					w.write("__result[0, ");
					printTypeSize(w, resultType);
					w.write("].unpack1('");
					printTypeUnpack(w, resultType);
					w.write("')");
					if (resultType.isa<mlir::rlc::BoolType>())
					{
						w.write(" != 0");
					}
				}
			}
			w.endLine().endLine();
		}

		w.writenl("end").endLine();
	}

	static void declareOverload(
			llvm::StringRef unmangledName,
			llvm::ArrayRef<llvm::StringRef> argsInfo,
			mlir::FunctionType type,
			StreamWriter& w,
			bool isMemberFunction)
	{
		if (unmangledName.starts_with("_"))
			return;
		mlir::Type resultType = type.getNumResults() == 0
																? mlir::rlc::VoidType::get(type.getContext())
																: type.getResult(0);
		w.write("def ", unmangledName, ": ");
		printTypeHintingFunctionDecl(
				type.getInputs(), argsInfo, w, resultType, isMemberFunction);
		w.endLine();
	}

	// When we declare a python function we
	// * Emit the mangled function
	//     (eg: def rl_ugly_mangled_name_r_bool(arg: ctypes.c_bool):)
	//   that dispaches directly to the symbol in the binary
	//
	// * Emit the non mangled typehinting
	//     @overload
	//     def nice_name(arg: Bool):
	//        return
	//
	// * stick that overload in the signatures global list so
	//   other people can discover it dynamically if they need.
	static void declareRubyFunction(
			llvm::StringRef unmangledName,
			llvm::ArrayRef<llvm::StringRef> argsInfo,
			mlir::FunctionType type,
			StreamWriter& w,
			bool isMemberFunction,
			bool declareOveralods = true)
	{
		if (unmangledName.starts_with("_"))
			return;
		mlir::Type resultType = type.getNumResults() == 0
																? mlir::rlc::VoidType::get(type.getContext())
																: type.getResult(0);

		// if (declareOveralods)
		// declareOverload(unmangledName, argsInfo, type, w, isMemberFunction);
		auto mangledName =
				mlir::rlc::mangledName(unmangledName, isMemberFunction, type);

		printMangledWrapper(
				unmangledName,
				mangledName,
				argsInfo,
				type,
				w,
				resultType,
				isMemberFunction);

		// register the overload
		w.writenl("wrappers[\"", unmangledName, "\"] << :", mangledName);
		w.write("signatures[:", mangledName, "] = [");
		if (returnsVoid(type).succeeded())
			w.write("nil, ");
		else
			w.writeType(type.getResults()[0]).write(", ");

		for (auto type : type.getInputs())
			w.writeType(type).write(", ");
		w.writenl("]").endLine();
	}

	static void emitToString(
			mlir::Type type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder)

	{
		mlir::rlc::OverloadResolver resolver(builder.getSymbolTable(), nullptr);
		auto toStringOverload = resolver.findOverload(
				mlir::UnknownLoc::get(type.getContext()), false, "to_string", { type });

		if (not toStringOverload)
			return;

		auto overload = mlir::dyn_cast_or_null<mlir::rlc::FunctionOp>(
				toStringOverload.getDefiningOp());
		if (not overload or overload.getType().getNumResults() == 0)
			return;

		auto casted =
				mlir::dyn_cast<mlir::rlc::ClassType>(overload.getType().getResult(0));

		if (casted and casted.getName() == "String")
		{
			w.writenl("def to_s");
			{
				auto _ = w.indent();
				w.writenl("__string = String.malloc");
				w.writenl(
						overload.getMangledName(),
						"(__string.content.to_ptr, @content.to_ptr)");
				w.writenl("return __string.content.to_s");
			}
			w.writenl("end");
		}
	}

	// the three rlc special functions init, drop and assing must
	// be special cased to ensure they are always called, otherwise
	// the user could access invalid memory.
	//
	// In practice this means overriding python __init__, __del__ and
	// clone so that we can dispatch to the proper rlc methods.
	//
	// Furthermore, if rlc to_string is available, we override python
	static void emitSpecialFunctions(
			mlir::Type type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder)
	{
		emitToString(type, w, table, builder);
		w.writenl("def initialize(ptr=nil)");
		{
			auto _ = w.indent();
			w.writenl("@to_erase = ptr.nil?");
			w.write("@content = ptr ? ptr :  MyLib::RLC_");
			w.writeType(type);
			w.writenl(".malloc");
			if (not table.isTriviallyInitializable(type))
			{
				w.writenl("if @to_erase");
				{
					auto _ = w.indent();
					auto mangled_init_function_name = mangledName(
							"init",
							true,
							mlir::FunctionType::get(type.getContext(), { type }, {}));
					w.writenl("MyLib::", mangled_init_function_name, " @content.to_ptr")
							.endLine();
				}
				w.writenl("end");
			}
		}
		w.writenl("end");

		w.writenl("def free");
		{
			auto _ = w.indent();
			w.writenl("if @to_erase");
			{
				auto _2 = w.indent();
				if (not table.isTriviallyDestructible(type))
				{
					auto mangled_init_function_name = mangledName(
							"drop",
							true,
							mlir::FunctionType::get(type.getContext(), { type }, {}));
					w.writenl("_", mangled_init_function_name, "@content.to_ptr")
							.endLine();
				}
				w.writenl("@content.to_ptr.free");
			}
			w.writenl("end");
		}
		w.writenl("end");

		if (not table.isTriviallyCopiable(type))
		{
			w.writenl("def dup");
			{
				auto _2 = w.indent();

				auto mangled_assign_name = mangledName(
						"assign",
						true,
						mlir::FunctionType::get(type.getContext(), { type, type }, {}));
				w.write("new_one = ");
				w.writeType(type);
				w.writenl(".malloc");
				w.writenl(
						"_ ",
						mangled_assign_name,
						" new_one.content.to_ptr @content.to_ptr");
				w.writenl("return new_one").endLine();
			}
			w.writenl("end");
		}
	}

	static void emitMemberType(
			mlir::rlc::StreamWriter& w, mlir::Type type, llvm::StringRef name)
	{
		if (auto casted = type.dyn_cast<mlir::rlc::ClassType>())
		{
			w.write("{", name, ": RLC_", casted.mangledName(), "}");
			return;
		}
		if (auto casted = type.dyn_cast<mlir::rlc::AlternativeType>())
		{
			w.write("{", name, ": RLC_", casted.getMangledName(), "}");
			return;
		}
		if (auto casted = type.dyn_cast<mlir::rlc::ArrayType>())
		{
			w.write("'");
			w.writeType(casted.getUnderlying(), 1);
			w.write(" ", name);
			w.write("[");
			w.write(casted.getSize(), "]");
			w.write("'");
			return;
		}
		w.write("{", name, ":");
		w.writeType(type, 1);
		w.write("}");
	}

	static void emitMembers(
			llvm::ArrayRef<mlir::Type> types,
			llvm::ArrayRef<llvm::StringRef> memberNames,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		size_t i = 1;
		for (auto [type, name] : llvm::zip(types, memberNames))
		{
			emitMemberType(w, type, name);
			if (i++ != memberNames.size())
			{
				w.write(",");
			}
			w.endLine();
		}
	}

	static void emitMembers(
			llvm::ArrayRef<mlir::Type> types,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		for (auto iter : llvm::enumerate(types))
		{
			w.write("(\"_alternative", iter.index(), "\", ");
			w.writeType(iter.value(), 1);
			w.write("), ");
		}
	}

	static void emitHinting(
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

	// Python has no innate overload, so we have to create runtime
	// dispatchers that look at the types of arguments and find the
	// right overload.
	//
	// In practice this just means doing
	// def f(*args):
	//   if len(args) == overload_arg_count and isinstance(args[0], t1) and ...:
	//     return overload(*args)
	//   ...
	//   raise TypeError()
	static void emitOverloadDispatcher(
			llvm::StringRef name,
			llvm::ArrayRef<mlir::FunctionType> overloads,
			mlir::rlc::StreamWriter& w,
			bool isMethod)
	{
		w.writenl("def ", name, "(*args)");
		{
			auto _ = w.indent();
			for (auto overload : overloads)
			{
				w.write("if args.length == ", overload.getNumInputs() - isMethod);
				for (auto argument :
						 llvm::drop_begin(llvm::enumerate(overload.getInputs()), isMethod))
				{
					w.write(" and args[", argument.index() - isMethod, "].class == ");
					w.writeType(argument.value());
				}
				w.endLine();
				{
					auto _ = w.indent();
					w.writenl(
							"return _",
							mangledName(name, isMethod, overload),
							isMethod ? " self, " : "",
							" *args");
				}
				w.writenl("end");
			}

			w.writenl("raise \"", name, " invoked with incorrect arguments types)\"")
					.endLine();
		}
		w.writenl("end");
	}

	static void emitMemberConverter(
			mlir::rlc::ClassFieldAttr attr, mlir::rlc::StreamWriter& w)
	{
		if (auto clsType = attr.getType().dyn_cast<mlir::rlc::ClassType>())
		{
			w.write(clsType.mangledName(), ".new(");
			w.write("@content.", attr.getName(), ")");
			return;
		}
		if (auto clsType = attr.getType().dyn_cast<mlir::rlc::AlternativeType>())
		{
			w.write(clsType.getMangledName(), ".new(");
			w.write("@content.", attr.getName(), ")");
			return;
		}
		w.write("@content.", attr.getName(), ".content");
	}

	static void emitMemberAccessors(
			mlir::rlc::ClassType type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		w.writenl("def _data");
		{
			auto _ = w.indent();
			w.write("return @content");
			w.endLine();
		}
		w.writenl("end");

		for (auto member : type.getMembers())
		{
			w.writenl("def ", member.getName());
			{
				auto _ = w.indent();
				w.write("return ");
				emitMemberConverter(member, w);
				w.endLine();
			}

			w.writenl("end");

			w.writenl("def ", member.getName(), "=(val)");
			{
				auto _ = w.indent();
				w.write("@content.", member.getName(), ".content = val");
				w.endLine();
			}

			w.writenl("end");
		}
	}

	static void emitMemberFunctions(
			mlir::Type type, mlir::rlc::StreamWriter& w, MemberFunctionsTable& table)
	{
		llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
		for (auto memberFunction : table.getMemberFunctionsOf(type))
		{
			// declareOverload(
			// memberFunction.getUnmangledName(),
			// memberFunction.getInfo().getArgNames(),
			// memberFunction.getType(),
			// w,
			// true);
			sortedOverloads[memberFunction.getUnmangledName()].push_back(
					memberFunction.getType());
		}

		for (auto& pair : sortedOverloads)
			emitOverloadDispatcher(pair.first(), pair.second, w, true);
	}

	static void emitEnumDeclaration(
			mlir::rlc::EnumDeclarationOp enumDecl,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table)
	{
		for (auto value : llvm::enumerate(
						 enumDecl.getBody().getOps<mlir::rlc::EnumFieldDeclarationOp>()))
		{
			w.writenl("def ", value.value().getName());
			auto _ = w.indent();
			w.writenl("__result = ", enumDecl.getName(), ".malloc");
			w.writenl("__result.value = ", value.index());
			w.writenl("return __result").endLine();
		}
	}

	static void emitFiddleDeclaration(
			mlir::rlc::ClassType type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder,
			mlir::rlc::EnumDeclarationOp enumDeclaration = nullptr)
	{
		w.write("RLC_");
		w.writeType(type);
		w.writenl(" = struct([");
		{
			auto _ = w.indent();
			emitMembers(type.getMemberTypes(), type.getMemberNames(), w, table);
		}
		w.writenl("])");
	}

	static void declareFiddleClasses(
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder,
			mlir::ModuleOp op)
	{
		for (auto t : ::rlc::postOrderTypes(op))
		{
			if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(t))
			{
				emitFiddleDeclaration(casted, w, table, builder);
			}
		}
	}

	static void emitDeclaration(
			mlir::rlc::ClassType type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder,
			mlir::rlc::EnumDeclarationOp enumDeclaration = nullptr)
	{
		w.write("class ");
		w.writeType(type);
		w.endLine();

		{
			auto _ = w.indent();

			emitSpecialFunctions(type, w, table, builder);
			//  emitHinting(type.getMemberTypes(), type.getMemberNames(), w, table);
			emitMemberAccessors(type, w, table);
			emitMemberFunctions(type, w, table);

			if (enumDeclaration)
				emitEnumDeclaration(enumDeclaration, w, table);
		}
	}

	// Emits python imports and bookinping global variables.
	static void printPrelude(
			StreamWriter& writer,
			bool isMac,
			bool isWindows,
			mlir::ModuleOp op,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder)
	{
		writer.writenl("require 'fiddle'");
		writer.writenl("require 'fiddle/import'");

		writer.writenl("Boolean = [TrueClass, FalseClass]");

		writer.writenl("module MyLib");
		{
			auto _ = writer.indent();
			writer.writenl("extend Fiddle::Importer");

			writer.write("dlload __dir__ + '/lib");
			if (isMac)
				writer.writenl(".dylib'");
			else if (isWindows)
				writer.writenl(".dll'");
			else
				writer.writenl(".so'");
			writer.writenl("RLC_int = struct([\"long content\"])");
			writer.writenl("RLC_voidp = struct([\"void* content\"])");
			writer.writenl("RLC_string_lit = struct([\"char* content\"])");
			writer.writenl("RLC_array = struct([\"char* content\"])");
			writer.writenl("RLC_bool = struct([\"char content\"])");
			writer.writenl("RLC_byte = struct([\"char content\"])");
			writer.writenl("RLC_float = struct([\"double content\"])");

			declareFiddleClasses(writer, table, builder, op);
			printCFunDecls(writer, op, builder);
		}
		writer.writenl("end");
		writer.writenl("actions = Hash.new { |hash, key| hash[key] = [] }");
		writer.writenl("wrappers = Hash.new { |hash, key| hash[key] = [] }");
		writer.writenl("signatures = {}");
		writer.writenl("actionToAnyFunctionType = {}");

		writer.writenl("def rlc_convert_to_Integer(value = 0)");
		writer.writenl("\tp = Fiddle::Pointer.malloc(Fiddle::SIZEOF_INT)");
		writer.writenl("\tp[0, Fiddle::SIZEOF_LONG_LONG] = [value].pack(\"q\")");
		writer.writenl("end");

		writer.writenl("def rlc_convert_to_Float(value = 0)");
		writer.writenl("\tp = Fiddle::Pointer.malloc(Fiddle::SIZEOF_INT)");
		writer.writenl("\tp[0, Fiddle::SIZEOF_DOUBLE] = [value].pack(\"q\")");
		writer.writenl("end");
	}

	static void emitDeclaration(
			mlir::rlc::AlternativeType type,
			mlir::rlc::StreamWriter& w,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder)
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

		emitSpecialFunctions(type, w, table, builder);

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
			w.writenl("return nil");
		}
		w.endLine();
		emitMemberFunctions(type, w, table);
	}

	class AliasToRubyAlias
	{
		public:
		void apply(mlir::rlc::TypeAliasOp op, mlir::rlc::StreamWriter& w)
		{
			w.write(op.getName(), " = ");
			w.writeType(
					mlir::cast<mlir::rlc::AliasType>(op.getDeclaredType())
							.getUnderlying());
			w.endLine();
			w.endLine();
		}
	};

	// ActionFunction end up generating:
	// * A class that rappresents the ActionFunction
	// * A free function to start the ActionFunction
	// * Optionally, the precondition function of the free function.
	// * the is_done member function.
	//
	// then, for each actions statement:
	// * emit the member function to trigger that action
	// * emit the precondition of that action member function.
	class ActionToRubyFunction
	{
		private:
		mlir::rlc::ModuleBuilder& builder;

		public:
		ActionToRubyFunction(mlir::rlc::ModuleBuilder& builder): builder(builder) {}

		void apply(mlir::rlc::ActionFunction op, mlir::rlc::StreamWriter& w)
		{
			declareRubyFunction(
					op.getUnmangledName(),
					op.getInfo().getArgNames(),
					op.getMainActionType(),
					w,
					op.getIsMemberFunction());
			if (not op.getPrecondition().empty())
				declareRubyFunction(
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
				llvm::SmallVector<llvm::StringRef> argNames = { "__self" };
				for (auto arg : actionStatement.getInfo().getArguments())
					argNames.push_back(arg.getName());

				auto fType = mlir::cast<mlir::FunctionType>(value.getType());
				auto mangled = mangledName(actionStatement.getName(), true, fType);

				// declareRubyFunction(
				// actionStatement.getName(), argNames, fType, w, true, false);

				auto canDoType = mlir::FunctionType::get(
						fType.getContext(),
						fType.getInputs(),
						{ mlir::rlc::BoolType::get(fType.getContext()) });
				declareRubyFunction(
						"can_" + actionStatement.getName().str(),
						argNames,
						canDoType,
						w,
						true,
						false);

				// w.writenl(
				//"actions[\"",
				// actionStatement.getName(),
				//"\"].append(",
				// mangled,
				//")");
			}

			declareRubyFunction(
					"is_done",
					{ "_self" },
					mlir::FunctionType::get(
							op.getContext(),
							{ op.getClassType() },
							{ mlir::rlc::BoolType::get(op.getContext()) }),
					w,
					true,
					false);
		}
	};

	class FunctionToRubyFunction
	{
		public:
		void apply(mlir::rlc::FunctionOp op, mlir::rlc::StreamWriter& w)
		{
			declareRubyFunction(
					op.getUnmangledName(),
					op.getInfo().getArgNames(),
					op.getFunctionType(),
					w,
					op.getIsMemberFunction(),
					not op.getIsMemberFunction());
			if (not op.getPrecondition().empty())
				declareRubyFunction(
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
			OS << "Fiddle::TYPE_VOID";
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
			OS << "Integer";
		});
		matcher.add([](mlir::rlc::BoolType type, llvm::raw_string_ostream& OS) {
			OS << ":Boolean";
		});
		matcher.add([](mlir::rlc::FloatType type, llvm::raw_string_ostream& OS) {
			OS << "Float";
		});
		matcher.add([&](mlir::rlc::ArrayType type, llvm::raw_string_ostream& OS) {
			OS << "Array";
		});
		matcher.add([&](mlir::rlc::ClassType type, llvm::raw_string_ostream& OS) {
			OS << type.mangledName();
		});
		matcher.add(
				[&](mlir::rlc::OwningPtrType type, llvm::raw_string_ostream& OS) {
					OS << "MyLib::RLC_voidp";
				});
		matcher.add(
				[&](mlir::rlc::ReferenceType type, llvm::raw_string_ostream& OS) {
					OS << "MyLib::RLC_voidp";
				});
		matcher.add([](mlir::rlc::StringLiteralType type,
									 llvm::raw_string_ostream& OS) { OS << "String"; });
		registerCommonTypeConversion(matcher);
	}

	static void registerCTypesConversions(TypeSerializer& ser)
	{
		ser.add([](mlir::rlc::IntegerType type, llvm::raw_string_ostream& OS) {
			if (type.getSize() == 64)
				OS << "MyLib::RLC_int";
			else
				OS << "MyLib::RLC_byte";
		});
		ser.add([](mlir::rlc::FloatType type, llvm::raw_string_ostream& OS) {
			OS << "MyLib::RLC_float";
		});
		ser.add([](mlir::rlc::BoolType type, llvm::raw_string_ostream& OS) {
			OS << "MyLib::RLC_bool";
		});
		ser.add([&](mlir::rlc::ClassType type, llvm::raw_string_ostream& OS) {
			OS << type.mangledName();
		});
		ser.add(
				[](mlir::rlc::StringLiteralType type, llvm::raw_string_ostream& OS) {
					OS << "MyLib::RLC_string_lit";
				});
		ser.add([&](mlir::rlc::ArrayType type, llvm::raw_string_ostream& OS) {
			OS << "MyLib::RLC_array";
		});
		ser.add([&](mlir::rlc::OwningPtrType type, llvm::raw_string_ostream& OS) {
			OS << "MyLib::RLC_voidp";
		});
		ser.add([&](mlir::rlc::ReferenceType type, llvm::raw_string_ostream& OS) {
			OS << "MyLib::RLC_voidp";
		});
		registerCommonTypeConversion(ser);
	}

	// emitting a action function is ugly because we need to collect
	// the frame type from the action function, and the arguments from
	// the action statements.
	static void emitActionFunction(
			mlir::rlc::ClassType frameType,
			llvm::StringRef actionName,
			mlir::TypeRange argTypes,
			mlir::Type resultType,
			llvm::ArrayRef<FunctionArgumentAttr> argsInfo,
			mlir::rlc::ModuleBuilder& builder,
			StreamWriter& OS)
	{
		const bool returnVoid = mlir::isa<mlir::rlc::VoidType>(resultType);
		OS.write("def ", actionName, "(");
		size_t i = 1;
		for (auto [info, type] : llvm::zip(argsInfo, argTypes))
		{
			OS.write(info.getName());
			if (i++ != argsInfo.size())
				OS.write(", ");
		}
		OS.writenl(")");
		{
			auto _ = OS.indent();

			for (auto [info, type] : llvm::zip(argsInfo, argTypes))
			{
				OS.write("if ");
				OS.write(info.getName(), ".class != ");
				OS.writeType(type);
				OS.endLine();
				OS.indentOnce(1);
				OS.write(
						"raise \"",
						actionName,
						" invoked with incorrect argument type for argument ",
						info.getName(),
						". Expected ");
				OS.writeType(type);
				OS.writenl("\"");
				OS.writenl("end");
			}

			llvm::SmallVector<mlir::Type> args = { frameType };
			for (auto arg : argTypes)
				args.push_back(arg);
			auto fType = mlir::FunctionType::get(
					resultType.getContext(), args, { resultType });
			std::string mangled = mangledName(actionName, true, fType);

			if (not returnVoid)
			{
				OS.write("__result = ");
				OS.writeType(resultType, 1);
				OS.writenl(".malloc");
			}

			OS.write(
					"MyLib::",
					mangled,
					"(",
					returnVoid ? "" : "__result, ",
					"@content.to_ptr, ");

			for (auto [info, type] : llvm::zip(argsInfo, argTypes))
			{
				printArg(type, info.getName(), OS);
			}
			OS.writenl(")");

			if (not returnVoid)
			{
				OS.write("return __result", needsUnwrapping(fType) ? ".content" : "");
				if (resultType.isa<mlir::rlc::BoolType>())
					OS.write(" != 0");
			}

			OS.endLine().endLine();
		}
		OS.writenl("end");
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

#define GEN_PASS_DEF_PRINTRUBYPASS
#include "rlc/dialect/Passes.inc"
	/***
	 *This pass emits a python module that describes the content of the rlc
	 *module being compiled. It does the following:
	 * 1 For each ClassType and AlternativeType in the RLC program it emits a
	 *   CType structure or union that with same members, and makes sure that
	 *   constructors, clone and destructor invoke the right method in the RLC
	 *   shared library.
	 *
	 *
	 * 2 For each alias it emits the same alias
	 *
	 * 3 For each free function it emits a ctypes function declaration that
	 *wraps that RLC function, annotated with the real signature of the
	 *function. The emitted function has the same mangled name as the function
	 *in the library.
	 *
	 *   Then, it emits a dispatcher function that accepts a argument list and
	 *   invokes the correct overload according to the types of the variables
	 *the python user has provided.
	 *
	 *   Finally it emits typehinting annotations so that the user can see the
	 *   proper overloads available for a free function
	 *
	 * 4 For each member function it does the same thing that it did in step 3,
	 *   but instead of putting it into the global name space, the generated
	 *stuff is placed inside the class that own the free function
	 *
	 * 5 For each ActionFunction it emits the ActionFunction as a ctypes class,
	 *   and inside that class it emits the ctypes function wrappers to invoke
	 *   the is_done function and actions statements that the ActionFunction
	 *   declares.
	 ***/
	struct PrintRubyPass: impl::PrintRubyPassBase<PrintRubyPass>
	{
		using impl::PrintRubyPassBase<PrintRubyPass>::PrintRubyPassBase;

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

			matcher.add<FunctionToRubyFunction>();
			matcher.add<ActionToRubyFunction>(builder);
			// matcher.add<AliasToRubyAlias>();

			//// emit includes
			printPrelude(
					matcher.getWriter(),
					isMac,
					isWindows,
					getOperation(),
					table,
					builder);

			//// discover all enums
			llvm::StringMap<mlir::rlc::EnumDeclarationOp> enums;
			for (auto op : getOperation().getOps<mlir::rlc::EnumDeclarationOp>())
			{
				enums[op.getName()] = op;
			}

			// emit declarations of types
			for (auto t : ::rlc::postOrderTypes(getOperation()))
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(t))
				{
					emitDeclaration(
							casted,
							matcher.getWriter(),
							table,
							builder,
							enums.count(casted.getName()) ? enums[casted.getName()]
																						: nullptr);
					if (builder.isClassOfAction(casted))
					{
						auto action = mlir::cast<mlir::rlc::ActionFunction>(
								builder.getActionOf(casted).getDefiningOp());
						emitActionFunctions(action, builder, matcher.getWriter());
					}

					matcher.getWriter().writenl("end");
				}
				if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(t))
					emitDeclaration(casted, matcher.getWriter(), table, builder);
			}

			//// emit declarations of free functions
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

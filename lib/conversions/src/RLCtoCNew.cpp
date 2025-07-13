/*
Copyright 2025 Leila Shekofteh

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
#include "llvm/Support/Format.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/IR/Types.h"
#include "mlir/Pass/Pass.h"
#include "rlc/conversions/RLCToC.hpp"
#include "rlc/dialect/MemberFunctionsTable.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/SymbolTable.h"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"
#include "rlc/utils/PatternMatcher.hpp"

namespace mlir::rlc
{

	static void printFieldName(
			llvm::StringRef fieldName,
			mlir::Type type,
			StreamWriter& writer,
			bool isRef = false)
	{
		if (isRef)
			writer.write("(&");
		writer.write(" " + fieldName);
		if (isRef)
			writer.write(")");
		if (auto casted = mlir::dyn_cast<mlir::rlc::ArrayType>(type))
		{
			writer.write("[");
			writer.write(
					casted.getSize().cast<mlir::rlc::IntegerLiteralType>().getValue());
			writer.write("]");
		}
	}

	static void printTypeDefinition(mlir::Type type, StreamWriter& writer)
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
		{
			writer.write("typedef union ");
			writer.write(casted.mangledName());
			writer.writenl("{ ");
			// Defenition
			writer.write("struct _Content");
			writer.write(casted.mangledName());
			writer.writenl("{");
			for (auto field : (casted.getMembers()))
			{
				writer.write("   ");
				writer.writeType(field.getType());
				printFieldName(field.getName(), field.getType(), writer);
				writer.writenl(";");
			}
			writer.write(" ");
			writer.writenl("} content;");
			writer.write("} ");
			writer.writenl(casted.mangledName() + ";");
		}
		if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
		{
			writer.write("struct ");
			writer.write(casted.getMangledName());
			writer.writenl("{");

			// Defenition
			writer.write(" ");
			writer.write("union _Content");
			writer.write(casted.getMangledName());
			writer.writenl("{");
			for (auto field : llvm::enumerate(casted.getUnderlying()))
			{
				writer.write("   ");
				writer.writeType(field.value());
				printFieldName(
						(llvm::Twine("field") + llvm::Twine(field.index())).str(),
						casted,
						writer);

				writer.writenl(";");
			}
			writer.endLine();
			writer.write("  ");
			writer.writenl("} content;");
			writer.write("  ");
			writer.writenl("int64_t active_index;");
			writer.writenl("};");
		}
	}

	static void printPrelude(StreamWriter& writer, mlir::ModuleOp Module)
	{
		// writer.writenl("#ifdef __cplusplus");
		// writer.writenl("extern \"C\" {");
		// writer.writenl("endif");
		writer.writenl("#ifndef RLC_HEADER");
		writer.writenl("#ifdef RLC_C_HEADER");
		writer.writenl("#undef RLC_C_HEADER");
		writer.writenl("#define RLC_HEADER");
		writer.writenl("#include \"stddef.h\"");
		writer.writenl("#include \"stdint.h\"");
		writer.writenl("#define RLC_GET_FUNCTION_DECLS");
		writer.writenl("#define RLC_GET_TYPE_DECLS");
		writer.writenl("#endif");

		writer.writenl("#endif");

		writer.writenl("#ifdef RLC_GET_TYPE_DECLS");

		for (auto type : ::rlc::postOrderTypes(Module))
		{
			printTypeDefinition(type, writer);
		}

		writer.writenl("#undef RLC_GET_TYPE_DECLS");
		writer.writenl("#endif");
		writer.endLine();

		writer.writenl("#ifdef RLC_TYPE");
		for (auto type : ::rlc::postOrderTypes(Module))
			if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
			{
				writer.write("RLC_TYPE(");
				writer.write(casted.getName());
				writer.write(")");
				writer.endLine();
			}
		writer.writenl("#undef RLC_TYPE");
		writer.writenl("#endif");
		writer.endLine();
	}

	static void printTypeField(
			llvm::StringRef fieldName,
			mlir::Type type,
			StreamWriter& w,
			bool isRef = false)
	{
		if (auto casted = dyn_cast<mlir::rlc::FrameType>(type))
			type = casted.getUnderlying();
		else if (auto casted = dyn_cast<mlir::rlc::ContextType>(type))
			type = casted.getUnderlying();

		w.writeType(type);
		printFieldName(fieldName, type, w);
	}

	static void registerCTypeSerialization(TypeSerializer& s)
	{
		s.add([](IntegerType type, llvm::raw_string_ostream& OS) -> void {
			OS << "int" << type.getSize() << "_t";
		});
		s.add([](FloatType type, llvm::raw_string_ostream& OS) -> void {
			OS << "double";
		});
		s.add([](BoolType type, llvm::raw_string_ostream& OS) -> void {
			OS << "bool";
		});
		s.add([](StringLiteralType type, llvm::raw_string_ostream& OS) -> void {
			OS << "char*";
		});
		s.add([](VoidType type, llvm::raw_string_ostream& OS) -> void {
			OS << "void";
		});
		s.add([&](AlternativeType type, llvm::raw_string_ostream& OS) {
			OS << "struct " << type.getMangledName();
		});
		s.add([&](ClassType type, llvm::raw_string_ostream& OS) {
			OS << "union " << type.mangledName();
		});
		s.add([](IntegerLiteralType type, llvm::raw_string_ostream& OS) -> void {
			OS << type.getValue();
		});
		s.add([&](mlir::rlc::FrameType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying());
		});
		s.add([&](mlir::rlc::ContextType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying());
		});
		s.add([&](mlir::rlc::ArrayType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying());
		});
		s.add([&](mlir::rlc::OwningPtrType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying()) << "*";
		});
		s.add([&](mlir::rlc::ReferenceType type, llvm::raw_string_ostream& OS) {
			OS << s.convert(type.getUnderlying()) << "*";
		});
	}

	static void printFunctionDecl(
			llvm::ArrayRef<llvm::StringRef> argsNames,
			TypeRange types,
			mlir::Type resultType,
			StreamWriter& writer)
	{
		writer.write("(");

		if (not mlir::isa<mlir::rlc::VoidType>(resultType))
		{
			writer.writeType(resultType);
			printFieldName(" * __result", resultType, writer);
			if (types.size() != 0)
				writer.write(", ");
		}

		for (size_t index = 0; index != argsNames.size(); index++)
		{
			writer.writeType(types[index]);
			printFieldName("* " + argsNames[index].str(), types[index], writer);
			if (index + 1 < argsNames.size())
				writer.write(", ");
		}
		writer.writenl(");");
	}

	static void declareFunction(
			llvm::StringRef unmangledName,
			bool isMemberFunction,
			llvm::StringRef cShortName,
			mlir::FunctionType type,
			llvm::ArrayRef<llvm::StringRef> argsNames,
			StreamWriter& writer)
	{
		std::string mangledName =
				mlir::rlc::mangledName(unmangledName, isMemberFunction, type);

		mlir::Type resultType = type.getNumResults() == 0
																? mlir::rlc::VoidType::get(type.getContext())
																: type.getResult(0);

		writer.writenl("#ifdef RLC_GET_FUNCTION_DECLS");
		writer.write("void ");
		writer.write(mangledName);
		printFunctionDecl(argsNames, type.getInputs(), resultType, writer);
		writer.writenl("#endif");
		writer.endLine();
	}

	static void printFunctionAndCanFunctionSignature(
			llvm::StringRef UnmangledName,
			bool isMemberFunction,
			llvm::StringRef cShortName,
			mlir::FunctionType type,
			llvm::ArrayRef<llvm::StringRef> argNames,
			bool preconditionExist,
			StreamWriter& writer)
	{
		declareFunction(
				UnmangledName, isMemberFunction, cShortName, type, argNames, writer);

		auto canDoType = mlir::FunctionType::get(
				type.getContext(),
				type.getInputs(),
				{ mlir::rlc::BoolType::get(type.getContext()) });

		if (preconditionExist)
			declareFunction(
					"can_" + UnmangledName.str(),
					isMemberFunction,
					"can_" + cShortName.str(),
					canDoType,
					argNames,
					writer);
	}

	class FunctionToCFunction
	{
		public:
		void apply(FunctionOp op, StreamWriter& w)
		{
			if (op.isInternal())
				return;
			if (op.getUnmangledName() == "main")
				return;
			std::string cShortName =
					((not op.getFunctionType().getInputs().empty() and
						op.getFunctionType()
								.getInputs()
								.front()
								.isa<mlir::rlc::ClassType>())
							 ? op.getFunctionType()
												 .getInputs()
												 .front()
												 .cast<mlir::rlc::ClassType>()
												 .getName() +
										 "_" + op.getUnmangledName()
							 : op.getUnmangledName())
							.str();

			printFunctionAndCanFunctionSignature(
					op.getUnmangledName(),
					op.getIsMemberFunction(),
					cShortName,
					op.getFunctionType(),
					op.getInfo().getArgNames(),
					!op.getPrecondition().empty(),
					w);
		}
	};

	class ActionToCFunction
	{
		private:
		mlir::rlc::ModuleBuilder& builder;

		public:
		ActionToCFunction(mlir::rlc::ModuleBuilder& builder): builder(builder) {}
		void apply(mlir::rlc::ActionFunction op, mlir::rlc::StreamWriter& w)
		{
			std::string cShortName =
					(op.getClassType().getName() + "_" + op.getUnmangledName()).str();

			printFunctionAndCanFunctionSignature(
					op.getUnmangledName(),
					false,
					cShortName,
					op.getFunctionType(),
					op.getInfo().getArgNames(),
					!op.getPrecondition().empty(),
					w);

			for (const auto& value : op.getActions())
			{
				mlir::Operation* statement =
						builder.actionFunctionValueToActionStatement(value).front();

				auto actionStatement =
						mlir::cast<mlir::rlc::ActionStatement>(statement);

				llvm::SmallVector<llvm::StringRef> argNames = { "self" };
				for (auto arg : actionStatement.getDeclaredNames())
					argNames.push_back(arg);

				auto functionType = mlir::cast<mlir::FunctionType>(value.getType());

				std::string cShortName =
						(op.getClassType().getName() + "_" + actionStatement.getName())
								.str();

				printFunctionAndCanFunctionSignature(
						actionStatement.getName(),
						true,
						cShortName,
						functionType,
						argNames,
						true,
						w);
			}

			cShortName = (op.getClassType().getName() + "_is_done").str();

			printFunctionAndCanFunctionSignature(
					"is_done",
					true,
					cShortName,
					op.getIsDoneFunctionType(),
					{ "arg0" },
					false,
					w);

			w.writenl("#ifdef RLC_GET_FUNCTION_DECLS");
			w.writenl("#undef RLC_GET_FUNCTION_DECLS");
			w.writenl("#endif");
			w.endLine();

			w.writenl("#ifdef RLC_VISIT_FUNCTION");
			w.writenl("#undef RLC_VISIT_FUNCTION");
			w.writenl("#endif");
		}
	};

	class AliasToCAlias
	{
		public:
		void apply(mlir::rlc::TypeAliasOp op, mlir::rlc::StreamWriter& w)
		{
			w.write("typedef ");
			w.writeType(op.getAliased());
			w.write(" ");
			w.write(op.getName());
			w.writenl(";");
		}
	};

#define GEN_PASS_DEF_PRINTNEWCHEADERPASS
#include "rlc/dialect/Passes.inc"

	struct PrintNewCHeaderPass: impl::PrintNewCHeaderPassBase<PrintNewCHeaderPass>
	{
		using impl::PrintNewCHeaderPassBase<
				PrintNewCHeaderPass>::PrintNewCHeaderPassBase;
		void runOnOperation() override
		{
			rlc::PatternMatcher matcher(*OS);
			MemberFunctionsTable table(getOperation());
			mlir::rlc::ModuleBuilder builder(getOperation());
			mlir::ModuleOp module;
			matcher.addTypeSerializer();
			registerCTypeSerialization(matcher.getWriter().getTypeSerializer());

			matcher.add<FunctionToCFunction>();
			matcher.add<ActionToCFunction>(builder);
			matcher.add<AliasToCAlias>();

			printPrelude(matcher.getWriter(), getOperation());

			// emit declarations of functions
			matcher.apply(getOperation());
		}
	};

}	 // namespace mlir::rlc

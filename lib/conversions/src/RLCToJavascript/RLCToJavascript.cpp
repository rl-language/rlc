#include "JavascriptWrapperStandard.h"
#include "clang/Format/Format.h"
#include "clang/Tooling/Core/Replacement.h"
#include "llvm/Support/Error.h"
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

// TODO: Come gestire i varargs?
// TODO: Come gestire gli alias?

namespace mlir::rlc
{

#define GEN_PASS_DEF_PRINTJAVASCRIPTPASS
#include "rlc/dialect/Passes.inc"

	namespace
	{
		class JavascriptCodeGenerator
		{
			private:
			std::string unformattedBuffer{};
			llvm::raw_string_ostream printer{ unformattedBuffer };
			bool isWasm64{};
			llvm::raw_ostream* OS{};

			public:
			JavascriptCodeGenerator(bool isWasm64, llvm::raw_ostream* OS)
					: isWasm64(isWasm64), OS(OS)
			{
				if (this->isWasm64)
				{
					// Rimuovi questo if
				}
			}

			void autoIndentMyCode()
			{
				printer.flush();

				clang::format::FormatStyle style = clang::format::getLLVMStyle();
				style.Language = clang::format::FormatStyle::LK_JavaScript;

				std::vector<clang::tooling::Range> ranges{
					1, clang::tooling::Range(0, unformattedBuffer.size())
				};

				clang::tooling::Replacements replaces = clang::format::reformat(
						style, unformattedBuffer, ranges, "youCanInsertAnythingHere.js");

				auto formattedCode =
						clang::tooling::applyAllReplacements(unformattedBuffer, replaces);

				if (formattedCode)
				{
					*OS << (*formattedCode);
				}
				else
				{
					llvm::consumeError(formattedCode.takeError());
					*OS << unformattedBuffer;
				}
			}

			void generateGeneralWrapper() { printer << standardJavascriptWrapper; }

			void emitJavascriptType(mlir::Type type)
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::IntegerType>(type))
				{
					if (casted.getSize() == 64)
					{
						printer << "IntWrapper";
					}
					else
					{
						printer << "ByteWrapper";
					}
				}
				else if (mlir::isa<mlir::rlc::FloatType>(type))
				{
					printer << "FloatWrapper";
				}
				else if (mlir::isa<mlir::rlc::BoolType>(type))
				{
					printer << "BoolWrapper";
				}
				else if (mlir::isa<mlir::rlc::StringLiteralType>(type))
				{
					printer << "StringLiteralWrapper";
				}
				else if (mlir::isa<mlir::rlc::VoidType>(type))
				{
					printer << "null";
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
				{
					printer << casted.getName();
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
				{
					// TODO:
					printer << "";
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ArrayType>(type))
				{
					printer << "Array_";
					emitJavascriptType(casted.getUnderlying());
					printer << "_" << casted.getSize();
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::OwningPtrType>(type))
				{
					printer << "Ptr_";
					emitJavascriptType(casted.getUnderlying());
				}
				else if (auto casted = mlir::dyn_cast<mlir::rlc::ReferenceType>(type))
				{
					emitJavascriptType(casted.getUnderlying());
				}
				else
				{
					std::abort();
				}
			}

			void emitOverloadDispatcher(
					llvm::StringRef name,
					llvm::ArrayRef<mlir::FunctionType> overloads,
					bool isMethod)
			{
				if (isMethod == false)
				{
					printer << "function ";
				}
				printer << name << "(...args){";

				if (isMethod)
				{
					printer << "this._assertAddress();";
				}

				printer << "const signatures = [";

				for (auto overload : overloads)
				{
					printer << "[\"" << mangledName(name, isMethod, overload) << "\", [";

					llvm::SmallVector<std::string, 4> vector{};
					for (auto type : overload.getInputs())
					{
						emitJavascriptType(type);
						printer << ", ";
					}

					mlir::Type result{};
					if (overload.getNumResults() == 0)
					{
						result = mlir::rlc::VoidType::get(overload.getContext());
					}
					else
					{
						result = overload.getResults()[0];
					}
					printer << "], ";
					emitJavascriptType(result);
					printer << ", ";
					if (mlir::dyn_cast<mlir::rlc::ReferenceType>(result))
					{
						printer << "true";
					}
					else
					{
						printer << "false";
					}
					printer << "], ";
				}
				printer << "];";

				if (isMethod)
				{
					printer << "args.unshift(this);";
				}

				printer << "return generalFunction(args, signatures);}";
			}

			void emitGettersAndSetters(mlir::rlc::ClassType classType)
			{
				for (auto [type, name] :
						 llvm::zip(classType.getMemberTypes(), classType.getMemberNames()))
				{
					printer << "get " << name << "(){return this._get(";
					emitJavascriptType(type);
					printer << ", TODO);}";

					printer << "set " << name << "(value){this._set(";
					emitJavascriptType(type);
					printer << ", TODO, value);}";
				}
			}

			// TODO: Rendere MemberFunctionsTable un member field?
			void emitMemberFunctions(
					mlir::Type classType, MemberFunctionsTable& table)
			{
				llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
				for (auto memberFunction : table.getMemberFunctionsOf(classType))
				{
					sortedOverloads[memberFunction.getUnmangledName()].push_back(
							memberFunction.getType());
				}

				if (not table.isTriviallyInitializable(classType))
				{
					auto fun = mlir::FunctionType::get(
							classType.getContext(), { classType }, {});
					sortedOverloads["init"].push_back(fun);
				}

				if (not table.isTriviallyDestructible(classType))
				{
					auto fun = mlir::FunctionType::get(
							classType.getContext(), { classType }, {});
					sortedOverloads["drop"].push_back(fun);
				}

				if (not table.isTriviallyCopiable(classType))
				{
					auto fun = mlir::FunctionType::get(
							classType.getContext(), { classType, classType }, {});
					sortedOverloads["assign"].push_back(fun);
				}

				for (auto& pair : sortedOverloads)
				{
					emitOverloadDispatcher(pair.first(), pair.second, true);
				}
			}

			void emitClassDeclaration(
					mlir::rlc::ClassType type, MemberFunctionsTable& table)
			{
				printer << "class " << type.getName() << " extends ClassWrapper{";
				printer << "static _getSize() { return TODO;}";
				printer << "static _getMemberFieldNames(){ return [";
				for (auto name : type.getMemberNames())
				{
					printer << "\"" << name << "\", ";
				}

				printer << "]; }";
				emitGettersAndSetters(type);
				emitMemberFunctions(type, table);
				printer << "}";
			}

			void emitEnumDeclaration(
				mlir::rlc::ClassType type,
					mlir::rlc::EnumDeclarationOp enumDecl, MemberFunctionsTable& table)
			{
				printer << "enum " << type.getName() << " extends EnumWrapper{";
				int i{ 0 };
				for (auto value :
						 llvm::enumerate(enumDecl.getBody()
																 .getOps<mlir::rlc::EnumFieldDeclarationOp>()))
				{
					printer << "static " << value.value().getName() << " = " << i << ";";
					i++;
				}

				emitMemberFunctions(type, table);
				printer << "}";
			}
		};

	}	 // namespace

	struct PrintJavascriptPass: impl::PrintJavascriptPassBase<PrintJavascriptPass>
	{
		using impl::PrintJavascriptPassBase<
				PrintJavascriptPass>::PrintJavascriptPassBase;

		void runOnOperation() override
		{
			MemberFunctionsTable table(this->getOperation());
			mlir::rlc::ModuleBuilder builder(this->getOperation());

			// TODO: Passare table e builder qui?
			JavascriptCodeGenerator codeGenerator{ this->isWasm64, this->OS };

			codeGenerator.generateGeneralWrapper();

			llvm::StringMap<mlir::rlc::EnumDeclarationOp> enums;
			for (auto op : getOperation().getOps<mlir::rlc::EnumDeclarationOp>())
			{
				enums[op.getName()] = op;
			}

			for (auto t : ::rlc::postOrderTypes(this->getOperation()))
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(t))
				{
					if (enums.count(casted.getName()) == 0)
					{
						codeGenerator.emitClassDeclaration(casted, table);
					}
					else
					{
						codeGenerator.emitEnumDeclaration(casted, enums[casted.getName()], table);
					}
				}
			}

			codeGenerator.autoIndentMyCode();
		}
	};
}	 // namespace mlir::rlc

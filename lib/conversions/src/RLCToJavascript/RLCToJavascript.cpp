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

// TODO: Come gestire i arargs?
// TODO: Come gestire gli alias?

namespace mlir::rlc
{

#define GEN_PASS_DEF_PRINTJAVASCRIPTPASS
#include "rlc/dialect/Passes.inc"

	// TODO: Togliere static da unformattedByffer e da printer

	static std::string unformattedBuffer{};
	static llvm::raw_string_ostream printer{ unformattedBuffer };

	static std::string autoIndentMyCode()
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
			return *formattedCode;
		}
		else
		{
			llvm::consumeError(formattedCode.takeError());
			return unformattedBuffer;
		}
	}

	static void generateGeneralWrapper() { printer << standardJavascriptWrapper; }

	static void emitJavascriptType(mlir::Type type)
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
	}

	static void emitOverloadDispatcher(
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

			const auto result = overload.getResults()[0];
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

	static void emitGettersAndSetters(mlir::rlc::ClassType type)
	{
		// TODO
	}

	static void emitMemberFunctions(mlir::Type type, MemberFunctionsTable& table)
	{
		llvm::StringMap<llvm::SmallVector<mlir::FunctionType>> sortedOverloads;
		for (auto memberFunction : table.getMemberFunctionsOf(type))
		{
			sortedOverloads[memberFunction.getUnmangledName()].push_back(
					memberFunction.getType());
		}

		for (auto& pair : sortedOverloads)
		{
			emitOverloadDispatcher(pair.first(), pair.second, true);
		}

		printer << "}";
	}

	static void emitClassDeclaration(
			mlir::rlc::ClassType type,
			MemberFunctionsTable& table,
			mlir::rlc::ModuleBuilder& builder)
	{
		// TODO: Il parametro "builder" a che serve?

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
	}

	struct PrintJavascriptPass: impl::PrintJavascriptPassBase<PrintJavascriptPass>
	{
		using impl::PrintJavascriptPassBase<
				PrintJavascriptPass>::PrintJavascriptPassBase;

		void runOnOperation() override
		{
			MemberFunctionsTable table(getOperation());
			mlir::rlc::ModuleBuilder builder(getOperation());

			generateGeneralWrapper();

			for (auto t : ::rlc::postOrderTypes(getOperation()))
			{
				if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(t))
				{
					emitClassDeclaration(casted, table, builder);
				}
			}

			(*OS) << autoIndentMyCode();
		}
	};
}	 // namespace mlir::rlc

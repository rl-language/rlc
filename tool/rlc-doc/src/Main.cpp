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

#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/ToolOutputFile.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/BuiltinOps.h"
#include "rlc/parser/Parser.hpp"

#define DEBUG_TYPE "rlc-lsp-server"

using namespace llvm;

static llvm::cl::OptionCategory Category("rlc-option options");

static cl::opt<std::string> InputFilePath(
		cl::Positional, cl::desc("<input-file>"), cl::init("-"), cl::cat(Category));

static cl::opt<bool> createDirectories(
		"create-dirs",
		cl::desc("create the non existing directories to the output files"),
		cl::init(false),
		cl::cat(Category));

static cl::opt<std::string> OutputFilePath(
		"o", cl::desc("<output-file>"), cl::init("-"), cl::cat(Category));

/// Print a template parameter inside back‑ticks.
static void printTemplateParameter(mlir::Attribute attr, llvm::raw_ostream &OS)
{
	auto param = attr.cast<mlir::TypeAttr>()
									 .getValue()
									 .cast<mlir::rlc::UncheckedTemplateParameterType>();
	OS << param.getName().str();
	if (!param.getTrait().str().empty())
	{
		OS << " : " << param.getTrait().str();
	}
}

/// Emit a fenced Markdown code‑block for an operation comment, if present.
static void writeComment(mlir::Operation *op, llvm::raw_ostream &OS)
{
	if (op->hasAttr("comment"))
	{
		auto comment = op->getAttr("comment").cast<mlir::StringAttr>();
		OS << "\n```text\n" << comment.strref() << "\n```\n\n";
	}
}

static void printFunction(mlir::rlc::FunctionOp op, llvm::raw_ostream &OS)
{
	// Skip internal functions
	if (op.isInternal())
		return;

	OS << "- **Function**: `" << op.getUnmangledName();

	// Handle template parameters if present
	if (!op.getTemplateParameters().empty())
	{
		OS << "<";
		for (auto &name : llvm::drop_end(op.getTemplateParameters()))
		{
			printTemplateParameter(name, OS);
			OS << ", ";
		}
		printTemplateParameter(*(op.getTemplateParameters().end() - 1), OS);
		OS << ">";
	}

	OS << mlir::rlc::prettyPrintFunctionTypeWithNameArgs(
						op.getType(), op.getInfo())
		 << "`\n";
	writeComment(op, OS);
}

static void printActionFunction(
		mlir::rlc::ActionFunction op, llvm::raw_ostream &OS)
{
	// Print action function signature
	OS << "- **Action**: `" << op.getUnmangledName()
		 << mlir::rlc::prettyType(op.getType()) << "`\n";
	writeComment(op, OS);

	// Walk through action statements
	op.walk([&](mlir::rlc::ActionStatement action) {
		OS << "  - **Action Statement**: `" << action.getName() << "(";
		for (size_t i = 0; i < action.getResultTypes().size(); ++i)
		{
			OS << mlir::rlc::prettyType(action.getResultTypes()[i]) << " "
				 << action.getDeclaredNames()[i];
			if (i < action.getResultTypes().size() - 1)
				OS << ", ";
		}
		OS << ")`\n";
		writeComment(action, OS);
	});
}

static void printEnumDecl(
		mlir::rlc::EnumDeclarationOp op, llvm::raw_ostream &OS)
{
	OS << "## Enum " << op.getName() << "\n\n";
	writeComment(op, OS);

	// Print enum fields with their types (if any expression gives more details)
	op.walk([&](mlir::rlc::EnumFieldDeclarationOp field) -> mlir::WalkResult {
		field.walk([&](mlir::rlc::EnumFieldExpressionOp exp) {
			OS << "- `" << mlir::rlc::prettyType(exp.getResult().getType()) << " "
				 << exp.getName() << "`\n";
		});
		// Interrupt after first level
		return mlir::WalkResult::interrupt();
	});

	// List plain field names
	for (auto &innerOp : op.getBody().front())
	{
		auto field = mlir::dyn_cast<mlir::rlc::EnumFieldDeclarationOp>(innerOp);
		if (!field)
			continue;
		OS << "- `" << field.getName() << "`\n";
	}
	OS << "\n";
}

static void printClassDecl(
		mlir::rlc::ClassDeclaration op, llvm::raw_ostream &OS)
{
	OS << "## Class " << op.getName() << "\n\n";
	writeComment(op, OS);

	// Fields
	if (!op.getMembers().empty())
	{
		OS << "### Fields\n";
		for (size_t i = 0; i < op.getMembers().size(); ++i)
		{
			auto name = op.getMemberField(i).getName();
			// Skip fields starting with underscore
			if (name.starts_with("_"))
				continue;
			OS << "- `"
				 << mlir::rlc::prettyType(
								op.getMemberField(i).getShugarizedType().getType())
				 << " " << name.str() << "`\n";
		}
		OS << "\n";
	}

	// Methods
	auto methodOps = op.getBody().getOps<mlir::rlc::FunctionOp>();
	if (!methodOps.empty())
	{
		OS << "### Methods\n";
		for (auto member : methodOps)
		{
			printFunction(member, OS);
		}
		OS << "\n";
	}
}

static void printTrait(
		mlir::rlc::UncheckedTraitDefinition op, llvm::raw_ostream &OS)
{
	OS << "## Trait " << op.getName() << "\n\n";
	writeComment(op, OS);

	auto funcs = op.getBody().getOps<mlir::rlc::FunctionOp>();
	for (auto member : funcs)
	{
		printFunction(member, OS);
	}
	OS << "\n";
}

int main(int argc, char *argv[])
{
	llvm::cl::HideUnrelatedOptions(Category);
	cl::ParseCommandLineOptions(argc, argv);

	mlir::MLIRContext context(mlir::MLIRContext::Threading::DISABLED);
	mlir::DialectRegistry registry;
	registry.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
	context.appendDialectRegistry(registry);
	context.loadAllAvailableDialects();

	auto buffer = llvm::MemoryBuffer::getFileOrSTDIN(InputFilePath);
	if (!buffer)
	{
		errs() << "failed to open " << InputFilePath << "\n";
		return -1;
	}

	rlc::Parser parser(
			&context,
			buffer.get()->getBuffer().str(),
			InputFilePath,
			/*silent=*/true);

	auto ast = mlir::ModuleOp::create(
			mlir::FileLineColLoc::get(&context, InputFilePath, 0, 0), InputFilePath);
	auto maybeAst = parser.system(ast);
	if (!maybeAst)
	{
		errs() << "failed to parse " << InputFilePath << "\n";
		return -1;
	}

	auto pathToDir = llvm::sys::path::parent_path(OutputFilePath);
	if (createDirectories && OutputFilePath != "-")
	{
		auto error_code = llvm::sys::fs::create_directories(pathToDir);
		if (error_code)
		{
			ExitOnError exitOnError;
			exitOnError(llvm::errorCodeToError(error_code));
			return -1;
		}
	}

	std::error_code EC;
	llvm::ToolOutputFile output(OutputFilePath, EC, llvm::sys::fs::OF_None);
	auto &OS = output.os();

	// Main heading
	OS << "# " << llvm::sys::path::filename(InputFilePath) << "\n\n";

	// Classes
	for (auto op : ast.getOps<mlir::rlc::ClassDeclaration>())
	{
		printClassDecl(op, OS);
	}

	// Actions
	auto actionFuncs = ast.getOps<mlir::rlc::ActionFunction>();
	if (!actionFuncs.empty())
	{
		OS << "## Actions\n\n";
		for (auto op : actionFuncs)
		{
			printActionFunction(op, OS);
		}
		OS << "\n";
	}

	// Free functions
	auto freeFuncs = ast.getOps<mlir::rlc::FunctionOp>();
	if (!freeFuncs.empty())
	{
		OS << "## Free Functions\n\n";
		for (auto op : freeFuncs)
		{
			printFunction(op, OS);
		}
		OS << "\n";
	}

	// Traits
	auto traits = ast.getOps<mlir::rlc::UncheckedTraitDefinition>();
	if (!traits.empty())
	{
		OS << "## Traits\n\n";
		for (auto op : traits)
		{
			printTrait(op, OS);
		}
		OS << "\n";
	}

	// Enums
	auto enums = ast.getOps<mlir::rlc::EnumDeclarationOp>();
	if (!enums.empty())
	{
		OS << "## Enums\n\n";
		for (auto op : enums)
		{
			printEnumDecl(op, OS);
		}
		OS << "\n";
	}

	output.keep();
	return 0;
}

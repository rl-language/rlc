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

static void printTemplateParameter(mlir::Attribute attr, llvm::raw_ostream& OS)
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

static void writeComment(mlir::Operation* op, llvm::raw_ostream& OS)
{
	if (op->hasAttr("comment"))
	{
		auto comment = op->getAttr("comment").cast<mlir::StringAttr>();
		OS << "<pre><code>\n" << comment.strref() << "\n</code></pre>\n";
	}
}

static void printFunction(mlir::rlc::FunctionOp op, llvm::raw_ostream& OS)
{
	// Skip internal functions
	if (op.isInternal())
		return;

	OS << "<li><strong>Function</strong>: <code>" << op.getUnmangledName();

	// Handle template parameters if present
	if (!op.getTemplateParameters().empty())
	{
		OS << "&lt;";
		for (auto& name : llvm::drop_end(op.getTemplateParameters()))
		{
			printTemplateParameter(name, OS);
			OS << ", ";
		}
		printTemplateParameter(*(op.getTemplateParameters().end() - 1), OS);
		OS << "&gt;";
	}

	OS << mlir::rlc::prettyPrintFunctionTypeWithNameArgs(
						op.getType(), op.getInfo())
		 << "</code></li>\n";
	writeComment(op, OS);
}

static void printActionFuntion(
		mlir::rlc::ActionFunction op, llvm::raw_ostream& OS)
{
	// Print action function signature
	OS << "<div><strong>Action</strong>: <code>" << op.getUnmangledName()
		 << mlir::rlc::prettyType(op.getType()) << "</code></div>\n";
	writeComment(op, OS);

	// Walk through action statements
	op.walk([&](mlir::rlc::ActionStatement action) {
		OS << "<ul>\n";
		OS << "  <li><strong>Action Statement</strong>: <code>" << action.getName()
			 << "(";
		for (size_t i = 0; i < action.getResultTypes().size(); ++i)
		{
			OS << mlir::rlc::prettyType(action.getResultTypes()[i]) << " "
				 << action.getDeclaredNames()[i];
			if (i < action.getResultTypes().size() - 1)
				OS << ", ";
		}
		OS << ")</code></li>\n";
		writeComment(action, OS);
		OS << "</ul>\n";
	});
}

static void printEnumDecl(
		mlir::rlc::EnumDeclarationOp op, llvm::raw_ostream& OS)
{
	OS << "<h2>Enum " << op.getName() << "</h2>\n";
	writeComment(op, OS);

	op.walk([&](mlir::rlc::EnumFieldDeclarationOp field) -> mlir::WalkResult {
		OS << "<ul>\n";
		field.walk([&](mlir::rlc::EnumFieldExpressionOp exp) {
			OS << "  <li><code>" << mlir::rlc::prettyType(exp.getResult().getType())
				 << " " << exp.getName() << "</code></li>\n";
		});
		OS << "</ul>\n";
		return mlir::WalkResult::interrupt();
	});
	OS << "<br/>\n";
	OS << "<ul>\n";
	// Fields
	for (auto& op : op.getBody().front())
	{
		auto casted = mlir::dyn_cast<mlir::rlc::EnumFieldDeclarationOp>(op);
		if (not casted)
			continue;

		OS << "  <li><code>" << casted.getName() << "</code></li>\n";
	}
	OS << "</ul>\n";
}

static void printClassDecl(
		mlir::rlc::ClassDeclaration op, llvm::raw_ostream& OS)
{
	OS << "<h2>Class " << op.getName() << "</h2>\n";
	writeComment(op, OS);

	// Fields
	if (!op.getMembers().empty())
		OS << "<h3>Fields</h3>\n<ul>\n";
	for (size_t i = 0; i < op.getMembers().size(); ++i)
	{
		auto name = op.getMemberField(i).getName();
		// Skip fields starting with underscore
		if (name.starts_with("_"))
			continue;

		OS << "  <li><code>"
			 << mlir::rlc::prettyType(
							op.getMemberField(i).getShugarizedType().getType())
			 << " " << name.str() << "</code></li>\n";
	}
	if (!op.getMembers().empty())
		OS << "</ul>\n";

	// Methods
	auto methodOps = op.getBody().getOps<mlir::rlc::FunctionOp>();
	if (!methodOps.empty())
	{
		OS << "<h3>Methods</h3>\n<ul>\n";
		for (auto member : methodOps)
		{
			printFunction(member, OS);
		}
		OS << "</ul>\n";
	}
}

static void printTrait(
		mlir::rlc::UncheckedTraitDefinition op, llvm::raw_ostream& OS)
{
	OS << "<h2>Trait " << op.getName() << "</h2>\n";
	writeComment(op, OS);

	auto funcs = op.getBody().getOps<mlir::rlc::FunctionOp>();
	if (!funcs.empty())
	{
		OS << "<ul>\n";
		for (auto member : funcs)
		{
			printFunction(member, OS);
		}
		OS << "</ul>\n";
	}
}

int main(int argc, char* argv[])
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
			&context, buffer.get()->getBuffer().str(), InputFilePath, true);

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
	llvm::ToolOutputFile output(
			OutputFilePath, EC, llvm::sys::fs::OpenFlags::OF_None);
	auto& OS = output.os();

	// Main heading
	OS << "<!DOCTYPE html>\n<html>\n<head>\n"
		 << "  <meta charset=\"utf-8\">\n"
		 << "  <title>Documentation - " << llvm::sys::path::filename(InputFilePath)
		 << "</title>\n</head>\n<body>\n";

	OS << "<h1>" << llvm::sys::path::filename(InputFilePath) << "</h1>\n";

	// Classes
	for (auto op : ast.getOps<mlir::rlc::ClassDeclaration>())
	{
		printClassDecl(op, OS);
	}

	// Actions
	auto actionFuncs = ast.getOps<mlir::rlc::ActionFunction>();
	if (!actionFuncs.empty())
	{
		OS << "<h2>Actions</h2>\n";
		for (auto op : actionFuncs)
		{
			printActionFuntion(op, OS);
		}
	}

	// Free functions
	auto freeFuncs = ast.getOps<mlir::rlc::FunctionOp>();
	// Filter out internal functions. printFunction() does that check internally.
	if (!freeFuncs.empty())
	{
		OS << "<h2>Free Functions</h2>\n<ul>\n";
		for (auto op : freeFuncs)
		{
			printFunction(op, OS);
		}
		OS << "</ul>\n";
	}

	// Traits
	auto traits = ast.getOps<mlir::rlc::UncheckedTraitDefinition>();
	if (!traits.empty())
	{
		OS << "<h2>Traits</h2>\n";
		for (auto op : traits)
		{
			printTrait(op, OS);
		}
	}

	auto enums = ast.getOps<mlir::rlc::EnumDeclarationOp>();
	if (!enums.empty())
	{
		OS << "<h2>Enums</h2>\n";
		for (auto op : enums)
		{
			printEnumDecl(op, OS);
		}
	}

	OS << "</body>\n</html>\n";

	output.keep();
	return 0;
}

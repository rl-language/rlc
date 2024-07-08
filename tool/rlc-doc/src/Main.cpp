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
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ToolOutputFile.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/BuiltinOps.h"
#include "rlc/parser/Parser.hpp"
#include "string"

#define DEBUG_TYPE "rlc-lsp-server"

using namespace llvm;

static llvm::cl::OptionCategory Category("rlc-option options");

static cl::opt<std::string> InputFilePath(
		cl::Positional, cl::desc("<input-file>"), cl::init("-"), cl::cat(Category));

static cl::opt<std::string> OutputFilePath(
		"o", cl::desc("<output-file>"), cl::init("-"), cl::cat(Category));

static void printTemplateParameter(mlir::Attribute attr, llvm::raw_ostream& OS)
{
	auto param = attr.cast<mlir::TypeAttr>()
									 .getValue()
									 .cast<mlir::rlc::UncheckedTemplateParameterType>();
	OS << param.getName();
	if (not param.getTrait().str().empty())
	{
		OS << " : " << param.getTrait().str();
	}
}

static void writeComment(mlir::Operation* op, llvm::raw_ostream& OS)
{
	if (op->hasAttr("comment"))
	{
		auto comment = op->getAttr("comment").cast<mlir::StringAttr>();
		OS << ">" << comment.strref();
	}
}

static void printFunction(mlir::rlc::FunctionOp op, llvm::raw_ostream& OS)
{
	if (op.isInternal())
		return;

	OS << "* fun `" << op.getUnmangledName();
	if (op.getTemplateParameters().size() != 0)
	{
		OS << "<";
		for (auto& name : llvm::drop_end(op.getTemplateParameters()))
		{
			printTemplateParameter(name, OS);
			OS << ", ";
		}
		printTemplateParameter(*(op.getTemplateParameters().end() - 1), OS);
		OS << ">";
	}
	OS << mlir::rlc::prettyType(op.getType()) << "`\n";
	writeComment(op, OS);
}

static void printActionFuntion(
		mlir::rlc::ActionFunction op, llvm::raw_ostream& OS)
{
	OS << "* act `" << op.getUnmangledName();
	OS << mlir::rlc::prettyType(op.getType()) << "`\n";
	writeComment(op, OS);
	op.walk([&](mlir::rlc::ActionStatement action) {
		OS << "   * act `" << action.getName() << "(";
		for (size_t i = 0; i != action.getResultTypes().size(); i++)
		{
			OS << mlir::rlc::prettyType(action.getResultTypes()[i]);
			OS << " "
				 << action.getDeclaredNames()[i].cast<mlir::StringAttr>().getValue();
			if (i != action.getResultTypes().size() - 1)
				OS << ", ";
		}
		OS << ")`\n";
		writeComment(action, OS);
	});
}

static void printClassDecl(
		mlir::rlc::ClassDeclaration op, llvm::raw_ostream& OS)
{
	OS << "## cls " << op.getName() << "\n";
	writeComment(op, OS);

	OS << "### Fields\n\n";
	for (size_t i = 0; i != op.getMemberNames().size(); i++)
	{
		auto name = op.getMemberNames()[i].cast<mlir::StringAttr>();
		if (name.strref().starts_with("_"))
			continue;

		OS << "* "
			 << mlir::rlc::prettyType(
							op.getMemberTypes()[i].cast<mlir::TypeAttr>().getValue())
			 << " " << name.str() << "\n";
	}

	OS << "\n### Methods\n\n";
	for (auto& member : op.getBody().getOps())
	{
		if (auto op = mlir::dyn_cast<mlir::rlc::FunctionOp>(member))
			printFunction(op, OS);
	}
	OS << "\n";
}

int main(int argc, char* argv[])
{
	llvm::cl::HideUnrelatedOptions(Category);
	cl::ParseCommandLineOptions(argc, argv);

	mlir::MLIRContext context(mlir::MLIRContext::Threading::DISABLED);
	mlir::DialectRegistry Registry;
	Registry.insert<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
	context.appendDialectRegistry(Registry);
	context.loadAllAvailableDialects();
	auto buffer = llvm::MemoryBuffer::getFileOrSTDIN(InputFilePath);
	if (not buffer)
	{
		errs() << "failed to open " << InputFilePath << "\n";
		return -1;
	}

	rlc::Parser parser(
			&context, buffer.get().get()->getBuffer().str(), InputFilePath, true);

	auto ast = mlir::ModuleOp::create(
			mlir::FileLineColLoc::get(&context, InputFilePath, 0, 0), InputFilePath);
	auto maybeAst = parser.system(ast);
	if (not maybeAst)
	{
		errs() << "failed to parse" << InputFilePath << "\n";
		return -1;
	}
	std::error_code EC;
	llvm::ToolOutputFile output(
			OutputFilePath, EC, llvm::sys::fs::OpenFlags::OF_None);
	auto& OS = output.os();
	OS << "# " << InputFilePath << "\n\n";
	OS << "### Classes \n\n";
	for (auto op : ast.getOps<mlir::rlc::ClassDeclaration>())

	{
		printClassDecl(op, OS);
	}

	OS << "### Actions \n\n";

	for (auto op : ast.getOps<mlir::rlc::ActionFunction>())

	{
		printActionFuntion(op, OS);
	}

	OS << "### Free functions\n\n";

	for (auto op : ast.getOps<mlir::rlc::FunctionOp>())
	{
		printFunction(op, OS);
	}
	output.keep();
	return 0;
}

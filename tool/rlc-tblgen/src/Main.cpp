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
#include "llvm/TableGen/Record.h"
#include "mlir/TableGen/GenInfo.h"
#include "mlir/Tools/mlir-tblgen/MlirTblgenMain.h"

using namespace llvm;
using namespace std;

using namespace llvm;
using namespace mlir;

static std::string toCamelCase(const llvm::StringRef &input)
{
	std::string result;
	bool capitalizeNext = false;

	for (char ch : input)
	{
		if (std::isspace(ch) || ch == '_' || ch == '-')
		{
			capitalizeNext = true;
		}
		else if (capitalizeNext)
		{
			result += std::toupper(ch);
			capitalizeNext = false;
		}
		else
		{
			result += ch;
		}
	}
	result[0] = std::toupper(result[0]);

	return result;
}

static bool isTypeContraint(llvm::DefInit *init)
{
	for (auto c : init->getDef()->getSuperClasses())
	{
		if (c.first->getName() == "TypeConstraint")
		{
			return true;
		}
	}
	return false;
}

static void printArgType(mlir::raw_ostream &os, Init *init, bool isOpArgument)
{
	if (DefInit *defInit = dyn_cast<DefInit>(init))
	{
		auto record = defInit->getDef();
		if (isOpArgument and isTypeContraint(defInit))
		{
			os << "mlir::Value";
		}
		else if (auto storageType = record->getValue("type"))
		{
			os << cast<llvm::StringInit>(storageType->getValue())
								->getAsUnquotedString();
		}
		else if (auto storageType = record->getValue("cppAccessorType"))
		{
			os << cast<llvm::StringInit>(storageType->getValue())
								->getAsUnquotedString();
		}
		else if (auto storageType = record->getValue("cppClassName"))
		{
			os << cast<llvm::StringInit>(storageType->getValue())
								->getAsUnquotedString();
		}
		else if (auto storageType = record->getValue("returnType"))
		{
			os << cast<llvm::StringInit>(storageType->getValue())
								->getAsUnquotedString();
		}
		else
		{
			assert(false && "unrechable");
		}

		os << " ";
		return;
	}

	if (StringInit *_ = dyn_cast<StringInit>(init))
	{
		os << init->getAsUnquotedString();
		os << " ";

		return;
	}
	assert(false && "unrechable");
}

static void printArgName(
		mlir::raw_ostream &os,
		StringInit *init,
		size_t argNumber,
		bool optionalParameter)
{
	if (not init)
	{
		os << "arg_" << argNumber;
	}
	else
	{
		os << init->getAsUnquotedString();
	}

	if (!optionalParameter)
		return;

	os << " = {}";
}

static bool isParameterOptional(Init *init)
{
	DefInit *defInit = dyn_cast_or_null<DefInit>(init);
	if (not defInit)
		return false;
	return (llvm::any_of(defInit->getDef()->getSuperClasses(), [](auto i) {
		return i.first->getName() == "OptionalParameter";
	}));
}

static bool isParameterOptionalAndFollowedByOptionals(
		DagInit *args, size_t curr)
{
	if (not isParameterOptional(args->getArg(curr)))
		return false;

	for (size_t i = curr + 1; i < args->arg_size(); i++)
	{
		if (not isParameterOptional(args->getArg(i)))
			return false;
	}

	return true;
}

static void printArguments(
		mlir::raw_ostream &os, DagInit *args, bool printTypes, bool areOpArgument)
{
	for (size_t i = 0; i != args->arg_size(); i++)
	{
		if (printTypes)
			printArgType(os, args->getArg(i), areOpArgument);

		printArgName(
				os,
				args->getArgName(i),
				i,
				isParameterOptionalAndFollowedByOptionals(args, i));
		if (i + 1 != args->arg_size())
			os << ", ";
	}
}

static void printArguments(
		mlir::raw_ostream &os,
		DagInit *results,
		DagInit *params,
		bool printTypes,
		bool isOp,
		bool inferredContext)
{
	os << "(";
	bool need_comma = false;
	if (not printTypes and not isOp and not inferredContext)
	{
		os << "this->getContext()";
		need_comma = true;
	}
	if (isOp)
	{
		if (printTypes)
		{
			os << "mlir::Location ";
		}
		os << "loc";
		need_comma = true;
	}
	if (results)
	{
		if (results->arg_size() != 0 and need_comma)
			os << ", ";
		printArguments(os, results, printTypes, false);
	}
	if (params->arg_size() != 0 and need_comma)
		os << ", ";
	printArguments(os, params, printTypes, isOp);
	os << ")";
}

static void printTypeBuilder(
		mlir::raw_ostream &os,
		llvm::StringRef ns,
		llvm::StringRef opName,
		DagInit *params,
		bool declaration,
		bool inteferredContext)
{
	os << ns << "::" << opName << " get" << opName;
	printArguments(os, nullptr, params, true, false, inteferredContext);
	if (declaration)
	{
		os << ";\n";
		return;
	}

	os << "{\n";
	os.indent(4);
	os << "return " << ns << "::" << opName << "::get";
	printArguments(os, nullptr, params, false, false, inteferredContext);
	os << ";\n}\n";
}

static void printBuilder(
		mlir::raw_ostream &os,
		llvm::StringRef ns,
		llvm::StringRef opName,
		DagInit *results,
		DagInit *params,
		bool declaration)
{
	os << ns << "::" << toCamelCase(opName) << " create" << toCamelCase(opName);
	printArguments(os, results, params, true, true, false);
	if (declaration)
	{
		os << ";\n";
		return;
	}

	os << "{\n";
	os.indent(4);
	os << "return this->template create<" << ns << "::" << toCamelCase(opName)
		 << ">";
	printArguments(os, results, params, false, true, false);
	os << ";\n}\n";
}

static void printBuildersSignature(
		mlir::raw_ostream &os, Record *opDecl, llvm::StringRef ns)
{
	printBuilder(
			os,
			ns,
			opDecl->getName().drop_front(4),
			opDecl->getValueAsDag("results"),
			opDecl->getValueAsDag("arguments"),
			false);
	if (opDecl->isValueUnset("builders"))
		return;
	auto builders = opDecl->getValueAsListOfDefs("builders");
	for (auto builder : builders)
	{
		auto params = builder->getValueAsDag("dagParams");
		printBuilder(
				os, ns, opDecl->getName().drop_front(4), nullptr, params, false);
	}
}

static void printTypeSignatures(
		mlir::raw_ostream &os, Record *opDecl, llvm::StringRef ns)
{
	if (opDecl->isValueUnset("builders"))
		return;
	auto builders = opDecl->getValueAsListOfDefs("builders");

	printTypeBuilder(
			os,
			ns,
			opDecl->getValueAsString("cppClassName"),
			opDecl->getValueAsDag("parameters"),
			false,
			false);

	for (auto builder : builders)
	{
		auto params = builder->getValueAsDag("dagParams");
		auto inferredContext = builder->getValueAsBit("hasInferredContextParam");
		if (inferredContext)
			continue;
		printTypeBuilder(
				os,
				ns,
				opDecl->getValueAsString("cppClassName"),
				params,
				false,
				inferredContext);
	}
}

// Generator that prints records.
GenRegistration printRecords(
		"print-records",
		"Print all records to stdout",
		[](const RecordKeeper &records, raw_ostream &os) {
			os << records;
			return false;
		});

GenRegistration printBuilderDecls(
		"print-builder-decls",
		"Print the signatures for the dialect specific ir builder",
		[](const RecordKeeper &records, raw_ostream &os) {
			auto dialect = records.getAllDerivedDefinitions("Dialect");
			auto ns = dialect.front()->getValueAsString("cppNamespace");
			for (auto &decl : records.getAllDerivedDefinitions("Op"))
			{
				printBuildersSignature(os, decl, ns);
			}
			for (auto &decl : records.getAllDerivedDefinitions("DialectType"))
			{
				if (decl->getValueAsDef("dialect") != dialect.front())
					continue;
				printTypeSignatures(os, decl, ns);
			}
			for (auto &decl : records.getAllDerivedDefinitions("DialectAttr"))
			{
				if (decl->getValueAsDef("dialect") != dialect.front())
					continue;
				printTypeSignatures(os, decl, ns);
			}
			return false;
		});

int main(int argc, char *argv[]) { return mlir::MlirTblgenMain(argc, argv); }

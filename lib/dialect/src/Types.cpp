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
#include "rlc/dialect/Types.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/AsmParser/AsmParser.h"
#include "mlir/AsmParser/AsmParserState.h"
#include "mlir/AsmParser/CodeComplete.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/StorageUniquerSupport.h"
#include "mlir/Parser/Parser.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/OverloadResolver.hpp"
#include "rlc/dialect/SymbolTable.h"
#include "rlc/dialect/TypeStorage.hpp"
#include "rlc/utils/IRange.hpp"

#define GET_TYPEDEF_CLASSES
#include "Types.inc"

void mlir::rlc::RLCDialect::registerTypes()
{
	addTypes<
			ClassType,
#define GET_TYPEDEF_LIST
#include "Types.inc"
			>();
}

using namespace mlir::rlc;

ClassType ClassType::getIdentified(
		MLIRContext *context,
		StringRef name,
		ArrayRef<Type> explicitTemplateParameters)
{
	for (auto type : explicitTemplateParameters)
		assert(not mlir::isa<mlir::rlc::UnknownType>(type));
	return Base::get(
			context, StructTypeStorage::KeyTy(name, explicitTemplateParameters));
}

ClassType ClassType::getNewIdentified(
		MLIRContext *context,
		StringRef name,
		ArrayRef<mlir::rlc::ClassFieldAttr> fields,
		ArrayRef<Type> explicitTemplateParameters)
{
	std::string stringName = name.str();
	unsigned counter = 0;
	do
	{
		auto type = ClassType::getIdentified(
				context, stringName, explicitTemplateParameters);
		if (type.isInitialized() || failed(type.setBody(fields)))
		{
			counter += 1;
			stringName = (Twine(name) + "." + std::to_string(counter)).str();
			continue;
		}
		return type;
	} while (true);
}

mlir::LogicalResult ClassType::setBody(
		ArrayRef<mlir::rlc::ClassFieldAttr> fields)
{
	return Base::mutate(fields);
}

bool ClassType::isInitialized() const { return getImpl()->isInitialized(); }
llvm::StringRef ClassType::getName() const
{
	return getImpl()->getIdentifier();
}
llvm::ArrayRef<mlir::rlc::ClassFieldAttr> ClassType::getMembers() const
{
	return getImpl()->getFields();
}

llvm::ArrayRef<mlir::Type> ClassType::getExplicitTemplateParameters() const
{
	return getImpl()->getExplicitTemplateParameters();
}

mlir::LogicalResult ClassType::verify(
		function_ref<InFlightDiagnostic()>, StructTypeStorage::KeyTy &)
{
	return success();
}

mlir::LogicalResult ClassType::verify(
		function_ref<InFlightDiagnostic()> emitError, ArrayRef<Type> types)
{
	return success();
}

void ClassType::walkImmediateSubElements(
		function_ref<void(Attribute)> walkAttrsFn,
		function_ref<void(Type)> walkTypesFn) const
{
	for (auto attr : getMembers())
		walkAttrsFn(attr);

	for (Type type : getExplicitTemplateParameters())
		walkTypesFn(type);
}

mlir::Type ClassType::replaceImmediateSubElements(
		ArrayRef<Attribute> replAttrs, ArrayRef<Type> replTypes) const
{
	auto type = ClassType::getIdentified(
			getContext(), getName(), replTypes.drop_front(getMembers().size()));
	llvm::SmallVector<mlir::rlc::ClassFieldAttr> fields;
	for (auto attr : replAttrs.take_front(getMembers().size()))
	{
		fields.push_back(mlir::cast<mlir::rlc::ClassFieldAttr>(attr));
	}
	auto result = type.setBody(fields);
	assert(result.succeeded());
	return type;
}

mlir::Type ClassType::parse(mlir::AsmParser &parser)
{
	if (parser.parseLess())
		return mlir::Type();

	std::string name;
	auto res = parser.parseKeywordOrString(&name);
	if (res.failed())
	{
		parser.emitError(
				parser.getCurrentLocation(), "failed to parse Class type nam");
		return {};
	}

	llvm::SmallVector<mlir::Type, 2> templateParameters;
	if (parser.parseOptionalLess().succeeded())
	{
		while (parser.parseOptionalGreater().failed())
		{
			mlir::Type type;
			if (parser.parseType(type).failed())
			{
				parser.emitError(
						parser.getCurrentLocation(),
						"failed to parse Class template parameter");
				return {};
			}
			templateParameters.push_back(type);
			if (parser.parseComma().failed())
				return {};
		}
	}

	auto toReturn =
			ClassType::getIdentified(parser.getContext(), name, templateParameters);

	llvm::SmallVector<mlir::rlc::ClassFieldAttr, 2> inners;

	if (parser.parseLBrace().succeeded())
	{
		while (parser.parseOptionalRBrace().failed())
		{
			mlir::Attribute field;
			if (parser.parseAttribute(field).failed())
			{
				parser.emitError(
						parser.getCurrentLocation(), "failed to parse class member");
				return {};
			}

			res = parser.parseComma();
			if (res.failed())
			{
				parser.emitError(
						parser.getCurrentLocation(),
						"expected comma after field declaration");
				return {};
			}

			inners.push_back(mlir::cast<mlir::rlc::ClassFieldAttr>(field));
		}
	}

	if (toReturn.setBody(inners).failed())
	{
		parser.emitError(
				parser.getCurrentLocation(),
				"failed to set Class sub types, it was already defined");
		return {};
	}

	if (parser.parseGreater().failed())
		return Type();
	return toReturn;
}

mlir::Type ClassType::print(mlir::AsmPrinter &p) const
{
	p << getMnemonic();
	p << "<";
	p.printKeywordOrString(getName());
	if (not getExplicitTemplateParameters().empty())
	{
		p << "<";
		size_t counter = 0;
		for (auto type : getExplicitTemplateParameters())
		{
			p.printType(type);
			if (counter++ != getExplicitTemplateParameters().size())
				p << ", ";
		}

		p << ">";
	}
	if (not isInitialized())
	{
		p << ">";
		return *this;
	}
	p << " {";
	for (auto member : getMembers())
	{
		p.printAttribute(member);
		p << ", ";
	}

	p << "}>";
	return *this;
}

/// Parse a type registered to this dialect.
::mlir::Type RLCDialect::parseType(::mlir::DialectAsmParser &parser) const
{
	::llvm::SMLoc typeLoc = parser.getCurrentLocation();
	::llvm::StringRef mnemonic;
	::mlir::Type genType;
	auto parseResult = generatedTypeParser(parser, &mnemonic, genType);
	if (parseResult.has_value())
		return genType;

	if (mnemonic == ClassType::getMnemonic())
	{
		return ClassType::parse(parser);
	}

	parser.emitError(typeLoc) << "unknown  type `" << mnemonic << "` in dialect `"
														<< getNamespace() << "`";
	return {};
}
/// Print a type registered to this dialect.
void RLCDialect::printType(
		::mlir::Type type, ::mlir::DialectAsmPrinter &printer) const
{
	if (::mlir::succeeded(generatedTypePrinter(type, printer)))
		return;
	if (auto casted = mlir::dyn_cast<ClassType>(type))
	{
		casted.print(printer);
		return;
	}
}

mlir::LogicalResult mlir::rlc::returnsVoid(mlir::FunctionType type)
{
	return mlir::success(
			type.getNumResults() == 0 or
			mlir::isa<mlir::rlc::VoidType>(type.getResult(0)));
}

static void typeToPretty(llvm::raw_ostream &OS, mlir::Type t)
{
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::TraitMetaType>(t))
	{
		OS << maybeType.getName();
		return;
	}

	if (auto maybeType =
					mlir::dyn_cast<mlir::rlc::UncheckedTemplateParameterType>(t))
	{
		OS << maybeType.getName();
		if (maybeType.getTrait() != nullptr)
		{
			OS << ":" << maybeType.getTrait();
		}
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::TemplateParameterType>(t))
	{
		OS << maybeType.getName();
		if (maybeType.getTrait() != nullptr)
		{
			OS << ":";
			typeToPretty(OS, maybeType.getTrait());
		}
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::IntegerType>(t))
	{
		if (maybeType.getSize() == 64)
			OS << "Int";
		else if (maybeType.getSize() == 8)
			OS << "Byte";
		else
			abort();
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::UnknownType>(t))
	{
		OS << "Unkown";
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::AliasType>(t))
	{
		OS << maybeType.getName();
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::FloatType>(t))
	{
		OS << "Float";
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::BoolType>(t))
	{
		OS << "Bool";
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::VoidType>(t))
	{
		OS << "Void";
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::ClassType>(t))
	{
		OS << maybeType.getName();
		if (not maybeType.getExplicitTemplateParameters().empty())
		{
			OS << "<";
			for (auto t : llvm::drop_end(maybeType.getExplicitTemplateParameters()))
			{
				typeToPretty(OS, t);
				OS << ", ";
			}
			typeToPretty(OS, maybeType.getExplicitTemplateParameters().back());
			OS << ">";
		}
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::AlternativeType>(t))
	{
		if (not maybeType.getName().empty())
		{
			OS << maybeType.getName();
			return;
		}
		size_t i = 0;
		for (auto input : maybeType.getUnderlying())
		{
			i++;
			typeToPretty(OS, input);

			if (i != maybeType.getUnderlying().size())
				OS << " | ";
		}
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::ArrayType>(t))
	{
		typeToPretty(OS, maybeType.getUnderlying());
		OS << "[";
		typeToPretty(OS, maybeType.getSize());
		OS << "]";
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::StringLiteralType>(t))
	{
		OS << "StringLiteral";
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::ContextType>(t))
	{
		OS << "ctx ";
		typeToPretty(OS, maybeType.getUnderlying());
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::FrameType>(t))
	{
		OS << "frm ";
		typeToPretty(OS, maybeType.getUnderlying());
		return;
	}
	if (auto maybeType = mlir::dyn_cast<mlir::rlc::ReferenceType>(t))
	{
		OS << "ref ";
		typeToPretty(OS, maybeType.getUnderlying());
		return;
	}

	if (auto maybeType = mlir::dyn_cast<mlir::rlc::IntegerLiteralType>(t))
	{
		OS << maybeType.getValue();
		return;
	}

	if (auto maybeType = mlir::dyn_cast<mlir::rlc::TemplateParameterType>(t))
	{
		OS << maybeType.getName();
		if (maybeType.getTrait() != nullptr)
		{
			OS << ": ";
			typeToPretty(OS, maybeType.getTrait());
		}
		return;
	}

	if (auto maybeType = mlir::dyn_cast<mlir::rlc::ScalarUseType>(t))
	{
		OS << maybeType.getReadType();
		if (maybeType.getExplicitTemplateParameters().size() != 0)
		{
			OS << "<";
			for (auto t : llvm::drop_end(maybeType.getExplicitTemplateParameters()))
			{
				typeToPretty(OS, t);
				OS << ", ";
			}
			typeToPretty(OS, maybeType.getExplicitTemplateParameters().back());
			OS << ">";
		}
		if (maybeType.getSize() != nullptr and
				maybeType.getSize() !=
						mlir::rlc::IntegerLiteralType::get(maybeType.getContext(), 0))
		{
			typeToPretty(OS, maybeType.getUnderlying());
			OS << "[";
			typeToPretty(OS, maybeType.getSize());
			OS << "]";
		}
		return;
	}

	if (auto maybeType = mlir::dyn_cast<mlir::rlc::OwningPtrType>(t))
	{
		OS << "OwningPtr<";
		typeToPretty(OS, maybeType.getUnderlying());
		OS << ">";
		return;
	}

	if (auto maybeType = mlir::dyn_cast<mlir::FunctionType>(t))
	{
		assert(maybeType.getResults().size() <= 1);

		OS << "(";
		for (size_t index = 0; index < maybeType.getNumInputs(); index++)
		{
			typeToPretty(OS, maybeType.getInput(index));
			if (index < maybeType.getNumInputs() - 1)
				OS << ", ";
		}
		OS << ")";
		OS << " -> ";
		if (not maybeType.getResults().empty())
		{
			typeToPretty(OS, maybeType.getResult(0));
		}
		else
		{
			OS << "Void";
		}
		return;
	}

	t.dump();
	assert(false && "unrechable");
}
namespace mlir::rlc
{
	static void typeToMangled(llvm::raw_ostream &OS, mlir::Type t)
	{
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::TraitMetaType>(t))
		{
			OS << maybeType.getName();
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::ContextType>(t))
		{
			typeToMangled(OS, maybeType.getUnderlying());
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::FrameType>(t))
		{
			typeToMangled(OS, maybeType.getUnderlying());
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::TemplateParameterType>(t))
		{
			OS << maybeType.getName();
			if (maybeType.getTrait() != nullptr)
			{
				OS << ":";
				typeToMangled(OS, maybeType.getTrait());
			}
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::StringLiteralType>(t))
		{
			OS << "strlit";
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::IntegerType>(t))
		{
			OS << "int" << int64_t(maybeType.getSize()) << "_t";
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::FloatType>(t))
		{
			OS << "double";
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::BoolType>(t))
		{
			OS << "bool";
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::VoidType>(t))
		{
			OS << "void";
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::ClassType>(t))
		{
			OS << maybeType.getName();
			if (not maybeType.getExplicitTemplateParameters().empty())
			{
				OS << "T";
				for (auto type : maybeType.getExplicitTemplateParameters())
				{
					typeToMangled(OS, type);
					OS << "T";
				}
			}
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::AlternativeType>(t))
		{
			if (not maybeType.getName().empty())
			{
				OS << maybeType.getName();
				return;
			}
			size_t i = 0;
			OS << "alt_";
			for (auto input : maybeType.getUnderlying())
			{
				i++;
				typeToMangled(OS, input);

				if (i != maybeType.getUnderlying().size())
					OS << "_or_";
			}
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::ArrayType>(t))
		{
			typeToMangled(OS, maybeType.getUnderlying());
			OS << "_";
			OS << maybeType.getArraySize();
			return;
		}
		if (auto maybeType = mlir::dyn_cast<mlir::rlc::ReferenceType>(t))
		{
			typeToMangled(OS, maybeType.getUnderlying());
			OS << "Ref";
			return;
		}

		if (auto maybeType = mlir::dyn_cast<mlir::rlc::OwningPtrType>(t))
		{
			typeToMangled(OS, maybeType.getUnderlying());
			OS << "Ptr";
			return;
		}

		if (auto maybeType = mlir::dyn_cast<mlir::FunctionType>(t))
		{
			assert(maybeType.getResults().size() <= 1);
			for (auto input : maybeType.getInputs())
			{
				OS << "_";
				typeToMangled(OS, input);
			}
			if (not maybeType.getResults().empty() and
					not mlir::isa<mlir::rlc::VoidType>(maybeType.getResults().front()))
			{
				OS << "_r_";
				typeToMangled(OS, maybeType.getResult(0));
			}
			return;
		}
		if (auto casted = mlir::dyn_cast<mlir::rlc::IntegerLiteralType>(t))
		{
			OS << casted.getValue();
			return;
		}

		t.dump();
		assert(false && "unrechable");
	}
}	 // namespace mlir::rlc

std::string mlir::rlc::typeToMangled(mlir::Type t)
{
	std::string s;
	llvm::raw_string_ostream OS(s);

	typeToMangled(OS, t);
	OS.flush();

	return s;
}

std::string mlir::rlc::ClassType::mangledName() { return typeToMangled(*this); }

std::string mlir::rlc::AlternativeType::getMangledName()
{
	return typeToMangled(*this);
}

std::string mlir::rlc::mangledName(
		llvm::StringRef functionName,
		bool isMemberFunction,
		mlir::FunctionType type)
{
	std::string s;
	llvm::raw_string_ostream OS(s);
	assert(not isMemberFunction or type.getInputs().size() >= 1);

	OS << "rl_" << (isMemberFunction ? "m_" : "") << functionName << "_";
	typeToMangled(OS, type);
	OS.flush();

	return s;
}

std::string mlir::rlc::prettyPrintFunctionTypeWithNameArgs(
		mlir::FunctionType fType, mlir::rlc::FunctionInfoAttr attr)
{
	std::string toReturn = "(";
	size_t current = 0;
	if (fType.getNumInputs() != attr.getArguments().size())
		return mlir::rlc::prettyType(fType);

	for (auto [type, arg] : llvm::zip(fType.getInputs(), attr.getArguments()))
	{
		toReturn += mlir::rlc::prettyType(type);
		toReturn += " ";
		toReturn += arg.getName();
		current++;
		if (current != fType.getInputs().size())
			toReturn += ", ";
	}
	toReturn += ") ";
	if (fType.getNumResults() != 0 and
			not mlir::isa<mlir::rlc::VoidType>(fType.getResult(0)))
	{
		toReturn += " -> ";
		toReturn += mlir::rlc::prettyType(fType.getResult(0));
	}
	return toReturn;
}

std::string mlir::rlc::prettyType(mlir::Type type)
{
	std::string s;
	llvm::raw_string_ostream OS(s);

	typeToPretty(OS, type);
	OS.flush();

	return s;
}

mlir::FunctionType mlir::rlc::replaceTemplateParameter(
		mlir::FunctionType original,
		mlir::rlc::TemplateParameterType toReplace,
		mlir::Type replacement)
{
	return mlir::cast<mlir::FunctionType>(original.replace(
			[toReplace, replacement](mlir::Type t) -> std::optional<mlir::Type> {
				if (auto Casted = mlir::dyn_cast<mlir::rlc::TemplateParameterType>(t))
				{
					if (Casted == toReplace)
						return replacement;
				}

				return std::nullopt;
			})

	);
}

static mlir::Type resultTypeOrVoid(mlir::FunctionType fType)
{
	return fType.getNumResults() == 0
						 ? mlir::rlc::VoidType::get(fType.getContext())
						 : fType.getResults()[0];
}

static mlir::LogicalResult sameSignatureMethodExists(
		mlir::Location callPoint,
		ValueTable &table,
		llvm::StringRef functionName,
		mlir::FunctionType functionType)
{
	auto resolver = OverloadResolver(table);
	auto overloads = resolver.findOverloads(
			callPoint, false, functionName, functionType.getInputs());

	for (auto &overload : overloads)
	{
		auto overloadType = mlir::cast<mlir::FunctionType>(overload.getType());

		if (resultTypeOrVoid(overloadType) == resultTypeOrVoid(functionType))
			return mlir::success();
	}
	return mlir::failure();
}

mlir::LogicalResult
mlir::rlc::TraitMetaType::typeRespectsTraitFunctionDeclaration(
		mlir::Location callPoint,
		mlir::Type type,
		mlir::rlc::SymbolTable<mlir::Value> &table,
		size_t index)
{
	auto methodType =
			mlir::cast<mlir::FunctionType>(getRequestedFunctionTypes()[index]);
	auto instantiated = replaceTemplateParameter(
			methodType,
			mlir::cast<mlir::rlc::TemplateParameterType>(
					getTemplateParameters().back()),
			type);

	llvm::StringRef methodName = getRequestedFunctionNames()[index];
	return sameSignatureMethodExists(callPoint, table, methodName, instantiated);
}

mlir::LogicalResult mlir::rlc::TraitMetaType::typeRespectsTrait(
		mlir::Location callPoint,
		mlir::Type type,
		mlir::rlc::SymbolTable<mlir::Value> &table)
{
	for (size_t i : ::rlc::irange(getRequestedFunctionTypes().size()))
	{
		if (typeRespectsTraitFunctionDeclaration(callPoint, type, table, i)
						.failed())
			return mlir::failure();
	}
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::isTemplateType(mlir::Type type)
{
	mlir::DenseMap<mlir::Type, bool> table;
	return isTemplateType(type, table);
}

mlir::LogicalResult mlir::rlc::isTemplateType(
		mlir::Type type, mlir::DenseMap<mlir::Type, bool> &table)
{
	if (auto t = table.find(type); t != table.end())
		return mlir::success(t->getSecond());

	if (auto casted = mlir::dyn_cast<mlir::rlc::TemplateParameterType>(type))
	{
		table[type] = true;
		return mlir::success();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::ClassType>(type))
	{
		for (auto child : casted.getExplicitTemplateParameters())
			if (isTemplateType(child, table).succeeded())
			{
				table[type] = true;
				return mlir::success();
			}
		if (not casted.isInitialized())
		{
			table[type] = false;
			return mlir::failure();
		}

		for (auto child : casted.getMembers())
			if (isTemplateType(child.getType(), table).succeeded())
			{
				table[type] = true;
				return mlir::success();
			}
		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
	{
		for (auto child : casted.getUnderlying())
			if (isTemplateType(child, table).succeeded())
			{
				table[type] = true;
				return mlir::success();
			}

		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::FunctionType>(type))
	{
		for (auto child : casted.getInputs())
			if (isTemplateType(child, table).succeeded())
			{
				table[type] = true;
				return mlir::success();
			}

		for (auto child : casted.getResults())
			if (isTemplateType(child, table).succeeded())
			{
				table[type] = true;
				return mlir::success();
			}

		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::FrameType>(type))
	{
		auto success = isTemplateType(casted.getUnderlying(), table);
		table[type] = success.succeeded();
		return success;
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::ContextType>(type))
	{
		auto success = isTemplateType(casted.getUnderlying(), table);
		table[type] = success.succeeded();
		return success;
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::StringLiteralType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::ReferenceType>(type))
	{
		auto success = isTemplateType(casted.getUnderlying(), table);
		table[type] = success.succeeded();
		return success;
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::ArrayType>(type))
	{
		auto isTemplate =
				(isTemplateType(casted.getUnderlying(), table).succeeded() or
				 isTemplateType(casted.getSize(), table).succeeded());

		table[type] = isTemplate;
		return mlir::success(isTemplate);
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::IntegerType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}
	if (auto casted = mlir::dyn_cast<mlir::rlc::FloatType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}
	if (auto casted = mlir::dyn_cast<mlir::rlc::BoolType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::VoidType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::TraitMetaType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::OwningPtrType>(type))
	{
		auto success = isTemplateType(casted.getUnderlying(), table);
		table[type] = success.succeeded();
		return success;
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::IntegerLiteralType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::ScalarUseType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}

	if (auto casted = mlir::dyn_cast<mlir::rlc::FunctionUseType>(type))
	{
		table[type] = false;
		return mlir::failure();
	}

	type.dump();
	llvm_unreachable("unhandled type");
	return mlir::failure();
}

mlir::Type mlir::rlc::decayCtxFrmType(mlir::Type t)
{
	if (auto casted = mlir::dyn_cast<mlir::rlc::FrameType>(t))
		return casted.getUnderlying();
	if (auto casted = mlir::dyn_cast<mlir::rlc::ContextType>(t))
		return casted.getUnderlying();
	return t;
}

int64_t mlir::rlc::ArrayType::getArraySize()
{
	return mlir::cast<mlir::rlc::IntegerLiteralType>(getSize()).getValue();
}

void mlir::rlc::TraitMetaType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << getName();
	llvm::SmallVector<mlir::Type, 2> nonTemplateTYpes;
	for (auto type : getTemplateParameters())
	{
		if (mlir::isa<mlir::rlc::TemplateParameterType>(type))
		{
			continue;
		}
		nonTemplateTYpes.push_back(type);
	}

	if (nonTemplateTYpes.empty())
		return;
	OS << "<";
	for (auto parameter : llvm::drop_end(nonTemplateTYpes))
	{
		mlir::cast<mlir::rlc::RLCSerializable>(parameter).rlc_serialize(OS, ctx);
		OS << ", ";
	}
	mlir::cast<mlir::rlc::RLCSerializable>(nonTemplateTYpes.back())
			.rlc_serialize(OS, ctx);
	OS << ">";
}

void mlir::rlc::IntegerType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	if (getSize() == 64)
		OS << "Int";
	else if (getSize() == 8)
		OS << "Byte";
	else
	{
		dump();
		abort();
	}
}

void mlir::rlc::AliasType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << getName();
	if (getExplicitTemplateParameters().empty())
		return;
	OS << "<";
	for (auto templateParameter : llvm::drop_end(getExplicitTemplateParameters()))
	{
		mlir::cast<mlir::rlc::RLCSerializable>(templateParameter)
				.rlc_serialize(OS, ctx);
		OS << ", ";
	}
	mlir::cast<mlir::rlc::RLCSerializable>(getExplicitTemplateParameters().back())
			.rlc_serialize(OS, ctx);
	OS << ">";
}

void mlir::rlc::StringLiteralType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << "StringLiteral";
}

void mlir::rlc::FloatType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << "Float";
}

void mlir::rlc::BoolType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << "Bool";
}

void mlir::rlc::ReferenceType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << "ref ";
	mlir::cast<mlir::rlc::RLCSerializable>(getUnderlying())
			.rlc_serialize(OS, ctx);
}

void mlir::rlc::OwningPtrType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << "OwningPtr<";
	mlir::cast<mlir::rlc::RLCSerializable>(getUnderlying())
			.rlc_serialize(OS, ctx);
	OS << ">";
}

void mlir::rlc::AlternativeType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	if (not getName().empty())
	{
		OS << getName();
		return;
	}

	for (size_t i = 0; i != getUnderlying().size(); i++)
	{
		mlir::cast<mlir::rlc::RLCSerializable>(getUnderlying()[i])
				.rlc_serialize(OS, ctx);
		if (i + 1 != getUnderlying().size())
			OS << " | ";
	}
}

void mlir::rlc::ArrayType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	mlir::cast<mlir::rlc::RLCSerializable>(getUnderlying())
			.rlc_serialize(OS, ctx);
	OS << "[";
	mlir::cast<mlir::rlc::RLCSerializable>(getSize()).rlc_serialize(OS, ctx);
	OS << "]";
}

void mlir::rlc::TemplateParameterType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	if (getTrait() != nullptr)
	{
		mlir::cast<mlir::rlc::RLCSerializable>(getTrait()).rlc_serialize(OS, ctx);
	}
	OS << getName();
}

mlir::Type mlir::rlc::AliasType::rename(llvm::StringRef newName) const
{
	return mlir::rlc::AliasType::get(newName, getUnderlying());
}

mlir::Type mlir::rlc::ClassType::rename(llvm::StringRef newName) const
{
	return getNewIdentified(
			getContext(), newName, getMembers(), getExplicitTemplateParameters());
}

void mlir::rlc::ContextType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << "ctx ";
	mlir::cast<mlir::rlc::RLCSerializable>(getUnderlying())
			.rlc_serialize(OS, ctx);
}

void mlir::rlc::FrameType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << "frm ";
	mlir::cast<mlir::rlc::RLCSerializable>(getUnderlying())
			.rlc_serialize(OS, ctx);
}

void mlir::rlc::IntegerLiteralType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << getValue();
}

void mlir::rlc::ClassType::rlc_serialize(
		llvm::raw_ostream &OS, const mlir::rlc::SerializationContext &ctx) const
{
	OS << getName();
	if (getExplicitTemplateParameters().size() != 0)
	{
		OS << "<";
		for (auto templateParameter :
				 llvm::drop_end(getExplicitTemplateParameters()))
		{
			mlir::cast<mlir::rlc::RLCSerializable>(templateParameter)
					.rlc_serialize(OS, ctx);
			OS << ", ";
		}

		mlir::cast<mlir::rlc::RLCSerializable>(
				getExplicitTemplateParameters().back())
				.rlc_serialize(OS, ctx);
		OS << ">";
	}
}

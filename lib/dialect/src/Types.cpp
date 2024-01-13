/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
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
			EntityType,
#define GET_TYPEDEF_LIST
#include "Types.inc"
			>();
}

using namespace mlir::rlc;

EntityType EntityType::getIdentified(
		MLIRContext *context,
		StringRef name,
		ArrayRef<Type> explicitTemplateParameters)
{
	for (auto type : explicitTemplateParameters)
		assert(not type.isa<mlir::rlc::UnknownType>());
	return Base::get(
			context, StructTypeStorage::KeyTy(name, explicitTemplateParameters));
}

EntityType EntityType::getNewIdentified(
		MLIRContext *context,
		StringRef name,
		ArrayRef<Type> elements,
		ArrayRef<std::string> fieldNames,
		ArrayRef<Type> explicitTemplateParameters)
{
	std::string stringName = name.str();
	unsigned counter = 0;
	do
	{
		auto type = EntityType::getIdentified(
				context, stringName, explicitTemplateParameters);
		if (type.isInitialized() || failed(type.setBody(elements, fieldNames)))
		{
			counter += 1;
			stringName = (Twine(name) + "." + std::to_string(counter)).str();
			continue;
		}
		return type;
	} while (true);
}

mlir::LogicalResult EntityType::setBody(
		ArrayRef<Type> types, ArrayRef<std::string> fieldNames)
{
	return Base::mutate(types, fieldNames);
}

bool EntityType::isInitialized() const { return getImpl()->isInitialized(); }
llvm::StringRef EntityType::getName() const
{
	return getImpl()->getIdentifier();
}
llvm::ArrayRef<mlir::Type> EntityType::getBody() const
{
	return getImpl()->getBody();
}

llvm::ArrayRef<mlir::Type> EntityType::getExplicitTemplateParameters() const
{
	return getImpl()->getExplicitTemplateParameters();
}

mlir::LogicalResult EntityType::verify(
		function_ref<InFlightDiagnostic()>, StructTypeStorage::KeyTy &)
{
	return success();
}

mlir::LogicalResult EntityType::verify(
		function_ref<InFlightDiagnostic()> emitError, ArrayRef<Type> types)
{
	return success();
}

void EntityType::walkImmediateSubElements(
		function_ref<void(Attribute)> walkAttrsFn,
		function_ref<void(Type)> walkTypesFn) const
{
	for (Type type : getBody())
		walkTypesFn(type);

	for (Type type : getExplicitTemplateParameters())
		walkTypesFn(type);
}

mlir::Type EntityType::replaceImmediateSubElements(
		ArrayRef<Attribute> replAttrs, ArrayRef<Type> replTypes) const
{
	auto type = EntityType::getIdentified(
			getContext(), getName(), replTypes.drop_front(getBody().size()));
	auto result =
			type.setBody(replTypes.take_front(getBody().size()), getFieldNames());
	assert(result.succeeded());
	return type;
}

llvm::ArrayRef<std::string> EntityType::getFieldNames() const
{
	return getImpl()->getFieldNames();
}

mlir::Type EntityType::parse(mlir::AsmParser &parser)
{
	if (parser.parseLess())
		return mlir::Type();

	std::string name;
	auto res = parser.parseKeywordOrString(&name);
	if (res.failed())
	{
		parser.emitError(
				parser.getCurrentLocation(), "failed to parse Entity type nam");
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
						"failed to parse Entity template parameter");
				return {};
			}
			templateParameters.push_back(type);
			if (parser.parseComma().failed())
				return {};
		}
	}

	auto toReturn =
			EntityType::getIdentified(parser.getContext(), name, templateParameters);

	llvm::SmallVector<std::string, 2> names;
	llvm::SmallVector<mlir::Type, 2> inners;

	if (parser.parseLBrace().succeeded())
	{
		while (parser.parseOptionalRBrace().failed())
		{
			names.emplace_back();
			if (parser.parseKeywordOrString(&names.back()).failed() or
					parser.parseColon().failed())
			{
				parser.emitError(
						parser.getCurrentLocation(), "failed to parse Entity sub type ");
				return {};
			}

			mlir::Type elem = {};
			auto res = parser.parseType(elem);
			if (res.failed())
			{
				parser.emitError(
						parser.getCurrentLocation(), "failed to parse Entity sub type ");
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

			inners.push_back(elem);
		}
	}

	if (toReturn.setBody(inners, names).failed())
	{
		parser.emitError(
				parser.getCurrentLocation(),
				"failed to set Entity sub types, it was already defined");
		return {};
	}

	if (parser.parseGreater().failed())
		return Type();
	return toReturn;
}

mlir::Type EntityType::print(mlir::AsmPrinter &p) const
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
	for (const auto &[type, name] : llvm::zip(getBody(), getFieldNames()))
	{
		p << name;
		p << ": ";
		p.printType(type);
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

	if (mnemonic == EntityType::getMnemonic())
	{
		return EntityType::parse(parser);
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
	if (auto casted = type.dyn_cast<EntityType>())
	{
		casted.print(printer);
		return;
	}
}

static void typeToPretty(llvm::raw_ostream &OS, mlir::Type t)
{
	if (auto maybeType = t.dyn_cast<mlir::rlc::TraitMetaType>())
	{
		OS << maybeType.getName();
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::rlc::UncheckedTemplateParameterType>())
	{
		OS << maybeType.getName();
		if (maybeType.getTrait() != nullptr)
		{
			OS << ":" << maybeType.getTrait();
		}
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::TemplateParameterType>())
	{
		OS << maybeType.getName();
		if (maybeType.getTrait() != nullptr)
		{
			OS << ":";
			typeToPretty(OS, maybeType.getTrait());
		}
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::IntegerType>())
	{
		if (maybeType.getSize() == 64)
			OS << "Int";
		else if (maybeType.getSize() == 8)
			OS << "Byte";
		else
			abort();
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::UnknownType>())
	{
		OS << "Unkown";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::FloatType>())
	{
		OS << "Float";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::BoolType>())
	{
		OS << "Bool";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::VoidType>())
	{
		OS << "Void";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::EntityType>())
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
	if (auto maybeType = t.dyn_cast<mlir::rlc::AlternativeType>())
	{
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
	if (auto maybeType = t.dyn_cast<mlir::rlc::ArrayType>())
	{
		typeToPretty(OS, maybeType.getUnderlying());
		OS << "[";
		typeToPretty(OS, maybeType.getSize());
		OS << "]";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::ReferenceType>())
	{
		OS << "ref ";
		typeToPretty(OS, maybeType.getUnderlying());
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::rlc::IntegerLiteralType>())
	{
		OS << maybeType.getValue();
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::rlc::TemplateParameterType>())
	{
		OS << maybeType.getName();
		if (maybeType.getTrait() != nullptr)
		{
			OS << ": ";
			typeToPretty(OS, maybeType.getTrait());
		}
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::rlc::ScalarUseType>())
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
			OS << "[";
			typeToPretty(OS, maybeType.getSize());
			OS << "]";
		}
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::rlc::OwningPtrType>())
	{
		OS << "OwningPtr<";
		typeToPretty(OS, maybeType.getUnderlying());
		OS << ">";
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::FunctionType>())
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

static void typeToMangled(llvm::raw_ostream &OS, mlir::Type t)
{
	if (auto maybeType = t.dyn_cast<mlir::rlc::TraitMetaType>())
	{
		OS << maybeType.getName();
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::TemplateParameterType>())
	{
		OS << maybeType.getName();
		if (maybeType.getTrait() != nullptr)
		{
			OS << ":";
			typeToMangled(OS, maybeType.getTrait());
		}
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::IntegerType>())
	{
		OS << "int" << int64_t(maybeType.getSize()) << "_t";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::FloatType>())
	{
		OS << "double";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::BoolType>())
	{
		OS << "bool";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::VoidType>())
	{
		OS << "void";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::EntityType>())
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
	if (auto maybeType = t.dyn_cast<mlir::rlc::AlternativeType>())
	{
		size_t i = 0;
		for (auto input : maybeType.getUnderlying())
		{
			i++;
			typeToMangled(OS, input);

			if (i != maybeType.getUnderlying().size())
				OS << "_or_";
		}
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::ArrayType>())
	{
		typeToMangled(OS, maybeType.getUnderlying());
		OS << "_";
		OS << maybeType.getArraySize();
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::ReferenceType>())
	{
		typeToMangled(OS, maybeType.getUnderlying());
		OS << "Ref";
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::rlc::OwningPtrType>())
	{
		typeToMangled(OS, maybeType.getUnderlying());
		OS << "Ptr";
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::FunctionType>())
	{
		assert(maybeType.getResults().size() <= 1);
		for (auto input : maybeType.getInputs())
		{
			OS << "_";
			typeToMangled(OS, input);
		}
		if (not maybeType.getResults().empty())
		{
			OS << "_r_";
			typeToMangled(OS, maybeType.getResult(0));
		}
		return;
	}

	t.dump();
	assert(false && "unrechable");
}

std::string mlir::rlc::EntityType::mangledName()
{
	std::string s;
	llvm::raw_string_ostream OS(s);

	typeToMangled(OS, *this);
	OS.flush();

	return s;
}

std::string mlir::rlc::AlternativeType::getMangledName()
{
	std::string s;
	llvm::raw_string_ostream OS(s);

	typeToMangled(OS, *this);
	OS.flush();

	return s;
}

std::string mlir::rlc::mangledName(
		llvm::StringRef functionName, mlir::FunctionType type)
{
	std::string s;
	llvm::raw_string_ostream OS(s);

	OS << "rl_" << functionName << "_";
	typeToMangled(OS, type);
	OS.flush();

	return s;
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
	return original
			.replace(
					[toReplace, replacement](mlir::Type t) -> std::optional<mlir::Type> {
						if (auto Casted = t.dyn_cast<mlir::rlc::TemplateParameterType>())
						{
							if (Casted == toReplace)
								return replacement;
						}

						return std::nullopt;
					})
			.cast<mlir::FunctionType>();
}

static mlir::LogicalResult sameSignatureMethodExists(
		ValueTable &table,
		llvm::StringRef functionName,
		mlir::FunctionType functionType)
{
	auto resolver = OverloadResolver(table);
	auto overloads =
			resolver.findOverloads(functionName, functionType.getInputs());

	for (auto &overload : overloads)
	{
		if (overload.getType().cast<mlir::FunctionType>().getResults() ==
				functionType.getResults())
			return mlir::success();
	}
	return mlir::failure();
}

mlir::LogicalResult
mlir::rlc::TraitMetaType::typeRespectsTraitFunctionDeclaration(
		mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table, size_t index)
{
	auto methodType =
			getRequestedFunctionTypes()[index].cast<mlir::FunctionType>();
	auto instantiated =
			replaceTemplateParameter(methodType, getTemplateParameterType(), type);

	llvm::StringRef methodName = getRequestedFunctionNames()[index];
	return sameSignatureMethodExists(table, methodName, instantiated);
}

mlir::LogicalResult mlir::rlc::TraitMetaType::typeRespectsTrait(
		mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table)
{
	for (size_t i : ::rlc::irange(getRequestedFunctionTypes().size()))
	{
		if (typeRespectsTraitFunctionDeclaration(type, table, i).failed())
			return mlir::failure();
	}
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::isTemplateType(mlir::Type type)
{
	if (auto casted = type.dyn_cast<mlir::rlc::TemplateParameterType>())
		return mlir::success();

	if (auto casted = type.dyn_cast<mlir::rlc::EntityType>())
	{
		for (auto child : casted.getExplicitTemplateParameters())
			if (isTemplateType(child).succeeded())
				return mlir::success();
		if (not casted.isInitialized())
			return mlir::success();

		for (auto child : casted.getBody())
			if (isTemplateType(child).succeeded())
				return mlir::success();
		return mlir::failure();
	}

	if (auto casted = type.dyn_cast<mlir::rlc::AlternativeType>())
	{
		for (auto child : casted.getUnderlying())
			if (isTemplateType(child).succeeded())
				return mlir::success();

		return mlir::failure();
	}

	if (auto casted = type.dyn_cast<mlir::FunctionType>())
	{
		for (auto child : casted.getInputs())
			if (isTemplateType(child).succeeded())
				return mlir::success();

		for (auto child : casted.getResults())
			if (isTemplateType(child).succeeded())
				return mlir::success();
		return mlir::failure();
	}

	if (auto casted = type.dyn_cast<mlir::rlc::ReferenceType>())
	{
		return isTemplateType(casted.getUnderlying());
	}

	if (auto casted = type.dyn_cast<mlir::rlc::ArrayType>())
	{
		return mlir::success(
				isTemplateType(casted.getUnderlying()).succeeded() or
				isTemplateType(casted.getSize()).succeeded());
	}

	if (auto casted = type.dyn_cast<mlir::rlc::IntegerType>())
	{
		return mlir::failure();
	}
	if (auto casted = type.dyn_cast<mlir::rlc::FloatType>())
	{
		return mlir::failure();
	}
	if (auto casted = type.dyn_cast<mlir::rlc::BoolType>())
	{
		return mlir::failure();
	}

	if (auto casted = type.dyn_cast<mlir::rlc::VoidType>())
	{
		return mlir::failure();
	}

	if (auto casted = type.dyn_cast<mlir::rlc::TraitMetaType>())
	{
		return mlir::failure();
	}

	if (auto casted = type.dyn_cast<mlir::rlc::OwningPtrType>())
	{
		return isTemplateType(casted.getUnderlying());
	}

	if (auto casted = type.dyn_cast<mlir::rlc::IntegerLiteralType>())
	{
		return mlir::failure();
	}

	type.dump();
	llvm_unreachable("unhandled type");
	return mlir::failure();
}

int64_t mlir::rlc::ArrayType::getArraySize()
{
	return getSize().cast<mlir::rlc::IntegerLiteralType>().getValue();
}

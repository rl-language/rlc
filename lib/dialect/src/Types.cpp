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

EntityType EntityType::getIdentified(MLIRContext *context, StringRef name)
{
	return Base::get(context, name);
}

EntityType EntityType::getNewIdentified(
		MLIRContext *context,
		StringRef name,
		ArrayRef<Type> elements,
		ArrayRef<std::string> fieldNames)
{
	std::string stringName = name.str();
	unsigned counter = 0;
	do
	{
		auto type = EntityType::getIdentified(context, stringName);
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

mlir::LogicalResult EntityType::verify(
		function_ref<InFlightDiagnostic()>, StringRef)
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
}

mlir::Type EntityType::replaceImmediateSubElements(
		ArrayRef<Attribute> replAttrs, ArrayRef<Type> replTypes) const
{
	// TODO: It's not clear how we support replacing sub-elements of mutable
	// types.
	llvm_unreachable("not implemented");
	return nullptr;
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

	auto toReturn = EntityType::getIdentified(parser.getContext(), name);
	if (parser.parseLBrace().failed())
		return toReturn;

	llvm::SmallVector<std::string, 2> names;
	llvm::SmallVector<mlir::Type, 2> inners;
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
	if (toReturn.setBody(inners, names).failed())
	{
		parser.emitError(
				parser.getCurrentLocation(),
				"failed to set Entity sub types, it was already defined");
		return {};
	}

	if (parser.parseGreater())
		return Type();
	return toReturn;
}

mlir::Type EntityType::print(mlir::AsmPrinter &p) const
{
	p << getMnemonic();
	p << "<";
	p.printKeywordOrString(getName());
	if (not isInitialized())
	{
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

static void typeToMangled(llvm::raw_ostream &OS, mlir::Type t)
{
	if (auto maybeType = t.dyn_cast<mlir::rlc::IntegerType>())
	{
		OS << "int64_t";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::FloatType>())
	{
		OS << "double";
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::BoolType>())
	{
		OS << "int8_t";
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
		return;
	}
	if (auto maybeType = t.dyn_cast<mlir::rlc::ArrayType>())
	{
		typeToMangled(OS, maybeType.getUnderlying());
		OS << "_";
		OS << maybeType.getSize();
		return;
	}

	if (auto maybeType = t.dyn_cast<mlir::FunctionType>())
	{
		assert(maybeType.getResults().size() <= 1);
		if (not maybeType.getResults().empty())
		{
			typeToMangled(OS, maybeType.getResult(0));
			OS << "_";
		}
		for (auto input : maybeType.getInputs())
		{
			typeToMangled(OS, input);
			OS << "_";
		}
		return;
	}

	assert(false && "unrechable");
}

std::string mlir::rlc::mangledName(
		llvm::StringRef functionName, mlir::FunctionType type)
{
	std::string s;
	llvm::raw_string_ostream OS(s);

	OS << functionName;
	typeToMangled(OS, type);
	OS.flush();

	return s;
}

static mlir::FunctionType replaceTemplateParameter(
		mlir::FunctionType original,
		mlir::rlc::TemplateParameterType toReplace,
		mlir::Type replacement)
{
	return original
			.replaceSubElements(
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

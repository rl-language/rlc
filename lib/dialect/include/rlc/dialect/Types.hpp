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
#pragma once

#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"

namespace mlir::rlc
{
	template<typename T>
	class SymbolTable;
}

#define GET_TYPEDEF_CLASSES
#include "rlc/dialect/TypeStorage.hpp"
#include "rlc/dialect/Types.inc"

namespace mlir::rlc
{
	template<typename T>
	class SymbolTable;

	std::string mangledName(
			llvm::StringRef functionName, mlir::FunctionType type);

	std::string prettyType(mlir::Type type);

	class EntityType
			: public Type::
						TypeBase<EntityType, Type, StructTypeStorage, TypeTrait::IsMutable>
	{
		public:
		/// Inherit base constructors.
		using Base::Base;

		static llvm::StringRef getMnemonic() { return name; }
		constexpr static const char *name = "entity";

		/// Gets or creates an identified struct with the given name in the provided
		/// context. Note that unlike llvm::StructType::create, this function will
		/// _NOT_ rename a struct in case a struct with the same name already exists
		/// in the context. Instead, it will just return the existing struct,
		/// similarly to the rest of MLIR type ::get methods.
		static EntityType getIdentified(
				MLIRContext *context,
				StringRef name,
				ArrayRef<Type> explicitTemplateParameters);

		/// Gets a new identified struct with the given body. The body _cannot_ be
		/// changed later. If a struct with the given name already exists, renames
		/// the struct by appending a `.` followed by a number to the name. Renaming
		/// happens even if the existing struct has the same body.
		static EntityType getNewIdentified(
				MLIRContext *context,
				StringRef name,
				ArrayRef<Type> elements,
				ArrayRef<std::string> fieldNames,
				ArrayRef<Type> explicitTemplateParameters);

		llvm::ArrayRef<std::string> getFieldNames() const;

		/// Set the body of an identified struct. Returns failure if the body could
		/// not be set, e.g. if the struct already has a body or if it was marked as
		/// intentionally opaque. This might happen in a multi-threaded context when
		/// a different thread modified the struct after it was created. Most
		/// callers are likely to assert this always succeeds, but it is possible to
		/// implement a local renaming scheme based on the result of this call.
		LogicalResult setBody(
				ArrayRef<Type> types, ArrayRef<std::string> fieldNames);

		/// Checks if a struct is initialized.
		bool isInitialized() const;

		/// Returns the name of an identified struct.
		StringRef getName() const;

		/// Returns the list of element types contained in a non-opaque struct.
		ArrayRef<Type> getBody() const;
		ArrayRef<Type> getExplicitTemplateParameters() const;

		/// Verifies that the type about to be constructed is well-formed.
		static LogicalResult verify(
				function_ref<InFlightDiagnostic()> emitError, StructTypeStorage::Key &);
		static LogicalResult verify(
				function_ref<InFlightDiagnostic()> emitError, ArrayRef<Type> types);
		std::string mangledName();

		void walkImmediateSubElements(
				function_ref<void(Attribute)> walkAttrsFn,
				function_ref<void(Type)> walkTypesFn) const;
		Type replaceImmediateSubElements(
				ArrayRef<Attribute> replAttrs, ArrayRef<Type> replTypes) const;

		static Type parse(AsmParser &parser);

		Type print(AsmPrinter &p) const;
	};

	mlir::LogicalResult isTemplateType(mlir::Type type);

	mlir::FunctionType replaceTemplateParameter(
			mlir::FunctionType original,
			mlir::rlc::TemplateParameterType toReplace,
			mlir::Type replacement);
}	 // namespace mlir::rlc
	 //
namespace mlir
{

	template<>
	struct AttrTypeSubElementHandler<mlir::rlc::EntityType>
	{
		static void walk(
				const rlc::EntityType &param, AttrTypeImmediateSubElementWalker &walker)
		{
			for (Type type : param.getBody())
				walker.walk(type);

			for (Type type : param.getExplicitTemplateParameters())
				walker.walk(type);
		}
		static FailureOr<rlc::EntityType> replace(
				const rlc::EntityType &param,
				AttrSubElementReplacements &attrRepls,
				TypeSubElementReplacements &typeRepls)
		{
			auto type = rlc::EntityType::getIdentified(
					param.getContext(),
					param.getName(),
					typeRepls.take_front(param.getBody().size()));
			auto result = type.setBody(
					typeRepls.take_front(param.getBody().size()), param.getFieldNames());
			assert(result.succeeded());
			return type;
		}
	};

}	 // namespace mlir

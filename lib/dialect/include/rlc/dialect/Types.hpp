#pragma once

#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#define GET_TYPEDEF_CLASSES
#include "rlc/dialect/Types.inc"

namespace mlir::rlc
{
	class StructTypeStorage;

	class EntityType: public Type::TypeBase<
												EntityType,
												Type,
												StructTypeStorage,
												SubElementTypeInterface::Trait,
												TypeTrait::IsMutable>
	{
		public:
		/// Inherit base constructors.
		using Base::Base;

		static llvm::StringRef getMnemonic() { return "entity"; }

		/// Gets or creates an identified struct with the given name in the provided
		/// context. Note that unlike llvm::StructType::create, this function will
		/// _NOT_ rename a struct in case a struct with the same name already exists
		/// in the context. Instead, it will just return the existing struct,
		/// similarly to the rest of MLIR type ::get methods.
		static EntityType getIdentified(MLIRContext *context, StringRef name);

		/// Gets a new identified struct with the given body. The body _cannot_ be
		/// changed later. If a struct with the given name already exists, renames
		/// the struct by appending a `.` followed by a number to the name. Renaming
		/// happens even if the existing struct has the same body.
		static EntityType getNewIdentified(
				MLIRContext *context,
				StringRef name,
				ArrayRef<Type> elements,
				ArrayRef<std::string> fieldNames);

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

		/// Verifies that the type about to be constructed is well-formed.
		static LogicalResult verify(
				function_ref<InFlightDiagnostic()> emitError, StringRef);
		static LogicalResult verify(
				function_ref<InFlightDiagnostic()> emitError, ArrayRef<Type> types);

		void walkImmediateSubElements(
				function_ref<void(Attribute)> walkAttrsFn,
				function_ref<void(Type)> walkTypesFn) const;
		Type replaceImmediateSubElements(
				ArrayRef<Attribute> replAttrs, ArrayRef<Type> replTypes) const;

		static Type parse(AsmParser &parser);

		Type print(AsmPrinter &p) const;
	};
}	 // namespace mlir::rlc

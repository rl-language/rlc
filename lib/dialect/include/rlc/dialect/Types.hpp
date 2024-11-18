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
#pragma once

#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#include "rlc/dialect/TypeInterfaces.hpp"

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
			llvm::StringRef functionName,
			bool isMemberFunction,
			mlir::FunctionType type);

	std::string typeToMangled(mlir::Type t);

	std::string prettyType(mlir::Type type);

	std::string prettyPrintFunctionTypeWithNameArgs(
			mlir::FunctionType fType, mlir::rlc::FunctionInfoAttr attr);

	class ClassType: public Type::TypeBase<
											 ClassType,
											 Type,
											 StructTypeStorage,
											 TypeTrait::IsMutable,
											 mlir::rlc::RLCSerializable::Trait>
	{
		public:
		/// Inherit base constructors.
		using Base::Base;

		static llvm::StringRef getMnemonic() { return name; }
		constexpr static const char *name = "class";

		/// Gets or creates an identified struct with the given name in the provided
		/// context. Note that unlike llvm::StructType::create, this function will
		/// _NOT_ rename a struct in case a struct with the same name already exists
		/// in the context. Instead, it will just return the existing struct,
		/// similarly to the rest of MLIR type ::get methods.
		static ClassType getIdentified(
				MLIRContext *context,
				StringRef name,
				ArrayRef<Type> explicitTemplateParameters);

		/// Gets a new identified struct with the given body. The body _cannot_ be
		/// changed later. If a struct with the given name already exists, renames
		/// the struct by appending a `.` followed by a number to the name. Renaming
		/// happens even if the existing struct has the same body.
		static ClassType getNewIdentified(
				MLIRContext *context,
				StringRef name,
				ArrayRef<mlir::rlc::ClassFieldAttr> fields,
				ArrayRef<Type> explicitTemplateParameters);

		/// Set the body of an identified struct. Returns failure if the body could
		/// not be set, e.g. if the struct already has a body or if it was marked as
		/// intentionally opaque. This might happen in a multi-threaded context when
		/// a different thread modified the struct after it was created. Most
		/// callers are likely to assert this always succeeds, but it is possible to
		/// implement a local renaming scheme based on the result of this call.
		LogicalResult setBody(ArrayRef<mlir::rlc::ClassFieldAttr> fields);

		/// Checks if a struct is initialized.
		bool isInitialized() const;

		/// Returns the name of an identified struct.
		StringRef getName() const;

		/// Returns the list of element types contained in a non-opaque struct.
		ArrayRef<mlir::rlc::ClassFieldAttr> getMembers() const;
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

		void rlc_serialize(
				llvm::raw_ostream &OS,
				const mlir::rlc::SerializationContext &ctx) const;
	};

	mlir::LogicalResult isTemplateType(mlir::Type type);

	mlir::Type decayCtxFrmType(mlir::Type);

	mlir::FunctionType replaceTemplateParameter(
			mlir::FunctionType original,
			mlir::rlc::TemplateParameterType toReplace,
			mlir::Type replacement);

}	 // namespace mlir::rlc
	 //

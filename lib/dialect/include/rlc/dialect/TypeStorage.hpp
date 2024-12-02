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

#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#include "mlir/Support/StorageUniquer.h"
#include "rlc/dialect/Attrs.hpp"

namespace llvm
{
	// template<>
	// struct DenseMapInfo<, 2>>>
	//{
	// using KeyTy = std::pair<StringRef, SmallVector<mlir::Type, 2>>;

	// static inline KeyTy getEmptyKey()
	//{
	// return std::make_pair(StringRef(), SmallVector<mlir::Type, 2>());
	//}

	// static inline KeyTy getTombstoneKey()
	//{
	// return std::make_pair(
	// StringRef("_______DoNoTUSe"), SmallVector<mlir::Type, 2>({}));
	//}

	// static unsigned getHashValue(const KeyTy &Val)
	//{
	// return llvm::hash_combine(
	// Val.first,
	// llvm::hash_combine_range(Val.second.begin(), Val.second.end()));
	//}

	// static bool isEqual(const KeyTy &LHS, const KeyTy &RHS)
	//{
	// return LHS.first.equals(RHS.first) &&
	// LHS.second.size() == RHS.second.size() &&
	// std::equal(
	// LHS.second.begin(), LHS.second.end(), RHS.second.begin());
	//}
	//};
}	 // end namespace llvm

namespace mlir::rlc
{

	/// Here we define a storage class for a RecursiveType that is identified by
	/// its name and contains another type.
	struct StructTypeStorage: public mlir::TypeStorage
	{
		public:
		struct Key
		{
			public:
			llvm::StringRef name;
			std::optional<llvm::SmallVector<mlir::rlc::ClassFieldAttr, 2>> fields;
			llvm::SmallVector<mlir::Type, 2> explicitTemplateParameters;

			Key(llvm::StringRef name,
					llvm::ArrayRef<Type> explicitTemplateParameters,
					std::optional<llvm::ArrayRef<mlir::rlc::ClassFieldAttr>> fields =
							std::nullopt)
					: name(name),
						fields(fields),
						explicitTemplateParameters(explicitTemplateParameters)
			{
			}

			llvm::hash_code hashValue() const
			{
				auto hashed = llvm::hash_value(name);
				for (auto templateParameter : explicitTemplateParameters)
					hashed = llvm::hash_combine(hashed, templateParameter);
				return hashed;
			}

			llvm::ArrayRef<mlir::rlc::ClassFieldAttr> getFields() const
			{
				assert(isInitialized());
				return *fields;
			}

			llvm::ArrayRef<mlir::Type> getExplicitTemplateParameters() const
			{
				return explicitTemplateParameters;
			}

			/// Define the comparison function.
			bool operator==(const Key &key) const
			{
				return std::tie(key.name, key.explicitTemplateParameters) ==
							 std::tie(name, explicitTemplateParameters);
			}

			[[nodiscard]] bool isInitialized() const { return fields.has_value(); }
		};

		/// The type is uniquely identified by its name. Note that the contained
		/// type is _not_ a part of the key.
		using KeyTy = Key;

		static llvm::hash_code hashKey(const KeyTy &key) { return key.hashValue(); }
		/// Construct the storage from the type name. Explicitly initialize the
		/// containedType to nullptr, which is used as marker for the mutable
		/// component being not yet initialized.
		StructTypeStorage(
				llvm::StringRef name, llvm::ArrayRef<Type> explicitTemplateParameters)
				: content(name, explicitTemplateParameters)
		{
		}

		/// Returns the key for the current storage.
		Key getAsKey() const
		{
			if (content.isInitialized())
				return Key(content.name, getExplicitTemplateParameters(), getFields());
			return Key(content.name, getExplicitTemplateParameters());
		}

		/// Define the comparison function.
		bool operator==(const KeyTy &key) const { return content == key; }

		/// Define a construction method for creating a new instance of the storage.
		static StructTypeStorage *construct(
				mlir::StorageUniquer::StorageAllocator &allocator, const KeyTy &key)
		{
			// Note that the key string is copied into the allocator to ensure it
			// remains live as long as the storage itself.
			auto toReturn =
					new (allocator.allocate<StructTypeStorage>()) StructTypeStorage(
							allocator.copyInto(key.name),
							allocator.copyInto<mlir::Type>(key.explicitTemplateParameters));
			if (key.isInitialized())
			{
				auto result = toReturn->mutate(allocator, *key.fields);
				assert(result.succeeded());
			}

			return toReturn;
		}

		/// Define a mutation method for changing the type after it is created. In
		/// many cases, we only want to set the mutable component once and reject
		/// any further modification, which can be achieved by returning failure
		/// from this function.
		mlir::LogicalResult mutate(
				mlir::StorageUniquer::StorageAllocator &,
				llvm::ArrayRef<mlir::rlc::ClassFieldAttr> fields)
		{
			if (content.fields.has_value() and
					fields == llvm::ArrayRef(*content.fields))
				return mlir::success();

			// If the contained type has been initialized already, and the call tries
			// to change it, reject the change.
			if (content.fields.has_value())
				return mlir::failure();

			this->content.fields.emplace();
			for (const auto &field : fields)
				this->content.fields->push_back(field);
			return mlir::success();
		}

		[[nodiscard]] llvm::StringRef getIdentifier() const { return content.name; }

		[[nodiscard]] bool isInitialized() const { return content.isInitialized(); }

		llvm::ArrayRef<mlir::Type> getExplicitTemplateParameters() const
		{
			return content.getExplicitTemplateParameters();
		}

		llvm::ArrayRef<mlir::rlc::ClassFieldAttr> getFields() const
		{
			return content.getFields();
		}

		private:
		Key content;
	};

}	 // namespace mlir::rlc

namespace mlir
{
	/// Allow walking and replacing the subelements of a LLVMStructTypeStorage
	/// key.
	template<>
	struct AttrTypeSubElementHandler<rlc::StructTypeStorage::Key>
	{
		static void walk(
				const rlc::StructTypeStorage::Key &param,
				AttrTypeImmediateSubElementWalker &walker)
		{
			for (Type type : param.getExplicitTemplateParameters())
				walker.walk(type);

			if (param.isInitialized())
				for (mlir::Attribute type : param.getFields())
					walker.walk(type);
		}
		static FailureOr<rlc::StructTypeStorage::Key> replace(
				const rlc::StructTypeStorage::Key &param,
				AttrSubElementReplacements &attrRepls,
				TypeSubElementReplacements &typeRepls)
		{
			if (param.isInitialized())
			{
				auto templateParameters =
						typeRepls.take_front(param.getExplicitTemplateParameters().size());
				auto members = attrRepls.take_front(param.getFields().size());
				llvm::SmallVector<rlc::ClassFieldAttr> attrs;
				for (auto member : members)
					attrs.push_back(member.cast<rlc::ClassFieldAttr>());

				return rlc::StructTypeStorage::Key(
						param.name, templateParameters, attrs);
			}

			return rlc::StructTypeStorage::Key(
					param.name,
					typeRepls.take_front(param.getExplicitTemplateParameters().size()));
		}
	};
}	 // namespace mlir

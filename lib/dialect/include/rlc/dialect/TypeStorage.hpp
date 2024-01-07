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

#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#include "mlir/Support/StorageUniquer.h"

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
			std::optional<llvm::SmallVector<mlir::Type, 2>> containedTypes;
			llvm::SmallVector<std::string, 2> fieldNames;
			llvm::SmallVector<mlir::Type, 2> explicitTemplateParameters;

			Key(llvm::StringRef name,
					llvm::ArrayRef<Type> explicitTemplateParameters,
					llvm::ArrayRef<std::string> fieldNames = {},
					std::optional<llvm::ArrayRef<mlir::Type>> containedTypes =
							std::nullopt)
					: name(name),
						containedTypes(containedTypes),
						fieldNames(fieldNames),
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

			llvm::ArrayRef<mlir::Type> getBody() const
			{
				assert(isInitialized());
				return *containedTypes;
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

			[[nodiscard]] bool isInitialized() const
			{
				return containedTypes.has_value();
			}
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
				return Key(
						content.name,
						getExplicitTemplateParameters(),
						getFieldNames(),
						getBody());
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
				auto result =
						toReturn->mutate(allocator, key.getBody(), key.fieldNames);
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
				llvm::ArrayRef<mlir::Type> body,
				llvm::ArrayRef<std::string> fieldNames)
		{
			if (content.containedTypes.has_value() and
					body == llvm::ArrayRef(*content.containedTypes))
				return mlir::success();

			// If the contained type has been initialized already, and the call tries
			// to change it, reject the change.
			if (content.containedTypes.has_value())
				return mlir::failure();

			content.containedTypes = llvm::SmallVector<mlir::Type, 2>();
			// Change the body successfully.
			for (auto type : body)
			{
				assert(type != nullptr);
				content.containedTypes->push_back(type);
			}
			this->content.fieldNames.clear();
			for (const auto &field : fieldNames)
				this->content.fieldNames.push_back(field);
			return mlir::success();
		}

		[[nodiscard]] llvm::StringRef getIdentifier() const { return content.name; }

		[[nodiscard]] bool isInitialized() const { return content.isInitialized(); }

		llvm::ArrayRef<std::string> getFieldNames() const
		{
			return content.fieldNames;
		}
		llvm::ArrayRef<mlir::Type> getExplicitTemplateParameters() const
		{
			return content.getExplicitTemplateParameters();
		}

		llvm::ArrayRef<mlir::Type> getBody() const { return content.getBody(); }

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
				for (Type type : param.getBody())
					walker.walk(type);
		}
		static FailureOr<rlc::StructTypeStorage::Key> replace(
				const rlc::StructTypeStorage::Key &param,
				AttrSubElementReplacements &attrRepls,
				TypeSubElementReplacements &typeRepls)
		{
			if (param.isInitialized())
				return rlc::StructTypeStorage::Key(
						param.name,
						typeRepls.take_front(param.getExplicitTemplateParameters().size()),
						param.fieldNames,
						typeRepls.take_front(param.getBody().size()));

			return rlc::StructTypeStorage::Key(
					param.name,
					typeRepls.take_front(param.getExplicitTemplateParameters().size()));
		}
	};
}	 // namespace mlir

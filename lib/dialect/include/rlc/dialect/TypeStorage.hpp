#pragma once

#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#include "mlir/Support/StorageUniquer.h"

namespace llvm
{
	template<>
	struct DenseMapInfo<std::pair<StringRef, SmallVector<mlir::Type, 2>>>
	{
		using KeyTy = std::pair<StringRef, SmallVector<mlir::Type, 2>>;

		static inline KeyTy getEmptyKey()
		{
			return std::make_pair(StringRef(), SmallVector<mlir::Type, 2>());
		}

		static inline KeyTy getTombstoneKey()
		{
			return std::make_pair(
					StringRef("_______DoNoTUSe"), SmallVector<mlir::Type, 2>({}));
		}

		static unsigned getHashValue(const KeyTy &Val)
		{
			return llvm::hash_combine(
					Val.first,
					llvm::hash_combine_range(Val.second.begin(), Val.second.end()));
		}

		static bool isEqual(const KeyTy &LHS, const KeyTy &RHS)
		{
			return LHS.first.equals(RHS.first) &&
						 LHS.second.size() == RHS.second.size() &&
						 std::equal(
								 LHS.second.begin(), LHS.second.end(), RHS.second.begin());
		}
	};
}	 // end namespace llvm

namespace mlir::rlc
{

	/// Here we define a storage class for a RecursiveType that is identified by
	/// its name and contains another type.
	struct StructTypeStorage: public mlir::TypeStorage
	{
		public:
		/// The type is uniquely identified by its name. Note that the contained
		/// type is _not_ a part of the key.
		using KeyTy = std::pair<llvm::StringRef, llvm::SmallVector<mlir::Type, 2>>;

		/// Construct the storage from the type name. Explicitly initialize the
		/// containedType to nullptr, which is used as marker for the mutable
		/// component being not yet initialized.
		StructTypeStorage(
				llvm::StringRef name, llvm::ArrayRef<Type> explicitTemplateParameters)
				: name(name), explicitTemplateParameters(explicitTemplateParameters)
		{
		}

		/// Define the comparison function.
		bool operator==(const KeyTy &key) const
		{
			return std::tie(key.first, key.second) ==
						 std::tie(name, explicitTemplateParameters);
		}

		/// Define a construction method for creating a new instance of the storage.
		static StructTypeStorage *construct(
				mlir::StorageUniquer::StorageAllocator &allocator, const KeyTy &key)
		{
			// Note that the key string is copied into the allocator to ensure it
			// remains live as long as the storage itself.
			return new (allocator.allocate<StructTypeStorage>()) StructTypeStorage(
					allocator.copyInto(key.first),
					allocator.copyInto<mlir::Type>(key.second));
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
			if (containedTypes.has_value() and body == *containedTypes)
				return mlir::success();

			// If the contained type has been initialized already, and the call tries
			// to change it, reject the change.
			if (containedTypes.has_value())
				return mlir::failure();

			containedTypes = llvm::SmallVector<mlir::Type, 2>();
			// Change the body successfully.
			for (auto type : body)
				containedTypes->push_back(type);
			this->fieldNames.clear();
			for (const auto &field : fieldNames)
				this->fieldNames.push_back(field);
			return mlir::success();
		}

		[[nodiscard]] llvm::StringRef getIdentifier() const { return name; }

		[[nodiscard]] bool isInitialized() const
		{
			return containedTypes.has_value();
		}

		llvm::ArrayRef<std::string> getFieldNames() const { return fieldNames; }
		llvm::ArrayRef<mlir::Type> getExplicitTemplateParameters() const
		{
			return explicitTemplateParameters;
		}

		llvm::ArrayRef<mlir::Type> getBody() const { return *containedTypes; }

		private:
		llvm::StringRef name;
		llvm::Optional<llvm::SmallVector<mlir::Type, 2>> containedTypes;
		llvm::SmallVector<std::string, 2> fieldNames;
		llvm::SmallVector<mlir::Type, 2> explicitTemplateParameters;
	};

}	 // namespace mlir::rlc

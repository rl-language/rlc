#pragma once

#include "mlir/IR/TypeSupport.h"
#include "mlir/IR/Types.h"
#include "mlir/Support/StorageUniquer.h"

namespace mlir::rlc
{

	/// Here we define a storage class for a RecursiveType that is identified by
	/// its name and contains another type.
	struct StructTypeStorage: public mlir::TypeStorage
	{
		/// The type is uniquely identified by its name. Note that the contained
		/// type is _not_ a part of the key.
		using KeyTy = llvm::StringRef;

		/// Construct the storage from the type name. Explicitly initialize the
		/// containedType to nullptr, which is used as marker for the mutable
		/// component being not yet initialized.
		StructTypeStorage(llvm::StringRef name): name(name) {}

		/// Define the comparison function.
		bool operator==(const KeyTy &key) const { return key == name; }

		/// Define a construction method for creating a new instance of the storage.
		static StructTypeStorage *construct(
				mlir::StorageUniquer::StorageAllocator &allocator, const KeyTy &key)
		{
			// Note that the key string is copied into the allocator to ensure it
			// remains live as long as the storage itself.
			return new (allocator.allocate<StructTypeStorage>())
					StructTypeStorage(allocator.copyInto(key));
		}

		/// Define a mutation method for changing the type after it is created. In
		/// many cases, we only want to set the mutable component once and reject
		/// any further modification, which can be achieved by returning failure
		/// from this function.
		mlir::LogicalResult mutate(
				mlir::StorageUniquer::StorageAllocator &,
				llvm::ArrayRef<mlir::Type> body)
		{
			if (containedTypes.hasValue() and body == *containedTypes)
				return mlir::success();

			// If the contained type has been initialized already, and the call tries
			// to change it, reject the change.
			if (containedTypes.hasValue())
				return mlir::failure();

			containedTypes = llvm::SmallVector<mlir::Type, 2>();
			// Change the body successfully.
			for (auto type : body)
				containedTypes->push_back(type);
			return mlir::success();
		}

		[[nodiscard]] llvm::StringRef getIdentifier() const { return name; }

		[[nodiscard]] bool isInitialized() const
		{
			return containedTypes.hasValue();
		}

		llvm::ArrayRef<mlir::Type> getBody() const
		{
			return containedTypes.getValue();
		}

		private:
		llvm::StringRef name;
		llvm::Optional<llvm::SmallVector<mlir::Type, 2>> containedTypes;
	};

}	 // namespace mlir::rlc

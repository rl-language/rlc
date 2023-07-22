#pragma once

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "rlc/dialect/Operations.hpp"

namespace llvm
{

	// template<>
	// struct GraphTraits<mlir::rlc::EntityType>
	//{
	// using NodeRef = mlir::Type;
	// using ChildIteratorType = llvm::ArrayRef<mlir::Type>::iterator;

	// static NodeRef getEntryNode(mlir::rlc::EntityType N) { return N; }

	// static ChildIteratorType child_begin(NodeRef N)
	//{
	// if (auto cast)
	// if (not N.cast<mlir::SubElementTypeInterface>())
	// return nullptr;

	// return N.cast<mlir::rlc::EntityType>().getBody().begin();
	//}

	// static ChildIteratorType child_end(NodeRef N)
	//{
	// if (not N.isa<mlir::rlc::EntityType>())
	// return nullptr;

	// return N.cast<mlir::rlc::EntityType>().getBody().begin();
	//}
	//};
}	 // namespace llvm

namespace rlc
{
	/// returns in post order (that is: contained elements first) each type of the
	/// type system exactly once
	inline llvm::SmallVector<mlir::Type, 4> postOrderTypes(mlir::ModuleOp module)
	{
		llvm::SmallVector<mlir::Type, 4> subElements(
				{ mlir::rlc::FloatType::get(module.getContext()),
					mlir::rlc::IntegerType::getInt64(module.getContext()),
					mlir::rlc::IntegerType::getInt8(module.getContext()),
					mlir::rlc::BoolType::get(module.getContext()) });
		llvm::DenseSet<mlir::Type> emitted;
		for (auto& elem : subElements)
			emitted.insert(elem);

		for (auto decl : module.getOps<mlir::rlc::EntityDeclaration>())
		{
			auto topLevelType = decl.getType().cast<mlir::rlc::EntityType>();
			if (emitted.contains(topLevelType))
				continue;
			emitted.insert(topLevelType);

			subElements.push_back(topLevelType);

			topLevelType.walkSubTypes([&](mlir::Type type) {
				if (emitted.contains(type))
					return;

				subElements.push_back(type);
				emitted.insert(type);
			});
		}

		return subElements;
	}
}	 // namespace rlc

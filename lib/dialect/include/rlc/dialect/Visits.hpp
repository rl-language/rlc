#pragma once

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "rlc/dialect/Operations.hpp"
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

		auto visit = [&](mlir::Type type) {
			if (emitted.contains(type))
				return;

			subElements.push_back(type);
			emitted.insert(type);
		};

		for (auto decl : module.getOps<mlir::rlc::EntityDeclaration>())
		{
			auto topLevelType = decl.getType().cast<mlir::rlc::EntityType>();
			if (emitted.contains(topLevelType))
				continue;

			subElements.push_back(topLevelType);
			emitted.insert(topLevelType);

			topLevelType.walk(visit);
		}

		for (auto f : module.getOps<mlir::rlc::FunctionOp>())
		{
			if (emitted.contains(f.getType()))
				continue;

			subElements.push_back(f.getType());
			emitted.insert(f.getType());

			f.getType().walk(visit);
		}

		return subElements;
	}
}	 // namespace rlc

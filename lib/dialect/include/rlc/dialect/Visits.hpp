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

			emitted.insert(topLevelType);

			topLevelType.walk(visit);

			subElements.push_back(topLevelType);
		}

		for (auto f : module.getOps<mlir::rlc::FunctionOp>())
		{
			if (emitted.contains(f.getType()))
				continue;

			emitted.insert(f.getType());

			f.getType().walk(visit);

			subElements.push_back(f.getType());
		}

		return subElements;
	}
}	 // namespace rlc

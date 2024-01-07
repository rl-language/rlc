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

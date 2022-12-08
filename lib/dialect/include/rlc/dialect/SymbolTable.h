#pragma once

#include "llvm/ADT/StringMap.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"

namespace mlir::rlc
{
	template<typename T>
	class SymbolTable
	{
		public:
		SymbolTable(SymbolTable* parent = nullptr): parent(parent) {}

		T getOne(llvm::StringRef name) const
		{
			auto iter = values.find(name);
			if (iter == values.end())
			{
				if (parent == nullptr)
					return nullptr;
				return parent->getOne(name);
			}
			if (iter->second.size() >= 2)
				return nullptr;

			return iter->second.front();
		}

		llvm::ArrayRef<T> get(llvm::StringRef name) const
		{
			auto iter = values.find(name);
			if (iter == values.end())
			{
				if (parent == nullptr)
					return {};
				return parent->get(name);
			}
			return iter->second;
		}

		void add(llvm::StringRef name, T value) { values[name].push_back(value); }

		private:
		SymbolTable* parent;
		llvm::StringMap<llvm::SmallVector<T, 2>> values;
	};
	using ValueTable = SymbolTable<mlir::Value>;
	using TypeTable = SymbolTable<mlir::Type>;

	mlir::Value findOverload(
			mlir::Operation& errorEmitter,
			ValueTable& table,
			llvm::StringRef name,
			mlir::TypeRange arguments);

	llvm::SmallVector<mlir::Value, 2> findOverloads(
			ValueTable& table, llvm::StringRef name, mlir::TypeRange arguments);
}	 // namespace mlir::rlc

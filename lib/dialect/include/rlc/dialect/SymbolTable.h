#pragma once

#include "llvm/ADT/StringMap.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/Transforms/DialectConversion.h"

namespace mlir::rlc
{
	class ActionFunction;

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

	ValueTable makeValueTable(mlir::ModuleOp);
	TypeTable makeTypeTable(mlir::ModuleOp);

	mlir::Value findOverload(
			mlir::Operation& errorEmitter,
			ValueTable& table,
			llvm::StringRef name,
			mlir::TypeRange arguments);

	llvm::SmallVector<mlir::Value, 2> findOverloads(
			ValueTable& table, llvm::StringRef name, mlir::TypeRange arguments);

	class ModuleBuilder
	{
		public:
		ModuleBuilder(mlir::ModuleOp op);

		ValueTable& getSymbolTable() { return values; }
		mlir::TypeConverter& getConverter() { return converter; }

		mlir::Value getAssignFunctionOf(mlir::Type type)
		{
			assert(typeToAssignFunction.count(type) != 0);
			return typeToAssignFunction[type];
		}

		mlir::Value getInitFunctionOf(mlir::Type type)
		{
			assert(typeToInitFunction.count(type) != 0);
			return typeToInitFunction[type];
		}

		bool isEntityOfAction(mlir::Type type)
		{
			return actionTypeToAction.count(type) != 0;
		}

		mlir::Type typeOfAction(mlir::rlc::ActionFunction& f);
		llvm::iterator_range<mlir::Operation**> actionStatementsOfAction(
				mlir::rlc::ActionFunction& val);

		llvm::iterator_range<std::string*> actionNames(mlir::Value val)
		{
			return actionDeclToActionNames[val];
		}

		private:
		mlir::ModuleOp op;
		ValueTable values;
		TypeTable types;
		mlir::TypeConverter converter;
		llvm::DenseMap<mlir::Type, mlir::Value> typeToInitFunction;
		llvm::DenseMap<mlir::Type, mlir::Value> typeToAssignFunction;
		llvm::DenseMap<mlir::Type, mlir::Value> actionTypeToAction;
		llvm::DenseMap<mlir::Value, mlir::Type> actionToActionType;
		llvm::DenseMap<mlir::Value, llvm::SmallVector<mlir::Operation*, 4>>
				actionDeclToActionStatements;
		llvm::DenseMap<mlir::Value, llvm::SmallVector<std::string, 4>>
				actionDeclToActionNames;
	};
}	 // namespace mlir::rlc

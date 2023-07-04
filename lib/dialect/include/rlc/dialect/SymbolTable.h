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
	class TraitDefinition;
	class ActionFunction;
	class TraitMetaType;
	class TraitMetaType;

	template<typename T>
	class SymbolTable
	{
		public:
		explicit SymbolTable(SymbolTable* parent): parent(parent) {}
		SymbolTable(): parent(nullptr) {}

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

		void add(llvm::StringRef name, T value)
		{
			values[name].push_back(value);
			reverseValues[value.getAsOpaquePointer()] = name.str();
		}

		const llvm::StringMap<llvm::SmallVector<T, 2>>& allDirectValues()
		{
			return values;
		}

		[[nodiscard]] llvm::StringRef lookUpValue(T value) const
		{
			if (auto iter = reverseValues.find(value.getAsOpaquePointer());
					iter != reverseValues.end())
				return iter->second;

			if (parent == nullptr)
				return "";

			return parent->lookUpValue(value);
		}

		private:
		SymbolTable* parent;
		llvm::StringMap<llvm::SmallVector<T, 2>> values;
		std::map<const void*, std::string> reverseValues;
	};
	using ValueTable = SymbolTable<mlir::Value>;
	using TypeTable = SymbolTable<mlir::Type>;

	ValueTable makeValueTable(mlir::ModuleOp);
	TypeTable makeTypeTable(mlir::ModuleOp);

	class RLCTypeConverter
	{
		public:
		explicit RLCTypeConverter(mlir::ModuleOp op);
		explicit RLCTypeConverter(RLCTypeConverter* parentScopeConverter);
		RLCTypeConverter(const RLCTypeConverter&) = delete;
		RLCTypeConverter(RLCTypeConverter&&) = delete;
		RLCTypeConverter& operator=(const RLCTypeConverter&) = delete;
		RLCTypeConverter& operator=(RLCTypeConverter&&) = delete;
		~RLCTypeConverter() = default;

		mlir::TypeConverter& getConverter() { return converter; }
		const TypeTable& getTypes() const { return types; }
		TypeTable& getTypes() { return types; }
		void registerType(llvm::StringRef name, mlir::Type type)
		{
			types.add(name, type);
		}

		mlir::Type convertType(mlir::Type type)
		{
			return converter.convertType(type);
		}

		private:
		TypeTable types;
		mlir::TypeConverter converter;
	};

	class ModuleBuilder
	{
		private:
		class SymbolTableRAIIDeleater
		{
			public:
			SymbolTableRAIIDeleater(ModuleBuilder* builder): builder(builder)
			{
				builder->pushSymbolTable();
			}
			~SymbolTableRAIIDeleater() { builder->popSymbolTable(); }

			private:
			ModuleBuilder* builder;
		};
		void pushSymbolTable()
		{
			values.emplace_back(std::make_unique<ValueTable>(&getSymbolTable()));
			converter.emplace_back(
					std::make_unique<RLCTypeConverter>(&getConverter()));
		}
		void popSymbolTable()
		{
			assert(not values.empty());
			values.pop_back();
			converter.pop_back();
		}

		public:
		[[nodiscard]] SymbolTableRAIIDeleater addSymbolTable()
		{
			return SymbolTableRAIIDeleater(this);
		}

		ModuleBuilder(mlir::ModuleOp op);

		mlir::IRRewriter& getRewriter() { return rewriter; }

		ValueTable& getSymbolTable() { return *values.back(); }
		mlir::rlc::RLCTypeConverter& getConverter() { return *converter.back(); }

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

		llvm::ArrayRef<mlir::Value> getNonInitOrAssignFunctionOf(mlir::Type type)
		{
			assert(typeToInitFunction.count(type) != 0);
			return typeToFunctions[type];
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

		mlir::Operation* actionFunctionValueToActionStatement(mlir::Value val)
		{
			return actionFunctionResultToActionStement[val];
		}

		llvm::ArrayRef<mlir::Value> getActionsOrZeroArityFunctions()
		{
			return actionsAndZeroParametersFunctions;
		}

		mlir::rlc::TraitDefinition getTraitDefinition(
				mlir::rlc::TraitMetaType type);

		void addTraitToAviableOverloads(mlir::rlc::TraitMetaType trait);

		private:
		mlir::ModuleOp op;
		mlir::IRRewriter rewriter;
		std::vector<std::unique_ptr<ValueTable>> values;
		std::vector<std::unique_ptr<RLCTypeConverter>> converter;
		llvm::DenseMap<mlir::Type, mlir::Value> typeToInitFunction;
		llvm::DenseMap<mlir::Type, mlir::Value> typeToAssignFunction;
		llvm::DenseMap<mlir::Type, mlir::Value> actionTypeToAction;
		llvm::DenseMap<mlir::Value, mlir::Type> actionToActionType;
		std::vector<mlir::Value> actionsAndZeroParametersFunctions;
		llvm::DenseMap<mlir::Value, mlir::Operation*>
				actionFunctionResultToActionStement;
		llvm::DenseMap<mlir::Type, llvm::SmallVector<mlir::Value, 4>>
				typeToFunctions;
		llvm::DenseMap<mlir::Value, llvm::SmallVector<mlir::Operation*, 4>>
				actionDeclToActionStatements;
		llvm::DenseMap<mlir::Value, llvm::SmallVector<std::string, 4>>
				actionDeclToActionNames;
		llvm::DenseMap<mlir::rlc::TraitMetaType, mlir::rlc::TraitDefinition> traits;
	};
}	 // namespace mlir::rlc

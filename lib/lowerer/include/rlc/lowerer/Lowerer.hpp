#pragma once

#include <memory>

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Operation.h"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/dialect/Operations.hpp"

namespace rlc
{
	using FuncType = mlir::rlc::FunctionOp;
	mlir::Location sourcePositionToLocation(
			const SourcePosition& location, mlir::MLIRContext* context);
	class Lowerer
	{
		public:
		llvm::Error lowerSystem(const System& system);
		llvm::Error lowerEntity(const Entity& entity, mlir::ModuleOp& module);
		llvm::Expected<FuncType> lowerDeclaration(
				const FunctionDeclaration& decl, mlir::ModuleOp& module);

		FuncType makeFunction(
				mlir::OpBuilder& builder, const FunctionDeclaration& decl);

		void lowerBuiltinFunctions(
				const System& system, mlir::ModuleOp& module) const;

		void lowerBuiltinFunctionsRlc(
				const System& system, mlir::ModuleOp& module) const;

		mlir::Value emitGetResumePointIndex(
				mlir::OpBuilder& builder,
				SymbolTable& table,
				mlir::Location position) const;

		llvm::Error emitResumePoints(
				const FunctionDefinition& definition,
				mlir::OpBuilder& builder,
				SymbolTable& table);

		llvm::Expected<FuncType> lowerDefinition(
				const FunctionDefinition& definition,
				mlir::ModuleOp& module,
				SymbolTable& table);

		llvm::Expected<FuncType> lowerDefinitionToRlc(
				const FunctionDefinition& definition,
				mlir::ModuleOp& module,
				SymbolTable& table);

		llvm::Error declareOpaqueStruct(
				const Entity& entity, mlir::ModuleOp& module);

		llvm::Expected<mlir::OpResult> lowerExpression(
				const Expression& expression, mlir::ModuleOp& module);

		[[nodiscard]] mlir::Type rlcToLlvmType(rlc::Type* t) const;

		[[nodiscard]] const mlir::ModuleOp& getModule(size_t index) const
		{
			return modules[index];
		}

		[[nodiscard]] size_t modulesCount() const { return modules.size(); }

		template<typename T = Statement>
		llvm::Error lowerStatementToRlc(
				const Statement&, mlir::OpBuilder& builder, SymbolTable& table) const;

		template<typename T = Expression>
		llvm::Expected<mlir::Value> lowerExpression(
				const Expression&, mlir::OpBuilder& builder, SymbolTable& table) const;

		template<typename T = Expression>
		llvm::Expected<mlir::Value> lowerExpressionRlc(
				const Expression&, mlir::OpBuilder& builder, SymbolTable& table) const;

		/**
		 * returns true if there were no errors
		 */
		bool verify(llvm::raw_ostream& OS);

		mlir::MLIRContext& getContext() const { return *context; }

		Lowerer(mlir::MLIRContext& context, TypeDB& db)
				: context(&context), tybeDB(&db)
		{
		}

		mlir::Value lowerAlloca(
				Type* type,
				mlir::OpBuilder& builder,
				const SourcePosition& position) const;

		void lowerMain(
				const System& system, mlir::ModuleOp module, SymbolTable& table) const;

		FuncType getAssignFunctionOf(Type* tp, SymbolTable& table) const;

		mlir::Value emitMemberAccess(
				mlir::OpBuilder& builder,
				mlir::Value pointerToStruct,
				rlc::Type* typeOfStruct,
				llvm::StringRef memberName,
				mlir::Location location) const;

		private:
		[[nodiscard]] mlir::Type uncahedRlcToLlvmType(rlc::Type* t) const;
		[[nodiscard]] mlir::Type rlcBuiltinTollvmType(rlc::Type* t) const;
		[[nodiscard]] mlir::FunctionType rlcFunctionTypeToLlvmType(
				rlc::Type* t) const;
		mlir::MLIRContext* context;
		llvm::SmallVector<mlir::ModuleOp, 2> modules;
		mutable llvm::DenseMap<rlc::Type*, mlir::Type> typeToTypeMap;
		mutable llvm::DenseMap<const rlc::ActionStatement*, mlir::Block*>
				actionToResumeRegion;
		mutable std::map<Symbol, std::variant<mlir::Value, FuncType>> symbolToValue;
		TypeDB* tybeDB;
	};

	template<>
	llvm::Error Lowerer::lowerStatementToRlc<Statement>(
			const Statement& statement,
			mlir::OpBuilder& builder,
			SymbolTable& table) const;

	template<>
	llvm::Expected<mlir::Value> Lowerer::lowerExpressionRlc<Expression>(
			const Expression&, mlir::OpBuilder& builder, SymbolTable& table) const;

	template<>
	llvm::Expected<mlir::Value> Lowerer::lowerExpression<Expression>(
			const Expression&, mlir::OpBuilder& builder, SymbolTable& table) const;
}	 // namespace rlc

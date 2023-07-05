#pragma once

#include <variant>

#include "llvm/ADT/ArrayRef.h"
#include "mlir/IR/FunctionInterfaces.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "rlc/dialect/Interfaces.hpp"
#include "rlc/dialect/OverloadResolver.hpp"
#include "rlc/dialect/Types.hpp"

namespace mlir::rlc::detail
{
	template<typename Op>
	mlir::LogicalResult handleBuiltin(
			Op op,
			mlir::IRRewriter& rewriter,
			mlir::Type expectedType,
			mlir::Type resType = nullptr)
	{
		mlir::SmallVector<mlir::Type, 4> operandTypes;
		mlir::SmallVector<mlir::Value, 4> operandValue;
		for (mlir::OpOperand& t : op.getOperation()->getOpOperands())
		{
			operandTypes.push_back(t.get().getType());
			operandValue.push_back(t.get());
		}

		if (llvm::all_of(operandTypes, [expectedType](mlir::Type in) {
					return in == expectedType;
				}))
		{
			rewriter.replaceOpWithNewOp<Op>(
					op, resType != nullptr ? resType : expectedType, operandValue);
			return mlir::success();
		}
		return mlir::failure();
	}

	template<typename Op>
	mlir::LogicalResult typeCheckInteralOp(
			Op op,
			mlir::rlc::ModuleBuilder& builder,
			mlir::TypeRange accetableTypes,
			mlir::Type res = nullptr);

}	 // namespace mlir::rlc::detail

namespace mlir::rlc
{

	mlir::LogicalResult typeCheck(
			mlir::Operation& op, mlir::rlc::ModuleBuilder& builder);
}	 // namespace mlir::rlc

#define GET_OP_CLASSES
#include "rlc/dialect/Operations.inc"

namespace mlir::rlc
{

	template<typename T>
	inline std::string builtinOperatorName()
	{
		return T::getOperationName()
				.drop_front(mlir::rlc::RLCDialect::getDialectNamespace().size() + 1)
				.str();
	}

	void lowerIsOperations(mlir::Operation* op, mlir::rlc::ValueTable table);
	void lowerConstructOps(
			mlir::rlc::ModuleBuilder& builder, mlir::Operation* op);
	void lowerAssignOps(mlir::rlc::ModuleBuilder& builder, mlir::Operation* op);

}	 // namespace mlir::rlc

namespace mlir::rlc::detail
{
	static bool areArgsArrayAndCompatible(mlir::ValueRange args)
	{
		assert(not args.empty());
		if (not(*args.begin()).getType().isa<mlir::rlc::ArrayType>())
			return false;

		auto currentSize =
				(*args.begin()).getType().cast<mlir::rlc::ArrayType>().getSize();

		for (auto arg : llvm::drop_begin(args))
		{
			auto casted = arg.getType().dyn_cast<mlir::rlc::ArrayType>();
			if (casted == nullptr)
				return false;
			if (casted.getSize() != currentSize)
				return false;
		}

		return true;
	}

	template<typename Op>
	mlir::LogicalResult typeCheckInteralOp(
			Op op,
			mlir::rlc::ModuleBuilder& builder,
			mlir::TypeRange accetableTypes,
			mlir::Type resType)
	{
		auto& rewriter = builder.getRewriter();
		std::string opName = builtinOperatorName<Op>();

		mlir::SmallVector<mlir::Type, 4> operandTypes;
		mlir::SmallVector<mlir::Value, 4> operandValues;

		for (mlir::OpOperand& t : op.getOperation()->getOpOperands())
		{
			operandTypes.push_back(t.get().getType());
			operandValues.push_back(t.get());
		}

		if (llvm::any_of(operandTypes, [&](mlir::Type t) {
					return t.isa<mlir::rlc::UnknownType>();
				}))
		{
			op.emitError("argument op operation had unknown type");
			return mlir::failure();
		}

		// if it's builtin, early out
		for (auto type : accetableTypes)
			if (handleBuiltin(op, rewriter, type, resType).succeeded())
				return mlir::success();

		bool isArrayInvocation = areArgsArrayAndCompatible(operandValues);

		mlir::SmallVector<mlir::Type, 4> lookUpUperandTypes(operandTypes);
		if (isArrayInvocation)
		{
			for (auto& operand : lookUpUperandTypes)
				operand = operand.cast<mlir::rlc::ArrayType>().getUnderlying();
		}

		mlir::rlc::OverloadResolver resolver(builder.getSymbolTable(), op);
		rewriter.setInsertionPoint(op);
		auto overload = resolver.instantiateOverload(
				rewriter, op.getLoc(), opName, lookUpUperandTypes);
		if (overload == nullptr)
			return mlir::failure();

		if (isArrayInvocation)
			rewriter.replaceOpWithNewOp<mlir::rlc::ArrayCallOp>(
					op, overload, operandValues);
		else
			rewriter.replaceOpWithNewOp<mlir::rlc::CallOp>(
					op, overload, operandValues);
		return mlir::success();
	}

}	 // namespace mlir::rlc::detail

namespace rlc
{
	using StatementTypes = std::variant<
			mlir::rlc::StatementList,
			mlir::rlc::ExpressionStatement,
			mlir::rlc::DeclarationStatement,
			mlir::rlc::IfStatement,
			mlir::rlc::ReturnStatement,
			mlir::rlc::WhileStatement,
			mlir::rlc::ActionStatement>;
}

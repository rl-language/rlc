#pragma once

#include "mlir/Dialect/LLVMIR/LLVMTypes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Types.hpp"

namespace mlir::rlc
{
	inline llvm::Optional<mlir::Type> boolToBuiltinBool(mlir::rlc::BoolType type)
	{
		return mlir::LLVM::LLVMPointerType::get(
				mlir::IntegerType::get(type.getContext(), 8));
	}

	inline llvm::Optional<mlir::Type> floatToBuiltinFloat(
			mlir::rlc::FloatType type)
	{
		return mlir::LLVM::LLVMPointerType::get(
				mlir::Float64Type::get(type.getContext()));
	}

	inline llvm::Optional<mlir::Type> intToBuiltinInt(mlir::rlc::IntegerType type)
	{
		return mlir::LLVM::LLVMPointerType::get(
				mlir::IntegerType::get(type.getContext(), type.getSize()));
	}

	inline llvm::Optional<mlir::Type> acceptPtrType(
			mlir::LLVM::LLVMPointerType type)
	{
		return type;
	}

	inline llvm::Optional<mlir::Type> acceptFloat(mlir::FloatType type)
	{
		return type;
	}

	inline llvm::Optional<mlir::Type> acceptInt(mlir::IntegerType type)
	{
		return type;
	}

	inline llvm::Optional<mlir::Type> acceptStrctType(
			mlir::LLVM::LLVMStructType type)
	{
		return type;
	}

	inline llvm::Optional<mlir::Type> voidToVoid(mlir::rlc::VoidType type)
	{
		return mlir::LLVM::LLVMVoidType::get(type.getContext());
	}

	inline llvm::Optional<mlir::Type> acceptVoidType(
			mlir::LLVM::LLVMVoidType type)
	{
		return type;
	}

	inline llvm::Optional<mlir::Type> acceptLLVMArrayType(
			mlir::LLVM::LLVMArrayType type)
	{
		return type;
	}

	inline llvm::Optional<mlir::Type> acceptLLVMFunctionType(
			mlir::LLVM::LLVMFunctionType type)
	{
		return type;
	}

	inline void registerConversions(TypeConverter& converter)
	{
		converter.addConversion(boolToBuiltinBool);
		converter.addConversion(floatToBuiltinFloat);
		converter.addConversion(intToBuiltinInt);
		converter.addConversion([&](mlir::rlc::OwningPtrType type) -> Type {
			assert(type.getUnderlying() != nullptr);
			auto newInner = converter.convertType(type.getUnderlying());
			return mlir::LLVM::LLVMPointerType::get(newInner);
		});
		converter.addConversion([&](mlir::rlc::ArrayType type) -> Type {
			auto newInner = converter.convertType(type.getUnderlying())
													.cast<mlir::LLVM::LLVMPointerType>()
													.getElementType();
			assert(type.getSize().isa<mlir::rlc::IntegerLiteralType>());
			return mlir::LLVM::LLVMPointerType::get(mlir::LLVM::LLVMArrayType::get(
					newInner,
					type.getSize().cast<mlir::rlc::IntegerLiteralType>().getValue()));
		});
		converter.addConversion([&](mlir::FunctionType type) -> Type {
			SmallVector<Type, 2> args;
			for (auto arg : type.getInputs())
				args.push_back(converter.convertType(arg));

			SmallVector<Type, 2> res;
			for (auto arg : type.getResults())
			{
				if (arg.isa<mlir::rlc::VoidType>())
					res.push_back(mlir::LLVM::LLVMVoidType::get(type.getContext()));
				else
					res.push_back(converter.convertType(arg)
														.cast<mlir::LLVM::LLVMPointerType>()
														.getElementType());
			}

			if (res.empty())
				res.push_back(mlir::LLVM::LLVMVoidType::get(type.getContext()));

			assert(res.size() == 1);
			return mlir::LLVM::LLVMPointerType::get(
					mlir::LLVM::LLVMFunctionType::get(res.front(), args));
		});
		converter.addConversion([&](mlir::rlc::EntityType type) -> Type {
			SmallVector<Type, 2> fields;
			for (auto arg : type.getBody())
				fields.push_back(converter.convertType(arg)
														 .cast<mlir::LLVM::LLVMPointerType>()
														 .getElementType());

			return mlir::LLVM::LLVMPointerType::get(
					mlir::LLVM::LLVMStructType::getNewIdentified(
							type.getContext(), type.getName(), fields));
		});
		converter.addConversion(voidToVoid);
	}
}	 // namespace mlir::rlc

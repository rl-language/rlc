#pragma once

#include "mlir/Dialect/LLVMIR/LLVMTypes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Types.hpp"

namespace mlir::rlc
{
	inline std::optional<mlir::Type> boolToBuiltinBool(mlir::rlc::BoolType type)
	{
		return mlir::IntegerType::get(type.getContext(), 8);
	}

	inline std::optional<mlir::Type> floatToBuiltinFloat(
			mlir::rlc::FloatType type)
	{
		return mlir::Float64Type::get(type.getContext());
	}

	inline std::optional<mlir::Type> intToBuiltinInt(mlir::rlc::IntegerType type)
	{
		return mlir::IntegerType::get(type.getContext(), type.getSize());
	}

	inline std::optional<mlir::Type> acceptPtrType(
			mlir::LLVM::LLVMPointerType type)
	{
		return type;
	}

	inline std::optional<mlir::Type> acceptFloat(mlir::FloatType type)
	{
		return type;
	}

	inline std::optional<mlir::Type> acceptInt(mlir::IntegerType type)
	{
		return type;
	}

	inline std::optional<mlir::Type> acceptStrctType(
			mlir::LLVM::LLVMStructType type)
	{
		return type;
	}

	inline std::optional<mlir::Type> voidToVoid(mlir::rlc::VoidType type)
	{
		return mlir::LLVM::LLVMVoidType::get(type.getContext());
	}

	inline std::optional<mlir::Type> acceptVoidType(mlir::LLVM::LLVMVoidType type)
	{
		return type;
	}

	inline std::optional<mlir::Type> acceptLLVMArrayType(
			mlir::LLVM::LLVMArrayType type)
	{
		return type;
	}

	inline std::optional<mlir::Type> acceptLLVMFunctionType(
			mlir::LLVM::LLVMFunctionType type)
	{
		return type;
	}

	inline void registerConversions(
			TypeConverter& converter, mlir::ModuleOp module)
	{
		converter.addConversion(boolToBuiltinBool);
		converter.addConversion(floatToBuiltinFloat);
		converter.addConversion(intToBuiltinInt);
		converter.addConversion(acceptPtrType);
		converter.addConversion([&](mlir::rlc::StringLiteralType type) -> Type {
			return mlir::LLVM::LLVMPointerType::get(type.getContext());
		});
		converter.addConversion([&](mlir::rlc::OwningPtrType type) -> Type {
			assert(type.getUnderlying() != nullptr);
			return mlir::LLVM::LLVMPointerType::get(type.getContext());
		});
		converter.addConversion([&](mlir::rlc::ArrayType type) -> Type {
			auto newInner = converter.convertType(type.getUnderlying());
			assert(type.getSize().isa<mlir::rlc::IntegerLiteralType>());
			return mlir::LLVM::LLVMArrayType::get(
					newInner,
					type.getSize().cast<mlir::rlc::IntegerLiteralType>().getValue());
		});
		converter.addConversion(
				[&converter, module](mlir::rlc::AlternativeType type) -> Type {
					llvm::SmallVector<mlir::Type> innerTypes;
					llvm::TypeSize size(0, false);
					unsigned int aligment = 0;
					mlir::Type maxAligmentType = nullptr;
					const auto& dataLayaout = mlir::DataLayout::closest(module);
					for (auto subType : type.getUnderlying())
					{
						auto result = converter.convertType(subType);
						assert(result != nullptr);
						innerTypes.push_back(result);
						size = std::max(size, dataLayaout.getTypeSize(result));
						if (auto maybeNewAlign =
										dataLayaout.getTypePreferredAlignment(result);
								aligment < maybeNewAlign)
						{
							aligment = maybeNewAlign;
							maxAligmentType = result;
						}
					}
					llvm::SmallVector<mlir::Type, 4> newBody;
					newBody.push_back(maxAligmentType);

					assert(maxAligmentType != nullptr);
					newBody.push_back(mlir::LLVM::LLVMArrayType::get(
							type.getContext(),
							mlir::IntegerType::get(type.getContext(), 8),
							size - dataLayaout.getTypeSize(maxAligmentType)));

					newBody.push_back(mlir::IntegerType::get(type.getContext(), 64));

					auto result = mlir::LLVM::LLVMStructType::getLiteral(
							type.getContext(), newBody);
					return result;
				});
		converter.addConversion([&](mlir::FunctionType type) -> Type {
			SmallVector<Type, 2> args;
			for (auto arg : type.getResults())
			{
				if (not arg.isa<mlir::rlc::VoidType>())
					args.push_back(mlir::LLVM::LLVMPointerType::get(type.getContext()));
			}

			for (auto arg : type.getInputs())
				args.push_back(mlir::LLVM::LLVMPointerType::get(type.getContext()));

			return mlir::LLVM::LLVMFunctionType::get(
					mlir::LLVM::LLVMVoidType::get(type.getContext()), args);
		});
		converter.addConversion([&](mlir::rlc::ReferenceType type) -> Type {
			return mlir::LLVM::LLVMPointerType::get(type.getContext());
		});
		converter.addConversion([&](mlir::rlc::ClassType type) -> Type {
			SmallVector<Type, 2> fields;
			for (auto arg : type.getBody())
				fields.push_back(converter.convertType(arg));

			return mlir::LLVM::LLVMStructType::getNewIdentified(
					type.getContext(), type.getName(), fields);
		});
		converter.addConversion(voidToVoid);
	}
}	 // namespace mlir::rlc

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

#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
#define GEN_PASS_DEF_EMITMAINPASS
#include "rlc/dialect/Passes.inc"
	// Wraps RLC's main function, which has a mangled name and returns an int64,
	// in a function named 'main'
	//	which returns an int32.
	struct EmitMainPass: public impl::EmitMainPassBase<EmitMainPass>
	{
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect, mlir::cf::ControlFlowDialect>();
		}

		void runOnOperation() override
		{
			auto mangeledMainName = mlir::rlc::mangledName(
					"main",
					false,
					mlir::FunctionType::get(
							&getContext(),
							mlir::TypeRange(),
							mlir::TypeRange(
									{ mlir::rlc::IntegerType::getInt64(&getContext()) })));

			// do nothing if this module does not have an LLVMFuncOp with the mangled
			// name.
			auto realMain =
					getOperation().lookupSymbol<mlir::LLVM::LLVMFuncOp>(mangeledMainName);
			if (realMain == nullptr)
				return;

			mlir::OpBuilder builder(realMain);

			// construct a function with return type int32 and name "main". As opposed
			// to int64 and the mangled name.
			auto returnType = builder.getI32Type();
			auto op = builder.create<mlir::LLVM::LLVMFuncOp>(
					realMain.getLoc(),
					"main",
					mlir::LLVM::LLVMFunctionType::get(returnType, {}));

			auto* block = op.addEntryBlock(builder);
			builder.setInsertionPoint(block, block->begin());

			// allocate an int64, pointed by the result of alloca
			auto count = builder.create<mlir::LLVM::ConstantOp>(
					realMain.getLoc(),
					builder.getI64Type(),
					builder.getI64IntegerAttr(1));
			auto alloca = builder.create<mlir::LLVM::AllocaOp>(
					realMain.getLoc(),
					mlir::LLVM::LLVMPointerType::get(&getContext()),
					builder.getI64Type(),
					count,
					0);

			// pass the int64 pointer to the RLC main function, as the return value
			// will be stored in the first argument.
			auto call = builder.create<mlir::LLVM::CallOp>(
					realMain.getLoc(), realMain, mlir::ValueRange({ alloca }));

			// Load the returned value and return it. Converting the 64-bit return to
			// 32-bit integer.
			auto aligment =
					mlir::DataLayout::closest(alloca).getTypePreferredAlignment(
							builder.getI64Type());
			auto loaded = builder.create<mlir::LLVM::LoadOp>(
					realMain.getLoc(), builder.getI64Type(), alloca, aligment);

			auto trunchated = builder.create<mlir::LLVM::TruncOp>(
					realMain.getLoc(), returnType, loaded);
			builder.create<mlir::LLVM::ReturnOp>(
					realMain.getLoc(), mlir::ValueRange({ trunchated }));
		}

		private:
		mlir::TypeConverter converter;
	};

}	 // namespace mlir::rlc

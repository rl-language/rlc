#pragma once
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

#include "mlir/Dialect/LLVMIR/LLVMAttrs.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "rlc/dialect/Operations.hpp"

namespace mlir::rlc
{

	class DebugInfoGenerator
	{
		public:
		explicit DebugInfoGenerator(
				mlir::ModuleOp op,
				mlir::OpBuilder& rewriter,
				mlir::TypeConverter* converter = nullptr,
				mlir::DataLayout* dl = nullptr);
		mlir::LLVM::DIFileAttr getFileOfLoc(mlir::Location loc) const;
		mlir::LLVM::DITypeAttr getDIAttrOf(mlir::Type type) const;
		mlir::LLVM::DISubprogramAttr getFunctionAttr(
				mlir::rlc::FlatFunctionOp fun) const;
		mlir::LLVM::DISubprogramAttr getFunctionAttr(
				mlir::LLVM::LLVMFuncOp fun,
				llvm::StringRef unmangledName = "",
				mlir::FunctionType rlcType = nullptr,
				bool artificial = false) const;

		mlir::LLVM::DILocalVariableAttr getLocalVar(
				mlir::Value value,
				llvm::StringRef name,
				mlir::FileLineColLoc location) const;

		mlir::LLVM::DILocalVariableAttr getArgument(
				mlir::Value value,
				mlir::LLVM::DISubprogramAttr functionAttr,
				llvm::StringRef name,
				mlir::FileLineColLoc location) const;

		private:
		mlir::LLVM::DITypeAttr getDIAttrOfImpl(mlir::Type type) const;
		mlir::LLVM::DISubprogramAttr makeFunctionAttr(
				llvm::StringRef name,
				llvm::StringRef mangledName,
				mlir::LLVM::DISubroutineTypeAttr type,
				bool declaration,
				mlir::Location location,
				bool artificial) const;
		mutable mlir::ModuleOp op;
		mlir::OpBuilder& rewriter;
		mlir::TypeConverter* converter;
		mlir::DataLayout* dl;

		mlir::LLVM::DICompileUnitAttr compileUnit;

		mutable DenseMap<mlir::Operation*, mlir::LLVM::DISubprogramAttr>
				functionToSubProgram;
		mutable DenseMap<mlir::Type, mlir::LLVM::DITypeAttr> typeToDITypeMap;
	};

}	 // namespace mlir::rlc

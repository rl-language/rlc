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

#include "rlc/dialect/SymbolTable.h"

namespace mlir::rlc
{
	class TemplateParameterType;
	class OverloadResolver
	{
		public:
		OverloadResolver(
				ValueTable& symbolTable, mlir::Operation* errorEmitter = nullptr)
				: symbolTable(&symbolTable), errorEmitter(errorEmitter)
		{
		}

		// like find overload, except that if the found overload is a template, it
		// gets istantiated
		mlir::Value instantiateOverload(
				mlir::IRRewriter& rewriter,
				bool isMemberCall,
				mlir::Location loc,
				llvm::StringRef name,
				mlir::TypeRange arguments);

		mlir::Value findOverload(
				mlir::Location callPoint,
				bool isMemberCall,
				llvm::StringRef name,
				mlir::TypeRange arguments);

		llvm::SmallVector<mlir::Value, 2> findOverloads(
				mlir::Location callPoint,
				bool isMemberCall,
				llvm::StringRef name,
				mlir::TypeRange arguments);

		// returns the type of the intantiated template if possibleCallee is the
		// type of a template, or returns possibleCallee if it is not a template.
		// returns nullptr if the overload does not match
		mlir::Type deduceTemplateCallSiteType(
				mlir::Location callPoint,
				mlir::TypeRange callSiteArgumentTypes,
				mlir::FunctionType possibleCallee,
				mlir::TypeRange templateParameters);

		mlir::LogicalResult deduceSubstitutions(
				mlir::Location callPoint,
				llvm::DenseMap<mlir::rlc::TemplateParameterType, mlir::Type>&
						substitutions,
				mlir::Type calleeArgument,
				mlir::Type callSiteArgument);

		private:
		ValueTable* symbolTable;
		mlir::Operation* errorEmitter;
		mlir::DenseMap<mlir::Type, bool> isTemplate;
	};

}	 // namespace mlir::rlc

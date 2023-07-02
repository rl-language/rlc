#pragma once

#include "rlc/dialect/SymbolTable.h"

namespace mlir::rlc
{

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
				mlir::Location loc,
				llvm::StringRef name,
				mlir::TypeRange arguments);

		mlir::Value findOverload(llvm::StringRef name, mlir::TypeRange arguments);

		llvm::SmallVector<mlir::Value, 2> findOverloads(
				llvm::StringRef name, mlir::TypeRange arguments);

		// returns the type of the intantiated template if possibleCallee is the
		// type of a template, or returns possibleCallee if it is not a template.
		// returns nullptr if the overload does not match
		mlir::Type deduceTemplateCallSiteType(
				mlir::TypeRange callSiteArgumentTypes,
				mlir::FunctionType possibleCallee);

		private:
		ValueTable* symbolTable;
		mlir::Operation* errorEmitter;
	};

}	 // namespace mlir::rlc

/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
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
				mlir::FunctionType possibleCallee);

		mlir::LogicalResult deduceSubstitutions(
				mlir::Location callPoint,
				llvm::DenseMap<mlir::rlc::TemplateParameterType, mlir::Type>&
						substitutions,
				mlir::Type calleeArgument,
				mlir::Type callSiteArgument);

		private:
		ValueTable* symbolTable;
		mlir::Operation* errorEmitter;
	};

}	 // namespace mlir::rlc

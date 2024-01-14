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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{

#define GEN_PASS_DEF_MEMBERFUNCTIONSTOREGULARFUNCTIONSPASS
#include "rlc/dialect/Passes.inc"

	struct MemberFunctionsToRegularFunctionsPass
			: impl::MemberFunctionsToRegularFunctionsPassBase<
						MemberFunctionsToRegularFunctionsPass>
	{
		using impl::MemberFunctionsToRegularFunctionsPassBase<
				MemberFunctionsToRegularFunctionsPass>::
				MemberFunctionsToRegularFunctionsPassBase;

		void runOnOperation() override
		{
			mlir::SmallVector<mlir::rlc::EntityDeclaration, 4> decls;
			for (auto decl : getOperation().getOps<mlir::rlc::EntityDeclaration>())
				decls.push_back(decl);

			mlir::IRRewriter rewriter(&getContext());
			for (auto decl : decls)
			{
				mlir::SmallVector<mlir::rlc::FunctionOp, 4> functions;
				for (auto function : decl.getOps<mlir::rlc::FunctionOp>())
				{
					functions.push_back(function);
				}
				rewriter.setInsertionPoint(decl);
				for (auto function : functions)
				{
					llvm::SmallVector<mlir::Type, 4> templateParameters;
					for (auto templateParamenter : decl.getTemplateParameters())
						templateParameters.push_back(
								templateParamenter.cast<mlir::TypeAttr>().getValue());

					llvm::SmallVector<mlir::Type, 4> newArgs;
					newArgs.push_back(mlir::rlc::ScalarUseType::get(
							&getContext(), decl.getName(), 0, templateParameters));

					for (auto originalArg : function.getFunctionType().getInputs())
						newArgs.push_back(originalArg);

					auto newType = mlir::FunctionType::get(
							&getContext(), newArgs, function.getFunctionType().getResults());

					for (auto templateParamenter : function.getTemplateParameters())
						templateParameters.push_back(
								templateParamenter.cast<mlir::TypeAttr>().getValue());

					llvm::SmallVector<llvm::StringRef, 4> argNames = { "self" };
					for (const auto& name : function.getArgNames())
						argNames.push_back(name.cast<mlir::StringAttr>());

					auto newF = rewriter.create<mlir::rlc::FunctionOp>(
							function.getLoc(),
							function.getUnmangledName(),
							newType,
							rewriter.getStrArrayAttr(argNames),
							templateParameters);

					newF.getPrecondition().takeBody(function.getPrecondition());
					newF.getBody().takeBody(function.getBody());

					newF.getPrecondition().insertArgument(
							(unsigned int) 0, newArgs[0], newF.getLoc());
					newF.getBody().insertArgument(
							(unsigned int) 0, newArgs[0], newF.getLoc());

					function.erase();
				}
			}
		}
	};

}	 // namespace mlir::rlc

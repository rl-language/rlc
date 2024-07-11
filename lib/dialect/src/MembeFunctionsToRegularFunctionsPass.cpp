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
			mlir::SmallVector<mlir::rlc::ClassDeclaration, 4> decls;
			for (auto decl : getOperation().getOps<mlir::rlc::ClassDeclaration>())
				decls.push_back(decl);

			mlir::IRRewriter rewriter(&getContext());
			for (auto decl : decls)
			{
				mlir::SmallVector<mlir::rlc::FunctionOp, 4> functions;
				for (auto function : decl.getOps<mlir::rlc::FunctionOp>())
				{
					functions.push_back(function);
				}
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

					function.setTemplateParametersAttr(
							rewriter.getTypeArrayAttr(templateParameters));
					function.setArgNamesAttr(rewriter.getStrArrayAttr(argNames));
					function.getResult().setType(newType);

					if (not function.isDeclaration())
					{
						function.getPrecondition().insertArgument(
								(unsigned int) 0, newArgs[0], function.getLoc());
						function.getBody().insertArgument(
								(unsigned int) 0, newArgs[0], function.getLoc());
					}
					function->moveAfter(decl);
				}
			}
		}
	};

}	 // namespace mlir::rlc

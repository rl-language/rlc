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

#define GEN_PASS_DEF_LOWERISOPERATIONSPASS
#include "rlc/dialect/Passes.inc"

	static void eraseIfStatementIfConstant(mlir::rlc::IfStatement op)
	{
		auto constant = op.getCondition()
												.front()
												.getTerminator()
												->getOperand(0)
												.getDefiningOp<mlir::rlc::Constant>();
		if (not constant)
			return;

		auto boolAttr = mlir::dyn_cast<mlir::BoolAttr>(constant.getValue());
		if (not boolAttr)
			return;

		mlir::IRRewriter rewriter(op.getContext());
		rewriter.setInsertionPoint(op);
		auto elseBranch = rewriter.create<mlir::rlc::StatementList>(op.getLoc());
		elseBranch.getBody().takeBody(
				boolAttr.getValue() ? op.getTrueBranch() : op.getElseBranch());
		op.erase();
	}

	static void lowerAlternativeIsOperations(mlir::Operation* op)
	{
		llvm::SmallVector<mlir::rlc::IsAlternativeTypeOp, 4> toReplace;
		op->walk(
				[&](mlir::rlc::IsAlternativeTypeOp op) { toReplace.push_back(op); });

		mlir::IRRewriter rewriter(op->getContext());
		for (auto op : toReplace)
		{
			if (isTemplateType(op.getInput().getType()).succeeded())
				continue;

			bool evalsToTrue =
					mlir::isa<mlir::rlc::AlternativeType>(op.getInput().getType());
			rewriter.setInsertionPoint(op);
			rewriter.replaceOpWithNewOp<mlir::rlc::Constant>(op, evalsToTrue);
		}
	}

	static void lowerNonAlternativeIsOperations(
			mlir::Operation* op, mlir::rlc::ValueTable& table)
	{
		llvm::SmallVector<mlir::rlc::IsOp, 4> toReplace;
		op->walk([&](mlir::rlc::IsOp op) { toReplace.push_back(op); });

		mlir::IRRewriter rewriter(op->getContext());
		for (auto op : toReplace)
		{
			if (isTemplateType(op.getTypeOrTrait()).succeeded() or
					isTemplateType(op.getExpression().getType()).succeeded() or
					(mlir::isa<mlir::rlc::AlternativeType>(
							 op.getExpression().getType()) and
					 not mlir::isa<mlir::rlc::TraitMetaType>(op.getTypeOrTrait())))
				continue;

			bool evalsToTrue = true;
			if (auto casted =
							mlir::dyn_cast<mlir::rlc::TraitMetaType>(op.getTypeOrTrait()))
			{
				evalsToTrue = casted
													.typeRespectsTrait(
															op.getLoc(), op.getExpression().getType(), table)
													.succeeded();
			}
			else
			{
				evalsToTrue = op.getExpression().getType() == op.getTypeOrTrait();
			}

			rewriter.setInsertionPoint(op);
			rewriter.replaceOpWithNewOp<mlir::rlc::Constant>(op, evalsToTrue);
		}
	}

	void lowerIsOperations(mlir::Operation* op, mlir::rlc::ValueTable& table)
	{
		lowerNonAlternativeIsOperations(op, table);
		lowerAlternativeIsOperations(op);
		op->walk(
				[&](mlir::rlc::IfStatement op) { eraseIfStatementIfConstant(op); });
	}

	struct LowerIsOperationsPass
			: impl::LowerIsOperationsPassBase<LowerIsOperationsPass>
	{
		using impl::LowerIsOperationsPassBase<
				LowerIsOperationsPass>::LowerIsOperationsPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			lowerIsOperations(getOperation(), builder.getSymbolTable());
		}
	};
}	 // namespace mlir::rlc

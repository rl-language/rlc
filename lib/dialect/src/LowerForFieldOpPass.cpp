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
	static void emitField(
			mlir::rlc::ModuleBuilder& builder,
			mlir::IRRewriter& rewriter,
			mlir::Location loc,
			llvm::StringRef memberName,
			mlir::ValueRange members,
			mlir::Type fieldType,
			mlir::rlc::ForFieldStatement toClone)
	{
		auto cloned = mlir::cast<mlir::rlc::ForFieldStatement>(
				rewriter.clone(*toClone.getOperation()));

		if (toClone.hasFieldNameVariable())
		{
			mlir::IRRewriter innerRewriter(rewriter.getContext());
			innerRewriter.setInsertionPoint(&cloned.getBody().front().front());
			auto name = innerRewriter.create<mlir::rlc::StringLiteralOp>(
					toClone.getLoc(), memberName);
			cloned.getBody().front().getArgument(0).replaceAllUsesWith(name);
		}

		auto newScope = rewriter.create<mlir::rlc::StatementList>(toClone.getLoc());

		for (auto [member, argument] :
				 llvm::zip(members, cloned.getNonFieldNameArgs()))
			argument.replaceAllUsesWith(member);

		mlir::AttrTypeReplacer replacer;
		replacer.addReplacement([&](mlir::Type t) -> std::optional<mlir::Type> {
			if (t == cloned.getNonFieldNameArgs().begin()->getType())
				return fieldType;
			return std::nullopt;
		});
		replacer.recursivelyReplaceElementsIn(cloned, true, true, true);

		auto* BB = rewriter.createBlock(&newScope.getBody());
		rewriter.setInsertionPointToStart(BB);

		while (not cloned.getBody().front().empty())
		{
			cloned.getBody().front().front().moveBefore(BB, BB->end());
			lowerForFields(builder, &BB->front());
		}

		rewriter.eraseOp(cloned);
		rewriter.setInsertionPointAfter(newScope);
	}

	static void lowerForField(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::ForFieldStatement op)
	{
		auto rewriter = mlir::IRRewriter(op.getContext());

		// if the for loop is iterating over a template type, do nothing
		if (isTemplateType(
						cast<mlir::rlc::Yield>(op.getCondition().back().getTerminator())
								.getArguments()[0]
								.getType())
						.succeeded())
			return;
		llvm::SmallVector<mlir::Value, 2> expressions;

		rewriter.setInsertionPointAfter(op);
		for (auto& op : op.getCondition().getOps())
		{
			auto* cloned = rewriter.clone(op);
			lowerForFields(builder, cloned);
			if (auto casted = mlir::dyn_cast<mlir::rlc::Yield>(cloned))
			{
				for (auto expression : casted.getArguments())
					expressions.push_back(expression);
				rewriter.eraseOp(cloned);
			}
		}

		if (auto casted = expressions[0].getType().dyn_cast<mlir::rlc::ClassType>())
		{
			for (auto field : llvm::enumerate(casted.getMembers()))
			{
				llvm::SmallVector<mlir::Value, 2> members;
				for (auto expression : expressions)
				{
					auto member = rewriter.create<mlir::rlc::MemberAccess>(
							op.getLoc(), expression, field.index());
					members.push_back(member);
				}
				emitField(
						builder,
						rewriter,
						op.getLoc(),
						field.value().getName(),
						members,
						field.value().getType(),
						op);
			}
		}
		else if (
				auto casted =
						expressions[0].getType().dyn_cast<mlir::rlc::AlternativeType>())
		{
			for (auto field : llvm::enumerate(casted.getUnderlying()))
			{
				llvm::SmallVector<mlir::Value, 2> members;
				for (auto expression : expressions)
				{
					auto member = rewriter.create<mlir::rlc::ValueUpcastOp>(
							op.getLoc(), field.value(), expression);
					members.push_back(member);
				}
				emitField(
						builder,
						rewriter,
						op.getLoc(),
						prettyType(field.value()),
						members,
						field.value(),
						op);
			}
		}
		rewriter.eraseOp(op);
	}

	void lowerForFields(mlir::rlc::ModuleBuilder& builder, mlir::Operation* op)
	{
		llvm::SmallVector<mlir::rlc::ForFieldStatement, 2> ops;
		op->walk([&](mlir::rlc::ForFieldStatement op) {
			ops.push_back(op);
			return mlir::WalkResult::skip();
		});
		for (auto op : ops)
			lowerForField(builder, op);
	}

#define GEN_PASS_DEF_LOWERFORFIELDOPPASS
#include "rlc/dialect/Passes.inc"

	struct LowerForFieldOpPass: impl::LowerForFieldOpPassBase<LowerForFieldOpPass>
	{
		using impl::LowerForFieldOpPassBase<
				LowerForFieldOpPass>::LowerForFieldOpPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			lowerForFields(builder, getOperation());
		}
	};
}	 // namespace mlir::rlc

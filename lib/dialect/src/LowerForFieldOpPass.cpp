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
	static void emitField(
			mlir::rlc::ModuleBuilder& builder,
			mlir::IRRewriter& rewriter,
			mlir::Location loc,
			mlir::ValueRange members,
			mlir::Type fieldType,
			size_t fieldIndex,
			mlir::rlc::ForFieldStatement toClone)
	{
		auto cloned = mlir::cast<mlir::rlc::ForFieldStatement>(
				rewriter.clone(*toClone.getOperation()));

		for (auto [member, argument] :
				 llvm::zip(members, cloned.getBody().getArguments()))
			argument.replaceAllUsesWith(member);

		mlir::AttrTypeReplacer replacer;
		replacer.addReplacement([&](mlir::Type t) -> std::optional<mlir::Type> {
			if (t == cloned.getBody().getArgument(0).getType())
				return fieldType;
			return std::nullopt;
		});
		replacer.recursivelyReplaceElementsIn(cloned, true, true, true);

		for (auto& op : cloned.getBody().getOps())
		{
			if (mlir::isa<mlir::rlc::Yield>(op))
				continue;

			auto* newOp = rewriter.clone(op);
			lowerForFields(builder, newOp);
		}

		rewriter.eraseOp(cloned);
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

		if (auto casted =
						expressions[0].getType().dyn_cast<mlir::rlc::EntityType>())
		{
			for (auto field : llvm::enumerate(casted.getBody()))
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
						members,
						field.value(),
						field.index(),
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
						members,
						field.value(),
						field.index(),
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

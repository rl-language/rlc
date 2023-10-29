
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
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

		if (not expressions[0].getType().isa<mlir::rlc::EntityType>())
		{
			builder.getRewriter().eraseOp(op);
			return;
		}
		auto casted = expressions[0].getType().cast<mlir::rlc::EntityType>();
		for (auto field : llvm::enumerate(casted.getBody()))
		{
			llvm::SmallVector<mlir::Value, 2> members;
			for (auto expression : expressions)
			{
				auto member = rewriter.create<mlir::rlc::MemberAccess>(
						op.getLoc(), expression, field.index());
				members.push_back(member);
			}

			auto cloned = mlir::cast<mlir::rlc::ForFieldStatement>(
					rewriter.clone(*op.getOperation()));

			for (auto [member, argument] :
					 llvm::zip(members, cloned.getBody().getArguments()))
				argument.replaceAllUsesWith(member);

			mlir::AttrTypeReplacer replacer;
			replacer.addReplacement([&](mlir::Type t) -> std::optional<mlir::Type> {
				if (t == cloned.getBody().getArgument(0).getType())
					return field.value();
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
		rewriter.eraseOp(op);
	}

	void lowerForFields(mlir::rlc::ModuleBuilder& builder, mlir::Operation* op)
	{
		llvm::SmallVector<mlir::rlc::ForFieldStatement, 2> ops;
		op->walk([&](mlir::rlc::ForFieldStatement op) {
			ops.push_back(op);
			return mlir::WalkResult::interrupt();
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

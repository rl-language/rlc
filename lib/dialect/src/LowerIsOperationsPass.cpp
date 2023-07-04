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

		auto boolAttr = constant.getValue().dyn_cast<mlir::BoolAttr>();
		if (not boolAttr)
			return;

		mlir::IRRewriter rewriter(op.getContext());
		rewriter.setInsertionPoint(op);
		auto elseBranch = rewriter.create<mlir::rlc::StatementList>(op.getLoc());
		elseBranch.getBody().takeBody(
				boolAttr.getValue() ? op.getTrueBranch() : op.getElseBranch());
		op.erase();
	}

	void lowerIsOperations(mlir::Operation* op, mlir::rlc::ValueTable table)
	{
		llvm::SmallVector<mlir::rlc::IsOp, 4> toReplace;
		op->walk([&](mlir::rlc::IsOp op) { toReplace.push_back(op); });

		mlir::IRRewriter rewriter(op->getContext());
		for (auto op : toReplace)
		{
			if (isTemplateType(op.getTypeOrTrait()).succeeded() or
					isTemplateType(op.getExpression().getType()).succeeded())
				continue;

			bool evalsToTrue = true;
			if (auto casted =
							op.getTypeOrTrait().dyn_cast<mlir::rlc::TraitMetaType>())
			{
				evalsToTrue =
						casted.typeRespectsTrait(op.getExpression().getType(), table)
								.succeeded();
			}
			else
			{
				evalsToTrue = op.getExpression().getType() == op.getTypeOrTrait();
			}

			rewriter.setInsertionPoint(op);
			rewriter.replaceOpWithNewOp<mlir::rlc::Constant>(op, evalsToTrue);
		}

		llvm::SmallVector<mlir::rlc::IfStatement, 4> ifs;
		op->walk([&](mlir::rlc::IfStatement op) { ifs.push_back(op); });

		// walk the statements backward so the innermost ones get deleated first
		// and you do not delate them twice
		for (auto ifOp : llvm::reverse(ifs))
			eraseIfStatementIfConstant(ifOp);
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

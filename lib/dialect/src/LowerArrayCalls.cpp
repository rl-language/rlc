#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
	class ArrayConstructRewriter
			: public mlir::OpConversionPattern<mlir::rlc::ArrayConstructOp>
	{
		using mlir::OpConversionPattern<
				mlir::rlc::ArrayConstructOp>::OpConversionPattern;

		mlir::LogicalResult matchAndRewrite(
				mlir::rlc::ArrayConstructOp op,
				OpAdaptor adaptor,
				mlir::ConversionPatternRewriter& rewriter) const final
		{
			int64_t size = op.getType().cast<mlir::rlc::ArrayType>().getSize();
			rewriter.setInsertionPoint(op);
			mlir::Value result = nullptr;

			result = rewriter.create<mlir::rlc::UninitializedConstruct>(
					op.getLoc(), op.getType());
			rewriter.replaceOp(op, result);

			rewriter.setInsertionPointAfter(result.getDefiningOp());
			auto index = rewriter.create<mlir::rlc::DeclarationStatement>(
					op.getLoc(), mlir::rlc::IntegerType::get(op.getContext()), "dc");
			rewriter.createBlock(&index.getBody());
			auto zero = rewriter.create<mlir::rlc::Constant>(
					op.getLoc(), static_cast<int64_t>(0));

			rewriter.create<mlir::rlc::Yield>(
					op.getLoc(), mlir::ValueRange({ zero }));

			rewriter.setInsertionPointAfter(index);
			auto whileStm = rewriter.create<mlir::rlc::WhileStatement>(op.getLoc());
			rewriter.createBlock(&whileStm.getCondition());
			auto max = rewriter.create<mlir::rlc::Constant>(op.getLoc(), size);
			auto cond = rewriter.create<mlir::rlc::LessOp>(op.getLoc(), index, max);
			rewriter.create<mlir::rlc::Yield>(
					op.getLoc(), mlir::ValueRange({ cond }));

			rewriter.createBlock(&whileStm.getBody());
			auto elem =
					rewriter.create<mlir::rlc::ArrayAccess>(op.getLoc(), result, index);

			rewriter.create<mlir::rlc::CallOp>(
					op.getLoc(), op.getInitializer(), mlir::ValueRange({ elem }));

			auto one = rewriter.create<mlir::rlc::Constant>(
					op.getLoc(), static_cast<int64_t>(1));
			auto added = rewriter.create<mlir::rlc::AddOp>(op.getLoc(), index, one);
			rewriter.create<mlir::rlc::BuiltinAssignOp>(op.getLoc(), index, added);

			rewriter.create<mlir::rlc::Yield>(op.getLoc());

			return mlir::success();
		}
	};

	class ArrayCallRewriter
			: public mlir::OpConversionPattern<mlir::rlc::ArrayCallOp>
	{
		using mlir::OpConversionPattern<
				mlir::rlc::ArrayCallOp>::OpConversionPattern;

		mlir::LogicalResult matchAndRewrite(
				mlir::rlc::ArrayCallOp op,
				OpAdaptor adaptor,
				mlir::ConversionPatternRewriter& rewriter) const final
		{
			int64_t size =
					op.getArgs().front().getType().cast<mlir::rlc::ArrayType>().getSize();
			rewriter.setInsertionPoint(op);
			mlir::Value result = nullptr;
			if (op.getNumResults() != 0)
			{
				result = rewriter.create<mlir::rlc::UninitializedConstruct>(
						op.getLoc(), op.getResultTypes());
				rewriter.replaceOp(op, result);
			}
			else
			{
				rewriter.eraseOp(op);
			}
			auto index = rewriter.create<mlir::rlc::DeclarationStatement>(
					op.getLoc(), mlir::rlc::IntegerType::get(op.getContext()), "dc");
			auto* block = rewriter.createBlock(&index.getBody());
			rewriter.setInsertionPoint(block, block->begin());
			auto zero = rewriter.create<mlir::rlc::Constant>(
					op.getLoc(), static_cast<int64_t>(0));

			rewriter.create<mlir::rlc::Yield>(
					op.getLoc(), mlir::ValueRange({ zero }));

			rewriter.setInsertionPointAfter(index);
			auto whileStm = rewriter.create<mlir::rlc::WhileStatement>(op.getLoc());
			auto* wCond = rewriter.createBlock(&whileStm.getCondition());
			rewriter.setInsertionPoint(wCond, wCond->begin());
			auto max = rewriter.create<mlir::rlc::Constant>(op.getLoc(), size);
			auto cond = rewriter.create<mlir::rlc::LessOp>(op.getLoc(), index, max);
			rewriter.create<mlir::rlc::Yield>(
					op.getLoc(), mlir::ValueRange({ cond }));

			auto* wBody = rewriter.createBlock(&whileStm.getBody());
			rewriter.setInsertionPoint(wBody, wBody->begin());
			llvm::SmallVector<mlir::Value> realArgs;
			for (auto arg : adaptor.getArgs())
			{
				realArgs.push_back(
						rewriter.create<mlir::rlc::ArrayAccess>(op.getLoc(), arg, index));
			}
			auto res = rewriter.create<mlir::rlc::CallOp>(
					op.getLoc(), op.getCallee(), realArgs);
			if (op.getNumResults() != 0)
			{
				auto resElem =
						rewriter.create<mlir::rlc::ArrayAccess>(op.getLoc(), result, index);
				rewriter.create<mlir::rlc::AssignOp>(
						op.getLoc(), resElem, res.getResult(0));
			}

			auto one = rewriter.create<mlir::rlc::Constant>(
					op.getLoc(), static_cast<int64_t>(1));
			auto added = rewriter.create<mlir::rlc::AddOp>(op.getLoc(), index, one);
			rewriter.create<mlir::rlc::BuiltinAssignOp>(op.getLoc(), index, added);

			rewriter.create<mlir::rlc::Yield>(op.getLoc());

			return mlir::success();
		}
	};

#define GEN_PASS_DEF_LOWERARRAYCALLSPASS
#include "rlc/dialect/Passes.inc"

	struct LowerArrayCallsPass: impl::LowerArrayCallsPassBase<LowerArrayCallsPass>
	{
		using impl::LowerArrayCallsPassBase<
				LowerArrayCallsPass>::LowerArrayCallsPassBase;

		void runOnOperation() override
		{
			mlir::ConversionTarget target(getContext());

			target.addLegalDialect<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
			target
					.addIllegalOp<mlir::rlc::ArrayCallOp, mlir::rlc::ArrayConstructOp>();

			mlir::RewritePatternSet patterns(&getContext());
			patterns.add<ArrayCallRewriter>(&getContext());
			patterns.add<ArrayConstructRewriter>(&getContext());

			if (failed(applyPartialConversion(
							getOperation(), target, std::move(patterns))))
				signalPassFailure();
		}
	};
}	 // namespace mlir::rlc

#include "rlc/dialect/Conversion.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static mlir::Value makeAlloca(
		mlir::RewriterBase& rewriter, mlir::Type type, mlir::Location loc)
{
	auto count = rewriter.create<mlir::LLVM::ConstantOp>(
			loc, rewriter.getI64Type(), rewriter.getI64IntegerAttr(1));
	return rewriter.create<mlir::LLVM::AllocaOp>(loc, type, count, 0);
}

static llvm::SmallVector<mlir::rlc::Yield, 2> getYieldTerminators(
		mlir::Region& region)
{
	llvm::SmallVector<mlir::rlc::Yield, 2> terminators;
	for (auto& block : region)
	{
		for (auto& entry : block)
		{
			if (auto yield = mlir::dyn_cast<mlir::rlc::Yield>(entry))
				terminators.push_back(yield);
		}
	}
	return terminators;
}

static llvm::SmallVector<mlir::rlc::Yield, 2> getYieldTerminators(auto& op)
{
	llvm::SmallVector<mlir::rlc::Yield, 2> terminators;
	for (auto& block : op.getBody())
	{
		for (auto& entry : block)
		{
			if (auto yield = mlir::dyn_cast<mlir::rlc::Yield>(entry))
				terminators.push_back(yield);
		}
	}
	return terminators;
}

static void mergeYieldsIntoSplittedBlock(
		llvm::ArrayRef<mlir::rlc::Yield> yields,
		mlir::Block* successor,
		mlir::IRRewriter& rewriter)
{
	if (yields.empty())
	{
		rewriter.eraseBlock(successor);
		return;
	}

	rewriter.eraseOp(&*successor->begin());
	if (yields.size() == 1)
	{
		rewriter.mergeBlocks(
				successor, yields.front()->getBlock(), mlir::ValueRange());
		rewriter.eraseOp(yields.front());
		return;
	}

	for (mlir::rlc::Yield yield : yields)
	{
		rewriter.setInsertionPoint(yield);
		rewriter.create<mlir::rlc::Branch>(yield.getLoc(), successor);
		rewriter.eraseOp(yield);
	}
}

class EntityDeclarationEraser
		: public mlir::OpConversionPattern<mlir::rlc::EntityDeclaration>
{
	using mlir::OpConversionPattern<
			mlir::rlc::EntityDeclaration>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::EntityDeclaration op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

class ConstructRewriter
		: public mlir::OpConversionPattern<mlir::rlc::ConstructOp>
{
	using mlir::OpConversionPattern<mlir::rlc::ConstructOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::ConstructOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto mapped = typeConverter->convertType(op.getType());
		auto alloca = makeAlloca(rewriter, mapped, op.getLoc());

		rewriter.create<mlir::LLVM::CallOp>(
				op.getLoc(),
				mlir::TypeRange(),
				mlir::ValueRange({ adaptor.getInitializer(), alloca }));
		rewriter.replaceOp(op, alloca);
		return mlir::LogicalResult::success();
	}
};

class MemberAccessRewriter
		: public mlir::OpConversionPattern<mlir::rlc::MemberAccess>
{
	using mlir::OpConversionPattern<mlir::rlc::MemberAccess>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::MemberAccess op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto structType = adaptor.getValue()
													.getType()
													.cast<mlir::LLVM::LLVMPointerType>()
													.getElementType();
		auto elem_type = structType.cast<mlir::LLVM::LLVMStructType>()
												 .getBody()[op.getMemberIndex()];

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto index = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI32Type(),
				rewriter.getI32IntegerAttr(op.getMemberIndex()));

		rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
				op,
				mlir::LLVM::LLVMPointerType::get(elem_type),
				adaptor.getValue(),
				mlir::ValueRange({ zeroValue, index }));

		return mlir::LogicalResult::success();
	}
};

class ArrayAccessRewriter
		: public mlir::OpConversionPattern<mlir::rlc::ArrayAccess>
{
	using mlir::OpConversionPattern<mlir::rlc::ArrayAccess>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::ArrayAccess op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto loaded = rewriter.create<mlir::LLVM::LoadOp>(
				op.getLoc(), adaptor.getMemberIndex());
		auto array_type = adaptor.getValue()
													.getType()
													.cast<mlir::LLVM::LLVMPointerType>()
													.getElementType();
		auto elem_type =
				array_type.cast<mlir::LLVM::LLVMArrayType>().getElementType();

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);

		rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
				op,
				mlir::LLVM::LLVMPointerType::get(elem_type),
				adaptor.getValue(),
				mlir::ValueRange({ zeroValue, loaded }));

		return mlir::LogicalResult::success();
	}
};

class InitRewriter: public mlir::OpConversionPattern<mlir::rlc::InitOp>
{
	using mlir::OpConversionPattern<mlir::rlc::InitOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::InitOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.setInsertionPoint(op);
		auto type = adaptor.getLhs()
										.getType()
										.cast<mlir::LLVM::LLVMPointerType>()
										.getElementType();
		auto zero = rewriter.getZeroAttr(type);
		auto zeroValue =
				rewriter.create<mlir::LLVM::ConstantOp>(op.getLoc(), type, zero);

		rewriter.create<mlir::LLVM::StoreOp>(
				op.getLoc(), zeroValue, adaptor.getLhs());
		rewriter.eraseOp(op);

		return mlir::LogicalResult::success();
	}
};

class AssignRewriter: public mlir::OpConversionPattern<mlir::rlc::AssignOp>
{
	using mlir::OpConversionPattern<mlir::rlc::AssignOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::AssignOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.setInsertionPoint(op);
		auto loaded =
				rewriter.create<mlir::LLVM::LoadOp>(op.getLoc(), adaptor.getRhs());
		rewriter.create<mlir::LLVM::StoreOp>(op.getLoc(), loaded, adaptor.getLhs());
		rewriter.replaceOp(op, adaptor.getLhs());

		return mlir::LogicalResult::success();
	}
};

class YieldRewriter: public mlir::OpConversionPattern<mlir::rlc::Yield>
{
	using mlir::OpConversionPattern<mlir::rlc::Yield>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::Yield op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.setInsertionPoint(op);
		assert(op.getArguments().size() <= 1);
		if (adaptor.getArguments().empty())
		{
			rewriter.replaceOpWithNewOp<mlir::LLVM::ReturnOp>(op, mlir::ValueRange());
		}
		else
		{
			auto loaded = rewriter.create<mlir::LLVM::LoadOp>(
					op.getLoc(), adaptor.getArguments()[0]);
			rewriter.replaceOpWithNewOp<mlir::LLVM::ReturnOp>(
					op, mlir::ValueRange({ loaded }));
		}

		return mlir::LogicalResult::success();
	}
};

static mlir::Value makeFCMP(
		mlir::ConversionPatternRewriter& builder,
		mlir::LLVM::FCmpPredicate predicate,
		mlir::Value l,
		mlir::Value r,
		mlir::Location position)
{
	mlir::Type boolT = builder.getI1Type();
	auto res =
			builder.create<mlir::LLVM::FCmpOp>(position, boolT, predicate, l, r);
	return builder.create<mlir::LLVM::ZExtOp>(position, builder.getI8Type(), res);
}

static mlir::Value makeICMP(
		mlir::ConversionPatternRewriter& builder,
		mlir::LLVM::ICmpPredicate predicate,
		mlir::Value l,
		mlir::Value r,
		mlir::Location position)
{
	mlir::Type boolT = builder.getI1Type();
	auto res =
			builder.create<mlir::LLVM::ICmpOp>(position, boolT, predicate, l, r);
	return builder.create<mlir::LLVM::ZExtOp>(position, builder.getI8Type(), res);
}

template<typename Op, typename... Args>
class ArithRewriterRewriter: public mlir::OpConversionPattern<Op>
{
	public:
	using mlir::OpConversionPattern<Op>::OpConversionPattern;
	using OpAdaptor =
			typename mlir::OpConversionPattern<Op>::OpConversionPattern::OpAdaptor;

	using F = mlir::Value (*)(
			Op op, mlir::ConversionPatternRewriter& rewriter, Args...);

	ArithRewriterRewriter(
			mlir::Value (*f)(
					Op op, mlir::ConversionPatternRewriter& rewriter, Args...),
			mlir::TypeConverter& conv,
			mlir::MLIRContext* cont)
			: mlir::OpConversionPattern<Op>(conv, cont), rewriteOp(f)
	{
	}

	template<size_t... I>
	mlir::Value dispatch(
			Op op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter,
			std::index_sequence<I...>) const
	{
		return rewriteOp(
				op,
				rewriter,
				rewriter.create<mlir::LLVM::LoadOp>(
						op.getLoc(), adaptor.getOperands()[I])...);
	}

	mlir::LogicalResult matchAndRewrite(
			Op op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		mlir::Value res = dispatch(
				op, adaptor, rewriter, std::make_index_sequence<sizeof...(Args)>());
		mlir::Value alloca = makeAlloca(
				rewriter, mlir::LLVM::LLVMPointerType::get(res.getType()), op.getLoc());
		rewriter.create<mlir::LLVM::StoreOp>(op.getLoc(), res, alloca);
		rewriter.replaceOp(op, alloca);
		return mlir::LogicalResult::success();
	}

	private:
	F rewriteOp;
};

template<typename Op, typename... Args>
static std::unique_ptr<ArithRewriterRewriter<Op, Args...>> makeArith(
		mlir::Value (*f)(Op op, mlir::ConversionPatternRewriter& rewriter, Args...),
		mlir::TypeConverter& conv,
		mlir::MLIRContext* cont)
{
	return std::make_unique<ArithRewriterRewriter<Op, Args...>>(f, conv, cont);
}

static bool isRLCFloat(mlir::Type t) { return t.isa<mlir::rlc::FloatType>(); }
static bool isRLCFBool(mlir::Type t) { return t.isa<mlir::rlc::BoolType>(); }
static bool isRLCFInt(mlir::Type t) { return t.isa<mlir::rlc::IntegerType>(); }

static mlir::Value lowerLess(
		mlir::rlc::LessOp op,
		mlir::ConversionPatternRewriter& rewriter,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return makeFCMP(
				rewriter, mlir::LLVM::FCmpPredicate::olt, lhs, rhs, op.getLoc());

	return makeICMP(
			rewriter, mlir::LLVM::ICmpPredicate::slt, lhs, rhs, op.getLoc());
}

class UninitializedConstructRewriter
		: public mlir::OpConversionPattern<mlir::rlc::UninitializedConstruct>
{
	using mlir::OpConversionPattern<
			mlir::rlc::UninitializedConstruct>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::UninitializedConstruct op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type = typeConverter->convertType(op.getType());
		auto alloca = makeAlloca(rewriter, type, op.getLoc());
		rewriter.replaceOp(op, alloca);
		return mlir::LogicalResult::success();
	}
};

class ReferenceRewriter: public mlir::OpConversionPattern<mlir::rlc::Reference>
{
	using mlir::OpConversionPattern<mlir::rlc::Reference>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::Reference op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type = typeConverter->convertType(op.getType());
		rewriter.replaceOpWithNewOp<mlir::LLVM::AddressOfOp>(
				op, type, adaptor.getReferred());
		return mlir::LogicalResult::success();
	}
};

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

		rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange({ zero }));

		rewriter.setInsertionPointAfter(index);
		auto whileStm = rewriter.create<mlir::rlc::WhileStatement>(op.getLoc());
		rewriter.createBlock(&whileStm.getCondition());
		auto max = rewriter.create<mlir::rlc::Constant>(op.getLoc(), size);
		auto cond = rewriter.create<mlir::rlc::LessOp>(op.getLoc(), index, max);
		rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange({ cond }));

		rewriter.createBlock(&whileStm.getBody());
		auto elem =
				rewriter.create<mlir::rlc::ArrayAccess>(op.getLoc(), result, index);

		rewriter.create<mlir::rlc::CallOp>(
				op.getLoc(), op.getInitializer(), mlir::ValueRange({ elem }));

		auto one = rewriter.create<mlir::rlc::Constant>(
				op.getLoc(), static_cast<int64_t>(1));
		auto added = rewriter.create<mlir::rlc::AddOp>(op.getLoc(), index, one);
		rewriter.create<mlir::rlc::AssignOp>(op.getLoc(), index, added);

		rewriter.create<mlir::rlc::Yield>(op.getLoc());

		return mlir::success();
	}
};

class ArrayCallRewriter
		: public mlir::OpConversionPattern<mlir::rlc::ArrayCallOp>
{
	using mlir::OpConversionPattern<mlir::rlc::ArrayCallOp>::OpConversionPattern;

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

		rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange({ zero }));

		rewriter.setInsertionPointAfter(index);
		auto whileStm = rewriter.create<mlir::rlc::WhileStatement>(op.getLoc());
		auto* wCond = rewriter.createBlock(&whileStm.getCondition());
		rewriter.setInsertionPoint(wCond, wCond->begin());
		auto max = rewriter.create<mlir::rlc::Constant>(op.getLoc(), size);
		auto cond = rewriter.create<mlir::rlc::LessOp>(op.getLoc(), index, max);
		rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange({ cond }));

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
		rewriter.create<mlir::rlc::AssignOp>(op.getLoc(), index, added);

		rewriter.create<mlir::rlc::Yield>(op.getLoc());

		return mlir::success();
	}
};

class CallRewriter: public mlir::OpConversionPattern<mlir::rlc::CallOp>
{
	using mlir::OpConversionPattern<mlir::rlc::CallOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::CallOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		llvm::SmallVector<mlir::Type, 2> out;

		for (auto type : op.getResultTypes())
		{
			if (type.isa<mlir::rlc::VoidType>())
				continue;

			auto res = typeConverter->convertType(type);

			out.push_back(res.cast<mlir::LLVM::LLVMPointerType>().getElementType());
		}

		rewriter.setInsertionPoint(op);
		llvm::SmallVector<mlir::Value, 2> args;
		args.push_back(adaptor.getCallee());
		for (auto arg : adaptor.getArgs())
			args.push_back(arg);

		auto res = rewriter.create<mlir::LLVM::CallOp>(op.getLoc(), out, args);

		if (not out.empty())
		{
			auto alloca = makeAlloca(
					rewriter, mlir::LLVM::LLVMPointerType::get(out.front()), op.getLoc());
			rewriter.replaceOp(op, alloca);
			rewriter.create<mlir::LLVM::StoreOp>(
					op.getLoc(), res.getResults().front(), alloca);
		}
		else
		{
			rewriter.eraseOp(op);
		}

		return mlir::LogicalResult::success();
	}
};

class BrRewriter: public mlir::OpConversionPattern<mlir::rlc::Branch>
{
	using mlir::OpConversionPattern<mlir::rlc::Branch>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::Branch op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.replaceOpWithNewOp<mlir::LLVM::BrOp>(
				op, mlir::ValueRange(), op.getDestination());

		return mlir::LogicalResult::success();
	}
};

class SelectRewriter: public mlir::OpConversionPattern<mlir::rlc::SelectBranch>
{
	using mlir::OpConversionPattern<mlir::rlc::SelectBranch>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::SelectBranch op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto loaded =
				rewriter.create<mlir::LLVM::LoadOp>(op.getLoc(), adaptor.getCond());

		llvm::SmallVector<int32_t, 2> indexes;
		llvm::SmallVector<mlir::ValueRange, 2> arguments;
		for (const auto& enumeration : llvm::enumerate(op.getSuccessors()))
		{
			indexes.push_back(enumeration.index());
			arguments.emplace_back(mlir::ValueRange());
		}

		rewriter.replaceOpWithNewOp<mlir::LLVM::SwitchOp>(
				op,
				loaded,
				op.getSuccessor(0),
				mlir::ValueRange(),
				indexes,
				op.getSuccessors(),
				arguments);

		return mlir::LogicalResult::success();
	}
};

class CbrRewriter: public mlir::OpConversionPattern<mlir::rlc::CondBranch>
{
	using mlir::OpConversionPattern<mlir::rlc::CondBranch>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::CondBranch op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto loaded =
				rewriter.create<mlir::LLVM::LoadOp>(op.getLoc(), adaptor.getCond());
		auto boolType = mlir::IntegerType::get(op.getContext(), 8);
		auto zero = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), boolType, rewriter.getZeroAttr(boolType));
		auto res = rewriter.create<mlir::LLVM::ICmpOp>(
				op.getLoc(),
				rewriter.getI1Type(),
				mlir::LLVM::ICmpPredicate::ne,
				loaded,
				zero);
		rewriter.replaceOpWithNewOp<mlir::LLVM::CondBrOp>(
				op, res, op.getTrueBranch(), op.getFalseBranch());

		return mlir::LogicalResult::success();
	}
};

class ConstantRewriter: public mlir::OpConversionPattern<mlir::rlc::Constant>
{
	using mlir::OpConversionPattern<mlir::rlc::Constant>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::Constant op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		llvm::SmallVector<mlir::Type, 2> out;
		auto err = typeConverter->convertType(op.getResult().getType(), out);
		assert(err.succeeded());
		if (err.failed())
			return err;

		rewriter.setInsertionPoint(op);
		auto alloca = makeAlloca(rewriter, out.front(), op.getLoc());
		auto constant = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				out.front().cast<mlir::LLVM::LLVMPointerType>().getElementType(),
				op.getValue());
		rewriter.replaceOp(op, alloca);
		rewriter.create<mlir::LLVM::StoreOp>(alloca.getLoc(), constant, alloca);

		return mlir::LogicalResult::success();
	}
};

static mlir::LogicalResult flatten(
		mlir::rlc::ExpressionStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto terminators = getYieldTerminators(op);

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();
	auto* after = rewriter.splitBlock(parentBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());

	mergeYieldsIntoSplittedBlock(terminators, after, rewriter);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::WhileStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto condTerminators = getYieldTerminators(op.getCondition());
	auto bodyTerminators = getYieldTerminators(op.getBody());

	auto* previousBlock = rewriter.getBlock();
	auto* afterBlock =
			rewriter.splitBlock(previousBlock, rewriter.getInsertionPoint());
	auto& conditionBlock = op.getCondition().front();
	auto& bodyBlock = op.getBody().front();

	rewriter.inlineRegionBefore(op.getCondition(), afterBlock);

	rewriter.inlineRegionBefore(op.getBody(), afterBlock);

	rewriter.setInsertionPoint(previousBlock, previousBlock->end());
	rewriter.create<mlir::rlc::Branch>(op.getLoc(), &conditionBlock);

	for (auto yield : condTerminators)
	{
		rewriter.setInsertionPoint(yield);
		rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), &bodyBlock, afterBlock);
		rewriter.eraseOp(yield);
	}

	for (auto yield : bodyTerminators)
	{
		rewriter.setInsertionPoint(yield);
		rewriter.replaceOpWithNewOp<mlir::rlc::Branch>(yield, &conditionBlock);
	}

	rewriter.eraseOp(op);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::IfStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto condTerminators = getYieldTerminators(op.getCondition());
	auto trueTerminators = getYieldTerminators(op.getTrueBranch());
	auto falseTerminators = getYieldTerminators(op.getElseBranch());

	auto& conditionBlock = op.getCondition().front();
	auto& trueBlock = op.getTrueBranch().front();
	auto& falseBlock = op.getElseBranch().front();
	auto* previousBlock = rewriter.getBlock();
	auto* afterBlock =
			rewriter.splitBlock(previousBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getCondition(), rewriter.getBlock());

	rewriter.inlineRegionBefore(op.getElseBranch(), afterBlock);
	rewriter.inlineRegionBefore(op.getTrueBranch(), afterBlock);

	rewriter.mergeBlocks(&conditionBlock, previousBlock, mlir::ValueRange());

	for (auto yield : condTerminators)
	{
		rewriter.setInsertionPoint(yield);
		rewriter.create<mlir::rlc::CondBranch>(
				yield.getLoc(), yield.getArguments().front(), &trueBlock, &falseBlock);
		rewriter.eraseOp(yield);
	}

	llvm::copy(falseTerminators, std::back_inserter(trueTerminators));
	mergeYieldsIntoSplittedBlock(trueTerminators, afterBlock, rewriter);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::StatementList op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto terminators = getYieldTerminators(op);

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();
	auto* after = rewriter.splitBlock(parentBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());

	mergeYieldsIntoSplittedBlock(terminators, after, rewriter);

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::DeclarationStatement op, mlir::IRRewriter& rewriter)
{
	rewriter.setInsertionPoint(op);
	auto terminators = getYieldTerminators(op);
	assert(terminators.size() == 1);

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();
	auto* after = rewriter.splitBlock(parentBlock, rewriter.getInsertionPoint());

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());

	auto alloca = terminators.front().getArguments();
	rewriter.replaceOp(op, { alloca });
	for (auto& terminator : terminators)
	{
		rewriter.mergeBlocks(after, terminator->getBlock(), mlir::ValueRange());
		rewriter.eraseOp(terminator);
	}

	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flatten(
		mlir::rlc::ReturnStatement op, mlir::IRRewriter& rewriter)
{
	if (op.getBody().empty())
	{
		assert(op.getResult() == mlir::rlc::VoidType::get(op.getContext()));
		rewriter.replaceOpWithNewOp<mlir::LLVM::ReturnOp>(op, mlir::ValueRange());
		return mlir::LogicalResult::success();
	}
	rewriter.setInsertionPoint(op);

	auto& entryBlock = op.getBody().front();
	auto* parentBlock = rewriter.getBlock();

	rewriter.inlineRegionBefore(op.getBody(), rewriter.getBlock());
	rewriter.mergeBlocks(&entryBlock, parentBlock, mlir::ValueRange());
	rewriter.eraseOp(op);

	return mlir::LogicalResult::success();
}

static void pruneUnrechableBlocks(mlir::Region& op, mlir::IRRewriter& rewriter)
{
	bool foundOne = true;

	while (foundOne)
	{
		foundOne = false;
		llvm::SmallVector<mlir::Block*, 3> blocks;
		for (auto& block : op)
			blocks.push_back(&block);

		for (auto& block : blocks)
		{
			if (block->hasNoPredecessors() and not block->isEntryBlock())
			{
				rewriter.eraseBlock(block);
				foundOne = true;
			}
		}
	}
}

class FunctionRewriter
		: public mlir::OpConversionPattern<mlir::rlc::FlatFunctionOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::FlatFunctionOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::FlatFunctionOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto linkage = op.isInternal() ? mlir::LLVM::linkage::Linkage::Internal
																	 : mlir::LLVM::linkage::Linkage::External;

		llvm::SmallVector<mlir::Type> realTypes;
		auto err = typeConverter->convertType(op.getFunctionType(), realTypes);
		assert(realTypes.size() == 1);
		assert(err.succeeded());
		if (err.failed())
		{
			return err;
		}

		auto fType =
				realTypes.front().cast<mlir::LLVM::LLVMPointerType>().getElementType();
		rewriter.setInsertionPoint(op);
		auto newF = rewriter.create<mlir::LLVM::LLVMFuncOp>(
				op.getLoc(), op.getMangledName(), fType, linkage);
		rewriter.cloneRegionBefore(
				op.getBody(), newF.getBody(), newF.getBody().begin());
		rewriter.eraseOp(op);
		auto convered =
				rewriter.convertRegionTypes(&newF.getRegion(), *typeConverter);
		assert(convered.hasValue());
		return mlir::success();
	}
};

static mlir::LogicalResult squashCF(
		mlir::rlc::FunctionOp& op, mlir::IRRewriter& rewriter)
{
	bool foundOne = true;
	auto dispatch = [&]<typename T>() -> mlir::LogicalResult {
		for (auto child : op.getBody().getOps<T>())
		{
			foundOne = true;
			return flatten(child, rewriter);
		}
		return mlir::LogicalResult::success();
	};

	while (foundOne)
	{
		foundOne = false;
		if (auto res = dispatch.operator()<mlir::rlc::ReturnStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::IfStatement>(); res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::StatementList>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::ExpressionStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::DeclarationStatement>();
				res.failed())
			return res;

		if (auto res = dispatch.operator()<mlir::rlc::WhileStatement>();
				res.failed())
			return res;
	}
	return mlir::LogicalResult::success();
}

static mlir::LogicalResult flattenModule(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 2> ops(
			op.getOps<mlir::rlc::FunctionOp>());

	for (auto f : ops)
	{
		rewriter.setInsertionPoint(f);
		llvm::SmallVector<mlir::OpOperand*> operands;
		for (auto& use : f.getResult().getUses())
			operands.push_back(&use);

		for (auto& use : operands)
		{
			rewriter.setInsertionPoint(use->getOwner());
			auto ref = rewriter.create<mlir::rlc::Reference>(
					use->getOwner()->getLoc(), f.getFunctionType(), f.getMangledName());
			use->set(ref);
		}
	}
	for (auto f : ops)
	{
		if (auto res = squashCF(f, rewriter); res.failed())
			return res;

		rewriter.setInsertionPoint(f);
		auto newF = rewriter.create<mlir::rlc::FlatFunctionOp>(
				op.getLoc(),
				f.getFunctionType(),
				f.getUnmangledName(),
				f.getArgNamesAttr());

		rewriter.cloneRegionBefore(
				f.getBody(), newF.getBody(), newF.getBody().begin());

		rewriter.eraseOp(f);

		pruneUnrechableBlocks(newF.getBody(), rewriter);
	}
	return mlir::LogicalResult::success();
}

static mlir::Value lowerReminder(
		mlir::rlc::ReminderOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return builder.create<mlir::LLVM::FRemOp>(lhs.getLoc(), lhs, rhs);

	return builder.create<mlir::LLVM::SRemOp>(lhs.getLoc(), lhs, rhs);
}

static mlir::Value lowerSubImpl(
		mlir::Location loc,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (builder.getF64Type() == lhs.getType())
		return builder.create<mlir::LLVM::FSubOp>(loc, lhs, rhs);

	return builder.create<mlir::LLVM::SubOp>(loc, lhs, rhs);
}

static mlir::Value lowerSub(
		mlir::rlc::SubOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	return lowerSubImpl(op.getLoc(), builder, lhs, rhs);
}

static mlir::Value lowerMinus(
		mlir::rlc::MinusOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value arg)
{
	auto zero = builder.getZeroAttr(arg.getType());
	auto zeroValue =
			builder.create<mlir::LLVM::ConstantOp>(arg.getLoc(), arg.getType(), zero);
	return lowerSubImpl(op.getLoc(), builder, zeroValue, arg);
}

static mlir::Value lowerDivide(
		mlir::rlc::DivOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return builder.create<mlir::LLVM::FDivOp>(op.getLoc(), lhs, rhs);

	return builder.create<mlir::LLVM::SDivOp>(op.getLoc(), lhs, rhs);
}

static mlir::Value makeFNEQ(
		mlir::ConversionPatternRewriter& builder,
		mlir::Value l,
		mlir::Value r,
		mlir::Location position)
{
	auto res = builder.create<mlir::LLVM::FCmpOp>(
			position, builder.getI1Type(), mlir::LLVM::FCmpPredicate::one, l, r);

	return builder.create<mlir::LLVM::ZExtOp>(position, builder.getI8Type(), res);
}

static mlir::Value makeFEQ(
		mlir::ConversionPatternRewriter& builder,
		mlir::Value l,
		mlir::Value r,
		mlir::Location position)
{
	auto res = builder.create<mlir::LLVM::FCmpOp>(
			position, builder.getI1Type(), mlir::LLVM::FCmpPredicate::oeq, l, r);
	return builder.create<mlir::LLVM::ZExtOp>(position, builder.getI8Type(), res);
}

static mlir::Value makeIEQ(
		mlir::ConversionPatternRewriter& builder,
		mlir::Value l,
		mlir::Value r,
		mlir::Location position)
{
	auto res = builder.create<mlir::LLVM::ICmpOp>(
			position, builder.getI1Type(), mlir::LLVM::ICmpPredicate::eq, l, r);
	return builder.create<mlir::LLVM::ZExtOp>(position, builder.getI8Type(), res);
}

static mlir::Value makeINEQ(
		mlir::ConversionPatternRewriter& builder,
		mlir::Value l,
		mlir::Value r,
		mlir::Location position)
{
	auto res = builder.create<mlir::LLVM::ICmpOp>(
			position, builder.getI1Type(), mlir::LLVM::ICmpPredicate::ne, l, r);
	return builder.create<mlir::LLVM::ZExtOp>(position, builder.getI8Type(), res);
}

static mlir::Value lowerNEqual(
		mlir::rlc::NotEqualOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getRhs().getType()))
		return makeFNEQ(builder, lhs, rhs, op.getLoc());
	return makeINEQ(builder, lhs, rhs, op.getLoc());
}

static mlir::Value lowerEqual(
		mlir::rlc::EqualOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return makeFEQ(builder, lhs, rhs, op.getLoc());
	return makeIEQ(builder, lhs, rhs, op.getLoc());
}

static mlir::Value lowerNot(
		mlir::rlc::NotOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs)
{
	auto zero = builder.getZeroAttr(lhs.getType());
	auto zeroValue =
			builder.create<mlir::LLVM::ConstantOp>(op.getLoc(), lhs.getType(), zero);
	return makeIEQ(builder, lhs, zeroValue, op.getLoc());
}

static mlir::Value toBool(
		mlir::ConversionPatternRewriter& builder, mlir::Value source)
{
	mlir::Type type = source.getType();
	auto zero = builder.getZeroAttr(source.getType());
	auto zeroValue = builder.create<mlir::LLVM::ConstantOp>(
			source.getLoc(), source.getType(), zero);

	if (builder.getF64Type() == type)
	{
		return makeFNEQ(builder, source, zeroValue, source.getLoc());
	}

	return makeINEQ(builder, source, zeroValue, source.getLoc());
}

static mlir::Value lowerToIntCast(
		mlir::rlc::CastOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs)
{
	if (isRLCFInt(op.getResult().getType()))
	{
		if (isRLCFloat(op.getLhs().getType()))
			return builder.create<mlir::LLVM::FPToSIOp>(
					lhs.getLoc(), builder.getI64Type(), lhs);

		auto isZero = makeINEQ(
				builder,
				lhs,
				builder.create<mlir::LLVM::ConstantOp>(
						op.getLoc(),
						builder.getI8Type(),
						builder.getZeroAttr(builder.getI8Type())),
				op.getLoc());
		return builder.create<mlir::LLVM::ZExtOp>(
				lhs.getLoc(), builder.getI64Type(), isZero);
	}

	if (isRLCFloat(op.getResult().getType()))
	{
		if (isRLCFInt(op.getLhs().getType()))
			return builder.create<mlir::LLVM::SIToFPOp>(
					lhs.getLoc(), builder.getF64Type(), lhs);

		auto isZero = makeINEQ(
				builder,
				lhs,
				builder.create<mlir::LLVM::ConstantOp>(
						op.getLoc(),
						builder.getI8Type(),
						builder.getZeroAttr(builder.getI8Type())),
				op.getLoc());
		return builder.create<mlir::LLVM::UIToFPOp>(
				lhs.getLoc(), builder.getF64Type(), isZero);
	}
	return toBool(builder, lhs);
}

static mlir::Value lowerLessEqual(
		mlir::rlc::LessEqualOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return makeFCMP(
				builder, mlir::LLVM::FCmpPredicate::ole, lhs, rhs, op.getLoc());
	return makeICMP(
			builder, mlir::LLVM::ICmpPredicate::sle, lhs, rhs, op.getLoc());
}

static mlir::Value lowerGreater(
		mlir::rlc::GreaterOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return makeFCMP(
				builder, mlir::LLVM::FCmpPredicate::ogt, lhs, rhs, op.getLoc());
	return makeICMP(
			builder, mlir::LLVM::ICmpPredicate::sgt, lhs, rhs, op.getLoc());
}

static mlir::Value lowerAdd(
		mlir::rlc::AddOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return builder.create<mlir::LLVM::FAddOp>(op.getLoc(), lhs, rhs);

	return builder.create<mlir::LLVM::AddOp>(op.getLoc(), lhs, rhs);
}

static mlir::Value lowerMult(
		mlir::rlc::MultOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return builder.create<mlir::LLVM::FMulOp>(op.getLoc(), lhs, rhs);

	return builder.create<mlir::LLVM::MulOp>(op.getLoc(), lhs, rhs);
}

static mlir::Value lowerOr(
		mlir::rlc::OrOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	return builder.create<mlir::LLVM::OrOp>(op.getLoc(), lhs, rhs);
}

static mlir::Value lowerAnd(
		mlir::rlc::AndOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	return builder.create<mlir::LLVM::AndOp>(op.getLoc(), lhs, rhs);
}

static mlir::Value lowerGreaterEqual(
		mlir::rlc::GreaterEqualOp op,
		mlir::ConversionPatternRewriter& builder,
		mlir::Value lhs,
		mlir::Value rhs)
{
	if (isRLCFloat(op.getLhs().getType()))
		return makeFCMP(
				builder, mlir::LLVM::FCmpPredicate::oge, lhs, rhs, op.getLoc());
	return makeICMP(
			builder, mlir::LLVM::ICmpPredicate::sge, lhs, rhs, op.getLoc());
}

void rlc::RLCToLLVMLoweringPass::runOnOperation()
{
	mlir::TypeConverter converter;
	mlir::rlc::registerConversions(converter);

	mlir::ConversionTarget target(getContext());

	target.addLegalDialect<mlir::BuiltinDialect, mlir::LLVM::LLVMDialect>();
	target.addIllegalDialect<mlir::rlc::RLCDialect>();

	mlir::RewritePatternSet patterns(&getContext());
	patterns.add<FunctionRewriter>(converter, &getContext())
			.add<CallRewriter>(converter, &getContext())
			.add<ConstantRewriter>(converter, &getContext())
			.add<CbrRewriter>(converter, &getContext())
			.add<BrRewriter>(converter, &getContext())
			.add<SelectRewriter>(converter, &getContext())
			.add<AssignRewriter>(converter, &getContext())
			.add<InitRewriter>(converter, &getContext())
			.add(makeArith(lowerLess, converter, &getContext()))
			.add(makeArith(lowerLessEqual, converter, &getContext()))
			.add(makeArith(lowerGreaterEqual, converter, &getContext()))
			.add(makeArith(lowerGreater, converter, &getContext()))
			.add(makeArith(lowerDivide, converter, &getContext()))
			.add(makeArith(lowerReminder, converter, &getContext()))
			.add(makeArith(lowerSub, converter, &getContext()))
			.add(makeArith(lowerMinus, converter, &getContext()))
			.add(makeArith(lowerToIntCast, converter, &getContext()))
			.add(makeArith(lowerAnd, converter, &getContext()))
			.add(makeArith(lowerNot, converter, &getContext()))
			.add(makeArith(lowerEqual, converter, &getContext()))
			.add(makeArith(lowerNEqual, converter, &getContext()))
			.add(makeArith(lowerOr, converter, &getContext()))
			.add(makeArith(lowerAdd, converter, &getContext()))
			.add(makeArith(lowerMult, converter, &getContext()))
			.add<YieldRewriter>(converter, &getContext())
			.add<ArrayAccessRewriter>(converter, &getContext())
			.add<UninitializedConstructRewriter>(converter, &getContext())
			.add<MemberAccessRewriter>(converter, &getContext())
			.add<ReferenceRewriter>(converter, &getContext())
			.add<EntityDeclarationEraser>(converter, &getContext())
			.add<ConstructRewriter>(converter, &getContext());

	if (failed(
					applyPartialConversion(getOperation(), target, std::move(patterns))))
		signalPassFailure();
}

void rlc::RLCToCfLoweringPass::runOnOperation()
{
	if (flattenModule(getOperation()).failed())
		signalPassFailure();
}

static llvm::SmallVector<mlir::Block*, 4> splitActionBlocks(
		mlir::rlc::FlatFunctionOp fun,
		mlir::IRRewriter& rewriter,
		mlir::Value resumeIndex)
{
	llvm::SmallVector<mlir::Operation*> ops;
	for (auto& operation : fun.getBody().getOps())
		ops.push_back(&operation);

	for (auto* operation : ops)
	{
		if (not mlir::isa<mlir::rlc::ReturnStatement>(operation) and
				not mlir::isa<mlir::rlc::Yield>(operation))
			continue;

		rewriter.setInsertionPoint(operation);
		auto newResumeIndexValue = rewriter.create<mlir::rlc::Constant>(
				fun.getLoc(), static_cast<int64_t>(-1));

		rewriter.create<mlir::rlc::AssignOp>(
				fun.getLoc(), resumeIndex, newResumeIndexValue);
	}

	llvm::SmallVector<mlir::Block*, 4> resumePoints;
	llvm::SmallVector<mlir::Block*, 4> blocksToAnalyze;

	for (auto& block : fun.getBlocks())
		blocksToAnalyze.push_back(&block);

	int64_t newResumeIndex = 1;
	while (not blocksToAnalyze.empty())
	{
		auto& block = blocksToAnalyze.back();
		blocksToAnalyze.pop_back();

		for (auto iter = block->begin(); iter != block->end(); iter++)
		{
			auto casted = mlir::dyn_cast<mlir::rlc::ActionStatement>(*iter);
			if (casted == nullptr)
				continue;

			resumePoints.push_back(rewriter.splitBlock(block, std::next(iter)));
			blocksToAnalyze.push_back(resumePoints.back());

			rewriter.setInsertionPoint(casted);

			auto newResumeIndexValue =
					rewriter.create<mlir::rlc::Constant>(casted.getLoc(), newResumeIndex);
			newResumeIndex++;
			rewriter.create<mlir::rlc::AssignOp>(
					casted.getLoc(), resumeIndex, newResumeIndexValue);
			rewriter.create<mlir::rlc::Yield>(casted.getLoc());
			rewriter.eraseOp(casted);

			break;
		}
	}

	return resumePoints;
}

static mlir::LogicalResult actionsToBraches(mlir::rlc::FlatFunctionOp fun)
{
	mlir::IRRewriter rewriter(fun.getContext());

	if (fun.getBody().getOps<mlir::rlc::ActionStatement>().empty())
		return mlir::success();

	mlir::IRRewriter builder(fun.getContext());
	for (auto op : fun.getBody().getOps<mlir::rlc::ActionStatement>())
	{
		for (const auto& [res, name] :
				 llvm::zip(op.getResults(), op.getDeclaredNames()))
		{
			llvm::SmallVector<mlir::OpOperand*, 4> uses;
			for (auto& use : res.getUses())
			{
				uses.push_back(&use);
			}
			for (auto* use : uses)
			{
				rewriter.setInsertionPoint(use->getOwner());
				auto frame = fun.getBlocks().front().getArgument(0);
				auto entityType = frame.getType().cast<mlir::rlc::EntityType>();

				size_t fieldIndex = std::distance(
						entityType.getFieldNames().begin(),
						llvm::find(
								entityType.getFieldNames(),
								name.cast<mlir::StringAttr>().str()));
				auto ref = rewriter.create<mlir::rlc::MemberAccess>(
						op.getLoc(), frame, fieldIndex);

				use->set(ref);
			}
		}
	}

	mlir::Block& entry = *fun.getBlocks().begin();
	auto* everythingElse = rewriter.splitBlock(&entry, entry.begin());

	rewriter.setInsertionPoint(&entry, entry.begin());
	auto routineIndex = rewriter.create<mlir::rlc::MemberAccess>(
			fun.getLoc(), entry.getArgument(0), 0);

	auto resumePoints = splitActionBlocks(fun, rewriter, routineIndex);
	resumePoints.insert(resumePoints.begin(), everythingElse);

	rewriter.setInsertionPoint(&entry, entry.end());
	rewriter.create<mlir::rlc::SelectBranch>(
			fun.getLoc(), routineIndex, resumePoints);
	return mlir::success();
}

static void replaceAllAccessesWithIndirectOnes(
		mlir::rlc::ActionFunction action, mlir::rlc::ModuleBuilder& builder)
{
	mlir::IRRewriter rewriter(action.getContext());

	for (auto* region : { &action.getBody(), &action.getPrecondition() })
	{
		auto argument = region->front().addArgument(
				builder.typeOfAction(action), action.getLoc());

		rewriter.setInsertionPointToStart(&region->front());

		for (const auto& arg :
				 llvm::enumerate(llvm::drop_end(region->front().getArguments())))
		{
			llvm::SmallVector<mlir::OpOperand*, 4> operands;
			for (auto& operand : arg.value().getUses())
			{
				operands.push_back(&operand);
			}
			for (auto& use : operands)
			{
				rewriter.setInsertionPoint(use->getOwner());
				auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
						use->getOwner()->getLoc(), argument, arg.index() + 1);
				use->set(refToMember);
			}
		}

		while (region->front().getNumArguments() != 1)
			region->front().eraseArgument(static_cast<size_t>(0));
	}
}

static void replaceAllDeclarationsWithIndirectOnes(
		mlir::rlc::ActionFunction action, mlir::rlc::ModuleBuilder& builder)
{
	mlir::IRRewriter rewriter(action.getContext());

	auto refToEntity = action.getBlocks().front().getArgument(0);
	auto type = refToEntity.getType().cast<mlir::rlc::EntityType>();

	llvm::SmallVector<mlir::rlc::DeclarationStatement, 4> decls;
	action.walk([&](mlir::rlc::DeclarationStatement statement) {
		decls.push_back(statement);
	});

	for (auto decl : decls)
	{
		size_t memberIndex = std::distance(
				type.getFieldNames().begin(),
				llvm::find(type.getFieldNames(), decl.getSymName()));

		llvm::SmallVector<mlir::OpOperand*, 4> uses;
		for (auto& operand : decl.getResult().getUses())
			uses.push_back(&operand);
		for (auto* operand : uses)
		{
			rewriter.setInsertionPoint(operand->getOwner());

			auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
					decl.getLoc(), refToEntity, memberIndex);

			operand->set(refToMember.getResult());
		}

		rewriter.setInsertionPointAfter(decl);
		auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
				decl.getLoc(), refToEntity, memberIndex);
		assert(decl.getType() != nullptr);
		rewriter.create<mlir::rlc::CallOp>(
				decl.getLoc(),
				builder.getAssignFunctionOf(decl.getType()),
				mlir::ValueRange({ refToMember, decl }));
	}
}

static void emitPreconditionCheckerWrapper(
		mlir::rlc::ActionFunction action, mlir::rlc::ModuleBuilder& builder)
{
	mlir::IRRewriter rewriter(action.getContext());
	rewriter.setInsertionPoint(action);

	auto ftype = mlir::FunctionType::get(
			action.getContext(),
			action.getFunctionType().getInputs(),
			{ mlir::rlc::BoolType::get(action.getContext()) });
	auto validityFunction = rewriter.create<mlir::rlc::FlatFunctionOp>(
			action.getLoc(),
			("can_" + action.getUnmangledName()).str(),
			ftype,
			action.getArgNamesAttr());
	rewriter.cloneRegionBefore(
			action.getPrecondition(),
			validityFunction.getBody(),
			validityFunction.getBody().begin());

	auto& yieldedConditions = validityFunction.getBody().front().back();
	rewriter.setInsertionPoint(&yieldedConditions);

	mlir::Value lastOperand =
			rewriter.create<mlir::rlc::Constant>(action.getLoc(), true);
	for (auto value : yieldedConditions.getOperands())
		lastOperand =
				rewriter.create<mlir::rlc::AndOp>(action.getLoc(), lastOperand, value);
	rewriter.replaceOpWithNewOp<mlir::rlc::Yield>(
			&yieldedConditions, mlir::ValueRange({ lastOperand }));
}

static void emitPreconditionCheckerWrapper(
		mlir::rlc::ActionFunction root,
		mlir::FunctionType actionType,
		mlir::rlc::FunctionOp action,
		mlir::rlc::ModuleBuilder& builder)
{
	mlir::IRRewriter rewriter(action.getContext());
	rewriter.setInsertionPoint(root);

	action.getPrecondition().front().insertArgument(
			static_cast<unsigned int>(0),
			action.getArgumentTypes().front(),
			action.getLoc());

	for (auto& ins : action.getPrecondition().front().getOperations())
	{
		for (auto& operand : ins.getOpOperands())
		{
			if (root.getBody().front().getArgument(0) == operand.get())
				operand.set(action.getPrecondition().getArgument(0));
		}
	}

	auto validityFunction = rewriter.create<mlir::rlc::FlatFunctionOp>(
			action.getLoc(),
			("can_" + action.getUnmangledName()).str(),
			mlir::FunctionType::get(
					action.getContext(),
					actionType.getInputs(),
					{ mlir::rlc::BoolType::get(action.getContext()) }),
			action.getArgNames());

	rewriter.cloneRegionBefore(
			action.getPrecondition(),
			validityFunction.getBody(),
			validityFunction.getBody().begin());

	auto& yieldedConditions = validityFunction.getBody().front().back();
	rewriter.setInsertionPoint(&yieldedConditions);

	mlir::Value lastOperand =
			rewriter.create<mlir::rlc::Constant>(action.getLoc(), true);
	for (auto value : yieldedConditions.getOperands())
		lastOperand =
				rewriter.create<mlir::rlc::AndOp>(action.getLoc(), lastOperand, value);
	rewriter.replaceOpWithNewOp<mlir::rlc::Yield>(
			&yieldedConditions, mlir::ValueRange({ lastOperand }));
}

static void emitActionWrapperCalls(
		mlir::rlc::ActionFunction action, mlir::rlc::ModuleBuilder& builder)
{
	mlir::IRRewriter rewriter(action.getContext());

	rewriter.setInsertionPoint(action);
	auto f = rewriter.create<mlir::rlc::FunctionOp>(
			action.getLoc(),
			action.getUnmangledName(),
			action.getType().cast<mlir::FunctionType>(),
			action.getArgNames());

	llvm::SmallVector<mlir::Location, 2> locs;
	for (size_t i = 0; i < f.getFunctionType().getInputs().size(); i++)
		locs.push_back(action.getLoc());

	rewriter.createBlock(
			&f.getBody(), f.getBody().begin(), f.getFunctionType().getInputs(), locs);

	action.getOperation()->getResult(0).replaceAllUsesWith(f);

	auto entityType = builder.typeOfAction(action).cast<mlir::rlc::EntityType>();
	auto frame = rewriter.create<mlir::rlc::ConstructOp>(
			action.getLoc(), entityType, builder.getInitFunctionOf(entityType));

	auto ptrToIndex =
			rewriter.create<mlir::rlc::MemberAccess>(action.getLoc(), frame, 0);

	auto constantZero =
			rewriter.create<mlir::rlc::Constant>(action.getLoc(), int64_t(0));

	rewriter.create<mlir::rlc::AssignOp>(
			action.getLoc(), ptrToIndex, constantZero);

	for (const auto& argument : llvm::enumerate(action.getArgNames()))
	{
		auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
				action.getLoc(), frame, argument.index() + 1);

		rewriter.create<mlir::rlc::AssignOp>(
				action.getLoc(),
				addressOfArgInFrame,
				f.getBlocks().front().getArgument(argument.index()));
	}

	rewriter.create<mlir::rlc::CallOp>(
			f.getLoc(),
			mlir::TypeRange(),
			action.getResult(),
			mlir::Value({ frame }));

	rewriter.create<mlir::rlc::Yield>(f.getLoc(), mlir::Value({ frame }));

	for (auto& subAction :
			 llvm::enumerate(builder.actionStatementsOfAction(action)))
	{
		rewriter.setInsertionPoint(action);
		auto subAct = mlir::cast<mlir::rlc::ActionStatement>(*subAction.value());
		auto type = action.getActions()[subAction.index()]
										.getType()
										.cast<mlir::FunctionType>();
		auto subF = rewriter.create<mlir::rlc::FunctionOp>(
				action.getLoc(), subAct.getName(), type, subAct.getDeclaredNames());
		subF.getPrecondition().takeBody(subAct.getPrecondition());

		action.getActions()[subAction.index()].replaceAllUsesWith(subF);

		llvm::SmallVector<mlir::Location, 2> locs;
		for (size_t i = 0; i < type.getInputs().size(); i++)
			locs.push_back(action.getLoc());

		rewriter.createBlock(
				&subF.getBody(), subF.getBody().begin(), type.getInputs(), locs);

		emitPreconditionCheckerWrapper(action, type, subF, builder);

		for (auto arg : llvm::enumerate(subAct.getDeclaredNames()))
		{
			size_t fieldIndex = std::distance(
					entityType.getFieldNames().begin(),
					llvm::find(
							entityType.getFieldNames(),
							arg.value().cast<mlir::StringAttr>().str()));

			auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
					action.getLoc(), subF.getBlocks().front().getArgument(0), fieldIndex);

			rewriter.create<mlir::rlc::AssignOp>(
					action.getLoc(),
					addressOfArgInFrame,
					subF.getBlocks().front().getArgument(arg.index() + 1));
		}
		rewriter.create<mlir::rlc::CallOp>(
				subF.getLoc(),
				mlir::TypeRange(),
				action.getResult(),
				mlir::Value({ subF.getBlocks().front().getArgument(0) }));
		rewriter.create<mlir::rlc::Yield>(subF.getLoc());
	}
}

void rlc::RLCLowerActions::runOnOperation()
{
	mlir::rlc::ModuleBuilder builder(getOperation());
	llvm::SmallVector<mlir::rlc::ActionFunction, 4> acts(
			getOperation().getOps<mlir::rlc::ActionFunction>());
	mlir::IRRewriter rewriter(getOperation().getContext());

	for (auto action : acts)
	{
		emitPreconditionCheckerWrapper(action, builder);
		replaceAllAccessesWithIndirectOnes(action, builder);
		replaceAllDeclarationsWithIndirectOnes(action, builder);
		emitActionWrapperCalls(action, builder);

		rewriter.setInsertionPoint(action);
		auto isDoneFunction = rewriter.create<mlir::rlc::FunctionOp>(
				action.getLoc(),
				"is_done",
				action.getIsDoneFunctionType(),
				rewriter.getStrArrayAttr({ "frame" }));

		auto* block = rewriter.createBlock(
				&isDoneFunction.getBody(),
				isDoneFunction.getBody().begin(),
				action.getEntityType(),
				{ action.getLoc() });
		rewriter.setInsertionPoint(block, block->begin());
		auto index = rewriter.create<mlir::rlc::MemberAccess>(
				action.getLoc(), block->getArgument(0), 0);
		auto constant = rewriter.create<mlir::rlc::Constant>(
				action.getLoc(), static_cast<int64_t>(-1));
		auto toReturn =
				rewriter.create<mlir::rlc::EqualOp>(action.getLoc(), index, constant);
		auto retStatement = rewriter.create<mlir::rlc::ReturnStatement>(
				action.getLoc(), isDoneFunction.getFunctionType().getResult(0));
		rewriter.createBlock(&retStatement.getBody());
		rewriter.create<mlir::rlc::Yield>(
				action.getLoc(), mlir::ValueRange({ toReturn }));
		action.getIsDoneFunction().replaceAllUsesWith(isDoneFunction);

		rewriter.setInsertionPoint(action);
		auto actionType = builder.typeOfAction(action);
		auto loweredToFun = rewriter.create<mlir::rlc::FunctionOp>(
				action.getLoc(),
				(action.getUnmangledName() + "_impl").str(),
				mlir::FunctionType::get(action.getContext(), { actionType }, {}),
				rewriter.getStrArrayAttr({ "frame" }));
		loweredToFun.getBody().takeBody(action.getBody());
		action.getResult().replaceAllUsesWith(loweredToFun);
		rewriter.eraseOp(action);
	}
}

void rlc::RLCActionsStatementsToCoro::runOnOperation()
{
	for (auto f : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
	{
		if (actionsToBraches(f).failed())
		{
			signalPassFailure();
			return;
		}
	}
}

void rlc::RLCLowerArrayCalls::runOnOperation()
{
	mlir::ConversionTarget target(getContext());

	target.addLegalDialect<mlir::BuiltinDialect, mlir::rlc::RLCDialect>();
	target.addIllegalOp<mlir::rlc::ArrayCallOp, mlir::rlc::ArrayConstructOp>();

	mlir::RewritePatternSet patterns(&getContext());
	patterns.add<ArrayCallRewriter>(&getContext());
	patterns.add<ArrayConstructRewriter>(&getContext());

	if (failed(
					applyPartialConversion(getOperation(), target, std::move(patterns))))
		signalPassFailure();
}

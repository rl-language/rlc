
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static mlir::Value makeAlloca(
		mlir::RewriterBase& rewriter, mlir::Type type, mlir::Location loc)
{
	auto count = rewriter.create<mlir::LLVM::ConstantOp>(
			loc, rewriter.getI64Type(), rewriter.getI64IntegerAttr(1));
	return rewriter.create<mlir::LLVM::AllocaOp>(loc, type, count, 0);
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

class TraitDeclarationEraser
		: public mlir::OpConversionPattern<mlir::rlc::TraitDefinition>
{
	using mlir::OpConversionPattern<
			mlir::rlc::TraitDefinition>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::TraitDefinition op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.eraseOp(op);
		return mlir::success();
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
		assert(mlir::succeeded(convered));
		return mlir::success();
	}
};

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

namespace mlir::rlc
{
#define GEN_PASS_DEF_LOWERTOLLVMPASS
#include "rlc/dialect/Passes.inc"

	struct LowerToLLVMPass: impl::LowerToLLVMPassBase<LowerToLLVMPass>
	{
		using impl::LowerToLLVMPassBase<LowerToLLVMPass>::LowerToLLVMPassBase;
		void runOnOperation() override
		{
			mlir::TypeConverter converter;
			mlir::rlc::registerConversions(converter);

			mlir::ConversionTarget target(getContext());

			target.addLegalDialect<mlir::BuiltinDialect, mlir::LLVM::LLVMDialect>();
			target.addIllegalDialect<mlir::rlc::RLCDialect>();

			mlir::RewritePatternSet patterns(&getContext());
			patterns.add<FunctionRewriter>(converter, &getContext())
					.add<TraitDeclarationEraser>(converter, &getContext())
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

			if (failed(applyPartialConversion(
							getOperation(), target, std::move(patterns))))
				signalPassFailure();
		}
	};
}	 // namespace mlir::rlc

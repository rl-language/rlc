
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Transforms/DialectConversion.h"
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
static mlir::LLVM::LoadOp makeAlignedLoad(
		mlir::RewriterBase& rewriter, mlir::Value pointerTo, mlir::Location loc)
{
	auto aligment =
			mlir::DataLayout::closest(pointerTo.getDefiningOp())
					.getTypePreferredAlignment(pointerTo.getType()
																				 .cast<mlir::LLVM::LLVMPointerType>()
																				 .getElementType());
	return rewriter.create<mlir::LLVM::LoadOp>(loc, pointerTo, aligment);
}

static mlir::LLVM::StoreOp makeAlignedStore(
		mlir::RewriterBase& rewriter,
		mlir::Value toStore,
		mlir::Value pointerTo,
		mlir::Location loc)
{
	auto aligment = mlir::DataLayout::closest(pointerTo.getDefiningOp())
											.getTypePreferredAlignment(toStore.getType());
	return rewriter.create<mlir::LLVM::StoreOp>(
			loc, toStore, pointerTo, aligment);
}

class LowerFree: public mlir::OpConversionPattern<mlir::rlc::FreeOp>
{
	public:
	LowerFree(
			mlir::TypeConverter& converter,
			mlir::MLIRContext* ctx,
			mlir::LLVM::LLVMFuncOp free)
			: mlir::OpConversionPattern<mlir::rlc::FreeOp>::OpConversionPattern(
						converter, ctx),
				free(free)
	{
	}
	mutable mlir::LLVM::LLVMFuncOp free;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::FreeOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto ptr = makeAlignedLoad(rewriter, adaptor.getArgument(), op.getLoc());

		auto ptrcasted = rewriter.create<mlir::LLVM::BitcastOp>(
				op.getLoc(),
				mlir::LLVM::LLVMPointerType::get(getContext()),
				ptr.getResult());
		rewriter.replaceOpWithNewOp<mlir::LLVM::CallOp>(
				op,
				mlir::TypeRange(),
				free.getSymName(),
				mlir::ValueRange({ ptrcasted }));
		return mlir::success();
	}
};

class LowerMalloc: public mlir::OpConversionPattern<mlir::rlc::MallocOp>
{
	public:
	using mlir::OpConversionPattern<mlir::rlc::MallocOp>::OpConversionPattern;

	LowerMalloc(
			mlir::TypeConverter& converter,
			mlir::MLIRContext* ctx,
			mlir::LLVM::LLVMFuncOp malloc)
			: mlir::OpConversionPattern<mlir::rlc::MallocOp>::OpConversionPattern(
						converter, ctx),
				malloc(malloc)
	{
	}
	mutable mlir::LLVM::LLVMFuncOp malloc;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::MallocOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto loadedCount =
				makeAlignedLoad(rewriter, adaptor.getSize(), op.getLoc());
		const auto& dl = mlir::DataLayout::closest(op);

		auto baseType = typeConverter->convertType(op.getType())
												.cast<mlir::LLVM::LLVMPointerType>()
												.getElementType();

		auto count = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI64Type(),
				rewriter.getI64IntegerAttr(dl.getTypeSize(
						baseType.cast<mlir::LLVM::LLVMPointerType>().getElementType())));
		auto loadedSize =
				rewriter.create<mlir::LLVM::MulOp>(op.getLoc(), loadedCount, count);

		auto voidptr = rewriter.create<mlir::LLVM::CallOp>(
				op.getLoc(),
				mlir::TypeRange(
						{ mlir::LLVM::LLVMPointerType::get(rewriter.getContext()) }),
				malloc.getSymName(),
				mlir::ValueRange({ loadedSize }));

		auto ptr = rewriter.create<mlir::LLVM::BitcastOp>(
				op.getLoc(), baseType, voidptr.getResult());

		auto alloca = makeAlloca(
				rewriter, mlir::LLVM::LLVMPointerType::get(ptr.getType()), op.getLoc());
		makeAlignedStore(rewriter, ptr, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);
		return mlir::success();
	}
};

static mlir::Value getOrCreateGlobalString(
		mlir::Location loc,
		mlir::OpBuilder& builder,
		mlir::StringRef name,
		mlir::StringRef value,
		mlir::ModuleOp module)
{
	// Create the global at the entry of the module.
	mlir::LLVM::GlobalOp global;
	if (!(global = module.lookupSymbol<mlir::LLVM::GlobalOp>(name)))
	{
		mlir::OpBuilder::InsertionGuard insertGuard(builder);
		builder.setInsertionPointToStart(module.getBody());
		auto type = mlir::LLVM::LLVMArrayType::get(
				mlir::IntegerType::get(builder.getContext(), 8), value.size());
		global = builder.create<mlir::LLVM::GlobalOp>(
				loc,
				type,
				/*isConstant=*/true,
				mlir::LLVM::Linkage::Internal,
				name,
				builder.getStringAttr(value),
				/*alignment=*/0);
	}

	// Get the pointer to the first character in the global string.
	mlir::Value globalPtr = builder.create<mlir::LLVM::AddressOfOp>(loc, global);
	mlir::Value cst0 = builder.create<mlir::LLVM::ConstantOp>(
			loc, builder.getI64Type(), builder.getIndexAttr(0));
	return builder.create<mlir::LLVM::GEPOp>(
			loc,
			mlir::LLVM::LLVMPointerType::get(
					mlir::IntegerType::get(builder.getContext(), 8)),
			globalPtr,
			mlir::ArrayRef<mlir::Value>({ cst0, cst0 }));
}

class EntityDeclarationRewriter
		: public mlir::OpConversionPattern<mlir::rlc::EntityDeclaration>
{
	using mlir::OpConversionPattern<
			mlir::rlc::EntityDeclaration>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::EntityDeclaration op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto pointerType = mlir::LLVM::LLVMPointerType::get(rewriter.getI8Type());
		auto i64Type = rewriter.getI64Type();
		auto arrayType =
				mlir::LLVM::LLVMArrayType::get(pointerType, op.getMemberTypes().size());
		auto structTest = ::mlir::LLVM::LLVMStructType::getNewIdentified(
				op->getContext(),
				"globalVariableType",
				::mlir::ArrayRef<mlir::Type>({ pointerType, i64Type, arrayType }));

		auto newOp = rewriter.create<mlir::LLVM::GlobalOp>(
				op->getLoc(),
				structTest,
				true,
				mlir::LLVM::Linkage::LinkonceODR,
				op.getType().cast<mlir::rlc::EntityType>().mangledName(),
				mlir::Attribute());

		auto* block = rewriter.createBlock(&newOp.getInitializer());
		rewriter.setInsertionPoint(block, block->begin());
		mlir::Value structValue =
				rewriter.create<mlir::LLVM::UndefOp>(op.getLoc(), structTest);

		auto variableName = getOrCreateGlobalString(
				op.getLoc(),
				rewriter,
				"__globalVariableName" +
						op.getType().cast<mlir::rlc::EntityType>().mangledName(),
				op.getName().str(),
				newOp->getParentOfType<mlir::ModuleOp>());
		structValue = rewriter.create<mlir::LLVM::InsertValueOp>(
				op.getLoc(), structValue, variableName, mlir::ArrayRef<int64_t>({ 0 }));

		auto numberOfFields = op.getMemberNames().size();
		auto numberOfFieldsValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), numberOfFields);
		structValue = rewriter.create<mlir::LLVM::InsertValueOp>(
				op.getLoc(),
				structValue,
				numberOfFieldsValue,
				mlir::ArrayRef<int64_t>({ 1 }));

		for (unsigned i = 0, e = op.getMemberNames().size(); i < e; ++i)
		{
			auto variableName = getOrCreateGlobalString(
					op.getLoc(),
					rewriter,
					op.getMemberNames()[i].cast<mlir::StringAttr>(),
					op.getMemberNames()[i].cast<mlir::StringAttr>(),
					newOp->getParentOfType<mlir::ModuleOp>());
			structValue = rewriter.create<mlir::LLVM::InsertValueOp>(
					op.getLoc(),
					structValue,
					variableName,
					mlir::ArrayRef<int64_t>({ 2, i }));
		}

		rewriter.create<mlir::LLVM::ReturnOp>(
				op->getLoc(), mlir::ValueRange({ structValue }));
		rewriter.eraseOp(op);

		return mlir::success();
	}
};

class ExplicitConstructRewriter
		: public mlir::OpConversionPattern<mlir::rlc::ExplicitConstructOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::ExplicitConstructOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::ExplicitConstructOp op,
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

class EnumUseLowerer: public mlir::OpConversionPattern<mlir::rlc::EnumUse>
{
	using mlir::OpConversionPattern<mlir::rlc::EnumUse>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::EnumUse op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type = getTypeConverter()->convertType(op.getResult().getType());
		auto alloca = makeAlloca(rewriter, type, op.getLoc());

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto dataType = type.cast<mlir::LLVM::LLVMPointerType>().getElementType();
		auto constantZero = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto gep = rewriter.create<mlir::LLVM::GEPOp>(
				op.getLoc(),
				mlir::LLVM::LLVMPointerType::get(dataType),
				alloca,
				mlir::ValueRange({ constantZero, constantZero }));

		auto value = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI64Type(),
				rewriter.getI64IntegerAttr(op.getEnumValue()));

		makeAlignedStore(rewriter, value, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);

		return mlir::success();
	}
};

class SetActiveOpRewriter
		: public mlir::OpConversionPattern<mlir::rlc::SetActiveEntryOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::SetActiveEntryOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::SetActiveEntryOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto lastElementIndex = adaptor.getToSet()
																.getType()
																.cast<mlir::LLVM::LLVMPointerType>()
																.getElementType()
																.cast<mlir::LLVM::LLVMStructType>()
																.getBody()
																.size() -
														1;

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto indexOp = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI32Type(),
				rewriter.getI32IntegerAttr(lastElementIndex));

		auto gep = rewriter.create<mlir::LLVM::GEPOp>(
				op.getLoc(),
				mlir::LLVM::LLVMPointerType::get(rewriter.getI64Type()),
				adaptor.getToSet(),
				mlir::ValueRange({ zeroValue, indexOp }));

		auto value = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI64Type(),
				rewriter.getI64IntegerAttr(op.getNewActive()));

		makeAlignedStore(rewriter, value, gep, op.getLoc());
		rewriter.eraseOp(op);

		return mlir::success();
	}
};

class IsOpRewriter: public mlir::OpConversionPattern<mlir::rlc::IsOp>
{
	using mlir::OpConversionPattern<mlir::rlc::IsOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::IsOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto resType = typeConverter->convertType(op.getType());
		auto resultAlloca = makeAlloca(rewriter, resType, op.getLoc());
		assert(op.getExpression().getType().isa<mlir::rlc::AlternativeType>());
		auto type = op.getExpression().getType().cast<mlir::rlc::AlternativeType>();
		const auto* index = llvm::find(type.getUnderlying(), op.getTypeOrTrait());

		auto unionIndex = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI64Type(),
				rewriter.getI64IntegerAttr(
						std::distance(type.getUnderlying().begin(), index)));

		assert(index != type.getUnderlying().end());

		auto lastElementIndex = adaptor.getExpression()
																.getType()
																.cast<mlir::LLVM::LLVMPointerType>()
																.getElementType()
																.cast<mlir::LLVM::LLVMStructType>()
																.getBody()
																.size() -
														1;

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto indexOp = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI32Type(),
				rewriter.getI32IntegerAttr(lastElementIndex));

		auto gep = rewriter.create<mlir::LLVM::GEPOp>(
				op.getLoc(),
				mlir::LLVM::LLVMPointerType::get(rewriter.getI64Type()),
				adaptor.getExpression(),
				mlir::ValueRange({ zeroValue, indexOp }));

		auto load = makeAlignedLoad(rewriter, gep, gep.getLoc());

		auto res = rewriter.create<mlir::LLVM::ICmpOp>(
				op.getLoc(),
				rewriter.getI1Type(),
				mlir::LLVM::ICmpPredicate::eq,
				load,
				unionIndex);
		auto extended = rewriter.create<mlir::LLVM::ZExtOp>(
				op.getLoc(), rewriter.getI8Type(), res);

		makeAlignedStore(rewriter, extended, resultAlloca, op.getLoc());
		rewriter.replaceOp(op, resultAlloca);

		return mlir::success();
	}
};

class FlatActionStatementEraser
		: public mlir::OpConversionPattern<mlir::rlc::FlatActionStatement>
{
	using mlir::OpConversionPattern<
			mlir::rlc::FlatActionStatement>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::FlatActionStatement op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.replaceOpWithNewOp<mlir::LLVM::BrOp>(
				op, mlir::ValueRange(), op.getNexts().front());
		return mlir::success();
	}
};

class EnumDeclarationEraser
		: public mlir::OpConversionPattern<mlir::rlc::EnumDeclarationOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::EnumDeclarationOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::EnumDeclarationOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

class ValueUpcastRewriter
		: public mlir::OpConversionPattern<mlir::rlc::ValueUpcastOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::ValueUpcastOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::ValueUpcastOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		if (op.getInput().getType() != op.getResult().getType() and
				not op.getInput().getType().isa<mlir::rlc::AlternativeType>())
		{
			op.emitError("internal error somehow a template upcast did not had the "
									 "same input and output type at lowering time");
			return mlir::failure();
		}
		if (not op.getInput().getType().isa<mlir::rlc::AlternativeType>())
		{
			op.replaceAllUsesWith(op.getInput());
			rewriter.eraseOp(op);
		}
		else
		{
			auto converted =
					getTypeConverter()->convertType(op.getResult().getType());
			rewriter.replaceOpWithNewOp<mlir::LLVM::BitcastOp>(
					op, converted, adaptor.getInput());
		}

		return mlir::success();
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
		auto loaded =
				makeAlignedLoad(rewriter, adaptor.getMemberIndex(), op.getLoc());
		auto array_type = adaptor.getValue()
													.getType()
													.cast<mlir::LLVM::LLVMPointerType>()
													.getElementType();

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);

		if (auto casted = array_type.dyn_cast<mlir::LLVM::LLVMArrayType>())
		{
			mlir::Type elem_type = casted.getElementType();

			auto gep = rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
					op,
					mlir::LLVM::LLVMPointerType::get(elem_type),
					adaptor.getValue(),
					mlir::ValueRange({ zeroValue, loaded }));
			return mlir::LogicalResult::success();
		}

		auto elem_type =
				array_type.cast<mlir::LLVM::LLVMPointerType>().getElementType();
		auto loadedPointerToMallocated =
				makeAlignedLoad(rewriter, adaptor.getValue(), op.getLoc());
		auto gep = rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
				op,
				mlir::LLVM::LLVMPointerType::get(elem_type),
				loadedPointerToMallocated,
				mlir::ValueRange({ loaded }));
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

		makeAlignedStore(rewriter, zeroValue, adaptor.getLhs(), op.getLoc());
		rewriter.eraseOp(op);

		return mlir::LogicalResult::success();
	}
};

class AssignRewriter
		: public mlir::OpConversionPattern<mlir::rlc::BuiltinAssignOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::BuiltinAssignOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::BuiltinAssignOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.setInsertionPoint(op);
		auto loaded = makeAlignedLoad(rewriter, adaptor.getRhs(), op.getLoc());
		makeAlignedStore(rewriter, loaded, adaptor.getLhs(), op.getLoc());
		rewriter.replaceOp(op, adaptor.getLhs());

		return mlir::LogicalResult::success();
	}
};

class CopyRewriter: public mlir::OpConversionPattern<mlir::rlc::CopyOp>
{
	using mlir::OpConversionPattern<mlir::rlc::CopyOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::CopyOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto result =
				makeAlloca(rewriter, adaptor.getToCopy().getType(), op.getLoc());

		auto loaded = makeAlignedLoad(rewriter, adaptor.getToCopy(), op.getLoc());
		makeAlignedStore(rewriter, loaded, result, op.getLoc());
		rewriter.replaceOp(op, result);

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
		assert(op.getOnEnd().empty());
		if (not adaptor.getArguments().empty())
		{
			auto loaded =
					makeAlignedLoad(rewriter, adaptor.getArguments()[0], op.getLoc());
			makeAlignedStore(
					rewriter,
					loaded,
					op->getParentOfType<mlir::LLVM::LLVMFuncOp>().getArguments()[0],
					op.getLoc());
		}
		auto newOp = rewriter.replaceOpWithNewOp<mlir::LLVM::ReturnOp>(
				op, mlir::ValueRange());

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
				makeAlignedLoad(rewriter, adaptor.getOperands()[I], op.getLoc())...);
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
		makeAlignedStore(rewriter, res, alloca, op.getLoc());
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
		rewriter.setInsertionPoint(op);
		llvm::SmallVector<mlir::Value, 2> args;
		assert(op.getNumResults() <= 1);
		args.push_back(adaptor.getCallee());
		auto loc = op.getLoc();

		if (op.getNumResults() == 1 and
				not op.getResult(0).getType().isa<mlir::rlc::VoidType>())
		{
			auto type = op.getResult(0).getType();

			auto res = typeConverter->convertType(type);

			auto resultType =
					res.cast<mlir::LLVM::LLVMPointerType>().getElementType();

			auto alloca = makeAlloca(
					rewriter, mlir::LLVM::LLVMPointerType::get(resultType), op.getLoc());

			args.push_back(alloca);
			rewriter.replaceOp(op, alloca);
		}
		else
		{
			rewriter.eraseOp(op);
		}

		for (auto arg : adaptor.getArgs())
			args.push_back(arg);

		auto newOp = rewriter.create<mlir::LLVM::CallOp>(
				op.getLoc(), mlir::ValueRange(), args);

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
		auto loaded = makeAlignedLoad(rewriter, adaptor.getCond(), op.getLoc());

		llvm::SmallVector<int32_t, 2> indexes;
		llvm::SmallVector<mlir::ValueRange, 2> arguments;
		for (const auto& enumeration : llvm::enumerate(op.getSuccessors()))
		{
			indexes.push_back(enumeration.index());
			arguments.emplace_back(mlir::ValueRange());
		}

		auto truncated = rewriter.create<mlir::LLVM::TruncOp>(
				op.getLoc(), rewriter.getI32Type(), loaded);
		rewriter.replaceOpWithNewOp<mlir::LLVM::SwitchOp>(
				op,
				truncated,
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
		auto loaded = makeAlignedLoad(rewriter, adaptor.getCond(), op.getLoc());
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

class ToByteArrayRewriter
		: public mlir::OpConversionPattern<mlir::rlc::AsByteArrayOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::AsByteArrayOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::AsByteArrayOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto resultType = typeConverter->convertType(op.getResult().getType());
		const auto& dl = mlir::DataLayout::closest(op);
		auto reinterpretedSize =
				dl.getTypeSize(adaptor.getLhs()
													 .getType()
													 .cast<mlir::LLVM::LLVMPointerType>()
													 .getElementType());
		auto sameSizeIntType = rewriter.getIntegerType(reinterpretedSize * 8);

		mlir::Operation* loadedValue =
				makeAlignedLoad(rewriter, adaptor.getLhs(), op.getLoc());
		loadedValue = rewriter.create<mlir::LLVM::BitcastOp>(
				op.getLoc(), sameSizeIntType, loadedValue->getResult(0));

		mlir::Operation* result = rewriter.create<mlir::LLVM::UndefOp>(
				op.getLoc(),
				resultType.cast<mlir::LLVM::LLVMPointerType>().getElementType());
		auto eight = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), sameSizeIntType, 8);
		for (size_t byteIndex = 0; byteIndex < reinterpretedSize; byteIndex++)
		{
			auto truncated = rewriter.create<mlir::LLVM::TruncOp>(
					op.getLoc(), rewriter.getI8Type(), loadedValue->getResult(0));
			result = rewriter.create<mlir::LLVM::InsertValueOp>(
					op.getLoc(), result->getResult(0), truncated, byteIndex);
			if (byteIndex != reinterpretedSize - 1)
				loadedValue = rewriter.create<mlir::LLVM::LShrOp>(
						op.getLoc(), loadedValue->getResult(0), eight);
		}

		auto inMemory = makeAlloca(rewriter, resultType, op.getLoc());
		makeAlignedStore(rewriter, result->getResult(0), inMemory, op.getLoc());
		rewriter.replaceOp(op, inMemory);

		return mlir::LogicalResult::success();
	}
};

class FromByteArrayRewriter
		: public mlir::OpConversionPattern<mlir::rlc::FromByteArrayOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::FromByteArrayOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::FromByteArrayOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto resultType = typeConverter->convertType(op.getResult().getType());
		const auto& dl = mlir::DataLayout::closest(op);
		auto reinterpretedSize = dl.getTypeSize(
				resultType.cast<mlir::LLVM::LLVMPointerType>().getElementType());
		auto sameSizeIntType = rewriter.getIntegerType(reinterpretedSize * 8);

		auto loadedArray = makeAlignedLoad(rewriter, adaptor.getLhs(), op.getLoc());

		mlir::Operation* result =
				rewriter.create<mlir::LLVM::UndefOp>(op.getLoc(), sameSizeIntType);
		auto eight = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), sameSizeIntType, 8);
		for (size_t byteIndex = reinterpretedSize; byteIndex != 0; byteIndex--)
		{
			result = rewriter.create<mlir::LLVM::ShlOp>(
					op.getLoc(), result->getResult(0), eight);
			mlir::Value extractedByte = rewriter.create<mlir::LLVM::ExtractValueOp>(
					op.getLoc(), loadedArray, byteIndex - 1);

			if (extractedByte.getType() != sameSizeIntType)
				extractedByte = rewriter.create<mlir::LLVM::SExtOp>(
						op.getLoc(), sameSizeIntType, extractedByte);

			result = rewriter.create<mlir::LLVM::OrOp>(
					op.getLoc(), extractedByte, result->getResult(0));
		}

		auto bitCasted = rewriter.create<mlir::LLVM::BitcastOp>(
				op.getLoc(),
				resultType.cast<mlir::LLVM::LLVMPointerType>().getElementType(),
				result->getResult(0));
		auto inMemory = makeAlloca(rewriter, resultType, op.getLoc());
		makeAlignedStore(rewriter, bitCasted, inMemory, op.getLoc());
		rewriter.replaceOp(op, inMemory);

		return mlir::LogicalResult::success();
	}
};

class AbortRewriter: public mlir::OpConversionPattern<mlir::rlc::AbortOp>
{
	using mlir::OpConversionPattern<mlir::rlc::AbortOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::AbortOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.create<mlir::LLVM::Trap>(op.getLoc());
		rewriter.create<mlir::LLVM::ReturnOp>(op.getLoc(), mlir::ValueRange());
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

class InitializerListLowerer
		: public mlir::OpConversionPattern<mlir::rlc::InitializerListOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::InitializerListOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::InitializerListOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type = getTypeConverter()->convertType(op.getResult().getType());
		auto underlying = type.cast<mlir::LLVM::LLVMPointerType>()
													.getElementType()
													.cast<mlir::LLVM::LLVMArrayType>()
													.getElementType();

		auto alloca = makeAlloca(rewriter, type, op.getLoc());

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);

		for (size_t i = 0; i < adaptor.getArgs().size(); i++)
		{
			auto index = rewriter.getI64IntegerAttr(i);
			auto indexValue = rewriter.create<mlir::LLVM::ConstantOp>(
					op.getLoc(), rewriter.getI64Type(), index);
			auto gep = rewriter.create<mlir::LLVM::GEPOp>(
					op.getLoc(),
					mlir::LLVM::LLVMPointerType::get(underlying),
					alloca,
					mlir::ValueRange({ zeroValue, indexValue }));

			auto loaded =
					makeAlignedLoad(rewriter, adaptor.getArgs()[i], op.getLoc());
			makeAlignedStore(rewriter, loaded, gep, op.getLoc());
		}

		rewriter.replaceOp(op, alloca);
		return mlir::success();
	}
};

class IntegerLiteralRewrtier
		: public mlir::OpConversionPattern<mlir::rlc::IntegerLiteralUse>
{
	using mlir::OpConversionPattern<
			mlir::rlc::IntegerLiteralUse>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::IntegerLiteralUse op,
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
				op.getInputType().cast<mlir::rlc::IntegerLiteralType>().getValue());
		rewriter.replaceOp(op, alloca);
		makeAlignedStore(rewriter, constant, alloca, alloca.getLoc());

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
		makeAlignedStore(rewriter, constant, alloca, alloca.getLoc());

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

		if (not op.isDeclaration() and op.getType().getResults().size() == 1 and
				not op.getType().getResults().front().isa<mlir::rlc::VoidType>())
			newF.getBody().front().insertArgument(
					size_t(0), op.getType().getResults().front(), op.getLoc());

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
	if (op.getResult().getType() == op.getOperand().getType())
	{
		return op.getOperand();
	}
	if (isRLCFInt(op.getResult().getType()))
	{
		auto resType = op.getResult().getType().cast<mlir::rlc::IntegerType>();

		if (isRLCFInt(op.getLhs().getType()))
		{
			auto inputTYpe = op.getLhs().getType().cast<mlir::rlc::IntegerType>();
			if (resType.getSize() < inputTYpe.getSize())
				return builder.create<mlir::LLVM::TruncOp>(
						lhs.getLoc(), builder.getIntegerType(resType.getSize()), lhs);
			return builder.create<mlir::LLVM::SExtOp>(
					lhs.getLoc(), builder.getIntegerType(resType.getSize()), lhs);
		}
		if (isRLCFloat(op.getLhs().getType()))
			return builder.create<mlir::LLVM::FPToSIOp>(
					lhs.getLoc(),
					builder.getIntegerType(op.getResult()
																		 .getType()
																		 .cast<mlir::rlc::IntegerType>()
																		 .getSize()),
					lhs);

		// input is bool
		auto isZero = makeINEQ(
				builder,
				lhs,
				builder.create<mlir::LLVM::ConstantOp>(
						op.getLoc(),
						builder.getI8Type(),
						builder.getZeroAttr(builder.getI8Type())),
				op.getLoc());

		// if the result size is 8, the same as the input, don't bother to zero
		// extend it
		if (resType.getSize() == 8)
			return isZero;

		return builder.create<mlir::LLVM::ZExtOp>(
				lhs.getLoc(), builder.getIntegerType(resType.getSize()), isZero);
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
			mlir::IRRewriter rewriter(&getContext());
			rewriter.setInsertionPoint(
					getOperation().getBody(0), getOperation().getBody(0)->begin());
			auto malloc = rewriter.create<mlir::LLVM::LLVMFuncOp>(
					getOperation().getLoc(),
					"malloc",
					mlir::LLVM::LLVMFunctionType::get(
							mlir::LLVM::LLVMPointerType::get(&getContext()),
							{ rewriter.getI64Type() }));

			auto free = rewriter.create<mlir::LLVM::LLVMFuncOp>(
					getOperation().getLoc(),
					"free",
					mlir::LLVM::LLVMFunctionType::get(
							mlir::LLVM::LLVMVoidType::get(&getContext()),
							{ mlir::LLVM::LLVMPointerType::get(&getContext()) }));

			mlir::TypeConverter converter;
			mlir::rlc::registerConversions(converter, getOperation());
			mlir::ConversionTarget target(getContext());

			target.addLegalDialect<mlir::BuiltinDialect, mlir::LLVM::LLVMDialect>();
			target.addIllegalDialect<mlir::rlc::RLCDialect>();

			mlir::RewritePatternSet patterns(&getContext());
			patterns.add<FunctionRewriter>(converter, &getContext())
					.add<TraitDeclarationEraser>(converter, &getContext())
					.add<ValueUpcastRewriter>(converter, &getContext())
					.add<EnumDeclarationEraser>(converter, &getContext())
					.add<FlatActionStatementEraser>(converter, &getContext())
					.add<EnumUseLowerer>(converter, &getContext())
					.add<CallRewriter>(converter, &getContext())
					.add<ConstantRewriter>(converter, &getContext())
					.add<IntegerLiteralRewrtier>(converter, &getContext())
					.add<IsOpRewriter>(converter, &getContext())
					.add<SetActiveOpRewriter>(converter, &getContext())
					.add<InitializerListLowerer>(converter, &getContext())
					.add<CbrRewriter>(converter, &getContext())
					.add<BrRewriter>(converter, &getContext())
					.add<FromByteArrayRewriter>(converter, &getContext())
					.add<ToByteArrayRewriter>(converter, &getContext())
					.add<SelectRewriter>(converter, &getContext())
					.add<AssignRewriter>(converter, &getContext())
					.add<InitRewriter>(converter, &getContext())
					.add<LowerMalloc>(converter, &getContext(), malloc)
					.add<LowerFree>(converter, &getContext(), free)
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
					.add<CopyRewriter>(converter, &getContext())
					.add<ArrayAccessRewriter>(converter, &getContext())
					.add<UninitializedConstructRewriter>(converter, &getContext())
					.add<MemberAccessRewriter>(converter, &getContext())
					.add<ReferenceRewriter>(converter, &getContext())
					.add<EntityDeclarationRewriter>(converter, &getContext())
					.add<ExplicitConstructRewriter>(converter, &getContext())
					.add<AbortRewriter>(converter, &getContext());

			if (failed(applyPartialConversion(
							getOperation(), target, std::move(patterns))))
				signalPassFailure();
		}
	};
}	 // namespace mlir::rlc

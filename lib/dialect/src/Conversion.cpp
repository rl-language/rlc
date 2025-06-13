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
#include "mlir/Dialect/LLVMIR/LLVMAttrs.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/DebugInfo.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static mlir::Value makeAlloca(
		mlir::RewriterBase& rewriter, mlir::Type type, mlir::Location loc)
{
	auto count = rewriter.create<mlir::LLVM::ConstantOp>(
			loc, rewriter.getI64Type(), rewriter.getI64IntegerAttr(1));
	return rewriter.create<mlir::LLVM::AllocaOp>(
			loc, mlir::LLVM::LLVMPointerType::get(type.getContext()), type, count, 0);
}
static mlir::LLVM::LoadOp makeAlignedLoad(
		mlir::RewriterBase& rewriter,
		mlir::Type elementType,
		mlir::Value pointerTo,
		mlir::Location loc)
{
	auto aligment = mlir::DataLayout::closest(pointerTo.getDefiningOp())
											.getTypePreferredAlignment(elementType);
	return rewriter.create<mlir::LLVM::LoadOp>(
			loc, elementType, pointerTo, aligment);
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

class VarNameLowerer: public mlir::OpConversionPattern<mlir::rlc::VarNameOp>
{
	public:
	mlir::rlc::DebugInfoGenerator& diGenerator;
	bool addDebugInfo;

	VarNameLowerer(
			mlir::TypeConverter& converter,
			mlir::MLIRContext* ctx,
			mlir::rlc::DebugInfoGenerator& diGenerator,
			bool addDebugInfo)
			: mlir::OpConversionPattern<mlir::rlc::VarNameOp>::OpConversionPattern(
						converter, ctx),
				diGenerator(diGenerator),
				addDebugInfo(addDebugInfo)
	{
	}

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::VarNameOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		if (addDebugInfo)
		{
			auto localVar = diGenerator.getLocalVar(
					op.getValue(),
					op.getName(),
					op.getLoc().cast<mlir::FileLineColLoc>());
			if (localVar)
			{
				rewriter.create<mlir::LLVM::DbgDeclareOp>(
						op.getLoc(), adaptor.getValue(), localVar);
			}
		}
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

class NullLowerer: public mlir::OpConversionPattern<mlir::rlc::NullOp>
{
	public:
	using mlir::OpConversionPattern<mlir::rlc::NullOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::NullOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto alloca = makeAlloca(
				rewriter, mlir::LLVM::LLVMPointerType::get(getContext()), op.getLoc());
		auto value = rewriter.create<mlir::LLVM::ZeroOp>(
				op.getLoc(), mlir::LLVM::LLVMPointerType::get(getContext()));
		auto store = makeAlignedStore(rewriter, value, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);
		return mlir::success();
	}
};

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
		auto ptr = makeAlignedLoad(
				rewriter,
				mlir::LLVM::LLVMPointerType::get(getContext()),
				adaptor.getArgument(),
				op.getLoc());

		rewriter.replaceOpWithNewOp<mlir::LLVM::CallOp>(
				op, mlir::TypeRange(), free.getSymName(), mlir::ValueRange({ ptr }));
		return mlir::success();
	}
};

static mlir::LLVM::ConstantOp lowerConstant(
		mlir::ConversionPatternRewriter& rewriter,
		mlir::Attribute attr,
		mlir::Location loc)
{
	if (auto casted = attr.dyn_cast<mlir::BoolAttr>())
	{
		if (casted.getValue())
			return rewriter.create<mlir::LLVM::ConstantOp>(
					loc,
					rewriter.getI8Type(),
					mlir::IntegerAttr::get(rewriter.getI64Type(), 1));
		else
			return rewriter.create<mlir::LLVM::ConstantOp>(
					loc,
					rewriter.getI8Type(),
					mlir::IntegerAttr::get(rewriter.getI64Type(), 0));
	}
	if (auto casted = attr.dyn_cast<mlir::IntegerAttr>())
		return rewriter.create<mlir::LLVM::ConstantOp>(
				loc, casted.getType(), casted.getValue());
	if (auto casted = attr.dyn_cast<mlir::FloatAttr>())
		return rewriter.create<mlir::LLVM::ConstantOp>(
				loc, casted.getType(), casted.getValue());
	assert(false);
	return nullptr;
}

static mlir::LLVM::ConstantOp lowerConstant(
		mlir::ConversionPatternRewriter& rewriter,
		const mlir::TypeConverter* typeConverter,
		mlir::rlc::Constant op)
{
	mlir::Type type = typeConverter->convertType(
			mlir::rlc::ProxyType::get(op.getResult().getType()));

	return lowerConstant(rewriter, op.getValue(), op.getLoc());
}

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
		auto sizeType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getSize().getType()));
		mlir::Value loadedCount =
				makeAlignedLoad(rewriter, sizeType, adaptor.getSize(), op.getLoc());

		// if we are on a platform where the size_t type is 32 bits, trucate the
		// number
		if (loadedCount.getType() != malloc.getArgumentTypes().front())
		{
			loadedCount = rewriter.create<mlir::LLVM::TruncOp>(
					op.getLoc(), malloc.getArgumentTypes().front(), loadedCount);
		}

		const auto& dl = mlir::DataLayout::closest(op);

		auto baseType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(
						op.getType().cast<mlir::rlc::OwningPtrType>().getUnderlying()));

		auto baseSize = rewriter.getI64IntegerAttr(dl.getTypeSize(baseType));
		auto count = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), malloc.getArgumentTypes().front(), baseSize);
		auto loadedSizeAndOverflow =
				rewriter.create<mlir::LLVM::UMulWithOverflowOp>(
						op.getLoc(),
						mlir::LLVM::LLVMStructType::getLiteral(
								getContext(), { loadedCount.getType(), rewriter.getI1Type() }),
						loadedCount,
						count);

		auto loadedSize = rewriter.create<mlir::LLVM::ExtractValueOp>(
				op.getLoc(), loadedSizeAndOverflow, 0);

		auto voidptr = rewriter.create<mlir::LLVM::CallOp>(
				op.getLoc(),
				mlir::TypeRange(
						{ mlir::LLVM::LLVMPointerType::get(rewriter.getContext()) }),
				malloc.getSymName(),
				mlir::ValueRange({ loadedSize }));

		auto alloca = makeAlloca(
				rewriter,
				mlir::LLVM::LLVMPointerType::get(voidptr.getContext()),
				op.getLoc());
		makeAlignedStore(rewriter, voidptr.getResult(), alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);
		return mlir::success();
	}
};

static mlir::Value getOrCreateGlobalString(
		mlir::Location loc,
		mlir::OpBuilder& builder,
		mlir::StringRef maybeName,
		mlir::StringRef value,
		mlir::ModuleOp module,
		llvm::StringMap<mlir::LLVM::GlobalOp>& stringsCache)
{
	// Create the global at the entry of the module.
	auto type = mlir::LLVM::LLVMArrayType::get(
			mlir::IntegerType::get(builder.getContext(), 8), value.size());

	std::string name =
			maybeName.empty()
					? (llvm::Twine("str_") + llvm::Twine(stringsCache.size())).str()
					: maybeName.str();

	mlir::LLVM::GlobalOp global;
	if (auto iter = stringsCache.find(value); iter != stringsCache.end())
	{
		global = iter->second;
	}
	else
	{
		mlir::OpBuilder::InsertionGuard insertGuard(builder);
		builder.setInsertionPointToStart(module.getBody());
		global = builder.create<mlir::LLVM::GlobalOp>(
				loc,
				type,
				/*isConstant=*/true,
				mlir::LLVM::Linkage::Internal,
				name,
				mlir::StringAttr::get(loc.getContext(), value),
				/*alignment=*/0);
		stringsCache[value] = global;
	}

	// Get the pointer to the first character in the global string.
	mlir::Value globalPtr = builder.create<mlir::LLVM::AddressOfOp>(loc, global);
	mlir::Value cst0 = builder.create<mlir::LLVM::ConstantOp>(
			loc, builder.getI64Type(), builder.getIndexAttr(0));
	return builder.create<mlir::LLVM::GEPOp>(
			loc,
			mlir::LLVM::LLVMPointerType::get(builder.getContext()),
			type,
			globalPtr,
			mlir::ArrayRef<mlir::Value>({ cst0, cst0 }));
}

class ClassDeclarationRewriter
		: public mlir::OpConversionPattern<mlir::rlc::ClassDeclaration>
{
	using mlir::OpConversionPattern<
			mlir::rlc::ClassDeclaration>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::ClassDeclaration op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.eraseOp(op);

		return mlir::success();
	}
};

class MakeRefRewriter: public mlir::OpConversionPattern<mlir::rlc::MakeRefOp>
{
	using mlir::OpConversionPattern<mlir::rlc::MakeRefOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::MakeRefOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto converted = getTypeConverter()->convertType(
				mlir::rlc::ProxyType::get(op.getResult().getType()));
		auto alloca = makeAlloca(rewriter, converted, op.getLoc());
		makeAlignedStore(rewriter, adaptor.getRef(), alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);
		return mlir::LogicalResult::success();
	}
};

class DerefRewriter: public mlir::OpConversionPattern<mlir::rlc::DerefOp>
{
	using mlir::OpConversionPattern<mlir::rlc::DerefOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::DerefOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto res = makeAlignedLoad(
				rewriter,
				mlir::LLVM::LLVMPointerType::get(getContext()),
				adaptor.getRef(),
				op.getLoc());
		rewriter.replaceOp(op, res);
		return mlir::LogicalResult::success();
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
		auto mapped =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto alloca = makeAlloca(rewriter, mapped, op.getLoc());

		auto callOp = rewriter.create<mlir::LLVM::CallOp>(
				op.getLoc(),
				mlir::LLVM::LLVMFunctionType::get(
						mlir::LLVM::LLVMVoidType::get(op.getContext()),
						{ alloca.getType() }),
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
		auto structType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getValue().getType()));

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto index = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI32Type(),
				rewriter.getI32IntegerAttr(op.getMemberIndex()));

		auto res = rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
				op,
				mlir::LLVM::LLVMPointerType::get(getContext()),
				structType,
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
		auto type = getTypeConverter()->convertType(
				mlir::rlc::ProxyType::get(op.getResult().getType()));
		auto alloca = makeAlloca(rewriter, type, op.getLoc());

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto constantZero = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto gep = rewriter.create<mlir::LLVM::GEPOp>(
				op.getLoc(),
				mlir::LLVM::LLVMPointerType::get(getContext()),
				type,
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
		auto convertedUnionType =
				typeConverter
						->convertType(mlir::rlc::ProxyType::get(op.getToSet().getType()))
						.cast<mlir::LLVM::LLVMStructType>();
		auto lastElementIndex = convertedUnionType.getBody().size() - 1;

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto indexOp = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI32Type(),
				rewriter.getI32IntegerAttr(lastElementIndex));

		auto gep = rewriter.create<mlir::LLVM::GEPOp>(
				op.getLoc(),
				mlir::LLVM::LLVMPointerType::get(getContext()),
				convertedUnionType,
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
		auto resType =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto resultAlloca = makeAlloca(rewriter, rewriter.getI8Type(), op.getLoc());
		assert(op.getExpression().getType().isa<mlir::rlc::AlternativeType>());
		auto type = op.getExpression().getType().cast<mlir::rlc::AlternativeType>();
		const auto* index = llvm::find(type.getUnderlying(), op.getTypeOrTrait());

		auto unionIndex = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI64Type(),
				rewriter.getI64IntegerAttr(
						std::distance(type.getUnderlying().begin(), index)));

		if (index == type.getUnderlying().end())
		{
			auto value = rewriter.create<mlir::LLVM::ConstantOp>(
					op.getLoc(), rewriter.getI8Type(), rewriter.getI8IntegerAttr(0));
			makeAlignedStore(rewriter, value, resultAlloca, op.getLoc());
			rewriter.replaceOp(op, resultAlloca);
			return mlir::success();
		}

		auto convertedUnionType =
				typeConverter
						->convertType(
								mlir::rlc::ProxyType::get(op.getExpression().getType()))
						.cast<mlir::LLVM::LLVMStructType>();
		auto lastElementIndex = convertedUnionType.getBody().size() - 1;

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);
		auto indexOp = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI32Type(),
				rewriter.getI32IntegerAttr(lastElementIndex));

		auto gep = rewriter.create<mlir::LLVM::GEPOp>(
				op.getLoc(),
				mlir::LLVM::LLVMPointerType::get(getContext()),
				convertedUnionType,
				adaptor.getExpression(),
				mlir::ValueRange({ zeroValue, indexOp }));

		auto load =
				makeAlignedLoad(rewriter, rewriter.getI64Type(), gep, gep.getLoc());

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

class AliasTypeEraser: public mlir::OpConversionPattern<mlir::rlc::TypeAliasOp>
{
	using mlir::OpConversionPattern<mlir::rlc::TypeAliasOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::TypeAliasOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.eraseOp(op);
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
			op.emitError(
					"internal error somehow a template upcast did not had the "
					"same input and output type at lowering time");
			return mlir::failure();
		}
		rewriter.replaceOp(op, op.getInput());

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
		auto indexType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getMemberIndex().getType()));
		auto loaded = makeAlignedLoad(
				rewriter, indexType, adaptor.getMemberIndex(), op.getLoc());

		auto zero = rewriter.getZeroAttr(rewriter.getI64Type());
		auto zeroValue = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), zero);

		if (auto casted = op.getValue().getType().dyn_cast<mlir::rlc::ArrayType>())
		{
			auto array_type =
					typeConverter->convertType(mlir::rlc::ProxyType::get(casted));

			auto gep = rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
					op,
					mlir::LLVM::LLVMPointerType::get(getContext()),
					array_type,
					adaptor.getValue(),
					mlir::ValueRange({ zeroValue, loaded }));
			return mlir::LogicalResult::success();
		}
		else if (
				auto casted =
						op.getValue().getType().dyn_cast<mlir::rlc::StringLiteralType>())
		{
			auto array_type = mlir::IntegerType::get(casted.getContext(), 8);

			auto loadedPointerToString = makeAlignedLoad(
					rewriter,
					mlir::LLVM::LLVMPointerType::get(getContext()),
					adaptor.getValue(),
					op.getLoc());
			auto gep = rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
					op,
					mlir::LLVM::LLVMPointerType::get(getContext()),
					array_type,
					loadedPointerToString,
					mlir::ValueRange({ loaded }));
			return mlir::LogicalResult::success();
		}

		auto elemType = op.getValue()
												.getType()
												.cast<mlir::rlc::OwningPtrType>()
												.getUnderlying();
		auto convertedElemType =
				typeConverter->convertType(mlir::rlc::ProxyType::get(elemType));
		auto loadedPointerToMallocated = makeAlignedLoad(
				rewriter,
				mlir::LLVM::LLVMPointerType::get(getContext()),
				adaptor.getValue(),
				op.getLoc());
		auto gep = rewriter.replaceOpWithNewOp<mlir::LLVM::GEPOp>(
				op,
				mlir::LLVM::LLVMPointerType::get(getContext()),
				convertedElemType,
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
		auto type = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getLhs().getType()));
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
		auto type = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getLhs().getType()));
		auto loaded =
				makeAlignedLoad(rewriter, type, adaptor.getRhs(), op.getLoc());
		makeAlignedStore(rewriter, loaded, adaptor.getLhs(), op.getLoc());
		rewriter.eraseOp(op);

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
		auto type = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getToCopy().getType()));
		auto result = makeAlloca(rewriter, type, op.getLoc());

		auto resultType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getResult().getType()));
		const auto& dl = mlir::DataLayout::closest(op);

		auto len = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getIntegerType(64),
				rewriter.getI64IntegerAttr(dl.getTypeSize(resultType)));
		rewriter.create<mlir::LLVM::MemcpyOp>(
				op.getLoc(), result, adaptor.getToCopy(), len, false);
		rewriter.replaceOp(op, result);

		return mlir::LogicalResult::success();
	}
};

class YieldReferenceRewriter
		: public mlir::OpConversionPattern<mlir::rlc::YieldReference>
{
	using mlir::OpConversionPattern<
			mlir::rlc::YieldReference>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::YieldReference op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.setInsertionPoint(op);
		auto loaded = adaptor.getArgument();
		makeAlignedStore(
				rewriter,
				loaded,
				op->getParentOfType<mlir::LLVM::LLVMFuncOp>().getArguments()[0],
				op.getLoc());
		rewriter.replaceOpWithNewOp<mlir::LLVM::ReturnOp>(op, mlir::ValueRange());

		return mlir::LogicalResult::success();
	}
};

class RightShiftRewriter
		: public mlir::OpConversionPattern<mlir::rlc::RightShiftOp>
{
	using mlir::OpConversionPattern<mlir::rlc::RightShiftOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::RightShiftOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto alloca = makeAlloca(rewriter, type, op.getLoc());
		auto operand1 =
				makeAlignedLoad(rewriter, type, adaptor.getLhs(), op.getLoc());
		auto operand2 =
				makeAlignedLoad(rewriter, type, adaptor.getRhs(), op.getLoc());
		auto result =
				rewriter.create<mlir::LLVM::LShrOp>(op.getLoc(), operand1, operand2);
		makeAlignedStore(rewriter, result, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);

		return mlir::LogicalResult::success();
	}
};

class LeftShiftRewriter
		: public mlir::OpConversionPattern<mlir::rlc::LeftShiftOp>
{
	using mlir::OpConversionPattern<mlir::rlc::LeftShiftOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::LeftShiftOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto alloca = makeAlloca(rewriter, type, op.getLoc());
		auto operand1 =
				makeAlignedLoad(rewriter, type, adaptor.getLhs(), op.getLoc());
		auto operand2 =
				makeAlignedLoad(rewriter, type, adaptor.getRhs(), op.getLoc());
		auto result =
				rewriter.create<mlir::LLVM::ShlOp>(op.getLoc(), operand1, operand2);
		makeAlignedStore(rewriter, result, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);

		return mlir::LogicalResult::success();
	}
};

class LowerIsDoneOp: public mlir::OpConversionPattern<mlir::rlc::IsDoneOp>
{
	using mlir::OpConversionPattern<mlir::rlc::IsDoneOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::IsDoneOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto alloca = makeAlloca(rewriter, rewriter.getI8Type(), op.getLoc());
		auto loadedIndex = makeAlignedLoad(
				rewriter, rewriter.getI64Type(), adaptor.getCoroFrame(), op.getLoc());
		auto mask = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getI64Type(), rewriter.getI64IntegerAttr(-1));

		auto result = rewriter.create<mlir::LLVM::ICmpOp>(
				op.getLoc(),
				rewriter.getI1Type(),
				mlir::LLVM::ICmpPredicate::eq,
				loadedIndex,
				mask);

		auto extended = rewriter.create<mlir::LLVM::ZExtOp>(
				op.getLoc(), rewriter.getI8Type(), result);

		makeAlignedStore(rewriter, extended, alloca, op.getLoc());

		rewriter.replaceOp(op, alloca);
		return mlir::LogicalResult::success();
	}
};

class BitAndRewriter: public mlir::OpConversionPattern<mlir::rlc::BitAndOp>
{
	using mlir::OpConversionPattern<mlir::rlc::BitAndOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::BitAndOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto alloca = makeAlloca(rewriter, type, op.getLoc());
		auto operand1 =
				makeAlignedLoad(rewriter, type, adaptor.getLhs(), op.getLoc());
		auto operand2 =
				makeAlignedLoad(rewriter, type, adaptor.getRhs(), op.getLoc());
		auto result =
				rewriter.create<mlir::LLVM::AndOp>(op.getLoc(), operand1, operand2);
		makeAlignedStore(rewriter, result, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);

		return mlir::LogicalResult::success();
	}
};

class BitOrRewriter: public mlir::OpConversionPattern<mlir::rlc::BitOrOp>
{
	using mlir::OpConversionPattern<mlir::rlc::BitOrOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::BitOrOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto alloca = makeAlloca(rewriter, type, op.getLoc());
		auto operand1 =
				makeAlignedLoad(rewriter, type, adaptor.getLhs(), op.getLoc());
		auto operand2 =
				makeAlignedLoad(rewriter, type, adaptor.getRhs(), op.getLoc());
		auto result =
				rewriter.create<mlir::LLVM::OrOp>(op.getLoc(), operand1, operand2);
		makeAlignedStore(rewriter, result, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);

		return mlir::LogicalResult::success();
	}
};

class BitXorRewriter: public mlir::OpConversionPattern<mlir::rlc::BitXorOp>
{
	using mlir::OpConversionPattern<mlir::rlc::BitXorOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::BitXorOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto alloca = makeAlloca(rewriter, type, op.getLoc());
		auto operand1 =
				makeAlignedLoad(rewriter, type, adaptor.getLhs(), op.getLoc());
		auto operand2 =
				makeAlignedLoad(rewriter, type, adaptor.getRhs(), op.getLoc());
		auto result =
				rewriter.create<mlir::LLVM::XOrOp>(op.getLoc(), operand1, operand2);
		makeAlignedStore(rewriter, result, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);

		return mlir::LogicalResult::success();
	}
};

class BitNotRewriter: public mlir::OpConversionPattern<mlir::rlc::BitNotOp>
{
	using mlir::OpConversionPattern<mlir::rlc::BitNotOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::BitNotOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
		auto alloca = makeAlloca(rewriter, type, op.getLoc());
		auto mask = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), type, rewriter.getI64IntegerAttr(-1));
		auto operand =
				makeAlignedLoad(rewriter, type, adaptor.getUnderlying(), op.getLoc());
		auto result =
				rewriter.create<mlir::LLVM::XOrOp>(op.getLoc(), operand, mask);
		makeAlignedStore(rewriter, result, alloca, op.getLoc());
		rewriter.replaceOp(op, alloca);

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
			const auto& dl = mlir::DataLayout::closest(op);

			auto resultType = typeConverter->convertType(
					mlir::rlc::ProxyType::get(op.getArguments().front().getType()));
			auto len = rewriter.create<mlir::LLVM::ConstantOp>(
					op.getLoc(),
					rewriter.getIntegerType(64),
					rewriter.getI64IntegerAttr(dl.getTypeSize(resultType)));
			rewriter.create<mlir::LLVM::MemcpyOp>(
					op.getLoc(),
					op->getParentOfType<mlir::LLVM::LLVMFuncOp>().getArguments()[0],
					adaptor.getArguments()[0],
					len,
					false);
		}
		rewriter.replaceOpWithNewOp<mlir::LLVM::ReturnOp>(op, mlir::ValueRange());

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
			const mlir::TypeConverter* converter,
			std::index_sequence<I...>) const
	{
		return rewriteOp(
				op,
				rewriter,
				makeAlignedLoad(
						rewriter,
						converter->convertType(
								mlir::rlc::ProxyType::get(op->getOperands()[I].getType())),
						adaptor.getOperands()[I],
						op.getLoc())...);
	}

	mlir::LogicalResult matchAndRewrite(
			Op op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		mlir::Value res = dispatch(
				op,
				adaptor,
				rewriter,
				this->typeConverter,
				std::make_index_sequence<sizeof...(Args)>());
		mlir::Value alloca = makeAlloca(rewriter, res.getType(), op.getLoc());
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
static void replaceOpWithStringLiteral(
		mlir::Operation* op,
		mlir::ConversionPatternRewriter& rewriter,
		llvm::StringRef content,
		llvm::StringMap<mlir::LLVM::GlobalOp>& stringsCache)
{
	llvm::SmallVector<char, 4> out;
	auto str = getOrCreateGlobalString(
			op->getLoc(),
			rewriter,
			"",
			(content + llvm::Twine('\0')).toNullTerminatedStringRef(out),
			op->getParentOfType<mlir::ModuleOp>(),
			stringsCache);

	auto alloca = makeAlloca(
			rewriter,
			mlir::LLVM::LLVMPointerType::get(op->getContext()),
			op->getLoc());

	makeAlignedStore(rewriter, str, alloca, op->getLoc());

	rewriter.replaceOp(op, alloca);
}

class BuiltinMangledNameRewriter
		: public mlir::OpConversionPattern<mlir::rlc::BuiltinMangledNameOp>
{
	public:
	BuiltinMangledNameRewriter(
			mlir::TypeConverter& converter,
			mlir::MLIRContext* ctx,
			llvm::StringMap<mlir::LLVM::GlobalOp>& stringsCache)
			: mlir::OpConversionPattern<mlir::rlc::BuiltinMangledNameOp>::
						OpConversionPattern(converter, ctx),
				stringsCache(&stringsCache)
	{
	}
	llvm::StringMap<mlir::LLVM::GlobalOp>* stringsCache;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::BuiltinMangledNameOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto name = mlir::rlc::typeToMangled(op.getValue().getType());
		replaceOpWithStringLiteral(op, rewriter, name, *stringsCache);
		return mlir::LogicalResult::success();
	}
};

class BuiltinAsPtrRewriter
		: public mlir::OpConversionPattern<mlir::rlc::BuiltinAsPtr>
{
	using mlir::OpConversionPattern<mlir::rlc::BuiltinAsPtr>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::BuiltinAsPtr op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto alloca = makeAlloca(
				rewriter,
				mlir::LLVM::LLVMPointerType::get(op.getContext()),
				op.getLoc());
		auto casted = rewriter.create<mlir::LLVM::PtrToIntOp>(
				op.getLoc(), rewriter.getI64Type(), adaptor.getValue());
		makeAlignedStore(rewriter, casted, alloca, alloca.getLoc());
		rewriter.replaceOp(op, alloca);
		return mlir::LogicalResult::success();
	}
};

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
		auto type =
				typeConverter->convertType(mlir::rlc::ProxyType::get(op.getType()));
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
		rewriter.replaceOpWithNewOp<mlir::LLVM::AddressOfOp>(
				op,
				mlir::LLVM::LLVMPointerType::get(getContext()),
				adaptor.getReferred());
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
		llvm::SmallVector<mlir::Type, 2> argTypes;
		assert(op.getNumResults() <= 1);
		args.push_back(adaptor.getCallee());
		auto loc = op.getLoc();

		mlir::Type type = mlir::rlc::VoidType::get(op.getContext());
		mlir::Value result = nullptr;
		if (op.getNumResults() == 1 and
				not op.getResult(0).getType().isa<mlir::rlc::VoidType>())
		{
			type = op.getCalleeType().getResult(0);

			auto res = typeConverter->convertType(mlir::rlc::ProxyType::get(type));

			result = makeAlloca(rewriter, res, op.getLoc());
			args.push_back(result);
		}

		for (auto arg : adaptor.getArgs())
		{
			args.push_back(arg);
			argTypes.push_back(arg.getType());
		}

		auto newOp = rewriter.create<mlir::LLVM::CallOp>(
				op.getLoc(),
				mlir::LLVM::LLVMFunctionType::get(
						mlir::LLVM::LLVMVoidType::get(op.getContext()), argTypes),
				args);

		if (type.isa<mlir::rlc::ReferenceType>())
			result = makeAlignedLoad(
					rewriter,
					mlir::LLVM::LLVMPointerType::get(getContext()),
					result,
					op.getLoc());

		if (result != nullptr)
			rewriter.replaceOp(op, result);
		else
			rewriter.eraseOp(op);

		return mlir::LogicalResult::success();
	}
};

class MemSetZeroRewriter
		: public mlir::OpConversionPattern<mlir::rlc::MemSetZero>
{
	using mlir::OpConversionPattern<mlir::rlc::MemSetZero>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::MemSetZero op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		const auto& dl = mlir::DataLayout::closest(op);

		auto resultType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getDest().getType()));
		auto len = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getIntegerType(64),
				rewriter.getI64IntegerAttr(dl.getTypeSize(resultType)));

		auto zero = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(), rewriter.getIntegerType(8), rewriter.getI64IntegerAttr(0));
		rewriter.replaceOpWithNewOp<mlir::LLVM::MemsetOp>(
				op, adaptor.getDest(), zero, len, false);

		return mlir::LogicalResult::success();
	}
};

class MemMoveRewriter: public mlir::OpConversionPattern<mlir::rlc::MemMove>
{
	using mlir::OpConversionPattern<mlir::rlc::MemMove>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::MemMove op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		const auto& dl = mlir::DataLayout::closest(op);

		auto resultType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getSource().getType()));
		auto len = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getIntegerType(64),
				rewriter.getI64IntegerAttr(dl.getTypeSize(resultType)));
		rewriter.replaceOpWithNewOp<mlir::LLVM::MemmoveOp>(
				op, adaptor.getDest(), adaptor.getSource(), len, false);

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
		auto conditionType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getCond().getType()));
		auto loaded = makeAlignedLoad(
				rewriter, conditionType, adaptor.getCond(), op.getLoc());

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
		auto conditionType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getCond().getType()));
		auto loaded = makeAlignedLoad(
				rewriter, conditionType, adaptor.getCond(), op.getLoc());
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
		auto resultType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getResult().getType()));
		const auto& dl = mlir::DataLayout::closest(op);
		auto lhsType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getLhs().getType()));
		auto reinterpretedSize = dl.getTypeSize(lhsType);
		auto sameSizeIntType = rewriter.getIntegerType(reinterpretedSize * 8);

		mlir::Operation* loadedValue =
				makeAlignedLoad(rewriter, lhsType, adaptor.getLhs(), op.getLoc());
		loadedValue = rewriter.create<mlir::LLVM::BitcastOp>(
				op.getLoc(), sameSizeIntType, loadedValue->getResult(0));

		mlir::Operation* result =
				rewriter.create<mlir::LLVM::UndefOp>(op.getLoc(), resultType);
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
		auto lhsType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getLhs().getType()));
		auto resultType = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getResult().getType()));
		const auto& dl = mlir::DataLayout::closest(op);
		auto reinterpretedSize = dl.getTypeSize(resultType);
		auto sameSizeIntType = rewriter.getIntegerType(reinterpretedSize * 8);

		auto loadedArray =
				makeAlignedLoad(rewriter, lhsType, adaptor.getLhs(), op.getLoc());

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
				op.getLoc(), resultType, result->getResult(0));
		auto inMemory = makeAlloca(rewriter, resultType, op.getLoc());
		makeAlignedStore(rewriter, bitCasted, inMemory, op.getLoc());
		rewriter.replaceOp(op, inMemory);

		return mlir::LogicalResult::success();
	}
};

class AbortRewriter: public mlir::OpConversionPattern<mlir::rlc::AbortOp>
{
	public:
	AbortRewriter(
			mlir::TypeConverter& converter,
			mlir::MLIRContext* ctx,
			mlir::LLVM::LLVMFuncOp puts,
			llvm::StringMap<mlir::LLVM::GlobalOp>& stringsCache,
			mlir::StringRef abort = "")
			: mlir::OpConversionPattern<mlir::rlc::AbortOp>::OpConversionPattern(
						converter, ctx),
				puts(puts),
				stringsCache(&stringsCache),
				abort(abort)
	{
	}
	mutable mlir::LLVM::LLVMFuncOp puts;
	llvm::StringMap<mlir::LLVM::GlobalOp>* stringsCache;
	llvm::StringRef abort;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::AbortOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto global = getOrCreateGlobalString(
				op.getLoc(),
				rewriter,
				"",
				op.getMessage(),
				op->getParentOfType<mlir::ModuleOp>(),
				*stringsCache);

		rewriter.setInsertionPoint(op);

		if (not abort.empty())
		{
			rewriter.create<mlir::LLVM::CallOp>(
					op.getLoc(), mlir::TypeRange(), abort, mlir::ValueRange({ global }));
		}
		else
		{
			if (not op.getMessage().empty())
			{
				rewriter.create<mlir::LLVM::CallOp>(
						op.getLoc(),
						mlir::TypeRange(rewriter.getI32Type()),
						puts.getSymName(),
						mlir::ValueRange({ global }));
			}
			rewriter.create<mlir::LLVM::Trap>(op.getLoc());
		}

		rewriter.create<mlir::LLVM::ReturnOp>(op.getLoc(), mlir::ValueRange());
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

static void emitGlobalVectorInitialization(
		mlir::ArrayAttr array,
		mlir::Value& toReturn,
		llvm::SmallVector<int64_t, 4>& indicies,
		mlir::ConversionPatternRewriter& rewriter,
		const mlir::TypeConverter* typeConverter,
		mlir::Location loc)
{
	indicies.push_back(0);
	for (auto& constant : array.getValue())
	{
		if (auto subElement = mlir::dyn_cast<mlir::ArrayAttr>(constant))
		{
			emitGlobalVectorInitialization(
					subElement, toReturn, indicies, rewriter, typeConverter, loc);
		}
		else
		{
			auto lowered = lowerConstant(rewriter, constant, loc);

			toReturn = rewriter.create<mlir::LLVM::InsertValueOp>(
					loc, toReturn, lowered, indicies);
		}
		indicies.back()++;
	}
	indicies.pop_back();
}

class GlobalArrayRewriter
		: public mlir::OpConversionPattern<mlir::rlc::FlatConstantGlobalOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::FlatConstantGlobalOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::FlatConstantGlobalOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type = getTypeConverter()->convertType(
				mlir::rlc::ProxyType::get(op.getType()));
		auto global = rewriter.create<mlir::LLVM::GlobalOp>(
				op->getLoc(),
				type,
				true,
				mlir::LLVM::Linkage::Internal,
				op.getName(),
				mlir::Attribute());

		auto* block = rewriter.createBlock(&global.getInitializer());
		rewriter.setInsertionPoint(block, block->begin());
		mlir::Value toReturn;

		if (auto casted = op.getValues().dyn_cast<mlir::ArrayAttr>())
		{
			toReturn = rewriter.create<mlir::LLVM::UndefOp>(op.getLoc(), type);

			llvm::SmallVector<int64_t, 4> indicies;
			emitGlobalVectorInitialization(
					casted, toReturn, indicies, rewriter, typeConverter, op.getLoc());
		}
		else
		{
			toReturn = lowerConstant(rewriter, op.getValues(), op.getLoc());
		}

		rewriter.create<mlir::LLVM::ReturnOp>(
				op->getLoc(), mlir::ValueRange({ toReturn }));

		rewriter.replaceOp(op, global);
		return mlir::success();
	}
};

class LowerStringLiteral
		: public mlir::OpConversionPattern<mlir::rlc::StringLiteralOp>
{
	public:
	LowerStringLiteral(
			mlir::TypeConverter& converter,
			mlir::MLIRContext* ctx,
			llvm::StringMap<mlir::LLVM::GlobalOp>& stringsCache)
			: mlir::OpConversionPattern<
						mlir::rlc::StringLiteralOp>::OpConversionPattern(converter, ctx),
				stringsCache(&stringsCache)
	{
	}
	llvm::StringMap<mlir::LLVM::GlobalOp>* stringsCache;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::StringLiteralOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		replaceOpWithStringLiteral(op, rewriter, op.getValue(), *stringsCache);
		return mlir::LogicalResult::success();
	}
};

class IsAlternativeOpRewriter
		: public mlir::OpConversionPattern<mlir::rlc::IsAlternativeTypeOp>
{
	using mlir::OpConversionPattern<
			mlir::rlc::IsAlternativeTypeOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::IsAlternativeTypeOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto alloca = makeAlloca(
				rewriter, mlir::IntegerType::get(op.getContext(), 8), op.getLoc());
		auto res = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				rewriter.getI8IntegerAttr(
						op.getInput().getType().isa<mlir::rlc::AlternativeType>()));

		makeAlignedStore(rewriter, res, alloca, op.getLoc());

		rewriter.replaceOp(op, alloca);

		return mlir::LogicalResult::success();
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
		auto err = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getResult().getType()), out);
		assert(err.succeeded());
		if (err.failed())
			return err;

		rewriter.setInsertionPoint(op);
		auto alloca = makeAlloca(rewriter, out.front(), op.getLoc());
		auto constant = rewriter.create<mlir::LLVM::ConstantOp>(
				op.getLoc(),
				out.front(),
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
		rewriter.setInsertionPoint(op);
		auto constant = lowerConstant(rewriter, typeConverter, op);
		auto alloca = makeAlloca(rewriter, constant.getType(), op.getLoc());
		makeAlignedStore(rewriter, constant, alloca, alloca.getLoc());
		rewriter.replaceOp(op, alloca);

		return mlir::LogicalResult::success();
	}
};

class FunctionRewriter
		: public mlir::OpConversionPattern<mlir::rlc::FlatFunctionOp>
{
	public:
	mlir::rlc::DebugInfoGenerator& diGenerator;
	bool addDebugInfo;

	FunctionRewriter(
			mlir::TypeConverter& converter,
			mlir::MLIRContext* ctx,
			mlir::rlc::DebugInfoGenerator& diGenerator,
			bool addDebugInfo)
			: mlir::OpConversionPattern<
						mlir::rlc::FlatFunctionOp>::OpConversionPattern(converter, ctx),
				diGenerator(diGenerator),
				addDebugInfo(addDebugInfo)
	{
	}

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::FlatFunctionOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto linkage = op.isInternal() ? mlir::LLVM::linkage::Linkage::Internal
																	 : mlir::LLVM::linkage::Linkage::External;

		llvm::SmallVector<mlir::Type> realTypes;
		auto err = typeConverter->convertType(
				mlir::rlc::ProxyType::get(op.getFunctionType()), realTypes);
		assert(realTypes.size() == 1);
		assert(err.succeeded());
		if (err.failed())
		{
			return err;
		}

		auto fType = realTypes.front();
		rewriter.setInsertionPoint(op);
		auto newF = rewriter.create<mlir::LLVM::LLVMFuncOp>(
				op.getLoc(), op.getMangledName(), fType, linkage);

		if (not op.isDeclaration())
			rewriter.cloneRegionBefore(
					op.getBody(), newF.getBody(), newF.getBody().begin());

		if (not op.isDeclaration() and op.getType().getResults().size() == 1 and
				not op.getType().getResults().front().isa<mlir::rlc::VoidType>())
			newF.getBody().front().insertArgument(
					unsigned(0), op.getType().getResults().front(), op.getLoc());

		auto res = rewriter.convertRegionTypes(&newF.getRegion(), *typeConverter);
		if (not op.isDeclaration() and addDebugInfo)
		{
			auto subprogramAttr = diGenerator.getFunctionAttr(
					newF, op.getUnmangledName(), op.getType());
			newF->setLoc(
					mlir::FusedLoc::get(
							op.getContext(), { newF.getLoc() }, subprogramAttr));

			rewriter.setInsertionPointToStart(&newF.getBody().front());
			for (auto [argument, value, newValue] : llvm::zip(
							 op.getInfo().getArguments(),
							 op.getBody().front().getArguments(),
							 newF.getBody().front().getArguments()))
			{
				auto localVarAttr = diGenerator.getArgument(
						value,
						subprogramAttr,
						argument.getName(),
						value.getLoc().cast<mlir::FileLineColLoc>());
				rewriter.create<mlir::LLVM::DbgDeclareOp>(
						op.getLoc(), newValue, localVarAttr);
			}
		}

		rewriter.eraseOp(op);

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
		return lhs;
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
			llvm::StringMap<mlir::LLVM::GlobalOp> stringsCache;
			mlir::IRRewriter rewriter(&getContext());
			rewriter.setInsertionPoint(
					getOperation().getBody(0), getOperation().getBody(0)->begin());

			mlir::TypeConverter realConverter;
			mlir::rlc::registerConversions(realConverter, getOperation());
			auto dl = mlir::DataLayout::closest(getOperation());
			mlir::rlc::DebugInfoGenerator diGenerator(
					getOperation(), rewriter, &realConverter, &dl);

			auto ptrType = mlir::LLVM::LLVMPointerType::get(&getContext());

			if (abort_symbol != "")
				rewriter.create<mlir::LLVM::LLVMFuncOp>(
						getOperation().getLoc(),
						abort_symbol,
						mlir::LLVM::LLVMFunctionType::get(
								mlir::LLVM::LLVMVoidType::get(&getContext()),
								{ mlir::LLVM::LLVMPointerType::get(&getContext()) }));

			auto malloc = rewriter.create<mlir::LLVM::LLVMFuncOp>(
					getOperation().getLoc(),
					"malloc",
					mlir::LLVM::LLVMFunctionType::get(
							mlir::LLVM::LLVMPointerType::get(&getContext()),
							{ rewriter.getIntegerType(dl.getTypeSizeInBits(ptrType)) }));

			auto puts = rewriter.create<mlir::LLVM::LLVMFuncOp>(
					getOperation().getLoc(),
					"puts",
					mlir::LLVM::LLVMFunctionType::get(
							rewriter.getI32Type(),
							{ mlir::LLVM::LLVMPointerType::get(&getContext()) }));

			auto free = rewriter.create<mlir::LLVM::LLVMFuncOp>(
					getOperation().getLoc(),
					"free",
					mlir::LLVM::LLVMFunctionType::get(
							mlir::LLVM::LLVMVoidType::get(&getContext()),
							{ mlir::LLVM::LLVMPointerType::get(&getContext()) }));

			mlir::TypeConverter converter;
			converter.addConversion([&](mlir::Type t) -> mlir::Type {
				if (auto casted = t.dyn_cast<mlir::rlc::ProxyType>())
					return realConverter.convertType(casted.getUnderlying());

				return mlir::LLVM::LLVMPointerType::get(t.getContext());
			});
			mlir::ConversionTarget target(getContext());

			target.addLegalDialect<mlir::BuiltinDialect, mlir::LLVM::LLVMDialect>();
			target.addIllegalDialect<mlir::rlc::RLCDialect>();

			mlir::RewritePatternSet patterns(&getContext());
			patterns
					.add<FunctionRewriter>(
							converter, &getContext(), diGenerator, debug_info)
					.add<TraitDeclarationEraser>(converter, &getContext())
					.add<ValueUpcastRewriter>(converter, &getContext())
					.add<EnumDeclarationEraser>(converter, &getContext())
					.add<FlatActionStatementEraser>(converter, &getContext())
					.add<EnumUseLowerer>(converter, &getContext())
					.add<CallRewriter>(converter, &getContext())
					.add<ConstantRewriter>(converter, &getContext())
					.add<IsAlternativeOpRewriter>(converter, &getContext())
					.add<IntegerLiteralRewrtier>(converter, &getContext())
					.add<IsOpRewriter>(converter, &getContext())
					.add<SetActiveOpRewriter>(converter, &getContext())
					.add<GlobalArrayRewriter>(converter, &getContext())
					.add<CbrRewriter>(converter, &getContext())
					.add<MemMoveRewriter>(converter, &getContext())
					.add<MemSetZeroRewriter>(converter, &getContext())
					.add<BrRewriter>(converter, &getContext())
					.add<FromByteArrayRewriter>(converter, &getContext())
					.add<ToByteArrayRewriter>(converter, &getContext())
					.add<SelectRewriter>(converter, &getContext())
					.add<AssignRewriter>(converter, &getContext())
					.add<DerefRewriter>(converter, &getContext())
					.add<MakeRefRewriter>(converter, &getContext())
					.add<InitRewriter>(converter, &getContext())
					.add<LowerMalloc>(converter, &getContext(), malloc)
					.add<LowerFree>(converter, &getContext(), free)
					.add<LowerStringLiteral>(converter, &getContext(), stringsCache)
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
					.add<BitNotRewriter>(converter, &getContext())
					.add<BitOrRewriter>(converter, &getContext())
					.add<BitAndRewriter>(converter, &getContext())
					.add<BitXorRewriter>(converter, &getContext())
					.add<LeftShiftRewriter>(converter, &getContext())
					.add<RightShiftRewriter>(converter, &getContext())
					.add<YieldRewriter>(converter, &getContext())
					.add<NullLowerer>(converter, &getContext())
					.add<YieldReferenceRewriter>(converter, &getContext())
					.add<AliasTypeEraser>(converter, &getContext())
					.add<CopyRewriter>(converter, &getContext())
					.add<ArrayAccessRewriter>(converter, &getContext())
					.add<UninitializedConstructRewriter>(converter, &getContext())
					.add<BuiltinMangledNameRewriter>(
							converter, &getContext(), stringsCache)
					.add<MemberAccessRewriter>(converter, &getContext())
					.add<VarNameLowerer>(
							converter, &getContext(), diGenerator, debug_info)
					.add<ReferenceRewriter>(converter, &getContext())
					.add<BuiltinAsPtrRewriter>(converter, &getContext())
					.add<LowerIsDoneOp>(converter, &getContext())
					.add<ClassDeclarationRewriter>(converter, &getContext())
					.add<ExplicitConstructRewriter>(converter, &getContext())
					.add<AbortRewriter>(
							converter, &getContext(), puts, stringsCache, abort_symbol);

			if (failed(
							applyFullConversion(getOperation(), target, std::move(patterns))))
				signalPassFailure();
		}
	};
}	 // namespace mlir::rlc

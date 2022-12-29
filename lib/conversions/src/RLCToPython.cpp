#include "rlc/conversions/RLCToPython.hpp"

#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/python/Operations.hpp"
#include "rlc/python/Types.hpp"

static void registerConversions(mlir::TypeConverter& converter)
{
	converter.addConversion([](mlir::rlc::IntegerType t) -> mlir::Type {
		return mlir::rlc::python::PythonIntType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::BoolType t) -> mlir::Type {
		return mlir::rlc::python::PythonBoolType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::FloatType t) -> mlir::Type {
		return mlir::rlc::python::PythonFloatType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::VoidType t) -> mlir::Type {
		return mlir::rlc::python::PythonNoneType::get(t.getContext());
	});

	converter.addConversion([&](mlir::rlc::ArrayType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		assert(converted);
		return mlir::rlc::python::CArrayType::get(
				t.getContext(), converted, t.getSize());
	});

	converter.addConversion([&](mlir::rlc::EntityType t) -> mlir::Type {
		llvm::SmallVector<mlir::Type, 3> types;
		for (auto sub : t.getBody())
		{
			auto converted = converter.convertType(sub);
			assert(converted);
			types.push_back(converted);
		}
		return mlir::rlc::python::CTypeStructType::get(
				t.getContext(), t.getName(), types);
	});

	converter.addConversion([&](mlir::FunctionType t) -> mlir::Type {
		llvm::SmallVector<mlir::Type, 3> resTypes;
		for (auto sub : t.getResults())
		{
			auto converted = converter.convertType(sub);
			assert(converted);
			resTypes.push_back(converted);
		}

		llvm::SmallVector<mlir::Type, 3> inputTypes;
		for (auto sub : t.getInputs())
		{
			auto converted = converter.convertType(sub);
			assert(converted);
			inputTypes.push_back(converted);
		}
		return mlir::FunctionType::get(t.getContext(), inputTypes, resTypes);
	});
}

class EntityDeclarationToClassDecl
		: public mlir::OpConversionPattern<mlir::rlc::EntityDeclaration>
{
	using mlir::OpConversionPattern<
			mlir::rlc::EntityDeclaration>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::EntityDeclaration op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		mlir::Type type = typeConverter->convertType(op.getType());
		llvm::SmallVector<llvm::StringRef, 2> names;
		for (const auto& name :
				 op.getType().cast<mlir::rlc::EntityType>().getFieldNames())
		{
			names.push_back(name);
		}

		rewriter.create<mlir::rlc::python::CTypeStructDecl>(
				op.getLoc(), type, rewriter.getStrArrayAttr(names));
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

static void emitFunctionWrapper(
		mlir::Location loc,
		mlir::rlc::python::CTypesLoad* library,
		mlir::ConversionPatternRewriter& rewriter,
		mlir::TypeConverter* converter,
		llvm::StringRef overloadName,
		llvm::StringRef fName,
		mlir::ArrayAttr argNames,
		mlir::FunctionType fType)
{
	if (fName.startswith("_"))
		return;

	auto funType = converter->convertType(fType).cast<mlir::FunctionType>();

	auto f = rewriter.create<mlir::rlc::python::PythonFun>(
			loc, funType, fName, overloadName, argNames);
	llvm::SmallVector<mlir::Location> locs;
	for (const auto& _ : funType.getInputs())
	{
		locs.push_back(loc);
	}

	auto* block = rewriter.createBlock(
			&f.getRegion(), f.getRegion().begin(), funType.getInputs(), locs);
	auto res = rewriter.create<mlir::rlc::python::PythonAccess>(
			loc, funType, *library, f.getSymName());

	auto resType =
			funType.getNumResults() == 0
					? mlir::rlc::python::PythonNoneType::get(fType.getContext())
					: funType.getResult(0);

	rewriter.create<mlir::rlc::python::AssignResultType>(loc, res, resType);

	auto result = rewriter.create<mlir::rlc::python::PythonCall>(
			loc, mlir::TypeRange({ resType }), res, block->getArguments());

	rewriter.create<mlir::rlc::python::PythonReturn>(loc, result.getResults());
}

class FunctionToPyFunction
		: public mlir::OpConversionPattern<mlir::rlc::FunctionOp>
{
	private:
	mlir::rlc::python::CTypesLoad* library;

	public:
	template<typename... Args>
	FunctionToPyFunction(mlir::rlc::python::CTypesLoad* library, Args&&... args)
			: mlir::OpConversionPattern<mlir::rlc::FunctionOp>(
						std::forward<Args>(args)...),
				library(library)
	{
	}

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::FunctionOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		emitFunctionWrapper(
				op.getLoc(),
				library,
				rewriter,
				getTypeConverter(),
				op.getUnmangledName(),
				op.getSymName(),
				op.getArgNames(),
				op.getFunctionType());
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

class ActionDeclToTNothing
		: public mlir::OpConversionPattern<mlir::rlc::ActionFunction>
{
	private:
	mlir::rlc::python::CTypesLoad* library;
	mlir::rlc::ModuleBuilder* builder;

	public:
	template<typename... Args>
	ActionDeclToTNothing(
			mlir::rlc::python::CTypesLoad* library,
			mlir::rlc::ModuleBuilder* builder,
			Args&&... args)
			: mlir::OpConversionPattern<mlir::rlc::ActionFunction>(
						std::forward<Args>(args)...),
				library(library),
				builder(builder)
	{
	}

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::ActionFunction op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		emitFunctionWrapper(
				op.getLoc(),
				library,
				rewriter,
				getTypeConverter(),
				op.getUnmangledName(),
				mlir::rlc::mangledName(op.getSymName(), op.getFunctionType()),
				op.getArgNames(),
				op.getFunctionType());
		rewriter.setInsertionPointAfter(op);
		for (const auto& [type, action] :
				 llvm::zip(op.getActions(), builder->actionStatementsOfAction(op)))
		{
			auto casted = mlir::cast<mlir::rlc::ActionStatement>(action);

			llvm::SmallVector<llvm::StringRef, 2> arrayAttr;
			arrayAttr.push_back("frame");
			for (const auto& attr : casted.getDeclaredNames())
				arrayAttr.push_back(attr.cast<mlir::StringAttr>());

			auto castedType = type.getType().cast<mlir::FunctionType>();
			emitFunctionWrapper(
					casted.getLoc(),
					library,
					rewriter,
					getTypeConverter(),
					casted.getName(),
					mlir::rlc::mangledName(casted.getName(), castedType),
					rewriter.getStrArrayAttr(arrayAttr),
					castedType);
			rewriter.setInsertionPointAfter(op);

			auto validityType = mlir::FunctionType::get(
					getContext(),
					castedType.getInputs(),
					mlir::rlc::BoolType::get(getContext()));
			auto name = ("can_" + casted.getName()).str();
			emitFunctionWrapper(
					casted.getLoc(),
					library,
					rewriter,
					getTypeConverter(),
					name,
					mlir::rlc::mangledName(name, validityType),
					rewriter.getStrArrayAttr(arrayAttr),
					validityType);
			rewriter.setInsertionPointAfter(op);
		}
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

void rlc::RLCToPython::runOnOperation()
{
	mlir::OpBuilder builder(&getContext());
	mlir::rlc::ModuleBuilder rlcBuilder(getOperation());
	builder.setInsertionPoint(&getOperation().getBodyRegion().front().front());
	auto lib = builder.create<mlir::rlc::python::CTypesLoad>(
			getOperation().getLoc(),
			mlir::rlc::python::CDLLType::get(&getContext()),
			"./lib.so");
	mlir::ConversionTarget target(getContext());
	mlir::TypeConverter converter;
	registerConversions(converter);

	target.addLegalDialect<mlir::rlc::python::RLCPython>();
	target.addIllegalDialect<mlir::rlc::RLCDialect>();

	mlir::RewritePatternSet patterns(&getContext());
	patterns.add<EntityDeclarationToClassDecl>(converter, &getContext());
	patterns.add<ActionDeclToTNothing>(
			&lib, &rlcBuilder, converter, &getContext());
	patterns.add<FunctionToPyFunction>(&lib, converter, &getContext());

	if (failed(
					applyPartialConversion(getOperation(), target, std::move(patterns))))
		signalPassFailure();
}

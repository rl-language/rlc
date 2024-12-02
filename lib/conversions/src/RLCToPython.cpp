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
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/ActionArgumentAnalysis.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"
#include "rlc/python/Dialect.h"
#include "rlc/python/Operations.hpp"
#include "rlc/python/Passes.hpp"
#include "rlc/python/Types.hpp"

static void registerBuiltinConversions(
		mlir::TypeConverter& converter, mlir::TypeConverter& ctypesConverter)
{
	converter.addConversion([](mlir::rlc::IntegerType t) -> mlir::Type {
		return mlir::rlc::python::IntType::get(t.getContext(), t.getSize());
	});

	converter.addConversion([](mlir::rlc::BoolType t) -> mlir::Type {
		return mlir::rlc::python::BoolType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::FloatType t) -> mlir::Type {
		return mlir::rlc::python::FloatType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::VoidType t) -> mlir::Type {
		return mlir::rlc::python::NoneType::get(t.getContext());
	});

	converter.addConversion([&](mlir::rlc::OwningPtrType t) -> mlir::Type {
		auto converted = ctypesConverter.convertType(t.getUnderlying());
		assert(converted);
		return mlir::rlc::python::CTypesPointerType::get(t.getContext(), converted);
	});

	converter.addConversion([&](mlir::rlc::ReferenceType t) -> mlir::Type {
		auto converted = ctypesConverter.convertType(t.getUnderlying());
		assert(converted);
		return mlir::rlc::python::CTypesPointerType::get(t.getContext(), converted);
	});

	converter.addConversion([&](mlir::rlc::FrameType t) -> mlir::Type {
		return converter.convertType(t.getUnderlying());
	});

	converter.addConversion([&](mlir::rlc::ContextType t) -> mlir::Type {
		return converter.convertType(t.getUnderlying());
	});

	converter.addConversion([](mlir::rlc::StringLiteralType t) -> mlir::Type {
		return mlir::rlc::python::StrType::get(t.getContext());
	});

	converter.addConversion([&](mlir::rlc::ArrayType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		assert(converted);
		return mlir::rlc::python::CArrayType::get(
				t.getContext(), converted, t.getArraySize());
	});

	converter.addConversion([&](mlir::rlc::AlternativeType t) -> mlir::Type {
		llvm::SmallVector<mlir::Type, 3> types;
		for (auto sub : t.getUnderlying())
		{
			auto converted = ctypesConverter.convertType(sub);
			assert(converted);
			types.push_back(converted);
		}
		return mlir::rlc::python::CTypeUnionType::get(t.getContext(), types);
	});

	converter.addConversion([&](mlir::rlc::ClassType t) -> mlir::Type {
		llvm::SmallVector<mlir::Type, 3> types;
		for (auto sub : t.getMembers())
		{
			auto converted = ctypesConverter.convertType(sub.getType());
			assert(converted);
			types.push_back(converted);
		}
		return mlir::rlc::python::CTypeStructType::get(
				t.getContext(), t.mangledName(), types);
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

static void registerCTypesConversions(mlir::TypeConverter& converter)
{
	converter.addConversion([](mlir::rlc::IntegerType t) -> mlir::Type {
		return mlir::rlc::python::CTypesIntType::get(t.getContext(), t.getSize());
	});

	converter.addConversion([](mlir::rlc::BoolType t) -> mlir::Type {
		return mlir::rlc::python::CTypesBoolType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::FloatType t) -> mlir::Type {
		return mlir::rlc::python::CTypesFloatType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::VoidType t) -> mlir::Type {
		return mlir::rlc::python::NoneType::get(t.getContext());
	});

	converter.addConversion([](mlir::rlc::StringLiteralType t) -> mlir::Type {
		return mlir::rlc::python::CTypesCharPType::get(t.getContext());
	});

	converter.addConversion([&](mlir::rlc::ArrayType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		assert(converted);
		return mlir::rlc::python::CArrayType::get(
				t.getContext(), converted, t.getArraySize());
	});

	converter.addConversion([&](mlir::rlc::OwningPtrType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		assert(converted);
		return mlir::rlc::python::CTypesPointerType::get(t.getContext(), converted);
	});

	converter.addConversion([&](mlir::rlc::ReferenceType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		assert(converted);
		return mlir::rlc::python::CTypesPointerType::get(t.getContext(), converted);
	});

	converter.addConversion([&](mlir::rlc::AlternativeType t) -> mlir::Type {
		llvm::SmallVector<mlir::Type, 3> types;
		for (auto sub : t.getUnderlying())
		{
			auto converted = converter.convertType(sub);
			assert(converted);
			types.push_back(converted);
		}
		return mlir::rlc::python::CTypeUnionType::get(t.getContext(), types);
	});

	converter.addConversion([&](mlir::rlc::ClassType t) -> mlir::Type {
		llvm::SmallVector<mlir::Type, 3> types;
		for (auto sub : t.getMembers())
		{
			auto converted = converter.convertType(sub.getType());
			assert(converted);
			types.push_back(converted);
		}
		return mlir::rlc::python::CTypeStructType::get(
				t.getContext(), t.mangledName(), types);
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
			if (not converted)
			{
				sub.dump();
				abort();
			}
			inputTypes.push_back(converted);
		}
		return mlir::FunctionType::get(t.getContext(), inputTypes, resTypes);
	});
}

static mlir::rlc::python::PythonFun emitFunctionWrapper(
		mlir::Location loc,
		mlir::rlc::python::CTypesLoad* library,
		mlir::ConversionPatternRewriter& rewriter,
		const mlir::TypeConverter* converter,
		llvm::StringRef overloadName,
		llvm::StringRef fName,
		llvm::ArrayRef<llvm::StringRef> argNames,
		mlir::FunctionType fType)
{
	if (fName.starts_with("_"))
		return nullptr;

	auto funType = converter->convertType(fType).cast<mlir::FunctionType>();

	auto f = rewriter.create<mlir::rlc::python::PythonFun>(
			loc, funType, fName, overloadName, rewriter.getStrArrayAttr(argNames));
	llvm::SmallVector<mlir::Location> locs;
	for (const auto& _ : funType.getInputs())
	{
		locs.push_back(loc);
	}

	auto* block = rewriter.createBlock(
			&f.getRegion(), f.getRegion().begin(), funType.getInputs(), locs);
	auto res = rewriter.create<mlir::rlc::python::PythonAccess>(
			loc, funType, *library, f.getSymName());

	auto resType = funType.getNumResults() == 0
										 ? mlir::rlc::python::NoneType::get(fType.getContext())
										 : mlir::rlc::pythonBuiltinToCTypes(funType.getResult(0));

	rewriter.create<mlir::rlc::python::AssignResultType>(loc, res, resType);
	llvm::SmallVector<mlir::Value> values;

	for (auto value : block->getArguments())
	{
		if (mlir::rlc::isBuiltinType(value.getType()))
		{
			auto res = rewriter.create<mlir::rlc::python::PythonCast>(
					value.getLoc(),
					mlir::rlc::pythonBuiltinToCTypes(value.getType()),
					value);
			values.push_back(res);
		}
		else if (value.getType().isa<mlir::rlc::python::StrType>())
		{
			auto res = rewriter.create<mlir::rlc::python::PythonCast>(
					value.getLoc(),
					mlir::rlc::python::CTypesCharPType::get(value.getContext()),
					value);
			values.push_back(res);
		}
		else
		{
			values.push_back(value);
		}
	}

	auto result = rewriter.create<mlir::rlc::python::PythonCall>(
			loc, mlir::TypeRange({ resType }), res, values);

	if (resType.isa<mlir::rlc::python::NoneType>())
	{
		rewriter.create<mlir::rlc::python::PythonReturn>(loc, mlir::ValueRange());
		return f;
	}

	mlir::Value toReturn = result.getResult(0);

	if (resType.isa<mlir::rlc::python::StrType>() or
			resType.isa<mlir::rlc::python::CTypesIntType>())
	{
		toReturn = rewriter.create<mlir::rlc::python::PythonAccess>(
				result.getLoc(),
				mlir::rlc::pythonCTypesToBuiltin(resType),
				toReturn,
				"value");
	}

	rewriter.create<mlir::rlc::python::PythonReturn>(loc, toReturn);
	return f;
}

class ClassDeclarationToNothing
		: public mlir::OpConversionPattern<mlir::rlc::ClassDeclaration>
{
	public:
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

class ConstantGlobalArrayOpToNothing
		: public mlir::OpConversionPattern<mlir::rlc::ConstantGlobalOp>
{
	public:
	using mlir::OpConversionPattern<
			mlir::rlc::ConstantGlobalOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::ConstantGlobalOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

class EnumDeclarationToNothing
		: public mlir::OpConversionPattern<mlir::rlc::EnumDeclarationOp>
{
	public:
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

class TypeAliasConverter
		: public mlir::OpConversionPattern<mlir::rlc::TypeAliasOp>
{
	public:
	using mlir::OpConversionPattern<mlir::rlc::TypeAliasOp>::OpConversionPattern;

	mlir::LogicalResult matchAndRewrite(
			mlir::rlc::TypeAliasOp op,
			OpAdaptor adaptor,
			mlir::ConversionPatternRewriter& rewriter) const final
	{
		auto type = getTypeConverter()->convertType(op.getAliased());
		rewriter.replaceOpWithNewOp<mlir::rlc::python::PythonTypeAliasOp>(
				op, op.getName(), type);
		return mlir::success();
	}
};

class TraitDeclarationToNothing
		: public mlir::OpConversionPattern<mlir::rlc::TraitDefinition>
{
	public:
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
		auto newFun = emitFunctionWrapper(
				op.getLoc(),
				library,
				rewriter,
				getTypeConverter(),
				op.getUnmangledName(),
				op.getMangledName(),
				op.getArgNames(),
				op.getFunctionType());

		if (newFun)
		{
			rewriter.setInsertionPointAfter(newFun);
			auto validityType = mlir::FunctionType::get(
					getContext(),
					op.getType().getInputs(),
					mlir::rlc::BoolType::get(getContext()));
			auto name = ("can_" + op.getUnmangledName()).str();
			auto preconditionCheckFunction = emitFunctionWrapper(
					op.getLoc(),
					library,
					rewriter,
					getTypeConverter(),
					name,
					mlir::rlc::mangledName(name, op.getIsMemberFunction(), validityType),
					op.getArgNames(),
					validityType);
			rewriter.setInsertionPointAfter(preconditionCheckFunction);
		}

		rewriter.eraseOp(op);
		return mlir::success();
	}
};

static mlir::rlc::ClassType getActionTypeOfActionStatement(
		mlir::rlc::ActionStatement action)
{
	auto* currentOp = action.getOperation()->getParentOp();
	assert(currentOp != nullptr);
	while (not mlir::dyn_cast<mlir::rlc::ActionFunction>(currentOp))
	{
		currentOp = currentOp->getParentOp();
		assert(currentOp != nullptr);
	}
	return mlir::cast<mlir::rlc::ActionFunction>(currentOp).getClassType();
}

static void emitActionContraints(
		mlir::rlc::ActionStatement action,
		mlir::Value emittedPythonFunction,
		mlir::ConversionPatternRewriter& rewriter)
{
	mlir::rlc::ActionArgumentAnalysis analysis(action);
	auto created = rewriter.create<mlir::rlc::python::PythonActionInfo>(
			action->getLoc(), emittedPythonFunction);

	mlir::rlc::ClassType ActionType = getActionTypeOfActionStatement(action);

	llvm::SmallVector<mlir::Location, 2> locs;
	llvm::SmallVector<mlir::Type, 2> types;

	locs.push_back(action.getLoc());
	types.push_back(ActionType);

	for (size_t i = 0; i < action.getResultTypes().size(); i++)
		locs.push_back(action.getLoc());

	for (auto type : action.getResultTypes())
		types.push_back(type);

	auto* block = rewriter.createBlock(
			&created.getBody(), created.getBody().begin(), types, locs);

	rewriter.setInsertionPoint(block, block->begin());

	for (const auto& [pythonArg, rlcArg] : llvm::zip(
					 block->getArguments().drop_front(),
					 action.getPrecondition().getArguments()))
	{
		const auto& argInfo = analysis.getBoundsOf(rlcArg);
		rewriter.create<mlir::rlc::python::PythonArgumentConstraint>(
				action.getLoc(), pythonArg, argInfo.getMin(), argInfo.getMax());
	}

	rewriter.setInsertionPointAfter(created);
}

static void emitActionContraints(
		mlir::rlc::ActionFunction action,
		mlir::Value emittedPythonFunction,
		mlir::ConversionPatternRewriter& rewriter)
{
	mlir::rlc::ActionArgumentAnalysis analysis(action);
	auto created = rewriter.create<mlir::rlc::python::PythonActionInfo>(
			action->getLoc(), emittedPythonFunction);

	llvm::SmallVector<mlir::Location, 2> locs;
	for (size_t i = 0; i < action.getFunctionType().getNumInputs(); i++)
		locs.push_back(action.getLoc());

	auto* block = rewriter.createBlock(
			&created.getBody(),
			created.getBody().begin(),
			action.getFunctionType().getInputs(),
			locs);

	rewriter.setInsertionPoint(block, block->begin());

	for (const auto& [pythonArg, rlcArg] : llvm::zip(
					 block->getArguments(), action.getBody().front().getArguments()))
	{
		const auto& argInfo = analysis.getBoundsOf(rlcArg);
		rewriter.create<mlir::rlc::python::PythonArgumentConstraint>(
				action.getLoc(), pythonArg, argInfo.getMin(), argInfo.getMax());
	}

	rewriter.setInsertionPointAfter(created);
}

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
		auto types = builder->getConverter().getTypes().get(
				("Any" + op.getClassType().getName() + "Action").str());
		if (not types.empty())
			rewriter.create<mlir::rlc::python::AddToMap>(
					op.getLoc(),
					"actionToAnyFunctionType",
					("\"" + op.getUnmangledName() + "\"").str(),
					("Any" + op.getClassType().getName() + "Action").str());

		auto f = emitFunctionWrapper(
				op.getLoc(),
				library,
				rewriter,
				getTypeConverter(),
				op.getUnmangledName(),
				op.getMangledName(),
				op.getArgNames(),
				op.getFunctionType());

		if (f == nullptr)
		{
			rewriter.eraseOp(op);
			return mlir::success();
		}

		rewriter.setInsertionPointAfter(op);

		emitActionContraints(op, f, rewriter);

		using ActionKey = std::pair<std::string, const void*>;
		std::set<ActionKey> alreadyAdded;
		for (auto type : op.getActions())
		{
			auto casted = mlir::cast<mlir::rlc::ActionStatement>(
					*builder->actionFunctionValueToActionStatement(type).begin());
			ActionKey key(casted.getName(), type.getAsOpaquePointer());
			if (alreadyAdded.contains(key))
				continue;

			alreadyAdded.insert(key);

			llvm::SmallVector<llvm::StringRef, 2> arrayAttr;
			arrayAttr.push_back("frame");
			for (const auto& attr : casted.getInfo().getArgs())
				arrayAttr.push_back(attr.getName());

			auto castedType = type.getType().cast<mlir::FunctionType>();
			auto f = emitFunctionWrapper(
					casted.getLoc(),
					library,
					rewriter,
					getTypeConverter(),
					casted.getName(),
					mlir::rlc::mangledName(casted.getName(), true, castedType),
					arrayAttr,
					castedType);
			rewriter.setInsertionPointAfter(f);
			if (f == nullptr)
				continue;

			emitActionContraints(casted, f, rewriter);

			auto validityType = mlir::FunctionType::get(
					getContext(),
					castedType.getInputs(),
					mlir::rlc::BoolType::get(getContext()));
			auto name = ("can_" + casted.getName()).str();
			auto preconditionCheckFunction = emitFunctionWrapper(
					casted.getLoc(),
					library,
					rewriter,
					getTypeConverter(),
					name,
					mlir::rlc::mangledName(name, true, validityType),
					arrayAttr,
					validityType);
			rewriter.setInsertionPointAfter(preconditionCheckFunction);
		}
		rewriter.eraseOp(op);
		return mlir::success();
	}
};

namespace mlir::python
{
#define GEN_PASS_DEF_RLCTOPYTHONPASS
#include "rlc/python/Passes.inc"
	struct RLCToPythonPass: impl::RLCToPythonPassBase<RLCToPythonPass>
	{
		using RLCToPythonPassBase<RLCToPythonPass>::RLCToPythonPassBase;
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
			registry.insert<mlir::rlc::python::RLCPython>();
		}

		void runOnOperation() override
		{
			mlir::OpBuilder builder(&getContext());
			mlir::rlc::ModuleBuilder rlcBuilder(getOperation());
			builder.setInsertionPoint(
					&getOperation().getBodyRegion().front().front());
			auto lib = builder.create<mlir::rlc::python::CTypesLoad>(
					getOperation().getLoc(),
					mlir::rlc::python::CDLLType::get(&getContext()),
					target_is_windows ? "lib.dll"
														: (target_is_mac ? "lib.dylib" : "lib.so"));
			mlir::ConversionTarget target(getContext());

			mlir::TypeConverter ctypesConverter;
			registerCTypesConversions(ctypesConverter);

			mlir::TypeConverter converter;
			registerBuiltinConversions(converter, ctypesConverter);

			target.addLegalDialect<mlir::rlc::python::RLCPython>();
			target.addIllegalDialect<mlir::rlc::RLCDialect>();

			mlir::RewritePatternSet patterns(&getContext());
			patterns.add<ActionDeclToTNothing>(
					&lib, &rlcBuilder, converter, &getContext());
			patterns.add<TraitDeclarationToNothing>(converter, &getContext());
			patterns.add<EnumDeclarationToNothing>(converter, &getContext());
			patterns.add<TypeAliasConverter>(converter, &getContext());
			patterns.add<ConstantGlobalArrayOpToNothing>(converter, &getContext());
			patterns.add<ClassDeclarationToNothing>(converter, &getContext());
			patterns.add<FunctionToPyFunction>(&lib, converter, &getContext());

			if (failed(applyPartialConversion(
							getOperation(), target, std::move(patterns))))
				signalPassFailure();
		}
	};

	static void emitDeclaration(
			mlir::Location loc,
			mlir::Type type,
			mlir::IRRewriter& rewriter,
			const mlir::TypeConverter& converter,
			bool hasCustomDestructor)
	{
		mlir::Type converted = converter.convertType(type);
		llvm::SmallVector<std::string, 2> names;
		llvm::SmallVector<llvm::StringRef, 2> refs;

		if (auto classDecl = type.dyn_cast<mlir::rlc::ClassType>())
		{
			for (auto field : classDecl.getMembers())
				names.push_back(field.getName().str());
		}
		else if (auto alternative = type.dyn_cast<mlir::rlc::AlternativeType>())
		{
			for (const auto& name : llvm::enumerate(alternative.getUnderlying()))
				names.push_back(
						(llvm::Twine("field") + llvm::Twine(name.index())).str());
		}
		else
		{
			return;
		}

		for (auto& name : names)
			refs.push_back(name);

		rewriter.create<mlir::rlc::python::CTypeStructDecl>(
				loc, converted, rewriter.getStrArrayAttr(refs), hasCustomDestructor);
	}

#define GEN_PASS_DEF_RLCTYPESTOPYTHONTYPESPASS
#include "rlc/python/Passes.inc"
	struct RLCTypesToPythonTypesPass
			: impl::RLCTypesToPythonTypesPassBase<RLCTypesToPythonTypesPass>
	{
		using RLCTypesToPythonTypesPassBase<
				RLCTypesToPythonTypesPass>::RLCTypesToPythonTypesPassBase;
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
			registry.insert<mlir::rlc::python::RLCPython>();
		}

		void runOnOperation() override
		{
			mlir::TypeConverter ctypesConverter;
			registerCTypesConversions(ctypesConverter);
			mlir::IRRewriter rewriter(&getContext());

			rewriter.setInsertionPointToStart(getOperation().getBody());
			mlir::DenseSet<mlir::Type> hasCustomDestructor;
			for (auto fun : getOperation().getBody()->getOps<mlir::rlc::FunctionOp>())
			{
				if (fun.getIsMemberFunction() and fun.getUnmangledName() == "drop" and
						fun.getArgumentTypes().size() == 1)
				{
					hasCustomDestructor.insert(fun.getArgumentTypes()[0]);
				}
			}
			for (auto t : ::rlc::postOrderTypes(getOperation()))
				emitDeclaration(
						getOperation().getLoc(),
						t,
						rewriter,
						ctypesConverter,
						hasCustomDestructor.contains(t));
		}
	};
}	 // namespace mlir::python

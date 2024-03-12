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

#include "mlir/IR/BuiltinDialect.h"
#include "mlir/Pass/Pass.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static mlir::LogicalResult findEntityDecls(
		mlir::ModuleOp op, llvm::StringMap<mlir::Operation*>& out)
{
	for (auto& entityDecl : op.getOps())
	{
		llvm::StringRef name;
		if (auto casted = mlir::dyn_cast<mlir::rlc::EntityDeclaration>(&entityDecl))
			name = casted.getName();
		else if (auto casted = mlir::dyn_cast<mlir::rlc::TypeAliasOp>(&entityDecl))
			name = casted.getName();
		else
			continue;

		if (auto prevDef = out.find(name); prevDef != out.end())
		{
			auto _ = mlir::rlc::logError(
					&entityDecl, "Multiple definition of entity " + name);

			return mlir::rlc::logRemark(prevDef->second, "Previous definition");
		}

		out[name] = &entityDecl;
	}

	return mlir::success();
}

static void collectEntityUsedTyepNames(
		mlir::Type type, llvm::SmallVector<llvm::StringRef, 2>& out)
{
	if (auto casted = type.dyn_cast<mlir::rlc::ScalarUseType>())
	{
		for (auto templ : casted.getExplicitTemplateParameters())
			collectEntityUsedTyepNames(templ, out);

		if (casted.getUnderlying() != nullptr)
		{
			collectEntityUsedTyepNames(casted.getUnderlying(), out);
			return;
		}
		out.push_back(casted.getReadType());
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::FunctionUseType>())
	{
		for (auto arg : casted.getSubTypes())
			collectEntityUsedTyepNames(arg, out);
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::OwningPtrType>())
	{
		collectEntityUsedTyepNames(casted.getUnderlying(), out);
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::AlternativeType>())
	{
		for (auto subType : casted.getUnderlying())
			collectEntityUsedTyepNames(subType, out);
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::IntegerLiteralType>())
	{
		return;
	}
	llvm_unreachable("unrechable");
}

static mlir::LogicalResult getEntityDeclarationSortedByDependencies(
		mlir::ModuleOp op, llvm::SmallVector<mlir::Operation*, 2>& out)
{
	llvm::StringMap<mlir::Operation*> nameToEntityDeclaration;
	if (findEntityDecls(op, nameToEntityDeclaration).failed())
		return mlir::failure();

	std::map<mlir::Operation*, llvm::SmallVector<mlir::Operation*, 2>>
			EntityToUsedEntities;

	for (auto entityDecl : op.getOps<mlir::rlc::EntityDeclaration>())
	{
		llvm::SmallVector<mlir::StringRef, 2> names;
		for (auto subtypes : entityDecl.getMemberTypes())
			collectEntityUsedTyepNames(
					subtypes.cast<mlir::TypeAttr>().getValue(), names);

		// remove the use of names that refer to the template parameters
		for (auto parameter : entityDecl.getTemplateParameters())
		{
			llvm::erase(
					names,
					parameter.cast<mlir::TypeAttr>()
							.getValue()
							.cast<mlir::rlc::UncheckedTemplateParameterType>()
							.getName());
		}

		EntityToUsedEntities[entityDecl];
		for (auto name : names)
		{
			if (auto iter = nameToEntityDeclaration.find(name);
					iter != nameToEntityDeclaration.end())
			{
				EntityToUsedEntities[entityDecl].push_back(iter->second);
			}
		}
	}

	for (auto alias : op.getOps<mlir::rlc::TypeAliasOp>())
	{
		llvm::SmallVector<mlir::StringRef, 2> names;
		collectEntityUsedTyepNames(alias.getAliased(), names);

		EntityToUsedEntities[alias];
		for (auto name : names)
		{
			if (auto iter = nameToEntityDeclaration.find(name);
					iter != nameToEntityDeclaration.end())
			{
				EntityToUsedEntities[alias].push_back(iter->second);
			}
		}
	}

	while (not EntityToUsedEntities.empty())
	{
		llvm::SmallVector<mlir::Operation*> justEmitted;

		for (auto& entry : EntityToUsedEntities)
		{
			llvm::erase_if(entry.second, [&](mlir::Operation* decl) {
				return not EntityToUsedEntities.contains(decl);
			});
			if (entry.second.empty())
			{
				justEmitted.push_back(entry.first);
				auto e = entry.first;
				out.push_back(entry.first);
			}
		}

		for (auto& entry : justEmitted)
			EntityToUsedEntities.erase(entry);

		if (justEmitted.empty() and not EntityToUsedEntities.empty())
		{
			return mlir::rlc::logError(
					*EntityToUsedEntities.begin()->second.begin(),
					"Forbidden mutual dependency in entities");
		}
	}

	return mlir::success();
}

static mlir::LogicalResult declareEntities(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::SmallVector<mlir::Operation*, 2> decls;
	if (getEntityDeclarationSortedByDependencies(op, decls).failed())
		return mlir::failure();

	for (auto& decl : decls)
	{
		auto casted = mlir::dyn_cast<mlir::rlc::EntityDeclaration>(decl);
		if (not casted)
			continue;

		llvm::SmallVector<mlir::Type, 2> templates;
		for (auto type :
				 casted.getTemplateParameters().getAsValueRange<mlir::TypeAttr>())
			templates.push_back(type);
		rewriter.setInsertionPoint(casted);
		auto newDecl = rewriter.create<mlir::rlc::EntityDeclaration>(
				casted.getLoc(),
				mlir::rlc::EntityType::getIdentified(
						casted.getContext(), casted.getName(), templates),
				casted.getName(),
				casted.getMemberTypes(),
				casted.getMemberNames(),
				casted.getTemplateParameters());
		rewriter.eraseOp(casted);
	}
	return mlir::success();
}

static mlir::LogicalResult declareActionEntities(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::StringMap<mlir::Operation*> decls;
	if (findEntityDecls(op, decls).failed())
		return mlir::failure();

	rewriter.setInsertionPointToEnd(op.getBody());
	for (auto action : op.getOps<mlir::rlc::ActionFunction>())
	{
		auto type = action.getEntityType();

		auto entity = rewriter.create<mlir::rlc::EntityDeclaration>(
				action.getLoc(),
				type,
				rewriter.getStringAttr(type.getName()),
				rewriter.getTypeArrayAttr({}),
				rewriter.getStrArrayAttr({}),
				rewriter.getArrayAttr({}));

		if (decls.count(type.getName()) != 0)
		{
			auto _ = mlir::rlc::logError(
					decls[type.getName()],
					"User defined type clashes with implicit action type" +
							type.getName());
			return mlir::rlc::logRemark(action, "Action defined here");
		}

		decls.try_emplace(type.getName(), entity);
	}

	return mlir::success();
}

static mlir::LogicalResult deduceEntityBody(
		mlir::ModuleOp op, mlir::rlc::EntityDeclaration decl)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter& rewriter = builder.getRewriter();
	rewriter.setInsertionPoint(decl);

	// entities of actions are discovered later, because they require to
	// typecheck the body of the action itself.
	if (builder.isEntityOfAction(decl.getType()))
		return mlir::success();

	llvm::SmallVector<std::string> names;
	llvm::SmallVector<mlir::Type, 2> types;

	llvm::SmallVector<mlir::Type, 2> checkedTemplateParameters;

	auto scopedConverter = mlir::rlc::RLCTypeConverter(&builder.getConverter());
	for (auto parameter : decl.getTemplateParameters())
	{
		auto unchecked = parameter.cast<mlir::TypeAttr>()
												 .getValue()
												 .cast<mlir::rlc::UncheckedTemplateParameterType>();

		auto checkedParameterType = scopedConverter.convertType(unchecked);
		if (not checkedParameterType)
		{
			auto _ = mlir::rlc::logError(
					decl, "No known type named " + mlir::rlc::prettyType(unchecked));
			return mlir::rlc::logRemark(decl, "In entity declaration");
		}
		checkedTemplateParameters.push_back(checkedParameterType);
		auto actualType =
				checkedParameterType.cast<mlir::rlc::TemplateParameterType>();
		scopedConverter.registerType(actualType.getName(), actualType);
	}

	for (const auto& [field, name] :
			 llvm::zip(decl.getMemberTypes(), decl.getMemberNames()))
	{
		auto fieldType = field.cast<mlir::TypeAttr>().getValue();
		auto converted = scopedConverter.convertType(fieldType);
		if (!converted)
		{
			return mlir::rlc::logError(
					decl,
					"No known type named " + mlir::rlc::prettyType(fieldType) +
							" used in field " + name.cast<mlir::StringAttr>().strref() +
							" in entity declaration.");
		}

		types.push_back(converted);
		names.push_back(name.cast<mlir::StringAttr>().str());
	}
	auto finalType = mlir::rlc::EntityType::getIdentified(
			decl.getContext(), decl.getName(), checkedTemplateParameters);

	if (finalType.setBody(types, names).failed())
	{
		assert(false && "unrechable");
		return mlir::failure();
	}

	decl = rewriter.replaceOpWithNewOp<mlir::rlc::EntityDeclaration>(
			decl,
			finalType,
			decl.getName(),
			rewriter.getTypeArrayAttr(types),
			decl.getMemberNames(),
			rewriter.getTypeArrayAttr(checkedTemplateParameters));

	return mlir::success();
}

static mlir::LogicalResult deduceEntitiesBodies(mlir::ModuleOp op)
{
	llvm::SmallVector<mlir::Operation*, 2> decls;
	if (getEntityDeclarationSortedByDependencies(op, decls).failed())
		return mlir::failure();

	for (auto& decl : decls)
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::EntityDeclaration>(decl))
		{
			if (deduceEntityBody(op, casted).failed())
				return mlir::failure();
		}
		else if (auto casted = mlir::dyn_cast<mlir::rlc::TypeAliasOp>(decl))
		{
			mlir::rlc::ModuleBuilder builder(op);
			if (casted.typeCheck(builder).failed())
				return mlir::failure();
		}
	}

	return mlir::success();
}

static mlir::LogicalResult deduceFunctionTypes(mlir::ModuleOp op)
{
	mlir::rlc::RLCTypeConverter converter(op);
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs;
	for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
		funs.push_back(fun);

	for (auto fun : funs)
	{
		if (fun.typeCheckFunctionDeclaration(rewriter, converter).failed())
		{
			fun.erase();
			return mlir::failure();
		}
	}

	return mlir::success();
}

static mlir::LogicalResult deduceOperationTypes(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);

	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs(
			op.getOps<mlir::rlc::FunctionOp>());
	for (auto fun : funs)
	{
		auto _ = builder.addSymbolTable();
		for (auto templateParameter : fun.getTemplateParameters())
		{
			auto casted = templateParameter.cast<mlir::TypeAttr>()
												.getValue()
												.cast<mlir::rlc::TemplateParameterType>();
			builder.getConverter().registerType(casted.getName(), casted);
			if (casted.getIsIntLiteral() == true)
			{
				builder.getRewriter().setInsertionPoint(&fun.getBody().front().front());
				auto integerLiteral =
						builder.getRewriter().create<mlir::rlc::IntegerLiteralUse>(
								op.getLoc(),
								mlir::rlc::IntegerType::getInt64(op.getContext()),
								casted);
				builder.getSymbolTable().add(casted.getName(), integerLiteral);
			}
			else
			{
				if (auto trait = casted.getTrait())
					builder.addTraitToAviableOverloads(trait);
			}
		}

		if (mlir::rlc::typeCheck(*fun.getOperation(), builder).failed())
			return mlir::failure();

		bool needsRet = not fun.getResultTypes().empty() and
										not fun.getResultTypes()[0].isa<mlir::rlc::VoidType>() and
										not fun.isDeclaration();

		if (needsRet)
			fun.walk([&](mlir::rlc::ReturnStatement statement) { needsRet = false; });

		if (needsRet)
		{
			return mlir::rlc::logError(
					fun,
					"Function with non-void return type needs at least a return "
					"statement");
		}
	}
	return mlir::success();
}

static mlir::LogicalResult deduceTraitTypes(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::UncheckedTraitDefinition, 4> funs(
			op.getOps<mlir::rlc::UncheckedTraitDefinition>());
	llvm::StringMap<mlir::rlc::UncheckedTraitDefinition> alreadyDeclared;

	for (auto fun : funs)
	{
		if (alreadyDeclared.contains(fun.getName()))
		{
			auto _ = mlir::rlc::logError(
					fun, "Trait " + fun.getName() + " already declared");

			return mlir::rlc::logRemark(
					alreadyDeclared[fun.getName()], "Previous declaration here");
		}
		alreadyDeclared[fun.getName()] = fun;
	}
	for (auto fun : funs)
	{
		if (mlir::rlc::typeCheck(*fun.getOperation(), builder).failed())
			return mlir::failure();
	}
	return mlir::success();
}

static mlir::LogicalResult deduceActionsMainFunctionType(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());

	mlir::rlc::ModuleBuilder builder(op);
	llvm::SmallVector<mlir::rlc::ActionFunction, 4> funs(
			op.getOps<mlir::rlc::ActionFunction>());
	for (auto fun : funs)
	{
		auto funType = builder.typeOfAction(fun);
		if (funType == nullptr)
		{
			return mlir::rlc::logError(
					fun,
					mlir::rlc::prettyType(fun.getEntityType()) +
							" is not allowed as action return type");
		}

		llvm::SmallVector<mlir::Type, 4> convertedArgs;
		for (auto arg : fun.getFunctionType().getInputs())
		{
			auto converted = builder.getConverter().convertType(arg);
			if (not converted)
			{
				return mlir::rlc::logError(
						fun,
						"No known type named " + mlir::rlc::prettyType(arg) +
								" in argument of Action Function declaration");
			}
			convertedArgs.emplace_back(converted);
		}

		auto convertedReturnType = builder.getConverter().convertType(funType);
		if (convertedReturnType == nullptr)
		{
			return mlir::rlc::logError(
					fun,
					"Return type of Action Function declaration must be a name, got "
					"instead " +
							mlir::rlc::prettyType(funType));
		}

		auto actionType = mlir::FunctionType::get(
				op.getContext(), convertedArgs, { convertedReturnType });
		auto isDoneType = mlir::FunctionType::get(
				rewriter.getContext(),
				mlir::TypeRange({ funType }),
				mlir::TypeRange({ mlir::rlc::BoolType::get(rewriter.getContext()) }));

		rewriter.setInsertionPoint(fun);
		fun.getResult().setType(actionType);
		fun.getIsDoneFunction().setType(isDoneType);
	}
	return mlir::success();
}

static mlir::LogicalResult typeCheckActions(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());

	bool foundOne = true;
	// the actions are allowed to type check each other, but that means that they
	// will change in the process, so if one typechecks and change, we have to
	// start again typechecking them all. if one typechecks and does not change,
	// it can't possible have changed any other either.
	while (foundOne)
	{
		foundOne = false;
		llvm::SmallVector<mlir::rlc::ActionFunction, 4> funs(
				op.getOps<mlir::rlc::ActionFunction>());
		for (auto fun : funs)
		{
			auto typeChecked = mlir::rlc::detail::typeCheckAction(fun);
			if (typeChecked == nullptr)
				return mlir::failure();
			if (typeChecked != fun)
			{
				foundOne = true;
				break;
			}
		}
	}
	return mlir::success();
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_TYPECHECKENTITIESPASS
#include "rlc/dialect/Passes.inc"

	struct TypeCheckEntitiesPass
			: impl::TypeCheckEntitiesPassBase<TypeCheckEntitiesPass>
	{
		using impl::TypeCheckEntitiesPassBase<
				TypeCheckEntitiesPass>::TypeCheckEntitiesPassBase;
		void runOnOperation() override
		{
			if (declareEntities(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}

			if (declareActionEntities(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}

			if (deduceEntitiesBodies(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}

			if (deduceTraitTypes(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}
		}
	};

#define GEN_PASS_DEF_TYPECHECKPASS
#include "rlc/dialect/Passes.inc"

	struct TypeCheckPass: impl::TypeCheckPassBase<TypeCheckPass>
	{
		using impl::TypeCheckPassBase<TypeCheckPass>::TypeCheckPassBase;
		void runOnOperation() override
		{
			if (deduceFunctionTypes(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}

			if (deduceActionsMainFunctionType(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}

			if (typeCheckActions(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}

			if (deduceOperationTypes(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}
		}
	};
}	 // namespace mlir::rlc

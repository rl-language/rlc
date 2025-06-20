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
#include "rlc/dialect/IRBuilder.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static void f(mlir::ModuleOp op, mlir::Value lhs, mlir::Value rhs)
{
	mlir::rlc::IRBuilder builder(op);
}

static mlir::LogicalResult findClassDecls(
		mlir::ModuleOp op, llvm::StringMap<mlir::Operation*>& out)
{
	for (auto& classDecl : op.getOps())
	{
		llvm::StringRef name;
		if (auto casted = mlir::dyn_cast<mlir::rlc::ClassDeclaration>(&classDecl))
			name = casted.getName();
		else if (auto casted = mlir::dyn_cast<mlir::rlc::TypeAliasOp>(&classDecl))
			name = casted.getName();
		else
			continue;

		if (auto prevDef = out.find(name); prevDef != out.end())
		{
			auto _ = mlir::rlc::logError(
					&classDecl, "Multiple definition of class " + name);

			return mlir::rlc::logRemark(prevDef->second, "Previous definition");
		}

		out[name] = &classDecl;
	}

	return mlir::success();
}

static void collectClassUsedTyepNames(
		mlir::Type type, llvm::SmallVector<llvm::StringRef, 2>& out)
{
	if (auto casted = type.dyn_cast<mlir::rlc::ScalarUseType>())
	{
		for (auto templ : casted.getExplicitTemplateParameters())
			collectClassUsedTyepNames(templ, out);

		if (casted.getUnderlying() != nullptr)
		{
			collectClassUsedTyepNames(casted.getUnderlying(), out);
			return;
		}
		out.push_back(casted.getReadType());
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::FunctionUseType>())
	{
		for (auto arg : casted.getSubTypes())
			collectClassUsedTyepNames(arg, out);
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::OwningPtrType>())
	{
		collectClassUsedTyepNames(casted.getUnderlying(), out);
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::AlternativeType>())
	{
		for (auto subType : casted.getUnderlying())
			collectClassUsedTyepNames(subType, out);
		return;
	}
	if (auto casted = type.dyn_cast<mlir::rlc::IntegerLiteralType>())
	{
		return;
	}
	llvm_unreachable("unrechable");
}

static mlir::LogicalResult getClassDeclarationSortedByDependencies(
		mlir::ModuleOp op, llvm::SmallVector<mlir::Operation*, 2>& out)
{
	llvm::StringMap<mlir::Operation*> nameToClassDeclaration;
	if (findClassDecls(op, nameToClassDeclaration).failed())
		return mlir::failure();

	std::map<mlir::Operation*, llvm::SmallVector<mlir::Operation*, 2>>
			ClassToUsedEntities;

	for (auto classDecl : op.getOps<mlir::rlc::ClassDeclaration>())
	{
		llvm::SmallVector<mlir::StringRef, 2> names;
		for (auto field : classDecl.getMemberFields())
			collectClassUsedTyepNames(field.getDeshugarizedType(), names);

		// remove the use of names that refer to the template parameters
		for (auto parameter : classDecl.getTemplateParameters())
		{
			llvm::erase(
					names,
					parameter.cast<mlir::TypeAttr>()
							.getValue()
							.cast<mlir::rlc::UncheckedTemplateParameterType>()
							.getName());
		}

		ClassToUsedEntities[classDecl];
		for (auto name : names)
		{
			if (auto iter = nameToClassDeclaration.find(name);
					iter != nameToClassDeclaration.end())
			{
				ClassToUsedEntities[classDecl].push_back(iter->second);
			}
		}
	}

	for (auto alias : op.getOps<mlir::rlc::TypeAliasOp>())
	{
		llvm::SmallVector<mlir::StringRef, 2> names;
		collectClassUsedTyepNames(alias.getAliased(), names);

		ClassToUsedEntities[alias];
		for (auto name : names)
		{
			if (auto iter = nameToClassDeclaration.find(name);
					iter != nameToClassDeclaration.end())
			{
				ClassToUsedEntities[alias].push_back(iter->second);
			}
		}
	}

	while (not ClassToUsedEntities.empty())
	{
		llvm::SmallVector<mlir::Operation*> justEmitted;

		for (auto& entry : ClassToUsedEntities)
		{
			llvm::erase_if(entry.second, [&](mlir::Operation* decl) {
				return not ClassToUsedEntities.contains(decl);
			});
			if (entry.second.empty())
			{
				justEmitted.push_back(entry.first);
				auto e = entry.first;
				out.push_back(entry.first);
			}
		}

		for (auto& entry : justEmitted)
			ClassToUsedEntities.erase(entry);

		if (justEmitted.empty() and not ClassToUsedEntities.empty())
		{
			return mlir::rlc::logError(
					*ClassToUsedEntities.begin()->second.begin(),
					"Forbidden mutual dependency in entities");
		}
	}

	return mlir::success();
}

static mlir::LogicalResult declareEntities(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::SmallVector<mlir::Operation*, 2> decls;
	if (getClassDeclarationSortedByDependencies(op, decls).failed())
		return mlir::failure();

	for (auto& decl : decls)
	{
		auto casted = mlir::dyn_cast<mlir::rlc::ClassDeclaration>(decl);
		if (not casted)
			continue;

		llvm::SmallVector<mlir::Type, 2> templates;
		for (auto type :
				 casted.getTemplateParameters().getAsValueRange<mlir::TypeAttr>())
			templates.push_back(type);
		rewriter.setInsertionPoint(casted);
		casted.getResult().setType(
				mlir::rlc::ClassType::getIdentified(
						casted.getContext(), casted.getName(), templates));
	}
	return mlir::success();
}

static mlir::LogicalResult declareActionEntities(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::StringMap<mlir::Operation*> decls;
	if (findClassDecls(op, decls).failed())
		return mlir::failure();

	rewriter.setInsertionPointToEnd(op.getBody());
	for (auto action : op.getOps<mlir::rlc::ActionFunction>())
	{
		if (action.getUnmangledName().empty())
		{
			return mlir::rlc::logError(
					action, "Action statements must have a return type");
		}
		mlir::rlc::ClassType type = action.getClassType();

		auto classDecl = rewriter.create<mlir::rlc::ClassDeclaration>(
				action.getLoc(),
				type,
				type.getName(),
				mlir::ArrayRef<mlir::Type>({}),
				nullptr);

		mlir::rlc::markSynthetic(classDecl);

		if (decls.count(type.getName()) != 0)
		{
			auto _ = mlir::rlc::logError(
					action,
					"User defined type clashes with implicit action type " +
							type.getName());
			return mlir::rlc::logRemark(decls[type.getName()], "Type declared here");
		}

		if (llvm::is_contained({ "Int", "Float", "Byte", "Bool" }, type.getName()))
		{
			return mlir::rlc::logError(
					action, "Action type cannot be primitive type " + type.getName());
		}

		decls.try_emplace(type.getName(), classDecl);
	}

	return mlir::success();
}

static mlir::LogicalResult deduceClassBody(
		mlir::ModuleOp op, mlir::rlc::ClassDeclaration decl)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter& rewriter = builder.getRewriter();
	rewriter.setInsertionPoint(decl);

	// entities of actions are discovered later, because they require to
	// typecheck the body of the action itself.
	if (builder.isClassOfAction(decl.getType()))
		return mlir::success();

	llvm::SmallVector<mlir::rlc::ClassFieldAttr> newFields;

	llvm::SmallVector<mlir::Type, 2> checkedTemplateParameters;

	auto scopedConverter = mlir::rlc::RLCTypeConverter(&builder.getConverter());
	scopedConverter.setErrorLocation(decl.getLoc());
	for (auto parameter : decl.getTemplateParameters())
	{
		auto unchecked = parameter.cast<mlir::TypeAttr>()
												 .getValue()
												 .cast<mlir::rlc::UncheckedTemplateParameterType>();

		auto checkedParameterType = scopedConverter.convertType(unchecked);
		if (not checkedParameterType)
		{
			return mlir::failure();
		}
		checkedTemplateParameters.push_back(checkedParameterType);
		auto actualType =
				checkedParameterType.cast<mlir::rlc::TemplateParameterType>();
		scopedConverter.registerType(actualType.getName(), actualType);
	}

	for (auto field : decl.getOps<mlir::rlc::ClassFieldDeclaration>())
	{
		auto converted = scopedConverter.convertType(
				field.getDeclaration().getDeshugarizedType());
		if (!converted)
		{
			return mlir::failure();
		}
		newFields.push_back(
				mlir::rlc::ClassFieldAttr::get(
						field.getDeclaration().getName(), converted));

		if (field.getDeclaration().getShugarizedType())
		{
			auto shugarizedType = scopedConverter.shugarizedConvertType(
					field.getDeclaration().getShugarizedType().getType());
			if (!shugarizedType)
			{
				return mlir::failure();
			}
			field.setDeclarationAttr(
					mlir::rlc::ClassFieldDeclarationAttr::get(
							field.getContext(),
							newFields.back(),
							field.getDeclaration().getShugarizedType().replaceType(
									shugarizedType)));
		}
		else
			field.setDeclarationAttr(
					mlir::rlc::ClassFieldDeclarationAttr::get(
							field.getContext(), newFields.back(), nullptr));
	}
	auto finalType = mlir::rlc::ClassType::getIdentified(
			decl.getContext(), decl.getName(), checkedTemplateParameters);

	if (finalType.setBody(newFields).failed())
	{
		assert(false && "unrechable");
		return mlir::failure();
	}

	decl.getResult().setType(finalType);
	decl.setTemplateParametersAttr(
			rewriter.getTypeArrayAttr(checkedTemplateParameters));

	return mlir::success();
}

static mlir::LogicalResult deduceEntitiesBodies(mlir::ModuleOp op)
{
	llvm::SmallVector<mlir::Operation*, 2> decls;
	if (getClassDeclarationSortedByDependencies(op, decls).failed())
		return mlir::failure();

	for (auto& decl : decls)
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::ClassDeclaration>(decl))
		{
			if (deduceClassBody(op, casted).failed())
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

static bool isControlFlowTerminated(mlir::Operation* current);

static bool isInnerControlFlowTerminated(mlir::Operation* current)
{
	auto dynCasted = mlir::dyn_cast<mlir::RegionBranchOpInterface>(current);

	if (!dynCasted)
		return false;

	llvm::SmallVector<mlir::RegionSuccessor, 4> next;
	mlir::DenseSet<mlir::Region*> visited;

	dynCasted.getEntrySuccessorRegions({}, next);

	// bfs to see if every path is return termintead
	while (!next.empty())
	{
		llvm::SmallVector<mlir::RegionSuccessor, 4> newNexts;
		for (auto node : next)
		{
			// if we jump to the parent, then we found a path that
			// is not return terminated
			if (node.isParent())
				return false;

			visited.insert(node.getSuccessor());

			// if the target region is return terminated on every path, do not
			// continue analyzing this path
			if (isControlFlowTerminated(&node.getSuccessor()->front().front()))
				continue;

			dynCasted.getSuccessorRegions(node, newNexts);
		}

		// erase the paths we already considered
		llvm::erase_if(newNexts, [&](mlir::RegionSuccessor succ) {
			return visited.contains(succ.getSuccessor());
		});

		next = std::move(newNexts);
	}

	return true;
}

static bool isControlFlowTerminated(mlir::Operation* current)
{
	const auto isBlockTerminatedByReturn = [](mlir::Block& block) {
		return block.empty() or isControlFlowTerminated(&block.front());
	};
	while (current != nullptr)
	{
		if (llvm::isa<mlir::rlc::ReturnStatement>(current) or
				isInnerControlFlowTerminated(current))
			return true;

		current = current->getNextNode();
	}

	return false;
}

// checks that every path is terminated with a return statement
static mlir::LogicalResult checkReturnsPath(mlir::rlc::FunctionOp fun)
{
	auto* lastOp = &fun.getBody().getBlocks().front().back();
	if (not isControlFlowTerminated(&fun.getBody().front().front()))
		return mlir::rlc::logError(
				fun,
				"Non void function requires to be terminated by a return "
				"statement");

	return mlir::success();
}

// enum field expressions are already moved to their proper location
// when we type check, but we type check them again here
// so that we can reserialize properly the file if we need to
static mlir::LogicalResult deduceEnumFieldTypes(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);

	mlir::IRRewriter rewriter(op.getContext());
	llvm::SmallVector<mlir::rlc::EnumDeclarationOp, 4> funs(
			op.getOps<mlir::rlc::EnumDeclarationOp>());

	for (auto enumDecl : funs)
	{
		for (auto field : enumDecl.getOps<mlir::rlc::EnumFieldDeclarationOp>())
		{
			for (auto exp : field.getOps<mlir::rlc::EnumFieldExpressionOp>())
			{
				llvm::SmallVector<mlir::Operation*, 4> ops;
				for (auto& op : exp.getBody().front())
					ops.push_back(&op);
				for (auto& op : ops)
				{
					if (mlir::rlc::typeCheck(*op, builder).failed())
						return mlir::failure();
				}
			}
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
		if (fun.isDeclaration())
			continue;

		for (auto overload : builder.getSymbolTable().get(fun.getUnmangledName()))
		{
			if (overload.getType() == fun.getType() and overload != fun)
			{
				auto _ = mlir::rlc::logError(
						fun, "Multiple definitions of function " + fun.getUnmangledName());
				auto _2 = mlir::rlc::logRemark(
						overload.getDefiningOp(), "Previous definition here.");

				return mlir::failure();
			}
		}

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
				mlir::rlc::markSynthetic(integerLiteral);
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
		{
			if (checkReturnsPath(fun).failed())
				return mlir::failure();
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
		builder.getConverter().setErrorLocation(fun.getLoc());
		auto funType = builder.typeOfAction(fun);
		if (funType == nullptr)
		{
			return mlir::rlc::logError(
					fun,
					mlir::rlc::prettyType(fun.getClassType()) +
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
		auto shugarized =
				builder.getConverter().shugarizedConvertType(fun.getFunctionType());
		if (convertedReturnType == nullptr or shugarized == nullptr)
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
		fun.setInfoAttr(
				fun.getInfo().replaceTypes(shugarized.cast<mlir::FunctionType>()));
		fun.getIsDoneFunction().setType(isDoneType);
	}
	return mlir::success();
}

static mlir::LogicalResult areArgumentsEqual(
		mlir::FunctionType lhs, mlir::FunctionType rhs)
{
	if (lhs.getNumInputs() != rhs.getNumInputs())
		return mlir::failure();

	for (auto [l, r] : llvm::zip(lhs.getInputs(), rhs.getInputs()))
		if (l != r)
			return mlir::failure();

	return mlir::success();
}

static mlir::LogicalResult checkForActionRedefinitions(
		mlir::rlc::ActionFunction typeChecked,
		llvm::StringMap<mlir::FunctionType>& actionNameToMainFunctionType)
{
	auto candidate =
			actionNameToMainFunctionType.find(typeChecked.getUnmangledName());
	if (candidate == actionNameToMainFunctionType.end())
	{
		actionNameToMainFunctionType[typeChecked.getUnmangledName()] =
				typeChecked.getFunctionType();
		return mlir::success();
	}
	if (areArgumentsEqual(candidate->getValue(), typeChecked.getFunctionType())
					.succeeded())
	{
		return logError(
				typeChecked,
				"Redefinition of action function named " +
						typeChecked.getUnmangledName());
	}
	actionNameToMainFunctionType[typeChecked.getUnmangledName()] =
			typeChecked.getFunctionType();
	return mlir::success();
}

static mlir::LogicalResult typeCheckActions(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::StringMap<mlir::FunctionType> actionNameToMainFunctionType;

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
			if (typeChecked == fun)
				continue;

			foundOne = true;
			if (checkForActionRedefinitions(typeChecked, actionNameToMainFunctionType)
							.failed())
				return mlir::failure();
			break;
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

			if (deduceEnumFieldTypes(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}
		}
	};
}	 // namespace mlir::rlc

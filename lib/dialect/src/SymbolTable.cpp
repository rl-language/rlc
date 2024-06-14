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
#include "rlc/dialect/SymbolTable.h"

#include <set>

#include "rlc/dialect/Operations.hpp"

static void makeValueTable(mlir::ModuleOp op, mlir::rlc::ValueTable& table)
{
	for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
		table.add(fun.getUnmangledName(), fun);
}

mlir::rlc::ValueTable mlir::rlc::makeValueTable(mlir::ModuleOp op)
{
	mlir::rlc::ValueTable table;
	::makeValueTable(op, table);

	return table;
}

mlir::rlc::TypeTable mlir::rlc::makeTypeTable(mlir::ModuleOp mod)
{
	mlir::rlc::TypeTable table;

	table.add("Int", mlir::rlc::IntegerType::getInt64(mod.getContext()));
	table.add("Byte", mlir::rlc::IntegerType::getInt8(mod.getContext()));
	table.add("Void", mlir::rlc::VoidType::get(mod.getContext()));
	table.add("Float", mlir::rlc::FloatType::get(mod.getContext()));
	table.add("Bool", mlir::rlc::BoolType::get(mod.getContext()));
	table.add(
			"StringLiteral", mlir::rlc::StringLiteralType::get(mod.getContext()));

	for (auto classDecl : mod.getOps<mlir::rlc::ClassDeclaration>())
		table.add(classDecl.getName(), classDecl.getType());

	for (auto traitDefinition : mod.getOps<mlir::rlc::TraitDefinition>())
		table.add(
				traitDefinition.getMetaType().getName(), traitDefinition.getMetaType());

	for (auto usingStatemenet : mod.getOps<mlir::rlc::TypeAliasOp>())
		table.add(usingStatemenet.getName(), usingStatemenet.getAliased());

	return table;
}

static mlir::Type instantiateTemplate(
		mlir::Type type, mlir::TypeRange values, mlir::Location errorPoint)
{
	if (values.empty())
	{
		if (auto casted = type.dyn_cast<mlir::rlc::ClassType>();
				casted and isTemplateType(casted).succeeded())
		{
			mlir::emitError(
					errorPoint,
					"use of template type without explicit template arguments is not "
					"allowed");
			return nullptr;
		}
		return type;
	}

	if (auto casted = type.dyn_cast<mlir::rlc::TraitMetaType>())
	{
		if (casted.getTemplateParameterTypes().size() != values.size() + 1)
		{
			mlir::emitError(
					errorPoint,
					mlir::Twine("Trait explicit instantiation needed ") +
							mlir::Twine((casted.getTemplateParameterTypes().size() - 1)) +
							mlir::Twine(" arguments, but ") + mlir::Twine(values.size()) +
							mlir::Twine(" where provided"));
			return nullptr;
		}
		mlir::Type toReturn = casted;
		for (auto pair : llvm::zip(casted.getTemplateParameterTypes(), values))
		{
			auto current = std::get<0>(pair);
			auto replacement = std::get<1>(pair);
			toReturn = casted.replace([&](mlir::Type t) -> mlir::Type {
				if (t == current)
					return replacement;
				return t;
			});
		}

		return toReturn;
	}

	if (not type.isa<mlir::rlc::ClassType>())
	{
		mlir::emitError(
				errorPoint,
				"explicit template instantiation on non template class or trait type");
		return nullptr;
	}
	auto casted = type.cast<mlir::rlc::ClassType>();
	if (casted.getExplicitTemplateParameters().size() != values.size())
	{
		mlir::emitError(
				errorPoint,
				llvm::Twine("Template type has ") +
						llvm::Twine(casted.getExplicitTemplateParameters().size()) +
						" parameters but " + llvm::Twine(values.size()) +
						" were provided.");
		return nullptr;
	}

	for (auto [first, second] :
			 llvm::zip(casted.getExplicitTemplateParameters(), values))
	{
		auto originalType = first;
		auto replacementType = second;
		casted = casted
								 .replace([&](mlir::Type t) -> mlir::Type {
									 if (t == originalType)
										 return replacementType;
									 return t;
								 })
								 .cast<mlir::rlc::ClassType>();
	}
	return casted;
}

static void registerConversions(
		mlir::TypeConverter& converter,
		mlir::rlc::TypeTable& types,
		mlir::Location& errorPoint)
{
	converter.addConversion(
			[&](mlir::rlc::ScalarUseType use) -> std::optional<mlir::Type> {
				if (use.getUnderlying() != nullptr)
				{
					auto convertedUnderlying = converter.convertType(use.getUnderlying());
					if (not convertedUnderlying)
					{
						return convertedUnderlying;
					}

					if (auto casted =
									use.getSize().dyn_cast<mlir::rlc::IntegerLiteralType>())
					{
						if (casted.getValue() != 0)
							return mlir::rlc::ArrayType::get(
									use.getContext(), convertedUnderlying, casted.getValue());

						return convertedUnderlying;
					}

					auto convertedSize = converter.convertType(use.getSize());
					if (not convertedSize)
						return convertedSize;
					return mlir::rlc::ArrayType::get(
							use.getContext(), convertedUnderlying, convertedSize);
				}

				llvm::SmallVector<mlir::Type> explicitTemplateParameters;
				for (auto templateParameter : use.getExplicitTemplateParameters())
				{
					auto parameter = converter.convertType(templateParameter);
					if (not parameter)
						return std::nullopt;
					explicitTemplateParameters.push_back(parameter);
				}

				auto maybeType = types.getOne(use.getReadType());
				if (maybeType == nullptr)
				{
					mlir::emitError(
							errorPoint, "No known type named " + use.getReadType());
					return std::nullopt;
				}
				maybeType = instantiateTemplate(
						maybeType, explicitTemplateParameters, errorPoint);
				if (maybeType == nullptr)
				{
					return std::nullopt;
				}

				if (auto casted =
								use.getSize().dyn_cast<mlir::rlc::IntegerLiteralType>())
				{
					if (casted.getValue() == 0)
						return maybeType;
					return mlir::rlc::ArrayType::get(
							use.getContext(), maybeType, use.getSize());
				}

				auto convertedSize = converter.convertType(use.getSize());
				if (not convertedSize)
					return convertedSize;

				return mlir::rlc::ArrayType::get(
						use.getContext(), maybeType, convertedSize);
			});
	converter.addConversion(
			[&](mlir::FunctionType use) -> std::optional<mlir::Type> {
				llvm::SmallVector<mlir::Type, 2> types;
				llvm::SmallVector<mlir::Type, 2> outs;

				for (auto subtype : use.getInputs())
				{
					auto converted = converter.convertType(subtype);
					if (not converted)
						return converted;
					types.push_back(converted);
				}

				for (auto subtype : use.getResults())
				{
					auto converted = converter.convertType(subtype);
					if (not converted)
						return converted;
					outs.push_back(converted);
				}

				return mlir::FunctionType::get(use.getContext(), types, outs);
			});
	converter.addConversion(
			[&](mlir::rlc::FunctionUseType use) -> std::optional<mlir::Type> {
				llvm::SmallVector<mlir::Type, 2> types;

				for (auto subtype : llvm::drop_end(use.getSubTypes()))
				{
					auto converted = converter.convertType(subtype);
					if (not converted)
						return converted;
					types.push_back(converted);
				}

				auto converted = converter.convertType(use.getSubTypes().back());
				if (not converted)
					return converted;

				return mlir::FunctionType::get(use.getContext(), types, { converted });
			});
	converter.addConversion([&](mlir::rlc::ReferenceType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		if (not converted)
			return converted;
		return mlir::rlc::ReferenceType::get(t.getContext(), converted);
	});
	converter.addConversion([&](mlir::rlc::FrameType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		if (not converted)
			return converted;
		return mlir::rlc::FrameType::get(t.getContext(), converted);
	});
	converter.addConversion([&](mlir::rlc::ContextType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		if (not converted)
			return converted;
		return mlir::rlc::ContextType::get(t.getContext(), converted);
	});
	converter.addConversion([](mlir::rlc::IntegerType t) { return t; });
	converter.addConversion([](mlir::rlc::VoidType t) { return t; });
	converter.addConversion([](mlir::rlc::BoolType t) { return t; });
	converter.addConversion([](mlir::rlc::FloatType t) { return t; });
	converter.addConversion([](mlir::rlc::IntegerLiteralType t) { return t; });
	converter.addConversion([&](mlir::rlc::OwningPtrType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		if (converted == nullptr)
			return converted;
		return mlir::rlc::OwningPtrType::get(t.getContext(), converted);
	});
	converter.addConversion([&](mlir::rlc::AlternativeType t) -> mlir::Type {
		llvm::SmallVector<mlir::Type, 2> content;
		for (mlir::Type underlying : t.getUnderlying())
		{
			auto converted = converter.convertType(underlying);
			if (converted == nullptr)
				return nullptr;
			content.push_back(converted);
		}

		return mlir::rlc::AlternativeType::get(t.getContext(), content);
	});
	converter.addConversion([&](mlir::rlc::ArrayType t) -> mlir::Type {
		auto converted = converter.convertType(t.getUnderlying());
		if (converted == nullptr)
			return converted;
		return mlir::rlc::ArrayType::get(t.getContext(), converted, t.getSize());
	});
	converter.addConversion(
			[&](mlir::rlc::ClassType t) -> mlir::Type { return t; });
	converter.addConversion(
			[&](mlir::rlc::UncheckedTemplateParameterType t)
					-> std::optional<mlir::Type> {
				if (t.getTrait() == "Int")
					return mlir::rlc::TemplateParameterType::get(
							t.getContext(), t.getName(), nullptr, true);

				if (t.getTrait() == "")
					return mlir::rlc::TemplateParameterType::get(
							t.getContext(), t.getName(), nullptr, false);

				if (auto maybeType = types.getOne(t.getTrait()))
				{
					if (auto trait = maybeType.dyn_cast<mlir::rlc::TraitMetaType>())
						return mlir::rlc::TemplateParameterType::get(
								t.getContext(), t.getName(), trait, false);

					return std::nullopt;
				}
				return std::nullopt;
			});
}

mlir::rlc::RLCTypeConverter::RLCTypeConverter(mlir::ModuleOp op)
		: types(makeTypeTable(op)), loc(op.getLoc())
{
	registerConversions(converter, types, loc);
}

mlir::rlc::RLCTypeConverter::RLCTypeConverter(
		mlir::rlc::RLCTypeConverter* parentScopeConverter)
		: types(&parentScopeConverter->types), loc(parentScopeConverter->loc)
{
	registerConversions(converter, types, loc);
}

static llvm::SmallVector<std::pair<int, mlir::rlc::ActionStatement>, 4>
distinctActionStatements(
		mlir::rlc::ActionFunction action, bool keeOnlyFirst = true)
{
	llvm::SmallVector<std::pair<int, mlir::rlc::ActionStatement>, 4> toReturn;
	using ActionKey = std::pair<std::string, std::vector<const void*>>;
	std::map<ActionKey, int> seenActionOverloads;

	action.walk([&](mlir::rlc::ActionStatement statement) {
		std::vector<const void*> types;
		for (auto type : statement.getResultTypes())
			types.push_back(type.getAsOpaquePointer());

		ActionKey key(statement.getName().str(), types);

		if (seenActionOverloads.contains(key) and keeOnlyFirst)
			return;

		if (not seenActionOverloads.contains(key))
			seenActionOverloads[key] = seenActionOverloads.size();

		toReturn.emplace_back(seenActionOverloads[key], statement);
	});

	return toReturn;
}

void mlir::rlc::ModuleBuilder::removeActionFromRootTable(mlir::Operation* op)
{
	auto action = mlir::cast<mlir::rlc::ActionFunction>(op);
	getRootTable().erase(
			action.getUnmangledName(), action.getOperation()->getOpResult(0));

	getRootTable().erase("is_done", action.getIsDoneFunction());

	if (action.getActions().empty())
		return;

	using ActionKey = std::pair<std::string, std::vector<const void*>>;
	std::map<ActionKey, int> seenActionOverloads;
	for (auto statement : distinctActionStatements(action))
	{
		auto resultValue = action.getActions()[statement.first];
		getRootTable().erase(statement.second.getName(), resultValue);
	}
}

void mlir::rlc::ModuleBuilder::addActionToRootTable(mlir::Operation* op)
{
	auto action = mlir::cast<mlir::rlc::ActionFunction>(op);
	getRootTable().add(
			action.getUnmangledName(), action.getOperation()->getOpResult(0));

	getRootTable().add("is_done", action.getIsDoneFunction());

	if (action.getActions().empty())
		return;

	using ActionKey = std::pair<std::string, std::vector<const void*>>;
	std::map<ActionKey, int> seenActionOverloads;
	for (auto statement : distinctActionStatements(action))
	{
		auto resultValue = action.getActions()[statement.first];
		getRootTable().add(statement.second.getName(), resultValue);
	}
}

void mlir::rlc::ModuleBuilder::removeAction(mlir::Operation* op)
{
	auto action = mlir::cast<mlir::rlc::ActionFunction>(op);

	auto type = action.getResultTypes()[0].cast<mlir::rlc::ClassType>();

	actionToActionType.erase(action.getResult());
	actionTypeToAction.erase(type);
	if (actionDeclToActionStatements.contains(action.getResult()))
		actionDeclToActionStatements.erase(action.getResult());

	if (action.getActions().empty())
		return;

	for (auto statement : distinctActionStatements(action, false))
	{
		auto resultValue = action.getActions()[statement.first];
		actionFunctionResultToActionStement.erase(resultValue);
	}

	for (auto statement : distinctActionStatements(action))
	{
		actionDeclToActionNames.erase(action.getResult());
	}
}

void mlir::rlc::ModuleBuilder::registerAction(mlir::Operation* op)
{
	auto action = mlir::cast<mlir::rlc::ActionFunction>(op);

	auto type = getConverter().getTypes().getOne(
			action.getResultTypes()[0].cast<mlir::rlc::ClassType>().getName());
	actionToActionType[action.getResult()] = type;
	actionTypeToAction[type] = action.getResult();

	action.walk([&](mlir::rlc::ActionStatement statement) {
		actionDeclToActionStatements[action.getResult()].push_back(
				statement.getOperation());
	});

	// stuff that we do only if we have already type checked
	if (action.getActions().empty())
		return;

	for (auto statement : distinctActionStatements(action, false))
	{
		auto resultValue = action.getActions()[statement.first];
		actionFunctionResultToActionStement[resultValue].push_back(
				statement.second);
	}

	for (auto statement : distinctActionStatements(action))
	{
		actionDeclToActionNames[action.getResult()].push_back(
				statement.second.getName().str());
	}
}

mlir::rlc::ModuleBuilder::ModuleBuilder(
		mlir::ModuleOp op, ValueTable* parentSymbolTable)
		: op(op), rewriter(op.getContext())
{
	values.emplace_back(std::make_unique<ValueTable>(parentSymbolTable));
	converter.emplace_back(std::make_unique<RLCTypeConverter>(op));
	if (parentSymbolTable == nullptr)
		::makeValueTable(op, getSymbolTable());
	for (auto trait : op.getOps<mlir::rlc::TraitDefinition>())
	{
		auto result = traits.try_emplace(trait.getMetaType().getName(), trait);
		assert(result.second);
	}

	for (auto fun : getSymbolTable().get(
					 mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>()))
	{
		auto casted = fun.getDefiningOp<mlir::rlc::FunctionOp>();
		if (casted and casted.getFunctionType().getNumInputs() == 1 and
				casted.getFunctionType().getNumResults() == 0)
			typeToInitFunction[casted.getArgumentTypes()[0]] = fun;
	}

	for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
	{
		if (fun.getUnmangledName() ==
						mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>() or
				fun.getUnmangledName() == builtinOperatorName<mlir::rlc::AssignOp>())
			continue;
	}

	for (auto action : op.getOps<mlir::rlc::ActionFunction>())
	{
		if (parentSymbolTable == nullptr)
			addActionToRootTable(action);
		registerAction(action);
	}

	for (auto decl : op.getOps<mlir::rlc::ClassDeclaration>())
		typeTypeDeclaration[decl.getResult().getType()] = decl.getOperation();
}

mlir::rlc::TraitDefinition mlir::rlc::ModuleBuilder::getTraitDefinition(
		mlir::rlc::TraitMetaType type)
{
	return traits[type.getName()];
}

mlir::Type mlir::rlc::ModuleBuilder::typeOfAction(mlir::rlc::ActionFunction& f)
{
	assert(actionToActionType.count(f.getResult()) != 0);
	return actionToActionType[f.getResult()];
}

// returns the list of action statements invoked by the body of this action.
llvm::iterator_range<mlir::Operation**>
mlir::rlc::ModuleBuilder::actionStatementsOfAction(
		mlir::rlc::ActionFunction& val)
{
	return actionDeclToActionStatements[val.getResult()];
}

void mlir::rlc::ModuleBuilder::addTraitToAviableOverloads(
		mlir::rlc::TraitMetaType trait)
{
	auto traitDecl = getTraitDefinition(trait);
	for (auto [name, value] :
			 llvm::zip(trait.getRequestedFunctionNames(), traitDecl.getResults()))
	{
		getSymbolTable().add(name, value);
	}
}

static mlir::Operation* emitBuiltinAssign(
		mlir ::rlc::ModuleBuilder& builder,
		mlir::Operation* callSite,
		llvm::StringRef name,
		mlir::ValueRange arguments)
{
	auto argTypes = arguments.getType();
	if (name != mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>() or
			arguments.size() != 2)
		return nullptr;

	if ((argTypes[0].isa<mlir::rlc::FloatType>() or
			 argTypes[0].isa<mlir::rlc::IntegerType>() or
			 argTypes[0].isa<mlir::rlc::BoolType>() or
			 argTypes[0].isa<mlir::rlc::OwningPtrType>()) and
			(argTypes[0] == argTypes[1]))
	{
		return builder.getRewriter().create<mlir::rlc::BuiltinAssignOp>(
				callSite->getLoc(), arguments[0], arguments[1]);
	}

	auto overload =
			builder.resolveFunctionCall(callSite, true, name, arguments, false);
	if (overload)
		return builder.getRewriter().create<mlir::rlc::CallOp>(
				callSite->getLoc(), overload, true, arguments);

	return builder.getRewriter().create<mlir::rlc::ImplicitAssignOp>(
			callSite->getLoc(), arguments[0], arguments[1]);
}

static mlir::rlc::CastOp emitCast(
		mlir::IRRewriter& rewriter,
		mlir::Operation* callSite,
		llvm::StringRef name,
		mlir::ValueRange arguments)
{
	auto argTypes = arguments.getType();
	if (name == "byte" and arguments.size() == 1 and
			(argTypes[0].isa<mlir::rlc::FloatType>() or
			 argTypes[0].isa<mlir::rlc::IntegerType>() or
			 argTypes[0].isa<mlir::rlc::BoolType>()))
	{
		return rewriter.create<mlir::rlc::CastOp>(
				callSite->getLoc(),
				arguments[0],
				mlir::rlc::IntegerType::getInt8(callSite->getContext()));
	}
	if (name == "int" and arguments.size() == 1 and
			(argTypes[0].isa<mlir::rlc::FloatType>() or
			 argTypes[0].isa<mlir::rlc::IntegerType>() or
			 argTypes[0].isa<mlir::rlc::BoolType>()))
	{
		return rewriter.create<mlir::rlc::CastOp>(
				callSite->getLoc(),
				arguments[0],
				mlir::rlc::IntegerType::getInt64(callSite->getContext()));
	}

	if (name == "float" and arguments.size() == 1 and
			(argTypes[0].isa<mlir::rlc::IntegerType>() or
			 argTypes[0].isa<mlir::rlc::FloatType>() or
			 argTypes[0].isa<mlir::rlc::BoolType>()))
	{
		return rewriter.create<mlir::rlc::CastOp>(
				callSite->getLoc(),
				arguments[0],
				mlir::rlc::FloatType::get(callSite->getContext()));
	}

	if (name == "bool" and arguments.size() == 1 and
			(argTypes[0].isa<mlir::rlc::FloatType>() or
			 argTypes[0].isa<mlir::rlc::BoolType>() or
			 argTypes[0].isa<mlir::rlc::IntegerType>()))
	{
		return rewriter.create<mlir::rlc::CastOp>(
				callSite->getLoc(),
				arguments[0],
				mlir::rlc::BoolType::get(callSite->getContext()));
	}
	return nullptr;
}

mlir::Operation* mlir::rlc::ModuleBuilder::emitCall(
		mlir::Operation* callSite,
		bool isMemberCall,
		llvm::StringRef name,
		mlir::ValueRange arguments,
		bool emitLog)
{
	if (auto maybeCast = emitCast(rewriter, callSite, name, arguments))
		return maybeCast;

	if (auto* maybeCast = emitBuiltinAssign(*this, callSite, name, arguments))
		return maybeCast;

	auto argTypes = arguments.getType();
	auto overload =
			resolveFunctionCall(callSite, isMemberCall, name, arguments, emitLog);

	if (overload == nullptr)
		return nullptr;

	if (not overload.getType().isa<mlir::FunctionType>())
	{
		auto _ = logRemark(callSite, "Cannot call non function type");
		return nullptr;
	}

	return rewriter.create<mlir::rlc::CallOp>(
			callSite->getLoc(), overload, isMemberCall, arguments);
}

mlir::Value mlir::rlc::ModuleBuilder::resolveFunctionCall(
		mlir::Operation* callSite,
		bool isMemberCall,
		llvm::StringRef name,
		mlir::ValueRange arguments,
		bool logErrors)
{
	mlir::rlc::OverloadResolver resolver(
			getSymbolTable(), logErrors ? callSite : nullptr);
	return resolver.instantiateOverload(
			rewriter, isMemberCall, callSite->getLoc(), name, arguments);
}

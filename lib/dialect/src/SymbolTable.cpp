#include "rlc/dialect/SymbolTable.h"

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

	for (auto entityDecl : mod.getOps<mlir::rlc::EntityDeclaration>())
		table.add(entityDecl.getName(), entityDecl.getType());

	for (auto traitDefinition : mod.getOps<mlir::rlc::TraitDefinition>())
		table.add(
				traitDefinition.getMetaType().getName(), traitDefinition.getMetaType());

	return table;
}

static mlir::Type instantiateStructType(mlir::Type type, mlir::TypeRange values)
{
	if (values.empty())
		return type;

	if (not type.isa<mlir::rlc::EntityType>())
	{
		type.dump();
		for (auto type : values)
			type.dump();
		mlir::emitError(
				mlir::UnknownLoc::get(type.getContext()),
				"explicit template instantiation on non template entity type");
		return nullptr;
	}
	auto casted = type.cast<mlir::rlc::EntityType>();
	if (casted.getExplicitTemplateParameters().size() != values.size())
	{
		casted.dump();
		for (auto type : values)
			type.dump();
		mlir::emitError(
				mlir::UnknownLoc::get(type.getContext()),
				"missmatche explicit template parameters count");
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
								 .cast<mlir::rlc::EntityType>();
	}
	return casted;
}

static void registerConversions(
		mlir::TypeConverter& converter, mlir::rlc::TypeTable& types)
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
							mlir::UnknownLoc::get(use.getContext()),
							"type " + use.getReadType() + " not found");
					return std::nullopt;
				}
				maybeType =
						instantiateStructType(maybeType, explicitTemplateParameters);
				if (maybeType == nullptr)
				{
					mlir::emitError(
							mlir::UnknownLoc::get(use.getContext()),
							"in type " + use.getReadType());
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
	converter.addConversion([&](mlir::rlc::ReferenceType t) {
		auto converted = converter.convertType(t.getUnderlying());
		return mlir::rlc::ReferenceType::get(t.getContext(), converted);
	});
	converter.addConversion([](mlir::rlc::IntegerType t) { return t; });
	converter.addConversion([](mlir::rlc::VoidType t) { return t; });
	converter.addConversion([](mlir::rlc::BoolType t) { return t; });
	converter.addConversion([](mlir::rlc::FloatType t) { return t; });
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
			[&](mlir::rlc::EntityType t) -> mlir::Type { return t; });
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

					mlir::emitError(
							mlir::UnknownLoc::get(t.getContext()),
							t.getTrait().str() + " is not a trait");
					return std::nullopt;
				}
				mlir::emitError(
						mlir::UnknownLoc::get(t.getContext()),
						"trait" + t.getTrait().str() + " not found");
				return std::nullopt;
			});
}

mlir::rlc::RLCTypeConverter::RLCTypeConverter(mlir::ModuleOp op)
		: types(makeTypeTable(op))
{
	registerConversions(converter, types);
}

mlir::rlc::RLCTypeConverter::RLCTypeConverter(
		mlir::rlc::RLCTypeConverter* parentScopeConverter)
		: types(&parentScopeConverter->types)
{
	registerConversions(converter, types);
}

mlir::rlc::ModuleBuilder::ModuleBuilder(mlir::ModuleOp op)
		: op(op), rewriter(op.getContext())
{
	values.emplace_back(std::make_unique<ValueTable>());
	converter.emplace_back(std::make_unique<RLCTypeConverter>(op));
	::makeValueTable(op, getSymbolTable());
	for (auto trait : op.getOps<mlir::rlc::TraitDefinition>())
	{
		auto result = traits.try_emplace(trait.getMetaType(), trait);
		assert(result.second);
	}

	for (auto fun : getSymbolTable().get(
					 mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>()))
	{
		typeToInitFunction[fun.getDefiningOp<mlir::rlc::FunctionOp>()
													 .getArgumentTypes()[0]] = fun;
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
		auto type = getConverter().getTypes().getOne(
				(action.getUnmangledName() + "Entity").str());
		actionToActionType[action.getResult()] = type;
		actionTypeToAction[type] = action.getResult();
		getSymbolTable().add(
				action.getUnmangledName(), action.getOperation()->getOpResult(0));

		getSymbolTable().add("is_done", action.getIsDoneFunction());

		size_t actionIndex = 0;
		action.walk([&](mlir::rlc::ActionStatement statement) {
			// before we type checked we do not know what the generated sub
			// actions of a actions are, so the list is empty.
			if (not action.getActions().empty())
			{
				getSymbolTable().add(
						statement.getName(), action.getActions()[actionIndex]);
				actionFunctionResultToActionStement[action.getActions()[actionIndex]] =
						statement;
			}
			actionDeclToActionNames[action.getResult()].push_back(
					statement.getName().str());
			actionDeclToActionStatements[action.getResult()].push_back(
					statement.getOperation());

			actionIndex++;
		});
	}
}

mlir::rlc::TraitDefinition mlir::rlc::ModuleBuilder::getTraitDefinition(
		mlir::rlc::TraitMetaType type)
{
	return traits[type];
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

	auto overload = builder.resolveFunctionCall(callSite, name, arguments, false);
	if (overload)
		return builder.getRewriter().create<mlir::rlc::CallOp>(
				callSite->getLoc(), overload, arguments);

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
		mlir::Operation* callSite, llvm::StringRef name, mlir::ValueRange arguments)
{
	if (auto maybeCast = emitCast(rewriter, callSite, name, arguments))
		return maybeCast;

	if (auto* maybeCast = emitBuiltinAssign(*this, callSite, name, arguments))
		return maybeCast;

	auto argTypes = arguments.getType();
	auto overload = resolveFunctionCall(callSite, name, arguments);

	if (overload == nullptr)
	{
		callSite->emitRemark("while calling");
		return nullptr;
	}

	if (not overload.getType().isa<mlir::FunctionType>())
	{
		callSite->emitError("cannot call non function type");
		return nullptr;
	}

	return rewriter.create<mlir::rlc::CallOp>(
			callSite->getLoc(), overload, arguments);
}

mlir::Value mlir::rlc::ModuleBuilder::resolveFunctionCall(
		mlir::Operation* callSite,
		llvm::StringRef name,
		mlir::ValueRange arguments,
		bool logErrors)
{
	mlir::rlc::OverloadResolver resolver(
			getSymbolTable(), logErrors ? callSite : nullptr);
	return resolver.instantiateOverload(
			rewriter, callSite->getLoc(), name, arguments);
}

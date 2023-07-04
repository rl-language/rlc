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

	table.add("Int", mlir::rlc::IntegerType::get(mod.getContext()));
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

static void registerConversions(
		mlir::TypeConverter& converter, mlir::rlc::TypeTable& types)
{
	converter.addConversion(
			[&](mlir::rlc::ScalarUseType use) -> llvm::Optional<mlir::Type> {
				if (use.getUnderlying() != nullptr)
				{
					auto convertedUnderlying = converter.convertType(use.getUnderlying());
					if (not convertedUnderlying)
					{
						return convertedUnderlying;
					}

					if (use.getSize() != 0)
						return mlir::rlc::ArrayType::get(
								use.getContext(), convertedUnderlying, use.getSize());
					return convertedUnderlying;
				}

				auto maybeType = types.getOne(use.getReadType());
				if (maybeType == nullptr)
				{
					mlir::emitError(
							mlir::UnknownLoc::get(use.getContext()),
							"type " + use.getReadType() + " not found");
					return std::nullopt;
				}

				if (use.getSize() != 0)
					return mlir::rlc::ArrayType::get(
							use.getContext(), maybeType, use.getSize());

				return maybeType;
			});
	converter.addConversion(
			[&](mlir::FunctionType use) -> llvm::Optional<mlir::Type> {
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
			[&](mlir::rlc::FunctionUseType use) -> llvm::Optional<mlir::Type> {
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
	converter.addConversion([](mlir::rlc::IntegerType t) { return t; });
	converter.addConversion([](mlir::rlc::VoidType t) { return t; });
	converter.addConversion([](mlir::rlc::BoolType t) { return t; });
	converter.addConversion([](mlir::rlc::FloatType t) { return t; });
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
				if (t.getTrait() == "")
					return mlir::rlc::TemplateParameterType::get(
							t.getContext(), t.getName(), nullptr);

				if (auto maybeType = types.getOne(t.getTrait()))
				{
					if (auto trait = maybeType.dyn_cast<mlir::rlc::TraitMetaType>())
						return mlir::rlc::TemplateParameterType::get(
								t.getContext(), t.getName(), trait);

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

	for (auto fun :
			 getSymbolTable().get(builtinOperatorName<mlir::rlc::AssignOp>()))
	{
		typeToAssignFunction[fun.getDefiningOp<mlir::rlc::FunctionOp>()
														 .getArgumentTypes()[0]] = fun;
	}
	for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
	{
		if (fun.getArgumentTypes().empty())
		{
			actionsAndZeroParametersFunctions.push_back(fun);
			continue;
		}

		if (fun.getUnmangledName() ==
						mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>() or
				fun.getUnmangledName() == builtinOperatorName<mlir::rlc::AssignOp>())
			continue;

		typeToFunctions[fun.getArgumentTypes().front()].push_back(fun);
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
		typeToFunctions[type].push_back(action.getIsDoneFunction());
		actionsAndZeroParametersFunctions.push_back(action.getResult());

		size_t actionIndex = 0;
		action.walk([&](mlir::rlc::ActionStatement statement) {
			// before we type checked we do not know what the generated sub
			// actions of a actions are, so the list is empty.
			if (not action.getActions().empty())
			{
				getSymbolTable().add(
						statement.getName(), action.getActions()[actionIndex]);
				typeToFunctions[type].push_back(action.getActions()[actionIndex]);
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

llvm::iterator_range<mlir::Operation**>
mlir::rlc::ModuleBuilder::actionStatementsOfAction(
		mlir::rlc::ActionFunction& val)
{
	return actionDeclToActionStatements[val.getResult()];
}

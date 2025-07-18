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
#include "rlc/dialect/Operations.hpp"

#include <set>

#include "llvm/ADT/StringExtras.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/IR/OpDefinition.h"
#include "rlc/dialect/ActionLiveness.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Interfaces.hpp"
#include "rlc/utils/IRange.hpp"
#define GET_OP_CLASSES
#include "./Operations.inc"

void mlir::rlc::RLCDialect::registerOperations()
{
	addOperations<
#define GET_OP_LIST
#include "./Operations.inc"
			>();
}

static void assignActionIndicies(mlir::rlc::ActionFunction fun)
{
	mlir::IRRewriter rewriter(fun.getContext());

	size_t lastId = 1;
	int64_t lastResumePoint = 1;
	llvm::SmallVector<mlir::rlc::ActionStatement, 2> statments;
	fun.walk([&](mlir::rlc::ActionStatement statement) {
		statments.push_back(statement);
	});

	llvm::DenseMap<mlir::rlc::ActionsStatement, int64_t> asctionsToResumePoint;

	for (auto statement : statments)
	{
		rewriter.setInsertionPoint(statement);

		int64_t resumePoint;
		if (auto parent = mlir::dyn_cast<mlir::rlc::ActionsStatement>(
						statement->getParentOp()))
		{
			if (asctionsToResumePoint.count(parent) == 0)
				asctionsToResumePoint[parent] = lastResumePoint++;
			resumePoint = asctionsToResumePoint[parent];
		}
		else
		{
			resumePoint = lastResumePoint++;
		}

		statement.setResumptionPoint(resumePoint);
		statement.setId(lastId);
		lastId++;
	}
}

static mlir::LogicalResult getFunctionsTypesGeneratedByAction(
		mlir::rlc::ModuleBuilder &builder,
		mlir::rlc::ActionFunction fun,
		llvm::SmallVector<mlir::Type, 3> &out)
{
	auto funType = builder.typeOfAction(fun);
	llvm::StringMap<mlir::Type> added;
	// for each subaction invoked by this action, add it to generatedFunctions
	for (const auto &op : builder.actionStatementsOfAction(fun))
	{
		llvm::SmallVector<mlir::Type, 3> args({ funType });

		for (auto type : op->getResultTypes())
		{
			auto converted = builder.getConverter().convertType(type);
			args.push_back(converted);
		}

		auto ftype =
				mlir::FunctionType::get(op->getContext(), args, mlir::TypeRange());
		auto overloadKey = llvm::cast<mlir::rlc::ActionStatement>(op).getName();
		if (added.contains(overloadKey))
		{
			if (added[overloadKey] != ftype)
			{
				auto _ = mlir::rlc::logError(
						op,
						"Multiple definitions of actions with same name but different "
						"argument types");
				added[overloadKey].dump();
				auto _2 = mlir::rlc::logRemark(
						op,
						"previous type was " + mlir::rlc::prettyType(added[overloadKey]));
				return mlir::rlc::logRemark(
						op, "current type is " + mlir::rlc::prettyType(ftype));
			}
			continue;
		}

		out.emplace_back(ftype);
		added[overloadKey] = ftype;
	}
	return mlir::success();
}

std::pair<mlir::rlc::ActionFrameContent, mlir::rlc::ActionFrameContent>
mlir::rlc::ActionFunction::getFrameLists()
{
	mlir::rlc::ActionFrameContent explicitFrame;
	mlir::rlc::ActionFrameContent hiddenFrame;

	const auto &addToFrames = [&](mlir::Value value, llvm::StringRef name) {
		if (auto castedType = value.getType().dyn_cast<mlir::rlc::FrameType>())
			explicitFrame.append(value, name);
		else
			hiddenFrame.append(value, name);
	};

	for (auto pair :
			 llvm::zip(getInfo().getArguments(), getBody().front().getArguments()))
	{
		auto name = std::get<0>(pair).getName();
		auto arg = std::get<1>(pair);
		addToFrames(arg, name);
	}

	walk([&](mlir::Operation *op) {
		if (auto casted = llvm::dyn_cast<mlir::rlc::DeclarationStatement>(op))
		{
			if (auto castedType = casted.getType().dyn_cast<mlir::rlc::FrameType>())
				explicitFrame.append(casted, casted.getSymName());
		}
		else if (auto casted = llvm::dyn_cast<mlir::rlc::ActionStatement>(op))
		{
			for (auto pair :
					 llvm::zip(casted.getInfo().getArguments(), casted.getResults()))
			{
				auto name = std::get<0>(pair).getName();
				auto arg = std::get<1>(pair);
				addToFrames(arg, name);
			}
		}
	});
	return { std::move(explicitFrame), std::move(hiddenFrame) };
}

static mlir::LogicalResult setFramesBody(mlir::rlc::ActionFunction fun)
{
	llvm::SmallVector<mlir::rlc::ClassFieldAttr, 4> fields;

	llvm::SmallVector<mlir::rlc::ClassFieldAttr, 4> hiddenMembers;

	// add the implicit local variable "resume_index" to members
	fields.push_back(
			mlir::rlc::ClassFieldAttr::get(
					"resume_index", mlir::rlc::IntegerType::getInt64(fun.getContext())));

	auto frames = fun.getFrameLists();
	for (auto &entry : frames.first.valueNamePairs)
	{
		fields.push_back(
				mlir::rlc::ClassFieldAttr::get(
						entry.second.str(),
						entry.first.getType()
								.cast<mlir::rlc::FrameType>()
								.getUnderlying()));
	}

	for (auto &entry : frames.second.valueNamePairs)
	{
		auto t = entry.first.getType();
		if (auto casted = t.dyn_cast<mlir::rlc::ContextType>())
			t = casted.getUnderlying();
		hiddenMembers.push_back(
				mlir::rlc::ClassFieldAttr::get(
						entry.second.str(), mlir::rlc::ReferenceType::get(t)));
	}

	for (auto field : fields)
	{
		bool foundRecursiveUse = false;
		field.walk([&](mlir::Type t) {
			if (t == fun.getClassType())
				foundRecursiveUse = true;
		});
		if (foundRecursiveUse)
			return mlir::rlc::logError(
					fun, "Action type contains itself, this in not allowed\n");
	}

	auto res = fun.getClassType().setBody(fields);
	assert(res.succeeded());

	auto res2 = mlir::rlc::ClassType::getIdentified(
			fun.getContext(), (fun.getClassType().getName() + "_shadow").str(), {});
	auto isOk = res2.setBody(hiddenMembers).succeeded();
	assert(isOk);
	return mlir::success();
}

/*
	Rewrites the Action Function operations in the module to include the type,
	which contains information about
	- Arguments
	- Members (arguments, declared variables, variables provided by subactions,
	resume_index)
	- Subactions
	- Preconditions
	- Body
*/

static mlir::rlc::ActionFunction deduceActionType(mlir::rlc::ActionFunction fun)
{
	auto op = fun->getParentOfType<mlir::ModuleOp>();
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter &rewriter = builder.getRewriter();

	auto funType = builder.typeOfAction(fun);

	mlir::Type actionType = fun.getType();

	llvm::SmallVector<mlir::Type, 3> generatedFunctions;
	if (getFunctionsTypesGeneratedByAction(builder, fun, generatedFunctions)
					.failed())
		return nullptr;

	// rewrite the Action Function Operation with the generatedFunctions.
	rewriter.setInsertionPoint(fun);
	auto newAction = rewriter.create<mlir::rlc::ActionFunction>(
			fun.getLoc(),
			actionType,
			mlir::FunctionType::get(
					rewriter.getContext(),
					mlir::TypeRange({ funType }),
					mlir::TypeRange({ mlir::rlc::BoolType::get(rewriter.getContext()) })),
			generatedFunctions,
			fun.getUnmangledName(),
			fun.getInfo());
	newAction->setDiscardableAttrs(fun->getDiscardableAttrDictionary());
	newAction.getBody().takeBody(fun.getBody());
	newAction.getPrecondition().takeBody(fun.getPrecondition());
	fun.getResult().replaceAllUsesWith(newAction.getResult());
	rewriter.eraseOp(fun);

	if (setFramesBody(newAction).failed())
		return nullptr;
	return newAction;
}

static void defineGetNameFunction(
		mlir::rlc::ActionFunction function,
		mlir::rlc::ModuleBuilder &builder,
		mlir::Value action,
		mlir::rlc::ActionStatement statement,
		mlir::rlc::ClassType actionType)
{
	builder.getRewriter().setInsertionPoint(function);
	llvm::SmallVector<mlir::Type, 4> types({ actionType });
	llvm::SmallVector<llvm::StringRef, 4> names({ "self" });
	llvm::SmallVector<mlir::Location, 4> locs({ statement.getLoc() });

	// fun get_type_name(ActionType self) { }
	//   return "<name>"
	//
	auto fType = mlir::FunctionType::get(
			function.getContext(),
			types,
			{ mlir::rlc::StringLiteralType::get(function.getContext()) });
	auto applyFunction = builder.getRewriter().create<mlir::rlc::FunctionOp>(
			statement.getLoc(),
			"get_type_name",
			fType,
			mlir::rlc::FunctionInfoAttr::get(fType.getContext(), names),
			false);

	mlir::rlc::markSynthetic(applyFunction);

	{
		auto *bodyBB = builder.getRewriter().createBlock(
				&applyFunction.getBody(), {}, types, locs);

		builder.getRewriter().setInsertionPointToStart(bodyBB);

		auto lit = builder.getRewriter().create<mlir::rlc::StringLiteralOp>(
				statement.getLoc(), statement.getName());

		auto ret = builder.getRewriter().create<mlir::rlc::ReturnStatement>(
				statement.getLoc(), lit.getType());
		auto *BB = builder.getRewriter().createBlock(&ret.getBody());

		builder.getRewriter().create<mlir::rlc::Yield>(
				statement.getLoc(), mlir::ValueRange({ lit }));
	}
}

static mlir::rlc::FunctionOp defineApplyFunction(
		mlir::rlc::ActionFunction function,
		mlir::rlc::ModuleBuilder &builder,
		mlir::Value action,
		mlir::rlc::ActionStatement statement,
		mlir::rlc::ClassType actionType)
{
	builder.getRewriter().setInsertionPoint(function);
	llvm::SmallVector<mlir::Type, 4> types(
			{ actionType, function.getClassType() });
	llvm::SmallVector<llvm::StringRef, 4> names({ "self", "frame" });
	llvm::SmallVector<mlir::Location, 4> locs(
			{ statement.getLoc(), statement.getLoc() });

	for (auto [actionType, name] :
			 llvm::zip(function.getType().getInputs(), function.getArgNames()))
	{
		if (auto casted = actionType.dyn_cast<mlir::rlc::ContextType>())
		{
			types.push_back(casted.getUnderlying());
			names.push_back(name);
			locs.push_back(statement.getLoc());
		}
	}

	// fun apply(ActionType self, FrameType frame) { casted = can(action);
	// casted(frame, self...) }
	//   action(frame, self...)
	//
	auto applyFunctionType =
			mlir::FunctionType::get(function.getContext(), types, {});
	auto applyFunction = builder.getRewriter().create<mlir::rlc::FunctionOp>(
			statement.getLoc(),
			"apply",
			applyFunctionType,
			mlir::rlc::FunctionInfoAttr::get(applyFunctionType.getContext(), names),
			false);

	mlir::rlc::markSynthetic(applyFunction);

	{
		auto *preconditionBB = builder.getRewriter().createBlock(
				&applyFunction.getPrecondition(), {}, types, locs);

		builder.getRewriter().setInsertionPointToStart(preconditionBB);
		auto canExecuteAction = builder.getRewriter().create<mlir::rlc::CanOp>(
				statement.getLoc(), action);

		llvm::SmallVector<mlir::Value, 4> args(
				std::next(preconditionBB->args_begin()), preconditionBB->args_end());
		for (auto [index, field] : llvm::enumerate(actionType.getMembers()))
		{
			auto member = builder.getRewriter().create<mlir::rlc::MemberAccess>(
					statement.getLoc(), preconditionBB->getArguments()[0], index);
			args.push_back(member);
		}

		auto result = builder.getRewriter().create<mlir::rlc::CallOp>(
				statement.getLoc(), canExecuteAction, true, args);

		builder.getRewriter().create<mlir::rlc::Yield>(
				statement.getLoc(), mlir::ValueRange({ result.getResults()[0] }));
	}

	{
		auto *bodyBB = builder.getRewriter().createBlock(
				&applyFunction.getBody(), {}, types, locs);

		builder.getRewriter().setInsertionPointToStart(bodyBB);

		llvm::SmallVector<mlir::Value, 4> args(
				std::next(bodyBB->args_begin()), bodyBB->args_end());
		for (auto [index, field] : llvm::enumerate(actionType.getMembers()))
		{
			auto member = builder.getRewriter().create<mlir::rlc::MemberAccess>(
					statement.getLoc(), bodyBB->getArguments()[0], index);
			args.push_back(member);
		}

		builder.getRewriter().create<mlir::rlc::CallOp>(
				statement.getLoc(), action, true, args);
		builder.getRewriter().create<mlir::rlc::Yield>(
				statement.getLoc(), mlir::ValueRange({}));
	}

	return applyFunction;
}

static std::string snakeCaseToCamelCase(llvm::StringRef str)
{
	auto toReturn = str.str();

	for (size_t i = 0; i + 1 < toReturn.size(); i++)
	{
		if (toReturn[i] == '_')
			toReturn[i + 1] = llvm::toUpper(toReturn[i + 1]);
	}

	llvm::erase_if(toReturn, [](char c) { return c == '_'; });
	toReturn[0] = llvm::toUpper(toReturn[0]);
	return toReturn;
}

static void defineApplyFunctionAlternative(
		mlir::rlc::ActionFunction function,
		mlir::rlc::ModuleBuilder &builder,
		mlir::rlc::AlternativeType alternative,
		llvm::MutableArrayRef<mlir::rlc::FunctionOp> applyFunctions)
{
	builder.getRewriter().setInsertionPoint(function);
	llvm::SmallVector<mlir::Type, 4> types(
			{ alternative, function.getClassType() });
	llvm::SmallVector<llvm::StringRef, 4> names({ "self", "frame" });
	llvm::SmallVector<mlir::Location, 4> locs(
			{ function.getLoc(), function.getLoc() });

	for (auto [actionType, name] :
			 llvm::zip(function.getType().getInputs(), function.getArgNames()))
	{
		if (auto casted = actionType.dyn_cast<mlir::rlc::ContextType>())
		{
			types.push_back(casted.getUnderlying());
			names.push_back(name);
			locs.push_back(function.getLoc());
		}
	}

	// fun apply(ActionType self, FrameType frame) { casted = can(action);
	// casted(frame, self...) }
	//   action(frame, self...)
	//
	auto applyFunctionType =
			mlir::FunctionType::get(function.getContext(), types, {});
	auto applyFunction = builder.getRewriter().create<mlir::rlc::FunctionOp>(
			function.getLoc(),
			"apply",
			applyFunctionType,
			mlir::rlc::FunctionInfoAttr::get(applyFunctionType.getContext(), names),
			false);
	mlir::rlc::markSynthetic(applyFunction);

	{
		auto *preconditionBB = builder.getRewriter().createBlock(
				&applyFunction.getPrecondition(), {}, types, locs);

		builder.getRewriter().setInsertionPointToStart(preconditionBB);

		for (auto [type, applyFunction] :
				 llvm::zip(alternative.getUnderlying(), applyFunctions))
		{
			auto orOp = builder.getRewriter().create<mlir::rlc::ShortCircuitingOr>(
					function.getLoc());
			builder.getRewriter().create<mlir::rlc::Yield>(
					function.getLoc(), mlir::ValueRange({ orOp }));
			auto orLHS = builder.getRewriter().createBlock(&orOp.getLhs());

			auto andOp = builder.getRewriter().create<mlir::rlc::ShortCircuitingAnd>(
					function.getLoc());

			builder.getRewriter().createBlock(&andOp.getLhs());

			auto actionType = type.cast<mlir::rlc::ClassType>();
			auto isOp = builder.getRewriter().create<mlir::rlc::IsOp>(
					function.getLoc(), preconditionBB->getArgument(0), actionType);
			builder.getRewriter().create<mlir::rlc::Yield>(
					function.getLoc(), mlir::ValueRange({ isOp }));
			builder.getRewriter().createBlock(&andOp.getRhs());
			auto canExecuteAction = builder.getRewriter().create<mlir::rlc::CanOp>(
					function.getLoc(), applyFunction);

			auto frame = builder.getRewriter().create<mlir::rlc::ValueUpcastOp>(
					function.getLoc(), actionType, preconditionBB->getArgument(0));

			llvm::SmallVector<mlir::Value, 4> args = { frame };
			for (auto *arg = std::next(preconditionBB->args_begin());
					 arg != preconditionBB->args_end();
					 arg++)
				args.push_back(*arg);

			auto result = builder.getRewriter().create<mlir::rlc::CallOp>(
					function.getLoc(), canExecuteAction, false, args);
			builder.getRewriter().create<mlir::rlc::Yield>(
					function.getLoc(), mlir::ValueRange({ result.getResult(0) }));
			builder.getRewriter().setInsertionPointToEnd(orLHS);
			builder.getRewriter().create<mlir::rlc::Yield>(
					function.getLoc(), mlir::ValueRange({ andOp }));
			builder.getRewriter().createBlock(&orOp.getRhs());
		}

		auto falseConstant = builder.getRewriter().create<mlir::rlc::Constant>(
				function.getLoc(), false);
		builder.getRewriter().create<mlir::rlc::Yield>(
				function.getLoc(), mlir::ValueRange({ falseConstant }));
	}

	{
		auto *bodyBB = builder.getRewriter().createBlock(
				&applyFunction.getBody(), {}, types, locs);

		for (auto [type, applyFunction] :
				 llvm::zip(alternative.getUnderlying(), applyFunctions))
		{
			auto ifStmt = builder.getRewriter().create<mlir::rlc::IfStatement>(
					function.getLoc());
			builder.getRewriter().createBlock(&ifStmt.getCondition());

			auto actionType = type.cast<mlir::rlc::ClassType>();
			auto isOp = builder.getRewriter().create<mlir::rlc::IsOp>(
					function.getLoc(), bodyBB->getArgument(0), actionType);
			builder.getRewriter().create<mlir::rlc::Yield>(
					function.getLoc(), mlir::ValueRange({ isOp }));
			builder.getRewriter().createBlock(&ifStmt.getTrueBranch());

			auto frame = builder.getRewriter().create<mlir::rlc::ValueUpcastOp>(
					function.getLoc(), actionType, bodyBB->getArgument(0));

			llvm::SmallVector<mlir::Value, 4> args = { frame };
			for (auto *arg = std::next(bodyBB->args_begin());
					 arg != bodyBB->args_end();
					 arg++)
				args.push_back(*arg);

			auto result = builder.getRewriter().create<mlir::rlc::CallOp>(
					function.getLoc(), applyFunction, false, args);
			builder.getRewriter().create<mlir::rlc::Yield>(function.getLoc());
			builder.getRewriter().createBlock(&ifStmt.getElseBranch());

			builder.getRewriter().create<mlir::rlc::Yield>(function.getLoc());
			builder.getRewriter().setInsertionPointToEnd(bodyBB);
		}
		builder.getRewriter().create<mlir::rlc::Yield>(function.getLoc());
	}
}

static mlir::Type declareActionStatementType(
		mlir::rlc::ActionFunction function,
		mlir::rlc::ModuleBuilder &builder,
		mlir::Value action,
		llvm::SmallVector<mlir::rlc::FunctionOp, 4> &applyFunctions)
{
	builder.getRewriter().setInsertionPoint(function);
	auto statement = mlir::dyn_cast<mlir::rlc::ActionStatement>(
			builder.actionFunctionValueToActionStatement(action).front());

	std::string name = function.getClassType().getName().str();
	name += snakeCaseToCamelCase(statement.getName());

	llvm::SmallVector<mlir::rlc::ClassFieldAttr, 4> fields;
	for (auto [name, result] :
			 llvm::zip(statement.getDeclaredNames(), statement.getResults()))
	{
		if (result.getType().isa<mlir::rlc::ContextType>())
			continue;
		auto type = result.getType();
		if (auto casted = type.dyn_cast<mlir::rlc::FrameType>())
			type = casted.getUnderlying();
		auto field = mlir::rlc::ClassFieldAttr::get(name, type);
		fields.push_back(field);
	}

	auto type = mlir::rlc::ClassType::getNewIdentified(
			function.getContext(), name, fields, {});

	auto built = builder.getRewriter().create<mlir::rlc::ClassDeclaration>(
			statement.getLoc(), type, name, llvm::ArrayRef<mlir::Type>({}), nullptr);
	builder.getRewriter().createBlock(&built.getBody());

	for (auto field : fields)
	{
		builder.getRewriter().create<mlir::rlc::ClassFieldDeclaration>(
				built.getLoc(), mlir::rlc::ClassFieldDeclarationAttr::get(field));
	}
	builder.getRewriter().setInsertionPointAfter(built);
	mlir::rlc::markSynthetic(built);
	auto applyFunction =
			defineApplyFunction(function, builder, action, statement, type);
	defineGetNameFunction(function, builder, action, statement, type);
	applyFunctions.push_back(applyFunction);

	return type;
}

// given a action function X, for each action statement y of X, declares a type
// called X + y[0].toUppercase() + y[1:] that has a field for each argument of y
// with the same name and the same type.
//
// the generated type contains a function called apply that takes the action
// frame and ever ctx argument, and invokes the underlying y action statement
// using the content of the generated type.
//
// it declares as well a type called XAction that is a alternative type
// containing a entry for each generated action type
static mlir::LogicalResult declareActionTypes(
		mlir::rlc::ActionFunction function, mlir::rlc::ModuleBuilder &builder)
{
	builder.getRewriter().setInsertionPoint(function);
	auto templateParameter = mlir::rlc::TemplateParameterType::get(
			function.getContext(), "T", nullptr, false);
	auto trait = builder.getRewriter().create<mlir::rlc::TraitDefinition>(
			function.getLoc(),
			mlir::rlc::TraitMetaType::get(
					function.getContext(),
					(function.getClassType().getName() + "Action").str(),
					{ templateParameter },
					{ mlir::FunctionType::get(
							function.getContext(),
							{ templateParameter, function.getClassType() },
							{}) },
					{ builder.getRewriter().getStringAttr("apply") },
					{ builder.getRewriter().getStrArrayAttr({ "self" }) }));
	mlir::rlc::markSynthetic(trait);

	llvm::SmallVector<mlir::Type, 4> declaredTypes;
	llvm::SmallVector<mlir::rlc::FunctionOp, 4> applyFunctions;
	for (auto action : function.getActions())
	{
		auto declaredType =
				declareActionStatementType(function, builder, action, applyFunctions);
		declaredTypes.push_back(declaredType);
	}

	builder.getRewriter().setInsertionPoint(function);

	if (declaredTypes.empty())
		return mlir::success();

	// declares a type that contains the alternative between all possible actions
	auto alternative = mlir::rlc::AlternativeType::get(
			function.getContext(),
			declaredTypes,
			("Any" + function.getClassType().getName() + "Action").str());

	auto alias = builder.getRewriter().create<mlir::rlc::TypeAliasOp>(

			function.getLoc(), alternative.getName(), alternative, nullptr, nullptr);
	mlir::rlc::markSynthetic(alias);

	defineApplyFunctionAlternative(
			function, builder, alternative, applyFunctions);

	return mlir::success();
}

mlir::rlc::ActionFunction mlir::rlc::detail::typeCheckAction(
		mlir::rlc::ActionFunction fun, mlir::rlc::ValueTable *parentSymbolTable)
{
	bool emitClasses = fun->hasAttr("emit_classes");
	if (fun.getUnmangledName() == "init")
	{
		auto _ = logError(
				fun,
				"Actions cannot be called init, that is a reserved name, use a "
				"different one");
		return nullptr;
	}
	if (fun.getUnmangledName() == "drop")
	{
		auto _ = logError(
				fun,
				"Actions cannot be called drop, that is a reserved name, use a "
				"different one");
		return nullptr;
	}
	if (fun.getUnmangledName() == "assign")
	{
		auto _ = logError(
				fun,
				"Actions cannot be called drop, that is a reserved name, use a "
				"different one");
		return nullptr;
	}
	mlir::rlc::ModuleBuilder builder(
			fun->getParentOfType<mlir::ModuleOp>(), parentSymbolTable);

	if (builder.typeOfAction(fun).cast<mlir::rlc::ClassType>().isInitialized())
		return fun;

	builder.getRewriter().setInsertionPointToStart(&fun.getBody().front());
	if (mlir::isa<mlir::rlc::UnderTypeCheckMarker>(fun.getBody().front().front()))
	{
		auto _ = logError(
				fun,
				"Found recursive call path involving Action Declaration. This is not "
				"allowed since would actions frame of infinite size");
		return nullptr;
	}

	builder.getRewriter().create<mlir::rlc::UnderTypeCheckMarker>(fun.getLoc());

	auto _ = builder.addSymbolTable();
	if (mlir::rlc::typeCheck(*fun.getOperation(), builder).failed())
		return nullptr;

	// remove the marker
	fun.getBody().front().front().erase();

	assignActionIndicies(fun);
	auto newF = deduceActionType(fun);
	if (not newF)
		return nullptr;

	if (emitClasses)
	{
		mlir::rlc::ModuleBuilder builder2(
				newF->getParentOfType<mlir::ModuleOp>(), parentSymbolTable);
		if (declareActionTypes(newF, builder2).failed())
			return nullptr;
	}

	if (newF.getActions().empty())
	{
		auto _ = logError(
				newF, "Actions cannot have 0 actions, turn it into a function instead");
		return nullptr;
	}

	return newF;
}

static llvm::SmallVector<mlir::Operation *, 4> ops(mlir::Region &region)
{
	llvm::SmallVector<mlir::Operation *, 4> toReturn;
	for (auto &op : region.getOps())
		toReturn.push_back(&op);

	return toReturn;
}

::mlir::LogicalResult mlir::rlc::CallOp::verifySymbolUses(
		::mlir::SymbolTableCollection &symbolTable)
{
	return LogicalResult::success();
}

mlir::MutableOperandRange
mlir::rlc::ExplicitConstructOp::getArgOperandsMutable()
{
	return mlir::MutableOperandRange(*this, 0, 0);
}

/// Get the argument operands to the called function.
mlir::OperandRange mlir::rlc::ExplicitConstructOp::getArgOperands()
{
	return getArgOperandsMutable();
}

mlir::CallInterfaceCallable
mlir::rlc::ExplicitConstructOp::getCallableForCallee()
{
	return getInitializer();
}

void mlir::rlc::ExplicitConstructOp::setCalleeFromCallable(
		mlir::CallInterfaceCallable callee)
{
	(*this)->setOperand(0, callee.get<mlir::Value>());
}

::mlir::LogicalResult mlir::rlc::ArrayCallOp::verifySymbolUses(
		::mlir::SymbolTableCollection &symbolTable)
{
	return LogicalResult::success();
}

mlir::LogicalResult mlir::rlc::ContinueStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (not getOperation()->getParentOfType<mlir::rlc::WhileStatement>() and
			not getOperation()->getParentOfType<mlir::rlc::ForLoopStatement>())
	{
		return mlir::rlc::logError(
				*this,
				"Continue statement cannot be used outside of a while or for "
				"statement");
	}
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::BreakStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (not getOperation()->getParentOfType<mlir::rlc::WhileStatement>() and
			not getOperation()->getParentOfType<mlir::rlc::ForLoopStatement>())
	{
		return mlir::rlc::logError(
				*this,
				"Break statement cannot be used outside of a while or for statement");
	}
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::TypeAliasOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getConverter().setErrorLocation(getLoc());
	auto deducedType = builder.getConverter().convertType(getAliased());
	if (not deducedType)
	{
		return mlir::rlc::logError(
				*this, "right hand of using is not a valid type");
	}
	if (getShugarizedType() != nullptr)
	{
		auto shugarized =
				builder.getConverter().shugarizedConvertType(getAliased());
		assert(shugarized != nullptr);
		this->setShugarizedTypeAttr(getShugarizedType()->replaceType(shugarized));
	}

	if (auto casted = deducedType.dyn_cast<mlir::rlc::AlternativeType>())
	{
		deducedType = mlir::rlc::AlternativeType::get(
				casted.getContext(), casted.getUnderlying(), getName());
	}

	builder.getConverter().registerType(
			getName(), mlir::rlc::AliasType::get(getName(), deducedType));
	this->setAliased(deducedType);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::IsOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UncheckedCanOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	for (auto *child : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*child, builder).failed())
			return mlir::failure();
	}

	if (getBody().front().empty())
	{
		return mlir::rlc::logError(
				*this, "can expressions can only refer to a function call");
	}
	auto call = mlir::dyn_cast<mlir::rlc::CallOp>(getBody().front().back());
	if (call == nullptr)
	{
		return mlir::rlc::logError(
				*this, "can expressions can only refer to a function call");
	}

	while (not getBody().front().empty())
		getBody().front().front().moveBefore(*this);

	builder.getRewriter().setInsertionPoint(call);
	auto properCanOp = builder.getRewriter().create<mlir::rlc::CanOp>(
			getLoc(), call.getCallee());
	auto newCall = builder.getRewriter().create<mlir::rlc::CallOp>(
			call.getLoc(), properCanOp, call.getIsMemberCall(), call.getArgs());
	if (call.getNumResults() == 1)
	{
		call.replaceAllUsesWith(newCall.getResults());
	}
	call.erase();
	replaceAllUsesWith(newCall.getResults()[0]);
	erase();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::AssertOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (!getAssertion().getType().isa<mlir::rlc::BoolType>())
	{
		return logError(
				getOperation(), "assert must have a expression of type bool");
	}
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::Comment::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::Import::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::BracketsOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	getResult().setType(getInput().getType());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UnderTypeCheckMarker::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

size_t mlir::rlc::SubActionStatement::forwardedArgsCount()
{
	return mlir::dyn_cast<mlir::rlc::Yield>(getForwardedArgs().front().back())
			.getArguments()
			.size();
}

namespace mlir::rlc
{

	static mlir::Value make_frame_access(
			mlir::rlc::SubActionStatement statement,
			mlir::Value frameInYield,
			mlir::IRRewriter &rewiter,
			mlir::Type underlyingType,
			mlir::Type alternativeEntry = nullptr)
	{
		mlir::IRMapping mapping;
		for (auto &op : llvm::drop_end(statement.getBody().front().getOperations()))
			rewiter.clone(op, mapping);

		mlir::Value frame = mapping.getValueMap().contains(frameInYield)
														? mapping.getValueMap().at(frameInYield)
														: frameInYield;

		auto cast = rewiter.create<mlir::rlc::StorageCast>(
				statement.getLoc(), underlyingType, frame);
		if (!alternativeEntry)
			return cast;

		return rewiter.create<mlir::rlc::ValueUpcastOp>(
				statement.getLoc(), alternativeEntry, cast);
	};

	static mlir::LogicalResult emitSubActionsInvocations(
			mlir::rlc::ModuleBuilder &builder,
			mlir::IRRewriter &rewiter,
			mlir::rlc::ActionFunction parent,
			mlir::ValueRange frames,
			mlir::rlc::SubActionStatement statement,
			mlir::Location loc,
			mlir::Type activeAlternativeType)
	{
		size_t countTotalActions = 0;
		llvm::SmallVector<mlir::rlc::ActionFunction, 2> actionFunctions;
		for (auto frame : frames)
		{
			mlir::Type underlying = decayCtxFrmType(frame.getType());
			auto actionFunction =
					builder
							.getActionOf(
									activeAlternativeType ? activeAlternativeType : underlying)
							.getDefiningOp<mlir::rlc::ActionFunction>();
			actionFunctions.push_back(actionFunction);
			countTotalActions += actionFunction.getActions().size();
		}

		auto actions =
				rewiter.create<mlir::rlc::ActionsStatement>(loc, countTotalActions);
		size_t currentActionSlot = 0;
		for (auto [frameIndex, frame] : llvm::enumerate(frames))
		{
			mlir::Type underlying = decayCtxFrmType(frame.getType());
			auto actionFunction = actionFunctions[frameIndex];
			llvm::SmallVector<mlir::Value, 2> actionValues =
					actionFunction.getActions();

			for (size_t i = 0; i < actionValues.size(); i++)
			{
				auto *bb = rewiter.createBlock(
						&actions.getActions()[currentActionSlot],
						actions.getActions()[currentActionSlot].begin());
				currentActionSlot++;
				rewiter.setInsertionPoint(bb, bb->begin());
				mlir::Value toCall = actionValues[i];
				mlir::rlc::ActionStatement referred =
						mlir::cast<mlir::rlc::ActionStatement>(
								*builder.actionFunctionValueToActionStatement(toCall)[0]);

				llvm::SmallVector<mlir::Type, 4> resultTypes;
				llvm::SmallVector<mlir::Location, 4> resultLoc;
				llvm::SmallVector<mlir::rlc::FunctionArgumentAttr> nameAttrs;

				for (auto arg :
						 llvm::zip(parent.getArgumentTypes(), parent.getArgNames()))
				{
					auto type = std::get<0>(arg);
					if (type.isa<mlir::rlc::ContextType>())
					{
						resultTypes.push_back(type);
						nameAttrs.push_back(
								mlir::rlc::FunctionArgumentAttr::get(
										type.getContext(), std::get<1>(arg), nullptr, nullptr));
						resultLoc.push_back(parent.getLoc());
					}
				}
				size_t contextArgsCount = resultTypes.size();
				if (statement.forwardedArgsCount() > referred.getResultTypes().size())
				{
					return mlir::rlc::logError(
							statement.getOperation(),
							(llvm::Twine("subaction statements is trying to forward ") +
							 llvm::Twine(statement.forwardedArgsCount()) +
							 llvm::Twine(" arguments, but the invoked action only accepts ") +
							 llvm::Twine(referred.getResultTypes().size()))
									.str());
				}

				for (auto type : llvm::drop_begin(
								 referred.getResultTypes(), statement.forwardedArgsCount()))
				{
					if (auto casted = type.dyn_cast<mlir::rlc::FrameType>())
						resultTypes.push_back(casted.getUnderlying());
					else
						resultTypes.push_back(type);
					resultLoc.push_back(actions.getLoc());
				}

				for (auto arg : llvm::drop_begin(
								 referred.getInfo().getArguments(),
								 statement.forwardedArgsCount()))
				{
					nameAttrs.push_back(arg);
				}

				auto fixed = rewiter.create<mlir::rlc::ActionStatement>(
						referred.getLoc(),
						resultTypes,
						referred.getName(),
						mlir::rlc::FunctionInfoAttr::get(referred.getContext(), nameAttrs),
						referred.getId(),
						referred.getResumptionPoint());

				auto *newBody = rewiter.createBlock(
						&fixed.getPrecondition(),
						fixed.getPrecondition().begin(),
						resultTypes,
						resultLoc);
				mlir::IRMapping mapping;
				for (auto &op : statement.getForwardedArgs().front())
					rewiter.clone(op, mapping);
				auto terminator =
						mlir::dyn_cast<mlir::rlc::Yield>(mapping.getOperationMap().at(
								statement.getForwardedArgs().front().getTerminator()));

				llvm::SmallVector<mlir::Value, 4> canArgs(
						terminator.getArguments().begin(), terminator.getArguments().end());
				for (auto [forwardedArg, toCallArg] :
						 llvm::zip_first(canArgs, referred.getResultTypes()))
				{
					if (forwardedArg.getType() != mlir::rlc::decayCtxFrmType(toCallArg))
					{
						return logError(
								statement.getOperation(),
								"forwarded arg type missmatch, expected: " +
										prettyType(toCallArg) + " got " +
										prettyType(decayCtxFrmType(forwardedArg.getType())));
					}
				}
				terminator.erase();

				for (auto arg :
						 llvm::drop_begin(newBody->getArguments(), contextArgsCount))
					canArgs.push_back(arg);

				canArgs.insert(
						canArgs.begin(),
						make_frame_access(
								statement, frame, rewiter, underlying, activeAlternativeType));

				auto casted =
						rewiter.create<mlir::rlc::CanOp>(actions.getLoc(), toCall);
				auto result = rewiter.create<mlir::rlc::CallOp>(
						actions.getLoc(), casted, false, canArgs);
				rewiter.create<mlir::rlc::Yield>(
						actions.getLoc(), mlir::ValueRange({ result.getResult(0) }));
				rewiter.setInsertionPointAfter(fixed);
				auto expression =
						rewiter.create<mlir::rlc::ExpressionStatement>(fixed.getLoc());

				// yield of the action statements
				rewiter.create<mlir::rlc::Yield>(actions.getLoc());

				rewiter.createBlock(&expression.getBody());

				mlir::IRMapping mapping2;
				for (auto &op : statement.getForwardedArgs().front())
					rewiter.clone(op, mapping2);
				terminator =
						mlir::dyn_cast<mlir::rlc::Yield>(mapping2.getOperationMap().at(
								statement.getForwardedArgs().front().getTerminator()));

				llvm::SmallVector<mlir::Value, 4> args(
						terminator.getArguments().begin(), terminator.getArguments().end());
				terminator.erase();
				for (auto result :
						 llvm::drop_begin(fixed.getResults(), contextArgsCount))
					args.push_back(result);

				args.insert(
						args.begin(),
						make_frame_access(
								statement, frame, rewiter, underlying, activeAlternativeType));

				rewiter.create<mlir::rlc::CallOp>(
						actions.getLoc(), toCall, false, args);

				// yield for the expression statement
				rewiter.create<mlir::rlc::Yield>(actions.getLoc());
			}
			rewiter.setInsertionPointAfter(actions);
		}
		return mlir::success();
	}

	static mlir::LogicalResult emitSubActionsInvocations(
			mlir::rlc::ModuleBuilder &builder,
			mlir::IRRewriter &rewiter,
			mlir::rlc::ActionFunction parent,
			mlir::ValueRange frames,
			mlir::rlc::SubActionStatement statement,
			mlir::Location loc)
	{
		auto frameTypes = frames.getTypes();
		auto alterantiveFrames =
				mlir::dyn_cast<mlir::rlc::AlternativeType>(frameTypes[0]);
		// if the argument is directly the coroutine frames, just emit
		// the actions for those
		if (not alterantiveFrames or frameTypes.size() != 1)
			return emitSubActionsInvocations(
					builder,
					rewiter,
					parent,
					frames,
					statement,
					statement.getLoc(),
					nullptr);
		// otherwise emit the switch that depending on
		// the current active alternative, resolves that entry of the alternative
		for (auto alternative : alterantiveFrames.getUnderlying())
		{
			auto ifStmt = rewiter.create<mlir::rlc::IfStatement>(statement.getLoc());
			rewiter.createBlock(&ifStmt.getCondition());
			auto isOp = rewiter.create<mlir::rlc::IsOp>(
					statement.getLoc(),
					make_frame_access(
							statement, frames[0], rewiter, decayCtxFrmType(frameTypes[0])),
					alternative);

			rewiter.create<mlir::rlc::Yield>(
					statement.getLoc(), mlir::ValueRange({ isOp }));
			auto *bb1 = rewiter.createBlock(&ifStmt.getTrueBranch());
			rewiter.create<mlir::rlc::Yield>(statement.getLoc());
			rewiter.setInsertionPointToStart(bb1);
			if (emitSubActionsInvocations(
							builder,
							rewiter,
							parent,
							frames,
							statement,
							statement.getLoc(),
							alternative)
							.failed())
				return mlir::failure();
			auto *bb = rewiter.createBlock(&ifStmt.getElseBranch());
			rewiter.create<mlir::rlc::Yield>(statement.getLoc());
			rewiter.setInsertionPointToStart(bb);
		}
		return mlir::success();
	}

	static mlir::Value emitVarDecl(
			mlir::IRRewriter &rewiter,
			mlir::rlc::ModuleBuilder &builder,
			mlir::rlc::SubActionStatement statement,
			mlir::Type underlyingType)
	{
		if (statement.getName().empty())
			return nullptr;
		rewiter.setInsertionPoint(statement);
		auto varDecl = rewiter.create<mlir::rlc::DeclarationStatement>(
				statement.getLoc(),
				mlir::rlc::FrameType::get(underlyingType),
				statement.getName());
		markSynthetic(varDecl.getOperation());
		varDecl.getBody().takeBody(statement.getBody());
		builder.getSymbolTable().add(statement.getName(), varDecl);
		rewiter.createBlock(&statement.getBody());
		rewiter.create<mlir::rlc::Yield>(
				statement.getLoc(), mlir::ValueRange({ varDecl }));
		return varDecl.getResult();
	}
}	 // namespace mlir::rlc

mlir::LogicalResult mlir::rlc::SubActionStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto parent = getOperation()->getParentOfType<mlir::rlc::ActionFunction>();
	if (not parent)
		return mlir::rlc::logError(
				*this, "SubAction statements can only appear in Action Functions");
	for (auto *child : ops(getForwardedArgs()))
	{
		if (mlir::rlc::typeCheck(*child, builder).failed())
			return mlir::failure();
	}

	for (auto *child : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*child, builder).failed())
			return mlir::failure();
	}

	mlir::rlc::Yield yield =
			mlir::cast<mlir::rlc::Yield>(getBody().front().getTerminator());
	for (auto frameVar : yield.getArguments())
	{
		if ((not frameVar.getType().isa<mlir::rlc::ClassType>() or
				 not builder.isClassOfAction(frameVar.getType())) and
				not frameVar.getType().isa<mlir::rlc::AlternativeType>())
		{
			return logError(
					*this,
					"Subaction statement must refer to a action, not a " +
							prettyType(frameVar.getType()));
		}
	}

	mlir::IRRewriter &rewiter = builder.getRewriter();

	// emit the var decl if the subaction statement is in the form
	// subaction name = exp
	auto varDecl = emitVarDecl(
			rewiter, builder, *this, yield.getArguments().front().getType());
	// reset the yield because emitting the var decl may have changed it
	yield = mlir::cast<mlir::rlc::Yield>(getBody().front().getTerminator());

	rewiter.setInsertionPoint(*this);
	auto subActionInfo = rewiter.create<mlir::rlc::SubActionInfo>(
			getLoc(),
			rewiter.getTypeArrayAttr(yield.getArguments().getTypes()),
			getRunOnce(),
			varDecl != nullptr);
	mlir::IRMapping mapping;
	getBody().cloneInto(&subActionInfo.getArguments(), mapping);
	getForwardedArgs().cloneInto(&subActionInfo.getForwardedArgs(), mapping);
	mlir::Block *expansionBlock = rewiter.createBlock(&subActionInfo.getBody());
	rewiter.setInsertionPointToEnd(expansionBlock);

	// if the subaction has *, emit a while loop that will execute
	// subactions until conclusion
	if (not getRunOnce())
	{
		auto loop = rewiter.create<mlir::rlc::WhileStatement>(getLoc());
		rewiter.createBlock(&loop.getCondition());

		mlir::Value result;
		for (mlir::Value frame : yield.getArguments())
		{
			auto isDone = rewiter.create<mlir::rlc::IsDoneOp>(

					getLoc(),
					make_frame_access(
							*this, frame, rewiter, decayCtxFrmType(frame.getType())));
			if (result == nullptr)
				result = isDone;

			result = rewiter.create<mlir::rlc::AndOp>(getLoc(), result, isDone);
		}

		auto isNotDone = rewiter.create<mlir::rlc::NotOp>(getLoc(), result);

		rewiter.create<mlir::rlc::Yield>(getLoc(), mlir::ValueRange({ isNotDone }));

		rewiter.createBlock(&loop.getBody());

		auto finalYield = rewiter.create<mlir::rlc::Yield>(getLoc());
		rewiter.setInsertionPoint(finalYield);
	}

	if (emitSubActionsInvocations(
					builder, rewiter, parent, yield.getArguments(), *this, getLoc())
					.failed())
		return mlir::failure();
	rewiter.setInsertionPointAfter(*this);
	erase();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::EnumUse::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ExpressionStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	return mlir::success();
}

static bool expressionIsReference(mlir::Value val)
{
	if (val.getDefiningOp<mlir::rlc::MemberAccess>())
		return true;

	if (val.getDefiningOp<mlir::rlc::ArrayAccess>())
		return true;

	if (val.getDefiningOp<mlir::rlc::DeclarationStatement>())
		return true;

	if (val.getDefiningOp<mlir::rlc::ActionStatement>())
		return true;

	// true if it is a function argument
	if (val.getDefiningOp() == nullptr)
		return true;

	if (auto casted = val.getDefiningOp<mlir::rlc::CallOp>())
	{
		auto resultsType =
				casted.getCallee().getType().cast<mlir::FunctionType>().getResults();
		if (resultsType.empty() or resultsType[0].isa<mlir::rlc::VoidType>())
			return false;

		return resultsType[0].isa<mlir::rlc::ReferenceType>();
	}
	return false;
}

bool mlir::rlc::DeclarationStatement::isReference()
{
	auto initializer = getBody().front().getTerminator()->getOperand(0);
	return expressionIsReference(initializer);
}

mlir::LogicalResult mlir::rlc::DeclarationStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	for (auto *child : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*child, builder).failed())
			return mlir::failure();
	}

	rewriter.setInsertionPoint(getOperation());
	auto deducedType = getBody().front().getTerminator()->getOperand(0).getType();

	if (getType().isa<mlir::rlc::ReferenceType>())
	{
		if (not isReference())
		{
			return mlir::rlc::logError(
					getOperation(),
					"Declaration statement is a reference but right hand side of "
					"assigment is not.");
		}
	}
	else
	{
		if (isReference())
		{
			auto yield =
					mlir::dyn_cast<mlir::rlc::Yield>(getBody().front().getTerminator());
			rewriter.setInsertionPoint(yield);
			auto construct = rewriter.create<mlir::rlc::ConstructOp>(
					getLoc(), yield.getArguments()[0].getType());
			rewriter.create<mlir::rlc::ImplicitAssignOp>(
					getLoc(), construct, yield.getArguments()[0]);
			yield->setOperand(0, construct);
			rewriter.setInsertionPoint(*this);
		}

		if (getType().isa<mlir::rlc::FrameType>())
		{
			if (getOperation()->getParentOfType<mlir::rlc::FunctionOp>())
			{
				return mlir::rlc::logError(
						getOperation(),
						"Frame variables are only allowed in action functions.");
			}
			deducedType = mlir::rlc::FrameType::get(deducedType);
		}
	}

	auto newOne = rewriter.create<mlir::rlc::DeclarationStatement>(
			getLoc(), deducedType, getSymName());
	newOne.getBody().takeBody(getBody());
	rewriter.replaceOp(*this, newOne);

	builder.getSymbolTable().add(newOne.getSymName(), newOne);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ArrayAccess::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (not getValue().getType().isa<mlir::rlc::ArrayType>() and
			not getValue().getType().isa<mlir::rlc::OwningPtrType>() and
			not getValue().getType().isa<mlir::rlc::StringLiteralType>())
	{
		auto newCall =
				builder.emitCall(*this, true, "get", { getValue(), getMemberIndex() });
		if (!newCall)
			return mlir::failure();
		replaceAllUsesWith(newCall);
		newCall->setAttr(
				"array_access_syntax", builder.getRewriter().getUnitAttr());
		erase();
		return mlir::success();
	}

	if (not getMemberIndex().getType().isa<mlir::rlc::IntegerType>())
	{
		return mlir::rlc::logError(
				getOperation(), "Array access must have a integer index operand.");
	}

	builder.getRewriter().replaceOpWithNewOp<mlir::rlc::ArrayAccess>(
			*this, getValue(), getMemberIndex());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::Yield::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

static bool isReturnTypeCompatible(
		mlir::Type returnValue, mlir::Type functionReturnType)
{
	if (auto casted = functionReturnType.dyn_cast<mlir::rlc::ReferenceType>())
		return casted.getUnderlying() == returnValue;

	return returnValue == functionReturnType;
}

mlir::LogicalResult mlir::rlc::ReturnStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	for (auto *op : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();
	}
	rewriter.setInsertionPoint(*this);
	auto yield =
			mlir::dyn_cast<mlir::rlc::Yield>(getBody().front().getTerminator());

	assert(yield);
	const bool returnsValue = yield->getNumOperands() != 0;

	auto returnedType = returnsValue ? yield->getOpOperand(0).get().getType()
																	 : mlir::rlc::VoidType::get(getContext());

	setResult(returnedType);

	if (getOperation()->getBlock()->getTerminator() != getOperation())
	{
		return mlir::rlc::logError(
				*this,
				"Return statement should be the last statement of its code block.");
	}

	if (auto parentFunction =
					getOperation()->getParentOfType<mlir::rlc::FunctionOp>())
	{
		mlir::Type returnType =
				(parentFunction.getType().getNumResults() != 0
						 ? parentFunction.getResultTypes()[0]
						 : mlir::rlc::VoidType::get(getContext()));

		if (not isReturnTypeCompatible(getResult(), returnType))
		{
			auto _ = mlir::rlc::logError(
					*this,
					"Return statement returns values incompatible with the function "
					"signature");
			_ = mlir::rlc::logRemark(
					*this, "Return value type is " + prettyType(getResult()));

			return mlir::rlc::logRemark(
					parentFunction,
					"Function return type is " +
							prettyType(parentFunction.getResultTypes()[0]));
		}

		// if we are returning something, and the thing we are returning is a
		// reference, and our parent function returns a non reference type, then
		// make a copy of the returned value
		if (not yield.getArguments().empty() and
				expressionIsReference(yield.getArguments()[0]) and
				not returnType.isa<mlir::rlc::ReferenceType>())
		{
			rewriter.setInsertionPoint(yield);

			auto construct = rewriter.create<mlir::rlc::ConstructOp>(
					yield.getLoc(), yield.getArguments()[0].getType());
			rewriter.create<mlir::rlc::ImplicitAssignOp>(
					yield.getLoc(), construct, yield.getArguments()[0]);
			yield->setOperand(0, construct);
		}
	}

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UnresolvedReference::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	bool usedByCall = llvm::any_of(
			getResult().getUsers(), [&](const mlir::OpOperand &operand) -> bool {
				auto call = mlir::dyn_cast<mlir::rlc::CallOp>(*operand.getOwner());
				return call and call.getCallee() == getResult();
			});
	if (usedByCall)
	{
		assert(llvm::count_if(getResult().getUsers(), [](const auto &) {
						 return true;
					 }) == 1);

		return mlir::success();
	}
	auto candidates = builder.getSymbolTable().get(getName());

	if (candidates.empty())
		return logError(*this, "No known value " + getName());

	if (auto casted =
					candidates.front().getType().dyn_cast<mlir::rlc::FrameType>())
	{
		auto newValue = builder.getRewriter().create<mlir::rlc::StorageCast>(
				getLoc(), casted.getUnderlying(), candidates.front());
		replaceAllUsesWith(newValue.getResult());
	}
	else if (
			auto casted =
					candidates.front().getType().dyn_cast<mlir::rlc::ContextType>())
	{
		auto newValue = builder.getRewriter().create<mlir::rlc::StorageCast>(
				getLoc(), casted.getUnderlying(), candidates.front());
		replaceAllUsesWith(newValue.getResult());
	}
	else
	{
		replaceAllUsesWith(candidates.front());
	}

	builder.getRewriter().eraseOp(*this);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::Constant::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ValueUpcastOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UncheckedIsOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	builder.getConverter().setErrorLocation(getLoc());
	auto deducedType = builder.getConverter().convertType(getTypeOrTrait());
	auto shugarizedType =
			builder.getConverter().shugarizedConvertType(getTypeOrTrait());
	if (deducedType == nullptr)
	{
		return mlir::rlc::logRemark(*this, "In Is expression");
	}

	rewriter.setInsertionPoint(getOperation());
	rewriter.replaceOpWithNewOp<mlir::rlc::IsOp>(
			*this,
			getExpression(),
			deducedType,
			getShugarizedType().replaceType(shugarizedType));

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UsingTypeOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	{
		auto _ = builder.addSymbolTable();

		for (auto *op : ops(getBody()))
			if (mlir::rlc::typeCheck(*op, builder).failed())
				return mlir::failure();
	}

	auto underlyingType =
			mlir::dyn_cast<mlir::rlc::Yield>(getBody().back().getTerminator())
					.getArguments()[0]
					.getType();
	builder.getConverter().registerType(
			getName(), mlir::rlc::AliasType::get(getName(), underlyingType));

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::StatementList::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto _ = builder.addSymbolTable();

	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::UncheckedTraitDefinition::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	auto _ = builder.addSymbolTable();
	for (auto t : getTemplateParameterTypes())
		builder.getConverter().registerType(t.getName(), t);

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> Ops(
			getBody().getOps<mlir::rlc::FunctionOp>());
	for (auto op : Ops)
		if (op.typeCheckFunctionDeclaration(rewriter, builder.getConverter())
						.failed())
			return mlir::failure();

	llvm::SmallVector<mlir::StringAttr> names;
	llvm::SmallVector<mlir::FunctionType> types;
	llvm::SmallVector<mlir::ArrayAttr> argNames;

	for (auto op : getBody().getOps<mlir::rlc::FunctionOp>())
	{
		names.push_back(op.getUnmangledNameAttr());
		types.push_back(op.getFunctionType());
		argNames.push_back(rewriter.getStrArrayAttr(op.getArgNames()));
	}

	llvm::SmallVector<mlir::Type> typeNames;
	for (auto t : getTemplateParameters())
	{
		typeNames.push_back(
				TemplateParameterType::get(
						getContext(),
						("TraitType" + t.cast<mlir::StringAttr>().strref()).str(),
						nullptr,
						false));
	}

	auto type = mlir::rlc::TraitMetaType::get(
			getContext(), getName(), typeNames, types, names, argNames);
	rewriter.setInsertionPointAfter(*this);
	auto op = rewriter.create<mlir::rlc::TraitDefinition>(getLoc(), type);

	// replace the template parameters provided by the user with one prefixed
	// with TraitType so that it does not clashes with regular names
	for (auto templateParameter : getTemplateParameterTypes())
	{
		mlir::AttrTypeReplacer replacer;
		auto renamedTemplateParameter = mlir::rlc::TemplateParameterType::get(
				op.getContext(),
				("TraitType" + templateParameter.getName()).str(),
				templateParameter.getTrait(),
				templateParameter.getIsIntLiteral());
		replacer.addReplacement(
				[templateParameter,
				 renamedTemplateParameter](mlir::Type t) -> mlir::Type {
					if (t == templateParameter)
						return renamedTemplateParameter;
					return t;
				});
		replacer.replaceElementsIn(op, true, false, true);
	}
	rewriter.eraseOp(*this);
	return mlir::success();
}

static void promoteArgumentOfIsOp(
		mlir::rlc::ModuleBuilder &builder, mlir::rlc::IfStatement op)
{
	auto &rewriter = builder.getRewriter();
	auto &table = builder.getSymbolTable();
	auto isOp = op.getCondition()
									.front()
									.getTerminator()
									->getOperand(0)
									.getDefiningOp<mlir::rlc::IsOp>();
	if (not isOp)
		return;

	rewriter.setInsertionPointToStart(&op.getTrueBranch().front());
	if (auto trait = isOp.getTypeOrTrait().dyn_cast<mlir::rlc::TraitMetaType>())
	{
		for (size_t index : ::rlc::irange(trait.getRequestedFunctionTypes().size()))
		{
			auto methodType =
					trait.getRequestedFunctionTypes()[index].cast<mlir::FunctionType>();
			auto instantiated = replaceTemplateParameter(
					methodType,
					mlir::cast<mlir::rlc::TemplateParameterType>(
							trait.getTemplateParameters().back()),
					isOp.getExpression().getType());
			auto upcastedValue = rewriter.create<mlir::rlc::TemplateInstantiationOp>(
					isOp.getLoc(),
					instantiated,
					builder.getTraitDefinition(trait).getResults()[index]);

			llvm::StringRef methodName = trait.getRequestedFunctionNames()[index];
			table.add(methodName, upcastedValue);
		}
		return;
	}

	mlir::Value isOperand = isOp.getExpression();
	llvm::StringRef name = table.lookUpValue(isOperand);
	if (auto casted = mlir::dyn_cast_or_null<mlir::rlc::StorageCast>(
					isOperand.getDefiningOp()))
	{
		name = table.lookUpValue(casted.getOperand());
		mlir::IRMapping mapping;
		isOperand =
				rewriter.clone(*isOperand.getDefiningOp(), mapping)->getResult(0);
	}

	if (name.empty())
		return;

	auto upcastedValue = rewriter.create<mlir::rlc::ValueUpcastOp>(
			isOp.getLoc(), isOp.getTypeOrTrait(), isOperand);
	table.add(name, upcastedValue);
}

mlir::LogicalResult mlir::rlc::StorageCast::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::TemplateInstantiationOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ShortCircuitingOr::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	for (auto *op : ops(getLhs()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	auto yield =
			mlir::cast<mlir::rlc::Yield>(getLhs().getBlocks().back().getTerminator());

	if (yield.getArguments().size() != 1 or
			!yield.getArguments().front().getType().isa<mlir::rlc::BoolType>())
	{
		auto op = yield.getArguments().front().getDefiningOp();
		return logError(
				op ? op : getOperation(), "Left hand operand of or must be Bool");
	}

	for (auto *op : ops(getRhs()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	yield =
			mlir::cast<mlir::rlc::Yield>(getRhs().getBlocks().back().getTerminator());

	if (yield.getArguments().size() != 1 or
			!yield.getArguments().front().getType().isa<mlir::rlc::BoolType>())
	{
		auto op = yield.getArguments().front().getDefiningOp();
		return logError(
				op ? op : getOperation(), "Right hand operand of or must be Bool");
	}

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ShortCircuitingAnd::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	for (auto *op : ops(getLhs()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	auto yield =
			mlir::cast<mlir::rlc::Yield>(getLhs().getBlocks().back().getTerminator());

	if (yield.getArguments().size() != 1 or
			!yield.getArguments().front().getType().isa<mlir::rlc::BoolType>())
	{
		auto op = yield.getArguments().front().getDefiningOp();
		return logError(
				op ? op : getOperation(), "Left hand operand of and must be Bool");
	}

	for (auto *op : ops(getRhs()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	yield =
			mlir::cast<mlir::rlc::Yield>(getRhs().getBlocks().back().getTerminator());

	if (yield.getArguments().size() != 1 or
			!yield.getArguments().front().getType().isa<mlir::rlc::BoolType>())
	{
		auto op = yield.getArguments().front().getDefiningOp();
		return logError(
				op ? op : getOperation(), "Right hand operand of and must be Bool");
	}

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::IfStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	for (auto *op : ops(getCondition()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	if (not getCondition()
							.front()
							.getTerminator()
							->getOperand(0)
							.getType()
							.isa<mlir::rlc::BoolType>())
	{
		return logError(*this, "While loop condition type must be Bool");
		return mlir::failure();
	}

	{
		auto _ = builder.addSymbolTable();
		promoteArgumentOfIsOp(builder, *this);
		for (auto *op : ops(getTrueBranch()))
			if (mlir::rlc::typeCheck(*op, builder).failed())
				return mlir::failure();
	}

	{
		auto _ = builder.addSymbolTable();
		for (auto *op : ops(getElseBranch()))
			if (mlir::rlc::typeCheck(*op, builder).failed())
				return mlir::failure();
	}

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ArrayCallOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	assert(
			(getNumResults() == 0 or
			 not getResult(0).getType().isa<mlir::rlc::UnknownType>()) and
			"unuspported, array calls should be only emitted with a correct type "
			"already");
	return mlir::success();
}

static mlir::LogicalResult maybeInstantiateAction(
		mlir::rlc::ModuleBuilder &builder, mlir::Value maybeActionClass)
{
	auto casted = dyn_cast<mlir::rlc::ClassType>(maybeActionClass.getType());
	if (not casted or not builder.isClassOfAction(casted))
		return mlir::success();

	auto actionFunction =
			builder.getActionOf(casted).getDefiningOp<mlir::rlc::ActionFunction>();

	builder.removeActionFromRootTable(actionFunction);
	builder.removeAction(actionFunction);
	auto newF = mlir::rlc::detail::typeCheckAction(
			actionFunction, &builder.getRootTable());

	if (newF)
	{
		builder.addActionToRootTable(newF);
		builder.registerAction(newF);
		return mlir::success();
	}
	return mlir::failure();
}

mlir::LogicalResult mlir::rlc::CallOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();

	mlir::Operation *newCall = nullptr;
	auto unresolvedCallee =
			getCallee().getDefiningOp<mlir::rlc::UnresolvedReference>();
	if (unresolvedCallee)
	{
		rewriter.setInsertionPoint(getOperation());
		newCall = builder.emitCall(
				unresolvedCallee,
				getIsMemberCall(),
				unresolvedCallee.getName(),
				getArgs());
		if (newCall == nullptr)
			return mlir::failure();
	}
	else
	{
		if (not getCallee().getType().isa<mlir::FunctionType>())
		{
			return logError(*this, "Cannot call non function type");
		}

		newCall = rewriter.create<mlir::rlc::CallOp>(
				getLoc(), getCallee(), getIsMemberCall(), getArgs());
	}
	if (getOperation()->hasAttr("post_fix_call"))
		newCall->setAttr("post_fix_call", rewriter.getUnitAttr());

	if (newCall->getNumResults() != 0)
	{
		rewriter.replaceOp(*this, newCall->getResults());
		if (unresolvedCallee)
			rewriter.eraseOp(unresolvedCallee);

		return maybeInstantiateAction(builder, newCall->getResults()[0]);
	}

	if (newCall->getNumResults() == 0 and not getResults().empty() and
			not getResults().front().getUsers().empty())
	{
		return logError(
				*this, "Call of void returning function cannot be used as expression");
	}

	rewriter.eraseOp(*this);
	if (unresolvedCallee)
		rewriter.eraseOp(unresolvedCallee);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::CanOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	rewriter.replaceOpWithNewOp<CanOp>(*this, getCallee());
	return mlir::success();
}

bool mlir::rlc::ForFieldStatement::hasFieldNameVariable()
{
	auto yield = mlir::cast<mlir::rlc::Yield>(
			getCondition().getBlocks().back().getTerminator());
	return yield.getArguments().size() + 1 == getNames().size();
}

mlir::LogicalResult mlir::rlc::ForFieldStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	for (auto *op : ops(getCondition()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	auto yield = mlir::cast<mlir::rlc::Yield>(
			getCondition().getBlocks().back().getTerminator());
	if (yield.getArguments().size() != getNames().size() and
			not hasFieldNameVariable())
	{
		return logError(
				*this,
				"Missmatched count between for induction variables and for "
				"arguments. The number of induction varaibles must be exactly the "
				"same "
				"as the number of expressions, or one more to capture the name of "
				"the "
				"field");
	}

	for (auto argument : yield.getArguments())
	{
		if (argument.getType() != yield.getArguments()[0].getType())
		{
			return logError(
					*this,
					"for field statement does not support expressions with "
					"different types");
		}
	}

	auto _ = builder.addSymbolTable();
	if (hasFieldNameVariable())
	{
		auto fieldName = getNames().begin()->cast<mlir::StringAttr>();
		getBody().front().getArguments()[0].setType(
				mlir::rlc::StringLiteralType::get(getContext()));
		builder.getSymbolTable().add(fieldName, getBody().getArgument(0));
	}
	for (auto [argument, name] :
			 llvm::zip(getNonFieldNameArgs(), getNonFieldNameNames()))
		builder.getSymbolTable().add(name.cast<mlir::StringAttr>(), argument);

	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ActionsStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (getOperation()->getParentOfType<mlir::rlc::FunctionOp>())
		return mlir::rlc::logError(
				*this, "Actions statements can only appear in Action Functions");
	auto &rewriter = builder.getRewriter();

	for (auto &region : getActions())
	{
		auto _ = builder.addSymbolTable();
		for (auto *op : ops(region))
			if (mlir::rlc::typeCheck(*op, builder).failed())
				return mlir::failure();
	}

	if (getActions().empty())
	{
		return logError(
				*this, "Actions statement must have at least 1 sub action statement");
	}

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ForLoopStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	bool foundOne = false;
	walk([&](ActionStatement statement) { foundOne = true; });
	if (foundOne)
	{
		return logError(
				*this,
				"At the moment actions inside for loops are not allowed, rewrite it as "
				"a while loop. In the future they will be supported");
	}

	for (auto *op : ops(getRangeExpression()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	auto &rewriter = builder.getRewriter();

	mlir::rlc::OverloadResolver resolver(builder.getSymbolTable(), nullptr);

	auto yield = mlir::cast<mlir::rlc::Yield>(
			getRangeExpression().front().getTerminator());
	auto inductionVarType = yield.getArguments()[0].getType();

	auto maybeSizeFunction =
			resolver.findOverload(getLoc(), true, "size", { inductionVarType });
	if (!maybeSizeFunction or
			maybeSizeFunction.getType().cast<mlir::FunctionType>().getResult(0) !=
					mlir::rlc::IntegerType::getInt64(getContext()))
	{
		return logError(
				*this,
				"For loop argument expression of type " + prettyType(inductionVarType) +
						" has no method size() -> Int");
	}

	auto maybeGetFunction = resolver.findOverload(
			getLoc(),
			true,
			"get",
			{ inductionVarType, mlir::rlc::IntegerType::getInt64(getContext()) });
	if (!maybeGetFunction or
			maybeGetFunction.getType().cast<mlir::FunctionType>().getResult(0) ==
					mlir::rlc::VoidType::get(getContext()))
	{
		return logError(
				*this,
				"For loop argument expression of type " + prettyType(inductionVarType) +
						" has no method get(Int index) that returns a non void type");
	}

	auto _ = builder.addSymbolTable();
	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ForLoopVarDeclOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getRewriter().setInsertionPointAfter(*this);
	mlir::rlc::OverloadResolver resolver(builder.getSymbolTable(), nullptr);
	auto parent = getOperation()->getParentOfType<mlir::rlc::ForLoopStatement>();
	if (not parent)
		return logError(*this, "for loop var decl found outside of for loop");

	auto yield = mlir::cast<mlir::rlc::Yield>(
			parent.getRangeExpression().front().getTerminator());
	auto inductionVarType = yield.getArguments()[0].getType();

	auto maybeGetFunction = resolver.instantiateOverload(
			builder.getRewriter(),
			true,
			getLoc(),
			"get",
			{ inductionVarType, mlir::rlc::IntegerType::getInt64(getContext()) });
	auto type =
			maybeGetFunction.getType().cast<mlir::FunctionType>().getResults()[0];
	if (auto casted = type.dyn_cast<mlir::rlc::ReferenceType>())
		type = casted.getUnderlying();

	getResult().setType(type);
	mlir::Value value = getResult();

	builder.getSymbolTable().add(getName(), value);
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::WhileStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto &rewriter = builder.getRewriter();
	for (auto *op : ops(getCondition()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	if (not getCondition()
							.front()
							.getTerminator()
							->getOperand(0)
							.getType()
							.isa<mlir::rlc::BoolType>())
	{
		return logError(*this, "While loop condition is not boolean");
	}

	auto _ = builder.addSymbolTable();
	for (auto *op : ops(getBody()))
		if (mlir::rlc::typeCheck(*op, builder).failed())
			return mlir::failure();

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ConstantGlobalOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getConverter().setErrorLocation(getLoc());
	auto deducedType = builder.getConverter().convertType(getType());
	auto shugarizedType = builder.getConverter().shugarizedConvertType(getType());
	if (deducedType == nullptr or shugarizedType == nullptr)
	{
		return mlir::failure();
	}

	getResult().setType(deducedType);
	setShugarizedTypeAttr(getShugarizedType()->replaceType(shugarizedType));
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ConstructOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getConverter().setErrorLocation(getLoc());
	auto deducedType = builder.getConverter().convertType(getType());
	auto shugarizedType = builder.getConverter().shugarizedConvertType(getType());
	if (deducedType == nullptr or shugarizedType == nullptr)
	{
		return mlir::failure();
	}

	getResult().setType(deducedType);
	setShugarizedTypeAttr(getShugarizedType()->replaceType(shugarizedType));
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::CastOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ActionStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto parent = getOperation()->getParentOfType<mlir::rlc::ActionFunction>();
	if (not parent)
		return mlir::rlc::logError(
				*this, "Action statements can only appear in Action Functions");

	auto &rewriter = builder.getRewriter();
	auto point = rewriter.saveInsertionPoint();

	if (getPrecondition().empty())
	{
		rewriter.createBlock(&getPrecondition());

		auto trueValue =
				builder.getRewriter().create<mlir::rlc::Constant>(getLoc(), true);
		builder.getRewriter().create<mlir::rlc::Yield>(
				getLoc(), mlir::ValueRange({ trueValue }));
	}
	rewriter.restoreInsertionPoint(point);

	llvm::SmallVector<mlir::Type, 4> newResultTypes;
	llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 4> argInfo;
	unsigned contextArgCounts = 0;
	for (auto arg : llvm::zip(parent.getArgumentTypes(), parent.getArgNames()))
	{
		auto type = std::get<0>(arg);
		if (type.isa<mlir::rlc::ContextType>())
		{
			getPrecondition().front().insertArgument(
					contextArgCounts++, type, getLoc());
			newResultTypes.push_back(type);
			argInfo.push_back(
					mlir::rlc::FunctionArgumentAttr::get(
							type.getContext(), std::get<1>(arg)));
		}
	}

	for (auto &operand : getPrecondition().front().getArguments())
	{
		builder.getConverter().setErrorLocation(operand.getLoc());
		auto converted = builder.getConverter().convertType(operand.getType());
		if (not converted)
		{
			return logRemark(*this, "in of argument of action statement");
		}
		operand.setType(converted);
	}

	for (auto result : getResults())
	{
		builder.getConverter().setErrorLocation(getLoc());
		auto deduced = builder.getConverter().convertType(result.getType());
		if (deduced == nullptr)
			return mlir::failure();
		result.setType(deduced);
	}

	for (auto result : getResults())
		newResultTypes.push_back(result.getType());

	for (auto arg : getInfo().getArguments())
	{
		auto deduced = builder.getConverter().shugarizedConvertType(
				arg.getShugarizedType().getType());
		argInfo.push_back(arg.replaceType(deduced));
	}

	auto newDecl = builder.getRewriter().create<mlir::rlc::ActionStatement>(
			getLoc(),
			newResultTypes,
			getName(),
			mlir::rlc::FunctionInfoAttr::get(getContext(), argInfo));

	newDecl.getPrecondition().takeBody(getPrecondition());

	llvm::SmallVector<mlir::Operation *, 4> ToCheck;
	for (auto &ops : newDecl.getPrecondition().getOps())
	{
		ToCheck.push_back(&ops);
	}

	{
		auto _ = builder.addSymbolTable();
		for (auto [name, res] : llvm::zip(
						 newDecl.getDeclaredNames(),
						 newDecl.getPrecondition().getArguments()))
			builder.getSymbolTable().add(name, res);

		for (auto *op : ToCheck)
		{
			if (mlir::rlc::typeCheck(*op, builder).failed())
				return mlir::failure();
		}
	}

	for (auto [name, res] :
			 llvm::zip(newDecl.getDeclaredNames(), newDecl.getResults()))
		builder.getSymbolTable().add(name.str(), res);

	erase();
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::AsByteArrayOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	int64_t size = 0;
	if (getLhs().getType().isa<mlir::rlc::BoolType>())
	{
		size = 1;
	}
	else if (getLhs().getType().isa<mlir::rlc::FloatType>())
	{
		size = 8;
	}
	else if (auto casted = getLhs().getType().dyn_cast<mlir::rlc::IntegerType>())
	{
		size = casted.getSize() / 8;
	}
	else
	{
		return logError(*this, "Input of to_byte_array must be a primitive type");
	}

	builder.getRewriter().replaceOpWithNewOp<mlir::rlc::AsByteArrayOp>(
			*this,
			mlir::rlc::ArrayType::get(
					getContext(), mlir::rlc::IntegerType::getInt8(getContext()), size),
			getLhs());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::FromByteArrayOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (not getLhs().getType().isa<mlir::rlc::ArrayType>())
	{
		return logError(
				*this, "Builtin from byte_array_argument must be a byte array");
		return mlir::failure();
	}
	auto castedInput = getLhs().getType().cast<mlir::rlc::ArrayType>();
	if (castedInput.getUnderlying() !=
			mlir::rlc::IntegerType::getInt8(getContext()))
	{
		return logError(
				*this, "builtin from_byte_array argument must be a byte array");
	}

	builder.getConverter().setErrorLocation(getLoc());
	auto converted = builder.getConverter().convertType(getResult().getType());

	if (not converted)
	{
		return logError(
				*this,
				"Type argument of __builtin_from_array must be a primitive type, "
				"not " +
						prettyType(getResult().getType()));
	}

	if (converted.isa<mlir::rlc::FloatType>())
	{
		if (castedInput.getArraySize() != 8)
		{
			return logError(
					*this,
					"Builtin from_byte_array to float must have a array of 8 bytes "
					"as input");
		}
		getResult().setType(converted);
		return mlir::success();
	}

	if (converted.isa<mlir::rlc::BoolType>())
	{
		if (castedInput.getArraySize() != 1)
		{
			return logError(
					*this,
					"Builtin from_byte_array to bool must have a array of 1 bytes "
					"as input");
		}
		getResult().setType(converted);
		return mlir::success();
	}

	if (auto casted = converted.dyn_cast<mlir::rlc::IntegerType>())
	{
		if (castedInput.getArraySize() != casted.getSize() / 8)
		{
			return logError(
					*this,
					std::string(
							"Builtin from_byte_array to integer must have a array of ") +
							llvm::Twine((casted.getSize() / 8)) +
							std::string(" bytes as input"));
		}
		getResult().setType(converted);
		return mlir::success();
	}
	return logError(
			*this,
			"Cannot convert byte array to desiderated output, only primitive types "
			"are supported");
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::ClassFieldDeclaration::getShugarizedTypes()
{
	llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2> toReturn;

	if (getDeclaration().getShugarizedType() != nullptr)
		toReturn.push_back(getDeclaration().getShugarizedType());
	return toReturn;
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::ConstantGlobalOp::getShugarizedTypes()
{
	llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2> toReturn;

	if (getShugarizedType() != nullptr)
		toReturn.push_back(*getShugarizedType());
	return toReturn;
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::ActionFunction::getShugarizedTypes()
{
	llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2> toReturn;

	for (auto parameter : getInfo().getArguments())
		if (parameter.getShugarizedType() != nullptr)
			toReturn.push_back(parameter.getShugarizedType());
	return toReturn;
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::ActionStatement::getShugarizedTypes()
{
	llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2> toReturn;

	for (auto parameter : getInfo().getArguments())
		if (parameter.getShugarizedType() != nullptr)
			toReturn.push_back(parameter.getShugarizedType());
	return toReturn;
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::FlatFunctionOp::getShugarizedTypes()
{
	llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2> toReturn;

	if (getInfo().getShugarizedReturnType() != nullptr)
		toReturn.push_back(getInfo().getShugarizedReturnType());

	for (auto parameter : getInfo().getArguments())
		if (parameter.getShugarizedType() != nullptr)
			toReturn.push_back(parameter.getShugarizedType());
	return toReturn;
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::FunctionOp::getShugarizedTypes()
{
	llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2> toReturn;

	if (getInfo().getShugarizedReturnType() != nullptr)
		toReturn.push_back(getInfo().getShugarizedReturnType());

	for (auto parameter : getInfo().getArguments())
		if (parameter.getShugarizedType() != nullptr)
			toReturn.push_back(parameter.getShugarizedType());
	return toReturn;
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::IsOp::getShugarizedTypes()
{
	if (not getShugarizedType().has_value())
		return {};
	return { *getShugarizedType() };
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::TypeAliasOp::getShugarizedTypes()
{
	if (getShugarizedType().has_value())
		return { *getShugarizedType() };
	return {};
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::UncheckedIsOp::getShugarizedTypes()
{
	return { getShugarizedType() };
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::FromByteArrayOp::getShugarizedTypes()
{
	return { getShugarizedType() };
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::ConstructOp::getShugarizedTypes()
{
	if (getShugarizedType().has_value())
		return { *getShugarizedType() };
	return {};
}

mlir::LogicalResult mlir::rlc::StringLiteralOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::InitOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	llvm::SmallVector<mlir::Type> acceptable;
	acceptable.push_back(mlir::rlc::IntegerType::getInt64(this->getContext()));
	acceptable.push_back(mlir::rlc::IntegerType::getInt8(this->getContext()));
	acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
	acceptable.push_back(mlir::rlc::FloatType::get(this->getContext()));
	return mlir::rlc::detail::typeCheckInteralOp(
			*this, builder, acceptable, mlir::rlc::VoidType::get(this->getContext()));
}

static bool initializerListTypeIsValid(mlir::Type t)
{
	if (t.isa<mlir::rlc::IntegerType>())
	{
		return true;
	}
	if (t.isa<mlir::rlc::BoolType>())
	{
		return true;
	}
	if (t.isa<mlir::rlc::FloatType>())
	{
		return true;
	}

	if (auto type = mlir::dyn_cast<mlir::rlc::ArrayType>(t))
	{
		return initializerListTypeIsValid(type.getUnderlying());
	}
	return false;
}

mlir::LogicalResult mlir::rlc::InitializerListOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto yield =
			mlir::dyn_cast<mlir::rlc::Yield>(getBody().back().getTerminator());

	if (yield.getArguments().empty())
		return logError(*this, "Initializer list cannot be empty");

	for (auto *child : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*child, builder).failed())
			return mlir::failure();
	}

	for (auto element : yield.getArguments())
	{
		if (element.getType() != yield.getArguments()[0].getType())
		{
			auto _ =
					logError(*this, "initializer list has arguments of different type");
			if (element.getDefiningOp() == nullptr)
			{
				return logRemark(
						element.getParentBlock()->getParentOp(),
						"missmatched argument here");
			}
			else
			{
				return logRemark(element.getDefiningOp(), "missmatched argument here");
			}
		}
	}

	auto type = mlir::rlc::ArrayType::get(
			getContext(),
			yield.getArguments()[0].getType(),
			yield.getArguments().size());

	// the reason we only accept this types is because they are trivially
	// copiable and the back. if the backend to mlir invoked copy assigment
	// operators correctly there would not be a need for this.
	if (not initializerListTypeIsValid(type))
	{
		emitOpError(
				"only acceptable types in initializer list are primitive types "
				"or arrays of primitive types");
		return mlir::failure();
	}

	getResult().setType(type);

	return mlir::success();
}

mlir::LogicalResult mlir::rlc::ImplicitAssignOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getRewriter().replaceOpWithNewOp<mlir::rlc::ImplicitAssignOp>(
			*this, getLhs(), getRhs());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::MallocOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getConverter().setErrorLocation(getLoc());
	auto converted = builder.getConverter().convertType(getResult().getType());
	auto shugarized = builder.getConverter().shugarizedConvertType(
			getShugarizedType().getType());
	if (not converted or not shugarized)
	{
		return mlir::failure();
	}
	setShugarizedTypeAttr(getShugarizedType().replaceType(shugarized));
	getResult().setType(converted);
	return mlir::success();
}

llvm::SmallVector<mlir::rlc::ShugarizedTypeAttr, 2>
mlir::rlc::MallocOp::getShugarizedTypes()
{
	return { getShugarizedType() };
}

mlir::LogicalResult mlir::rlc::InplaceInitializeOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::DestroyOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::FreeOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::BuiltinMangledNameOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::BuiltinAsPtr::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::BuiltinAssignOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getRewriter().replaceOpWithNewOp<mlir::rlc::BuiltinAssignOp>(
			*this, getLhs(), getRhs());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::AssignOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (getLhs().getType().isa<mlir::FunctionType>() or
			getRhs().getType().isa<mlir::FunctionType>())
	{
		return logError(
				*this, "Assigns cannot have operands that are function types");
	}
	builder.getRewriter().replaceOpWithNewOp<mlir::rlc::ImplicitAssignOp>(
			*this, getLhs(), getRhs());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::MemberAccess::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	builder.getRewriter().replaceOpWithNewOp<mlir::rlc::MemberAccess>(
			*this, getValue(), getMemberIndex());
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::IntegerLiteralUse::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	if (not getInputType()
							.cast<mlir::rlc::TemplateParameterType>()
							.getIsIntLiteral())
	{
		return logError(
				*this,
				"Input type of int literal must be a template parameter literal");
	}
	return mlir::success();
}

static bool locsAreInSameFile(mlir::Location l, mlir::Location r)
{
	auto casted1 = l.cast<mlir::FileLineColLoc>();
	auto catsed2 = r.cast<mlir::FileLineColLoc>();
	return casted1.getFilename() == catsed2.getFilename();
}

mlir::LogicalResult mlir::rlc::UnresolvedMemberAccess::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto structType = getValue().getType().dyn_cast<mlir::rlc::ClassType>();
	if (structType == nullptr)
	{
		return logError(*this, "Members of non-class types cannot be accessed");
	}

	if (getMemberName().empty())
	{
		return logError(*this, "Member accesses cannot refer to a empty name");
	}

	for (const auto &index : llvm::enumerate(structType.getMembers()))
	{
		if (index.value().getName() != getMemberName())
			continue;

		auto *maybeDecl = builder.getDeclarationOfType(getValue().getType());
		if (maybeDecl != nullptr and getMemberName().starts_with("_") and
				not locsAreInSameFile(maybeDecl->getLoc(), getLoc()))
		{
			return logError(
					*this,
					"Members starting with _ are private and cannot be accessed from "
					"another module");
		}

		builder.getRewriter().replaceOpWithNewOp<mlir::rlc::MemberAccess>(
				*this, getValue(), index.index());
		return mlir::success();
	}

	return logError(
			*this,
			"no known member " + getMemberName() + " in struct " +
					structType.getName());
}

mlir::LogicalResult mlir::rlc::IsAlternativeTypeOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::LogicalResult::success();
}

mlir::LogicalResult mlir::rlc::typeCheck(
		mlir::Operation &op, mlir::rlc::ModuleBuilder &builder)
{
	if (not op.hasTrait<mlir::rlc::TypeCheckable::Trait>())
	{
		op.emitOpError("does not implement type check");
		return mlir::failure();
	}

	builder.getRewriter().setInsertionPoint(&op);
	if (mlir::cast<mlir::rlc::TypeCheckable>(op).typeCheck(builder).failed())
		return mlir::failure();

	return mlir::LogicalResult::success();
}

mlir::LogicalResult mlir::rlc::FunctionOp::typeCheckFunctionDeclaration(
		mlir::IRRewriter &rewriter, mlir::rlc::RLCTypeConverter &converter)
{
	rewriter.setInsertionPoint(*this);
	auto scopedConverter = mlir::rlc::RLCTypeConverter(&converter);
	scopedConverter.setErrorLocation(getLoc());
	llvm::SmallVector<mlir::Type, 2> checkedTemplateParameters;
	for (auto parameter : getTemplateParameters())
	{
		auto unchecked = parameter.cast<mlir::TypeAttr>()
												 .getValue()
												 .cast<mlir::rlc::UncheckedTemplateParameterType>();

		auto checkedParameterType = converter.convertType(unchecked);
		if (not checkedParameterType)
		{
			return mlir::failure();
		}
		checkedTemplateParameters.push_back(checkedParameterType);
		auto actualType =
				checkedParameterType.cast<mlir::rlc::TemplateParameterType>();
		scopedConverter.registerType(actualType.getName(), actualType);
	}

	auto deducedType = scopedConverter.convertType(getFunctionType());
	auto shugarized = scopedConverter.shugarizedConvertType(getFunctionType());
	if (deducedType == nullptr or shugarized == nullptr)
	{
		return mlir::failure();
	}
	assert(deducedType.isa<mlir::FunctionType>());
	auto fType = deducedType.cast<mlir::FunctionType>();
	for (auto type : fType.getInputs())
	{
		if (type.isa<mlir::rlc::FrameType>() or type.isa<mlir::rlc::ContextType>())
			return mlir::rlc::logError(
					*this, "Only types in action functions can be marked as ctx or frm.");
	}
	if (fType.getNumResults() != 0 and
			(fType.getResult(0).isa<mlir::rlc::FrameType>() or
			 fType.getResult(0).isa<mlir::rlc::ContextType>()))
		return mlir::rlc::logError(
				*this, "Only types in action functions can be marked as ctx or frm.");

	getResult().setType(fType);
	setInfoAttr(getInfo().replaceTypes(shugarized.cast<mlir::FunctionType>()));
	setTemplateParametersAttr(
			rewriter.getTypeArrayAttr(checkedTemplateParameters));

	return mlir::success();
}

void mlir::rlc::ActionsStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// every region can be executed at most one.
	invocationBounds.append(getRegions().size(), mlir::InvocationBounds(0, 1));
}

mlir::MutableOperandRange mlir::rlc::Yield::getMutableSuccessorOperands(
		mlir::RegionBranchPoint point)
{
	return getArgumentsMutable().slice(0, 0);
}

void mlir::rlc::ActionsStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// from the op you can jump to any region
	if (succ.isParent())
		for (auto *region : getRegions())
			regions.push_back(
					mlir::RegionSuccessor(region, region->front().getArguments()));

	// from any region you jump out
	if (not succ.isParent())
		regions.push_back(mlir::RegionSuccessor(getOperation()->getResults()));
}

void mlir::rlc::ActionStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	if (not getPrecondition().empty())
		invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::ActionStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	if (succ.isParent())
	{
		if (not getPrecondition().empty())
			regions.push_back(
					mlir::RegionSuccessor(
							&getPrecondition(),
							getPrecondition().front().getArguments().slice(0, 0)));
		else
			regions.push_back(mlir::RegionSuccessor(getResults().slice(0, 0)));

		return;
	}

	regions.push_back(mlir::RegionSuccessor(getResults().slice(0, 0)));
}

void mlir::rlc::DeclarationStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::DeclarationStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a declaration you jump into the single body block
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));

	// when you are done with the region, you get out back to the statement
	if (not succ.isParent())
		regions.push_back(mlir::RegionSuccessor({}));
}

void mlir::rlc::StatementList::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::StatementList::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a statement you jump into the single body block
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));

	// when you are done with the region, you get out back to the statement
	if (not succ.isParent())
		regions.push_back(mlir::RegionSuccessor({}));
}

void mlir::rlc::Yield::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	if (not getOnEnd().empty())
		invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::Yield::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a yield you jump into the single body block
	if (succ.isParent())
	{
		if (not getOnEnd().empty())
		{
			regions.push_back(
					mlir::RegionSuccessor(
							&getOnEnd(), getOnEnd().front().getArguments()));
		}
		else
		{
			regions.push_back(mlir::RegionSuccessor());
		}
	}

	// when you are done with the region, you get out back to yield
	if (not succ.isParent())
		regions.push_back(mlir::RegionSuccessor());
}

void mlir::rlc::Yield::getSuccessorRegions(
		llvm::ArrayRef<::mlir::Attribute> operands,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	llvm::cast<::mlir::RegionBranchOpInterface>((*this)->getParentOp())
			.getSuccessorRegions((*this)->getParentRegion(), regions);
}

mlir::Type mlir::rlc::TypeAliasOp::getDeclaredType()
{
	return mlir::rlc::AliasType::get(getName(), getAliased());
}

std::optional<mlir::rlc::SourceRangeAttr>
mlir::rlc::TypeAliasOp::getDeclarationLocation()
{
	return getDeclaredNameLocation();
}

mlir::Type mlir::rlc::ClassDeclaration::getDeclaredType()
{
	return getResult().getType();
}

std::optional<mlir::rlc::SourceRangeAttr>
mlir::rlc::ClassDeclaration::getDeclarationLocation()
{
	return getTypeLocation();
}

void mlir::rlc::ReturnStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::ReturnStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a return statement you jump into the single body block
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));

	// when you are done with the region, you don't go anywhere!
}

void mlir::rlc::ExpressionStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::InitializerListOp::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::InitializerListOp::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a initializer list you jump into the single body block
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));

	// when you are done with the region, you get out back to initializer list
	if (not succ.isParent())
		regions.push_back(mlir::RegionSuccessor());
}

void mlir::rlc::ExpressionStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a expression statement you jump into the single body block
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));

	// when you are done with the region, you get out back to
	// ExpressionStatement
	if (not succ.isParent())
		regions.push_back(mlir::RegionSuccessor());
}

void mlir::rlc::IfStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// executes the precondition once
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));

	// executes the then else blocks zero times or one time
	invocationBounds.push_back(mlir::InvocationBounds(0, 1));
	invocationBounds.push_back(mlir::InvocationBounds(0, 1));
}

void mlir::rlc::IfStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a if statement you jump into the precondition
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(
						&getCondition(), getCondition().front().getArguments()));

	// from the condition, you can jump to the then or else branch
	if (succ.getRegionOrNull() == &getCondition())
	{
		regions.push_back(
				mlir::RegionSuccessor(
						&getTrueBranch(), getTrueBranch().front().getArguments()));
		regions.push_back(
				mlir::RegionSuccessor(
						&getElseBranch(), getElseBranch().front().getArguments()));
	}

	// from the then and else branch you go back out
	if (succ.getRegionOrNull() == &getTrueBranch() or
			succ.getRegionOrNull() == &getElseBranch())
	{
		regions.push_back(mlir::RegionSuccessor({}));
	}
}

void mlir::rlc::SubActionStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	if (getRunOnce())
		invocationBounds.push_back(mlir::InvocationBounds(1, 1));
	else
		invocationBounds.push_back(mlir::InvocationBounds(0, std::nullopt));
}

void mlir::rlc::ShortCircuitingOr::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// executes the lhs once
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));

	// executes the rhs once or zero times
	invocationBounds.push_back(mlir::InvocationBounds(0, 1));
}

void mlir::rlc::ContinueStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// executes the on_exit_scope code once
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::ContinueStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getOnEnd(), getOnEnd().front().getArguments()));

	if (succ.getRegionOrNull() == &getOnEnd())
	{
		regions.push_back(mlir::RegionSuccessor({}));
	}
}

void mlir::rlc::BreakStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// executes the on_exit_scope code once
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));
}

void mlir::rlc::BreakStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getOnEnd(), getOnEnd().front().getArguments()));

	if (succ.getRegionOrNull() == &getOnEnd())
	{
		regions.push_back(mlir::RegionSuccessor({}));
	}
}

void mlir::rlc::ShortCircuitingAnd::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// executes the lhs once
	invocationBounds.push_back(mlir::InvocationBounds(1, 1));

	// executes the rhs once or zero times
	invocationBounds.push_back(mlir::InvocationBounds(0, 1));
}

void mlir::rlc::ShortCircuitingOr::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// when you reach the op, you jump in to the right hand
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getLhs(), getLhs().front().getArguments()));

	// from the LHS region you can either jump out, or to the lhs region
	if (succ.getRegionOrNull() == &getLhs())
	{
		regions.push_back(
				mlir::RegionSuccessor(&getRhs(), getRhs().front().getArguments()));
		regions.push_back(mlir::RegionSuccessor({}));
	}

	// from the RHS region you jump to the exit
	if (succ.getRegionOrNull() == &getRhs())
	{
		regions.push_back(mlir::RegionSuccessor({}));
	}
}

void mlir::rlc::ShortCircuitingAnd::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// when you reach the op, you jump in to the right hand
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getLhs(), getLhs().front().getArguments()));

	// from the LHS region you can either jump out, or to the lhs region
	if (succ.getRegionOrNull() == &getLhs())
	{
		regions.push_back(
				mlir::RegionSuccessor(&getRhs(), getRhs().front().getArguments()));
		regions.push_back(mlir::RegionSuccessor({}));
	}

	// from the RHS region you jump to the exit
	if (succ.getRegionOrNull() == &getRhs())
	{
		regions.push_back(mlir::RegionSuccessor({}));
	}
}

void mlir::rlc::SubActionStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));

	if (not succ.isParent())
	{
		regions.push_back(mlir::RegionSuccessor({}));
	}
}

void mlir::rlc::ForLoopStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// executes the body any amount of time
	invocationBounds.push_back(mlir::InvocationBounds(0, std::nullopt));
}

void mlir::rlc::ForLoopStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a for statement you jump into the expression
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));

	// from the condition, you can jump out or to the body
	if (succ.getRegionOrNull() == &getBody())
	{
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));
		regions.push_back(mlir::RegionSuccessor({}));
	}
}

void mlir::rlc::WhileStatement::getRegionInvocationBounds(
		llvm::ArrayRef<mlir::Attribute> operands,
		llvm::SmallVectorImpl<mlir::InvocationBounds> &invocationBounds)
{
	// executes the precondition once or more
	invocationBounds.push_back(mlir::InvocationBounds(1, std::nullopt));

	// executes the body any amount of time
	invocationBounds.push_back(mlir::InvocationBounds(0, std::nullopt));
}

void mlir::rlc::WhileStatement::getSuccessorRegions(
		mlir::RegionBranchPoint succ,
		llvm::SmallVectorImpl<::mlir::RegionSuccessor> &regions)
{
	// When you hit a for statement you jump into the precondition
	if (succ.isParent())
		regions.push_back(
				mlir::RegionSuccessor(
						&getCondition(), getCondition().front().getArguments()));

	// from the condition, you can jump out or to the body
	if (succ.getRegionOrNull() == &getCondition())
	{
		regions.push_back(
				mlir::RegionSuccessor(&getBody(), getBody().front().getArguments()));
		regions.push_back(mlir::RegionSuccessor({}));
	}

	// from the body you jump to the condition
	if (succ.getRegionOrNull() == &getBody())
	{
		regions.push_back(
				mlir::RegionSuccessor(
						&getCondition(), getCondition().front().getArguments()));
	}
}

mlir::SuccessorOperands mlir::rlc::Branch::getSuccessorOperands(unsigned index)
{
	return mlir::SuccessorOperands(
			mlir::MutableOperandRange(getOperation(), 0, 0));
}

mlir::SuccessorOperands mlir::rlc::CondBranch::getSuccessorOperands(
		unsigned index)
{
	return mlir::SuccessorOperands(
			mlir::MutableOperandRange(getOperation(), 0, 0));
}

mlir::SuccessorOperands mlir::rlc::FlatActionStatement::getSuccessorOperands(
		unsigned index)
{
	return mlir::SuccessorOperands(
			mlir::MutableOperandRange(getOperation(), 0, 0));
}

mlir::SuccessorOperands mlir::rlc::SelectBranch::getSuccessorOperands(
		unsigned index)
{
	return mlir::SuccessorOperands(
			mlir::MutableOperandRange(getOperation(), 0, 0));
}

llvm::SmallVector<mlir::rlc::TemplateParameterType, 4>
mlir::rlc::UncheckedTraitDefinition::getTemplateParameterTypes()
{
	llvm::SmallVector<mlir::rlc::TemplateParameterType, 4> toReturn;
	for (auto t : getTemplateParameters())
		toReturn.push_back(
				mlir::rlc::TemplateParameterType::get(
						getContext(), t.cast<mlir::StringAttr>(), nullptr, false));
	return toReturn;
}

mlir::LogicalResult mlir::rlc::FunctionOp::isTemplate()
{
	if (isTemplateType(getFunctionType()).succeeded())
		return mlir::success();

	for (auto t : getTemplateParameters())
		if (isTemplateType(t.cast<mlir::TypeAttr>().getValue()).succeeded())
			return mlir::success();

	return mlir::failure();
}

mlir::OpFoldResult mlir::rlc::Constant::fold(
		mlir::rlc::ConstantGenericAdaptor<llvm::ArrayRef<mlir::Attribute>> attr)
{
	return getValueAttr();
}

mlir::OpFoldResult mlir::rlc::MinusOp::fold(
		mlir::rlc::MinusOpGenericAdaptor<llvm::ArrayRef<mlir::Attribute>> attr)
{
	if (attr.getLhs() == nullptr)
		return nullptr;
	if (auto castedLHS = attr.getLhs().dyn_cast<mlir::IntegerAttr>())
	{
		return mlir::IntegerAttr::get(
				castedLHS.getType(), -1 * castedLHS.getValue());
	}

	if (auto castedLHS = attr.getLhs().dyn_cast<mlir::FloatAttr>())
	{
		auto copy = castedLHS.getValue();
		copy.changeSign();
		return mlir::FloatAttr::get(castedLHS.getType(), copy);
	}

	return nullptr;
}

mlir::OpFoldResult mlir::rlc::InitializerListOp::fold(
		mlir::rlc::InitializerListOpGenericAdaptor<llvm::ArrayRef<mlir::Attribute>>
				attr)
{
	// if any operatation in the body is not foldable, we can't be turned into a
	// constant
	for (auto &op : getBody().getOps())
	{
		if (llvm::isa<mlir::rlc::Yield>(op))
			continue;
		llvm::SmallVector<mlir::OpFoldResult, 4> attr;
		if (op.fold(attr).failed() or attr.empty())
			return nullptr;
	}
	auto yield =
			mlir::dyn_cast<mlir::rlc::Yield>(getBody().front().getTerminator());
	llvm::SmallVector<mlir::Attribute> toReturn;
	for (auto arg : yield.getArguments())
	{
		llvm::SmallVector<mlir::OpFoldResult, 4> attr;
		// if it is a argument, do nothing
		if (arg.getDefiningOp() == nullptr)
			return nullptr;

		auto res = arg.getDefiningOp()->fold(attr);
		if (res.failed())
			return nullptr;
		assert(attr.size() == 1);
		assert(not attr[0].isNull());
		toReturn.push_back(mlir::cast<mlir::Attribute>(attr[0]));
	}

	return mlir::ArrayAttr::get(getContext(), toReturn);
}

size_t mlir::rlc::EnumDeclarationOp::countFields()
{
	auto range = getBody().front().getOps<mlir::rlc::EnumFieldDeclarationOp>();
	return std::distance(range.begin(), range.end());
}

llvm::SmallVector<mlir::rlc::ClassFieldDeclarationAttr, 4>
mlir::rlc::ClassDeclaration::getMemberFields()
{
	llvm::SmallVector<mlir::rlc::ClassFieldDeclarationAttr, 4> outs;
	for (auto member : getBody().getOps<mlir::rlc::ClassFieldDeclaration>())
		outs.push_back(member.getDeclaration());

	return outs;
}

mlir::rlc::ClassFieldDeclarationAttr
mlir::rlc::ClassDeclaration::getMemberField(size_t i)
{
	return getMemberFields()[i];
}

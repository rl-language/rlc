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
#include "mlir/IR/OpDefinition.h"
#include "rlc/dialect/ActionLiveness.hpp"
#include "rlc/dialect/Dialect.h"
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
		if (auto parent = statement->getParentOfType<mlir::rlc::ActionsStatement>())
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

static llvm::SmallVector<mlir::Type, 3> getFunctionsTypesGeneratedByAction(
		mlir::rlc::ModuleBuilder &builder, mlir::rlc::ActionFunction fun)
{
	auto funType = builder.typeOfAction(fun);
	llvm::SmallVector<mlir::Type, 3> ToReturn;
	using OverloadKey = std::pair<std::string, const void *>;
	std::set<OverloadKey> added;
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
		OverloadKey overloadKey(
				llvm::cast<mlir::rlc::ActionStatement>(op).getName().str(),
				ftype.getAsOpaquePointer());
		if (added.contains(overloadKey))
			continue;

		ToReturn.emplace_back(ftype);
		added.insert(overloadKey);
	}
	return ToReturn;
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

	for (auto pair : llvm::zip(getArgNames(), getBody().front().getArguments()))
	{
		auto name = std::get<0>(pair).cast<mlir::StringAttr>();
		auto arg = std::get<1>(pair);
		addToFrames(arg, name);
	}

	walk([&](mlir::Operation *op) {
		if (auto casted = llvm::dyn_cast<mlir::rlc::DeclarationStatement>(op))
		{
			if (auto castedType = casted.getType().dyn_cast<mlir::rlc::FrameType>())
				explicitFrame.append(casted, casted.getName());
		}
		else if (auto casted = llvm::dyn_cast<mlir::rlc::ActionStatement>(op))
		{
			for (auto pair :
					 llvm::zip(casted.getDeclaredNames(), casted.getResults()))
			{
				auto name = std::get<0>(pair).cast<mlir::StringAttr>();
				auto arg = std::get<1>(pair);
				addToFrames(arg, name);
			}
		}
	});
	return { std::move(explicitFrame), std::move(hiddenFrame) };
}

static void setFramesBody(mlir::rlc::ActionFunction fun)
{
	llvm::SmallVector<mlir::Type, 4> memberTypes;
	llvm::SmallVector<std::string, 4> memberNames;

	llvm::SmallVector<mlir::Type, 4> hiddenMemberTypes;
	llvm::SmallVector<std::string, 4> hiddenMemberNames;

	// add the implicit local variable "resume_index" to members
	memberTypes.push_back(mlir::rlc::IntegerType::getInt64(fun.getContext()));
	memberNames.push_back("resume_index");

	auto frames = fun.getFrameLists();
	for (auto &entry : frames.first.valueNamePairs)
	{
		memberNames.push_back(entry.second.str());
		memberTypes.push_back(
				entry.first.getType().cast<mlir::rlc::FrameType>().getUnderlying());
	}

	for (auto &entry : frames.second.valueNamePairs)
	{
		hiddenMemberNames.push_back(entry.second.str());
		auto t = entry.first.getType();
		if (auto casted = t.dyn_cast<mlir::rlc::ContextType>())
			t = casted.getUnderlying();
		hiddenMemberTypes.push_back(mlir::rlc::ReferenceType::get(t));
	}

	auto res = fun.getEntityType().setBody(memberTypes, memberNames);
	assert(res.succeeded());

	auto res2 = mlir::rlc::EntityType::getIdentified(
			fun.getContext(), (fun.getEntityType().getName() + "_shadow").str(), {});
	auto isOk = res2.setBody(hiddenMemberTypes, hiddenMemberNames).succeeded();
	assert(isOk);
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

	auto generatedFunctions = getFunctionsTypesGeneratedByAction(builder, fun);

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
			fun.getArgNames());
	newAction.getBody().takeBody(fun.getBody());
	newAction.getPrecondition().takeBody(fun.getPrecondition());
	fun.getResult().replaceAllUsesWith(newAction.getResult());
	rewriter.eraseOp(fun);

	setFramesBody(newAction);
	return newAction;
}

static void defineGetNameFunction(
		mlir::rlc::ActionFunction function,
		mlir::rlc::ModuleBuilder &builder,
		mlir::Value action,
		mlir::rlc::ActionStatement statement,
		mlir::rlc::EntityType actionType)
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
			builder.getRewriter().getStrArrayAttr(names),
			false);

	{
		auto *preconditionBB = builder.getRewriter().createBlock(
				&applyFunction.getPrecondition(), {}, types, locs);

		builder.getRewriter().setInsertionPointToStart(preconditionBB);
		auto trueValue = builder.getRewriter().create<mlir::rlc::Constant>(
				statement.getLoc(), true);

		builder.getRewriter().create<mlir::rlc::Yield>(
				statement.getLoc(), mlir::ValueRange({ trueValue }));
	}

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
		mlir::rlc::EntityType actionType)
{
	builder.getRewriter().setInsertionPoint(function);
	llvm::SmallVector<mlir::Type, 4> types(
			{ actionType, function.getEntityType() });
	llvm::SmallVector<llvm::StringRef, 4> names({ "self", "frame" });
	llvm::SmallVector<mlir::Location, 4> locs(
			{ statement.getLoc(), statement.getLoc() });

	for (auto [actionType, name] :
			 llvm::zip(function.getType().getInputs(), function.getArgNames()))
	{
		if (auto casted = actionType.dyn_cast<mlir::rlc::ContextType>())
		{
			types.push_back(casted.getUnderlying());
			names.push_back(name.cast<mlir::StringAttr>());
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
			builder.getRewriter().getStrArrayAttr(names),
			false);

	{
		auto *preconditionBB = builder.getRewriter().createBlock(
				&applyFunction.getPrecondition(), {}, types, locs);

		builder.getRewriter().setInsertionPointToStart(preconditionBB);
		auto canExecuteAction = builder.getRewriter().create<mlir::rlc::CanOp>(
				statement.getLoc(), action);

		llvm::SmallVector<mlir::Value, 4> args(
				std::next(preconditionBB->args_begin()), preconditionBB->args_end());
		for (auto [index, actionType] : llvm::enumerate(actionType.getBody()))
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
		for (auto [index, actionType] : llvm::enumerate(actionType.getBody()))
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

static mlir::Type declareActionStatementType(
		mlir::rlc::ActionFunction function,
		mlir::rlc::ModuleBuilder &builder,
		mlir::Value action,
		llvm::SmallVector<mlir::rlc::FunctionOp, 4> applyFunctions)
{
	builder.getRewriter().setInsertionPoint(function);
	auto statement = mlir::dyn_cast<mlir::rlc::ActionStatement>(
			builder.actionFunctionValueToActionStatement(action).front());

	std::string name = function.getEntityType().getName().str();
	name += snakeCaseToCamelCase(statement.getName());

	llvm::SmallVector<mlir::Type, 4> fieldTypes;
	llvm::SmallVector<std::string, 4> fieldNames;
	llvm::SmallVector<llvm::StringRef, 4> fieldNamesRef;

	for (auto [name, result] :
			 llvm::zip(statement.getDeclaredNames(), statement.getResults()))
	{
		if (result.getType().isa<mlir::rlc::ContextType>())
			continue;
		auto type = result.getType();
		if (auto casted = type.dyn_cast<mlir::rlc::FrameType>())
			type = casted.getUnderlying();
		fieldNames.push_back(name.cast<mlir::StringAttr>().str());
		fieldNamesRef.push_back(name.cast<mlir::StringAttr>());
		fieldTypes.push_back(type);
	}

	auto type = mlir::rlc::EntityType::getNewIdentified(
			function.getContext(), name, fieldTypes, fieldNames, {});

	auto built = builder.getRewriter().create<mlir::rlc::EntityDeclaration>(
			statement.getLoc(),
			type,
			name,
			builder.getRewriter().getTypeArrayAttr(fieldTypes),
			builder.getRewriter().getStrArrayAttr(fieldNamesRef),
			builder.getRewriter().getTypeArrayAttr({}));
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
	builder.getRewriter().create<mlir::rlc::TraitDefinition>(
			function.getLoc(),
			mlir::rlc::TraitMetaType::get(
					function.getContext(),
					(function.getEntityType().getName() + "Action").str(),
					{ builder.getRewriter().getStringAttr("T") },
					{ mlir::FunctionType::get(
							function.getContext(),
							{ mlir::rlc::TemplateParameterType::get(
										function.getContext(), "T", nullptr, false),
								function.getEntityType() },
							{}) },
					{ builder.getRewriter().getStringAttr("apply") }));

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
	mlir::Type alternative =
			mlir::rlc::AlternativeType::get(function.getContext(), declaredTypes);
	auto type = mlir::rlc::EntityType::getNewIdentified(
			function.getContext(),
			("Any" + function.getEntityType().getName() + "Action").str(),
			{ alternative },
			{ "content" },
			{});

	builder.getRewriter().create<mlir::rlc::EntityDeclaration>(
			function.getLoc(),
			type,
			type.getName(),
			builder.getRewriter().getTypeArrayAttr(type.getBody()),
			builder.getRewriter().getStrArrayAttr(
					llvm::SmallVector<llvm::StringRef, 2>(
							type.getFieldNames().begin(), type.getFieldNames().end())),
			builder.getRewriter().getTypeArrayAttr({}));

	return mlir::success();
}

mlir::rlc::ActionFunction mlir::rlc::detail::typeCheckAction(
		mlir::rlc::ActionFunction fun, mlir::rlc::ValueTable *parentSymbolTable)
{
	mlir::rlc::ModuleBuilder builder(
			fun->getParentOfType<mlir::ModuleOp>(), parentSymbolTable);

	if (builder.typeOfAction(fun).cast<mlir::rlc::EntityType>().isInitialized())
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

	mlir::rlc::ModuleBuilder builder2(
			newF->getParentOfType<mlir::ModuleOp>(), parentSymbolTable);
	if (declareActionTypes(newF, builder2).failed())
		return nullptr;

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

::mlir::LogicalResult mlir::rlc::ArrayCallOp::verifySymbolUses(
		::mlir::SymbolTableCollection &symbolTable)
{
	return LogicalResult::success();
}

mlir::LogicalResult mlir::rlc::UnderTypeCheckMarker::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	return mlir::success();
}

mlir::LogicalResult mlir::rlc::SubActionStatement::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto parent = getOperation()->getParentOfType<mlir::rlc::ActionFunction>();
	if (not parent)
		return mlir::rlc::logError(
				*this, "SubAction statements can only appear in Action Functions");
	for (auto *child : ops(getBody()))
	{
		if (mlir::rlc::typeCheck(*child, builder).failed())
			return mlir::failure();
	}

	mlir::rlc::Yield yield =
			mlir::cast<mlir::rlc::Yield>(getBody().front().getTerminator());
	mlir::Value frameVar = yield.getArguments()[0];
	if (not frameVar.getType().isa<mlir::rlc::EntityType>() or
			not builder.isEntityOfAction(frameVar.getType()))
	{
		return logError(
				*this,
				"Subaction statement must refer to a action, not a " +
						prettyType(frameVar.getType()));
	}
	auto underlyingType = frameVar.getType().cast<mlir::rlc::EntityType>();
	auto underlying = builder.getActionOf(underlyingType)
												.getDefiningOp<mlir::rlc::ActionFunction>();

	mlir::IRRewriter &rewiter = builder.getRewriter();
	rewiter.setInsertionPoint(*this);

	auto decl = rewiter.create<mlir::rlc::DeclarationStatement>(
			getLoc(), mlir::rlc::FrameType::get(underlyingType), getName());
	decl.getBody().takeBody(getBody());
	builder.getSymbolTable().add(getName(), decl);

	frameVar = builder.getRewriter().create<mlir::rlc::StorageCast>(
			getLoc(), underlyingType, decl);

	llvm::SmallVector<mlir::Value, 2> actionValues = underlying.getActions();
	if (actionValues.empty())
	{
		erase();
		return mlir::success();
	}

	if (not getRunOnce())
	{
		auto loop = rewiter.create<mlir::rlc::WhileStatement>(getLoc());
		rewiter.createBlock(&loop.getCondition());

		auto *call = builder.emitCall(
				*this, true, "is_done", mlir::ValueRange({ frameVar }));
		assert(call);
		auto isDone = call->getResult(0);

		auto isNotDone = rewiter.create<mlir::rlc::NotOp>(getLoc(), isDone);

		rewiter.create<mlir::rlc::Yield>(getLoc(), mlir::ValueRange({ isNotDone }));

		rewiter.createBlock(&loop.getBody());

		auto finalYield = rewiter.create<mlir::rlc::Yield>(getLoc());
		rewiter.setInsertionPoint(finalYield);
	}

	auto actions = rewiter.create<mlir::rlc::ActionsStatement>(
			getLoc(), actionValues.size());

	for (size_t i = 0; i < actionValues.size(); i++)
	{
		auto *bb = rewiter.createBlock(
				&actions.getActions()[i], actions.getActions()[i].begin());
		rewiter.setInsertionPoint(bb, bb->begin());
		mlir::Value toCall = actionValues[i];
		mlir::rlc::ActionStatement referred =
				mlir::cast<mlir::rlc::ActionStatement>(
						*builder.actionFunctionValueToActionStatement(toCall)[0]);

		llvm::SmallVector<mlir::Type, 4> resultTypes;
		llvm::SmallVector<mlir::Location, 4> resultLoc;
		llvm::SmallVector<std::string> nameAttrs;

		for (auto arg : llvm::zip(parent.getArgumentTypes(), parent.getArgNames()))
		{
			auto type = std::get<0>(arg);
			if (type.isa<mlir::rlc::ContextType>())
			{
				resultTypes.push_back(type);
				nameAttrs.push_back(std::get<1>(arg).cast<mlir::StringAttr>().str());
				resultLoc.push_back(parent.getLoc());
			}
		}
		size_t contextArgsCount = resultTypes.size();

		for (auto type :
				 llvm::drop_begin(referred.getResultTypes(), getForwardedArgs().size()))
			resultTypes.push_back(type);

		for (auto type :
				 llvm::drop_begin(referred.getResultTypes(), getForwardedArgs().size()))
			resultLoc.push_back(actions.getLoc());

		for (auto name : llvm::drop_begin(
						 referred.getDeclaredNames(), getForwardedArgs().size()))
		{
			nameAttrs.push_back(name.cast<mlir::StringAttr>().str());
		}

		auto fixed = rewiter.create<mlir::rlc::ActionStatement>(
				referred.getLoc(),
				resultTypes,
				referred.getName(),
				nameAttrs,
				referred.getId(),
				referred.getResumptionPoint());

		auto *newBody = rewiter.createBlock(
				&fixed.getPrecondition(),
				fixed.getPrecondition().begin(),
				resultTypes,
				resultLoc);

		llvm::SmallVector<mlir::Value, 4> canArgs(
				getForwardedArgs().begin(), getForwardedArgs().end());
		for (auto arg : llvm::drop_begin(newBody->getArguments(), contextArgsCount))
			canArgs.push_back(arg);
		canArgs.insert(canArgs.begin(), frameVar);

		auto casted = rewiter.create<mlir::rlc::CanOp>(actions.getLoc(), toCall);
		auto result = rewiter.create<mlir::rlc::CallOp>(
				actions.getLoc(), casted, false, canArgs);
		rewiter.create<mlir::rlc::Yield>(
				actions.getLoc(), mlir::ValueRange({ result.getResult(0) }));
		rewiter.setInsertionPointAfter(fixed);

		llvm::SmallVector<mlir::Value, 4> args(
				getForwardedArgs().begin(), getForwardedArgs().end());
		for (auto result : llvm::drop_begin(fixed.getResults(), contextArgsCount))
			args.push_back(result);

		args.insert(args.begin(), frameVar);

		rewiter.create<mlir::rlc::CallOp>(actions.getLoc(), toCall, false, args);

		rewiter.create<mlir::rlc::Yield>(actions.getLoc());
	}
	rewiter.setInsertionPointAfter(*this);
	rewiter.eraseOp(*this);

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

bool mlir::rlc::DeclarationStatement::isReference()
{
	auto initializer = getBody().front().getTerminator()->getOperand(0);

	if (initializer.getDefiningOp<mlir::rlc::MemberAccess>())
		return true;

	if (initializer.getDefiningOp<mlir::rlc::ArrayAccess>())
		return true;

	// true if it is a function argument
	if (initializer.getDefiningOp() == nullptr)
		return true;

	if (auto casted = initializer.getDefiningOp<mlir::rlc::CallOp>())
	{
		auto resultsType =
				casted.getCallee().getType().cast<mlir::FunctionType>().getResults();
		if (resultsType.empty() or resultsType[0].isa<mlir::rlc::VoidType>())
			return false;

		return resultsType[0].isa<mlir::rlc::ReferenceType>();
	}
	return false;
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
			getLoc(), deducedType, getName());
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
		return logError(
				*this,
				"Type of argument of array access expression must be a array or a "
				"owning "
				"pointer or a string literal");
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
	auto *yield = getBody().front().getTerminator();

	auto newOne = rewriter.create<mlir::rlc::ReturnStatement>(
			getLoc(),
			yield->getNumOperands() != 0 ? yield->getOpOperand(0).get().getType()
																	 : mlir::rlc::VoidType::get(getContext()));
	newOne.getBody().takeBody(getBody());
	rewriter.eraseOp(*this);

	if (auto parentFunction = newOne->getParentOfType<mlir::rlc::FunctionOp>())
	{
		if (not isReturnTypeCompatible(
						newOne.getResult(), parentFunction.getResultTypes()[0]))
		{
			auto _ = mlir::rlc::logError(
					newOne,
					"Return statement returns values incompatible with the function "
					"signature");
			_ = mlir::rlc::logRemark(
					newOne, "Return value type is " + prettyType(newOne.getResult()));

			return mlir::rlc::logRemark(
					parentFunction,
					"Function return type is " +
							prettyType(parentFunction.getResultTypes()[0]));
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
	auto deducedType = builder.getConverter().convertType(getTypeOrTrait());
	if (deducedType == nullptr)
	{
		return mlir::rlc::logRemark(*this, "In Is expression");
	}

	rewriter.setInsertionPoint(getOperation());
	auto replaced = rewriter.replaceOpWithNewOp<mlir::rlc::IsOp>(
			*this, getExpression(), deducedType);

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

	builder.getConverter().registerType(
			getName(),
			mlir::dyn_cast<mlir::rlc::Yield>(getBody().back().getTerminator())
					.getArguments()[0]
					.getType());

	builder.getRewriter().eraseOp(*this);
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

	for (auto op : getBody().getOps<mlir::rlc::FunctionOp>())
	{
		names.push_back(op.getUnmangledNameAttr());
		types.push_back(op.getFunctionType());
	}

	llvm::SmallVector<mlir::StringAttr> typeNames;
	for (auto t : getTemplateParameters())
	{
		auto name = builder.getRewriter().getStringAttr(
				"TraitType" + t.cast<mlir::StringAttr>().strref());
		typeNames.push_back(name);
	}

	auto type = mlir::rlc::TraitMetaType::get(
			getContext(), getName(), typeNames, types, names);
	rewriter.setInsertionPointAfter(*this);
	auto op = rewriter.create<mlir::rlc::TraitDefinition>(getLoc(), type);

	// replace the template parameters provided by the user with one prefixed with
	// TraitType so that it does not clashes with regular names
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
					trait.getTemplateParameterTypes().back(),
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

	auto name = table.lookUpValue(isOp.getExpression());
	if (name.empty())
		return;

	auto upcastedValue = rewriter.create<mlir::rlc::ValueUpcastOp>(
			isOp.getLoc(), isOp.getTypeOrTrait(), isOp.getExpression());
	table.add(name, upcastedValue);
}

mlir::LogicalResult mlir::rlc::TemplateInstantiationOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
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
		mlir::rlc::ModuleBuilder &builder, mlir::Value maybeActionEntity)
{
	auto casted = dyn_cast<mlir::rlc::EntityType>(maybeActionEntity.getType());
	if (not casted or not builder.isEntityOfAction(casted))
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
				"arguments. The number of induction varaibles must be exactly the same "
				"as the number of expressions, or one more to capture the name of the "
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

mlir::LogicalResult mlir::rlc::ConstructOp::typeCheck(
		mlir::rlc::ModuleBuilder &builder)
{
	auto deducedType = builder.getConverter().convertType(getType());
	auto &rewriter = builder.getRewriter();
	if (deducedType == nullptr)
	{
		return logRemark(*this, "in construction expression");
	}

	rewriter.replaceOpWithNewOp<mlir::rlc::ConstructOp>(*this, deducedType);

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

	llvm::SmallVector<mlir::Type, 4> newResultTypes;
	llvm::SmallVector<std::string, 4> newArgNames;
	unsigned contextArgCounts = 0;
	for (auto arg : llvm::zip(parent.getArgumentTypes(), parent.getArgNames()))
	{
		auto type = std::get<0>(arg);
		if (type.isa<mlir::rlc::ContextType>())
		{
			getPrecondition().front().insertArgument(
					contextArgCounts++, type, getLoc());
			newResultTypes.push_back(type);
			newArgNames.push_back(std::get<1>(arg).cast<mlir::StringAttr>().str());
		}
	}

	for (auto &operand : getPrecondition().front().getArguments())
	{
		auto converted = builder.getConverter().convertType(operand.getType());
		if (not converted)
		{
			return logRemark(*this, "in of argument of action statement");
		}
		operand.setType(converted);
	}

	for (auto result : getResults())
	{
		auto deduced = builder.getConverter().convertType(result.getType());
		if (deduced == nullptr)
			return mlir::failure();
		result.setType(deduced);
	}

	for (auto result : getResults())
		newResultTypes.push_back(result.getType());

	for (auto name : getDeclaredNames())
		newArgNames.push_back(name.dyn_cast<mlir::StringAttr>().str());

	auto newDecl = builder.getRewriter().create<mlir::rlc::ActionStatement>(
			getLoc(), newResultTypes, getName(), newArgNames);

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
			builder.getSymbolTable().add(name.cast<mlir::StringAttr>(), res);

		for (auto *op : ToCheck)
		{
			if (mlir::rlc::typeCheck(*op, builder).failed())
				return mlir::failure();
		}
	}

	for (auto [name, res] :
			 llvm::zip(newDecl.getDeclaredNames(), newDecl.getResults()))
		builder.getSymbolTable().add(name.cast<mlir::StringAttr>(), res);

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
		builder.getRewriter().replaceOpWithNewOp<mlir::rlc::FromByteArrayOp>(
				*this, converted, getLhs());
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
		builder.getRewriter().replaceOpWithNewOp<mlir::rlc::FromByteArrayOp>(
				*this, converted, getLhs());
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
		builder.getRewriter().replaceOpWithNewOp<mlir::rlc::FromByteArrayOp>(
				*this, casted, getLhs());
		return mlir::success();
	}
	return logError(
			*this,
			"Cannot convert byte array to desiderated output, only primitive types "
			"are supported");
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
			return logRemark(element.getDefiningOp(), "missmatched argument here");
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
		emitOpError("only acceptable types in initializer list are primitive types "
								"or arrays of primitive types");
		return mlir::failure();
	}

	builder.getRewriter().setInsertionPoint(*this);
	auto newVal = builder.getRewriter().create<mlir::rlc::InitializerListOp>(
			getLoc(), type);

	newVal.getBody().takeBody(getBody());

	builder.getRewriter().replaceOp(*this, newVal);

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
	auto converted = builder.getConverter().convertType(getResult().getType());
	if (not converted)
	{
		return mlir::failure();
	}

	builder.getRewriter().replaceOpWithNewOp<mlir::rlc::MallocOp>(
			*this, converted, this->getSize());

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
	auto structType = getValue().getType().dyn_cast<mlir::rlc::EntityType>();
	if (structType == nullptr)
	{
		return logError(*this, "Members of non-entity types cannot be accessed");
	}

	if (getMemberName().empty())
	{
		return logError(*this, "Member accesses cannot refer to a empty name");
	}

	for (const auto &index : llvm::enumerate(structType.getFieldNames()))
	{
		if (index.value() != getMemberName())
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
	llvm::SmallVector<mlir::Type, 2> checkedTemplateParameters;
	for (auto parameter : getTemplateParameters())
	{
		auto unchecked = parameter.cast<mlir::TypeAttr>()
												 .getValue()
												 .cast<mlir::rlc::UncheckedTemplateParameterType>();

		auto checkedParameterType = converter.convertType(unchecked);
		if (not checkedParameterType)
		{
			return logError(
					*this,
					"No know type " + prettyType(unchecked) +
							" in function declaration parameter");
		}
		checkedTemplateParameters.push_back(checkedParameterType);
		auto actualType =
				checkedParameterType.cast<mlir::rlc::TemplateParameterType>();
		scopedConverter.registerType(actualType.getName(), actualType);
	}

	auto deducedType = scopedConverter.convertType(getFunctionType());
	if (deducedType == nullptr)
	{
		return logError(
				*this,
				"No known type " + prettyType(getFunctionType().getResult(0)) +
						" in function declaration return type");
	}
	assert(deducedType.isa<mlir::FunctionType>());

	getResult().setType(deducedType.cast<mlir::FunctionType>());
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
			regions.push_back(mlir::RegionSuccessor(
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
			regions.push_back(mlir::RegionSuccessor(
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
		regions.push_back(mlir::RegionSuccessor(
				&getCondition(), getCondition().front().getArguments()));

	// from the condition, you can jump to the then or else branch
	if (succ.getRegionOrNull() == &getCondition())
	{
		regions.push_back(mlir::RegionSuccessor(
				&getTrueBranch(), getTrueBranch().front().getArguments()));
		regions.push_back(mlir::RegionSuccessor(
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
		regions.push_back(mlir::RegionSuccessor(
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
		regions.push_back(mlir::RegionSuccessor(
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
		toReturn.push_back(mlir::rlc::TemplateParameterType::get(
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
		auto res = arg.getDefiningOp()->fold(attr);
		assert(res.succeeded());
		assert(attr.size() == 1);
		assert(not attr[0].isNull());
		toReturn.push_back(mlir::cast<mlir::Attribute>(attr[0]));
	}

	return mlir::ArrayAttr::get(getContext(), toReturn);
}

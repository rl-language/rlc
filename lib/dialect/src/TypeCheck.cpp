#include "rlc/dialect/TypeCheck.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static mlir::LogicalResult findEntityDecls(
		mlir::ModuleOp op, llvm::StringMap<mlir::rlc::EntityDeclaration>& out)
{
	for (auto entityDecl : op.getOps<mlir::rlc::EntityDeclaration>())
	{
		if (auto prevDef = out.find(entityDecl.getName()); prevDef != out.end())
		{
			entityDecl.emitOpError(
					"multiple definition of entity" + entityDecl.getName());

			prevDef->second.emitRemark("previous definition");
			return mlir::failure();
		}

		out[entityDecl.getName()] = entityDecl;
	}

	return mlir::success();
}

static mlir::LogicalResult declareEntities(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::StringMap<mlir::rlc::EntityDeclaration> decls;
	if (findEntityDecls(op, decls).failed())
		return mlir::failure();

	rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
	for (auto& pair : decls)
	{
		auto decl = pair.second;
		pair.second = rewriter.replaceOpWithNewOp<mlir::rlc::EntityDeclaration>(
				decl,
				mlir::rlc::EntityType::getIdentified(decl.getContext(), decl.getName()),
				decl.getName(),
				decl.getMemberTypes(),
				decl.getMemberNames());
	}
	return mlir::success();
}

static mlir::LogicalResult declareActionEntities(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::StringMap<mlir::rlc::EntityDeclaration> decls;
	if (findEntityDecls(op, decls).failed())
		return mlir::failure();

	rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
	for (auto action : op.getOps<mlir::rlc::ActionFunction>())
	{
		auto type = mlir::rlc::EntityType::getIdentified(
				action.getContext(), (action.getUnmangledName() + "Entity").str());

		auto entity = rewriter.create<mlir::rlc::EntityDeclaration>(
				action.getLoc(),
				type,
				rewriter.getStringAttr(type.getName()),
				rewriter.getTypeArrayAttr({}),
				rewriter.getStrArrayAttr({}));

		if (decls.count(type.getName()) != 0)
		{
			mlir::emitError(
					decls[type.getName()].getLoc(),
					"user defined type clashes with implicit action type" +
							type.getName());
			mlir::emitRemark(action.getLoc(), "action defined here");
		}

		decls.try_emplace(type.getName(), entity);
	}

	return mlir::success();
}

static mlir::LogicalResult deduceEntitiesBodies(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter rewriter(op.getContext());
	llvm::StringMap<mlir::rlc::EntityDeclaration> decls;
	if (findEntityDecls(op, decls).failed())
		return mlir::failure();

	rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
	for (auto& pair : decls)
	{
		auto decl = pair.second;

		// entities of actions are discovered later, because they require to
		// typecheck the body of the action itself.
		if (builder.isEntityOfAction(decl.getType()))
			continue;

		llvm::SmallVector<std::string> names;
		llvm::SmallVector<mlir::Type, 2> types;

		for (const auto& [field, name] :
				 llvm::zip(decl.getMemberTypes(), decl.getMemberNames()))
		{
			auto converted = builder.getConverter().convertType(
					field.cast<mlir::TypeAttr>().getValue());
			if (!converted)
			{
				decl.emitRemark("in field of entity " + decl.getName());
				return mlir::failure();
			}

			types.push_back(converted);
			names.push_back(name.cast<mlir::StringAttr>().str());
		}

		if (decl.getType()
						.cast<mlir::rlc::EntityType>()
						.setBody(types, names)
						.failed())
		{
			assert(false && "unrechable");
			return mlir::failure();
		}

		pair.second = rewriter.replaceOpWithNewOp<mlir::rlc::EntityDeclaration>(
				decl,
				mlir::rlc::EntityType::getIdentified(decl.getContext(), decl.getName()),
				decl.getName(),
				rewriter.getTypeArrayAttr(types),
				decl.getMemberNames());
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
		rewriter.setInsertionPoint(fun);

		auto deducedType =
				converter.getConverter().convertType(fun.getFunctionType());
		if (deducedType == nullptr)
		{
			fun.emitRemark("in function declaration " + fun.getUnmangledName());
			return mlir::failure();
		}
		assert(deducedType.isa<mlir::FunctionType>());

		auto newF = rewriter.create<mlir::rlc::FunctionOp>(
				fun.getLoc(),
				fun.getUnmangledName(),
				deducedType.cast<mlir::FunctionType>(),
				fun.getArgNames());
		newF.getBody().takeBody(fun.getBody());
		newF.getPrecondition().takeBody(fun.getPrecondition());
		rewriter.eraseOp(fun);
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
		mlir::rlc::SymbolTable newTable(&builder.getSymbolTable());
		if (mlir::rlc::typeCheck(
						*fun.getOperation(), rewriter, newTable, builder.getConverter())
						.failed())
			return mlir::failure();
	}
	return mlir::success();
}

static mlir::LogicalResult emitBuiltinAssignments(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::SmallVector<mlir::Type, 3> types;
	types.push_back(mlir::rlc::IntegerType::get(op.getContext()));
	types.push_back(mlir::rlc::FloatType::get(op.getContext()));
	types.push_back(mlir::rlc::BoolType::get(op.getContext()));

	for (auto type : types)
	{
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(),
				"_assign",
				mlir::FunctionType::get(op.getContext(), { type, type }, { type }),
				rewriter.getStrArrayAttr({ "arg0", "arg1" }));

		auto* block = rewriter.createBlock(
				&fun.getBody(),
				fun.getBody().begin(),
				{ type, type },
				{ op.getLoc(), op.getLoc() });

		auto zero = rewriter.create<mlir::rlc::AssignOp>(
				op.getLoc(), block->getArgument(0), block->getArgument(1));

		rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange({ zero }));
	}
	return mlir::success();
}

static mlir::LogicalResult emitBuiltinInits(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::SmallVector<mlir::Attribute, 3> attr;
	attr.push_back(rewriter.getI64IntegerAttr(0));
	attr.push_back(rewriter.getF64FloatAttr(0.0));
	attr.push_back(rewriter.getBoolAttr(false));

	llvm::SmallVector<mlir::Type, 3> types;
	types.push_back(mlir::rlc::IntegerType::get(op.getContext()));
	types.push_back(mlir::rlc::FloatType::get(op.getContext()));
	types.push_back(mlir::rlc::BoolType::get(op.getContext()));

	for (const auto& [attr, type] : llvm::zip(attr, types))
	{
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(),
				"_init",
				mlir::FunctionType::get(op.getContext(), { type }, {}),
				rewriter.getStrArrayAttr({ "arg0" }));

		auto* block = rewriter.createBlock(
				&fun.getBody(), fun.getBody().begin(), { type }, { op.getLoc() });

		auto zero = rewriter.create<mlir::rlc::Constant>(op.getLoc(), type, attr);
		rewriter.create<mlir::rlc::AssignOp>(
				op.getLoc(), block->getArgument(0), zero);

		rewriter.create<mlir::rlc::Yield>(op.getLoc());
	}
	return mlir::success();
}

static mlir::LogicalResult emitBuiltinCasts(mlir::ModuleOp op)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::SmallVector<std::string, 3> names({ "int", "float", "bool" });

	llvm::SmallVector<mlir::Type, 3> types;
	types.push_back(mlir::rlc::IntegerType::get(op.getContext()));
	types.push_back(mlir::rlc::FloatType::get(op.getContext()));
	types.push_back(mlir::rlc::BoolType::get(op.getContext()));

	for (const auto& [name, dest] : llvm::zip(names, types))
	{
		for (const auto& source : types)
		{
			if (source == dest)
				continue;

			rewriter.setInsertionPointToStart(&op.getBodyRegion().front());

			auto fun = rewriter.create<mlir::rlc::FunctionOp>(
					op.getLoc(),
					name,
					mlir::FunctionType::get(op.getContext(), { source }, { dest }),
					rewriter.getStrArrayAttr({ "arg0" }));

			auto* block = rewriter.createBlock(
					&fun.getBody(), fun.getBody().begin(), { source }, { op.getLoc() });

			auto res = rewriter.create<mlir::rlc::CastOp>(
					op.getLoc(), block->getArgument(0), dest);

			rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange({ res }));
		}
	}
	return mlir::success();
}

static mlir::LogicalResult declareImplicitAssign(mlir::ModuleOp op)
{
	auto table = mlir::rlc::makeValueTable(op);

	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs;

	for (auto entity : op.getOps<mlir::rlc::EntityDeclaration>())
	{
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
		if (auto overloads = mlir::rlc::findOverloads(
						table, "_assign", { entity.getType(), entity.getType() });
				not overloads.empty())
		{
			continue;
		}

		auto fType = mlir::FunctionType::get(
				op.getContext(),
				{ entity.getType(), entity.getType() },
				{ entity.getType() });

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(),
				"_assign",
				fType,
				rewriter.getStrArrayAttr({ "arg0", "arg1" }));

		funs.emplace_back(fun);
		table.add("_assign", fun);
	}

	return mlir::success();
}

static mlir::LogicalResult emitImplicitAssigments(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter rewriter(op.getContext());

	for (auto decl : builder.getSymbolTable().get("_assign"))
	{
		auto fun = decl.getDefiningOp<mlir::rlc::FunctionOp>();
		if (not fun.getBody().empty())
			continue;

		auto type = fun.getResultTypes().front().cast<mlir::rlc::EntityType>();
		auto* block = rewriter.createBlock(
				&fun.getBody(),
				fun.getBody().begin(),
				{ type, type },
				{ fun.getLoc(), fun.getLoc() });

		for (auto field : llvm::enumerate(type.getBody()))
		{
			auto lhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(0), field.index());
			auto rhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(1), field.index());

			if (auto type = lhs.getType().dyn_cast<mlir::rlc::ArrayType>();
					type != nullptr)
			{
				rewriter.create<mlir::rlc::ArrayCallOp>(
						fun.getLoc(),
						builder.getAssignFunctionOf(type.getUnderlying()),
						mlir::ValueRange({ lhs, rhs }));
			}
			else
			{
				rewriter.create<mlir::rlc::CallOp>(
						fun.getLoc(),
						builder.getAssignFunctionOf(lhs.getType()),
						mlir::ValueRange({ lhs, rhs }));
			}
		}

		rewriter.create<mlir::rlc::Yield>(
				op.getLoc(), mlir::ValueRange({ block->getArgument(0) }));
	}
	return mlir::success();
}

static mlir::LogicalResult declareImplicitInits(mlir::ModuleOp op)
{
	auto table = mlir::rlc::makeValueTable(op);

	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs;

	for (auto entity : op.getOps<mlir::rlc::EntityDeclaration>())
	{
		if (auto overloads =
						mlir::rlc::findOverloads(table, "_init", { entity.getType() });
				not overloads.empty())
		{
			continue;
		}

		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());

		auto fType =
				mlir::FunctionType::get(op.getContext(), { entity.getType() }, {});

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(), "_init", fType, rewriter.getStrArrayAttr({ "arg0" }));

		funs.emplace_back(fun);
		table.add("_init", fun);
	}

	return mlir::success();
}

static mlir::LogicalResult emitImplicitInit(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter rewriter(op.getContext());

	for (auto op : builder.getSymbolTable().get("_init"))
	{
		auto fun = op.getDefiningOp<mlir::rlc::FunctionOp>();
		if (not fun.getBody().empty())
			continue;

		auto type = fun.getArgumentTypes().front().cast<mlir::rlc::EntityType>();
		auto* block = rewriter.createBlock(
				&fun.getBody(), fun.getBody().begin(), { type }, { fun.getLoc() });

		for (auto field : llvm::enumerate(type.getBody()))
		{
			auto lhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(0), field.index());

			if (auto type = lhs.getType().dyn_cast<mlir::rlc::ArrayType>();
					type != nullptr)
			{
				rewriter.create<mlir::rlc::ArrayCallOp>(
						fun.getLoc(),
						builder.getInitFunctionOf(type.getUnderlying()),
						mlir::ValueRange({ lhs }));
			}
			else
			{
				rewriter.create<mlir::rlc::CallOp>(
						fun.getLoc(),
						builder.getInitFunctionOf(lhs.getType()),
						mlir::ValueRange({ lhs }));
			}
		}

		rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange());
	}
	return mlir::success();
}

static mlir::LogicalResult typeCheckActions(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::ActionFunction, 4> funs(
			op.getOps<mlir::rlc::ActionFunction>());
	for (auto fun : funs)
	{
		mlir::rlc::SymbolTable newTable(&builder.getSymbolTable());
		if (mlir::rlc::typeCheck(
						*fun.getOperation(), rewriter, newTable, builder.getConverter())
						.failed())
			return mlir::failure();
	}
	return mlir::success();
}

static mlir::LogicalResult deduceActionTypes(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::ActionFunction, 4> funs(
			op.getOps<mlir::rlc::ActionFunction>());
	for (auto fun : funs)
	{
		llvm::SmallVector<mlir::Type, 3> generatedFunctions;

		mlir::Type actionType =
				builder.getConverter().convertType(mlir::FunctionType::get(
						op.getContext(),
						fun.getFunctionType().getInputs(),
						{ builder.typeOfAction(fun) }));

		for (const auto& op : builder.actionStatementsOfAction(fun))
		{
			llvm::SmallVector<mlir::Type, 3> args({ builder.typeOfAction(fun) });

			for (auto type : op->getResultTypes())
			{
				auto converted = builder.getConverter().convertType(type);
				args.push_back(converted);
			}

			generatedFunctions.emplace_back(
					mlir::FunctionType::get(op->getContext(), args, mlir::TypeRange()));
		}

		rewriter.setInsertionPoint(fun);
		auto newAction = rewriter.create<mlir::rlc::ActionFunction>(
				fun.getLoc(),
				actionType,
				mlir::FunctionType::get(
						rewriter.getContext(),
						mlir::TypeRange({ builder.typeOfAction(fun) }),
						mlir::TypeRange(
								{ mlir::rlc::BoolType::get(rewriter.getContext()) })),
				generatedFunctions,
				fun.getUnmangledName(),
				fun.getArgNames());
		newAction.getBody().takeBody(fun.getBody());
		newAction.getPrecondition().takeBody(fun.getPrecondition());
		rewriter.eraseOp(fun);

		llvm::SmallVector<mlir::Type, 4> memberTypes;
		llvm::SmallVector<std::string, 4> memberNames;
		memberTypes.push_back(mlir::rlc::IntegerType::get(op.getContext()));
		memberNames.push_back("resume_index");

		for (auto [type, name] :
				 llvm::zip(newAction.getArgumentTypes(), newAction.getArgNames()))
		{
			memberTypes.push_back(type);
			memberNames.push_back(name.cast<mlir::StringAttr>().str());
		}

		newAction.walk([&](mlir::rlc::ActionStatement statement) {
			for (auto [type, name] :
					 llvm::zip(statement.getResults(), statement.getDeclaredNames()))
			{
				memberTypes.push_back(type.getType());
				memberNames.push_back(name.cast<mlir::StringAttr>().str());
			}
		});

		newAction.walk([&](mlir::rlc::DeclarationStatement statement) {
			memberTypes.push_back(statement.getType());
			memberNames.push_back(statement.getSymName().str());
		});

		auto res = builder.typeOfAction(fun).cast<mlir::rlc::EntityType>().setBody(
				memberTypes, memberNames);
		assert(res.succeeded());
	}

	return mlir::success();
}

void rlc::RLCTypeCheck::runOnOperation()
{
	if (emitBuiltinAssignments(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (emitBuiltinInits(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (emitBuiltinCasts(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

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

	if (deduceFunctionTypes(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (declareImplicitInits(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (declareImplicitAssign(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (deduceEntitiesBodies(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (typeCheckActions(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (deduceActionTypes(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (emitImplicitAssigments(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	if (emitImplicitInit(getOperation()).failed())
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

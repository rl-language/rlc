#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/Pass/Pass.h"
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
		if (fun.typeCheckFunctionDeclaration(rewriter, converter).failed())
			return mlir::failure();
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

			if (auto trait = casted.getTrait())
				builder.addTraitToAviableOverloads(trait);
		}
		if (mlir::rlc::typeCheck(*fun.getOperation(), builder).failed())
			return mlir::failure();
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
		auto _ = builder.addSymbolTable();
		if (mlir::rlc::typeCheck(*fun.getOperation(), builder).failed())
			return mlir::failure();
	}
	return mlir::success();
}

static mlir::LogicalResult deduceTraitTypes(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::UncheckedTraitDefinition, 4> funs(
			op.getOps<mlir::rlc::UncheckedTraitDefinition>());
	for (auto fun : funs)
	{
		if (mlir::rlc::typeCheck(*fun.getOperation(), builder).failed())
			return mlir::failure();
	}
	return mlir::success();
}

static mlir::LogicalResult deduceActionTypes(mlir::ModuleOp op)
{
	mlir::rlc::ModuleBuilder builder(op);
	mlir::IRRewriter& rewriter = builder.getRewriter();

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

namespace mlir::rlc
{
#define GEN_PASS_DEF_TYPECHECKPASS
#include "rlc/dialect/Passes.inc"

	struct TypeCheckPass: impl::TypeCheckPassBase<TypeCheckPass>
	{
		using impl::TypeCheckPassBase<TypeCheckPass>::TypeCheckPassBase;
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

			if (deduceTraitTypes(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}

			if (deduceFunctionTypes(getOperation()).failed())
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

			if (deduceOperationTypes(getOperation()).failed())
			{
				signalPassFailure();
				return;
			}
		}
	};
}	 // namespace mlir::rlc

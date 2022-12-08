#include "rlc/dialect/TypeCheck.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

static mlir::rlc::TypeTable getTypeTable(mlir::ModuleOp mod)
{
	mlir::rlc::TypeTable table;

	table.add("Int", mlir::rlc::IntegerType::get(mod.getContext()));
	table.add("Void", mlir::rlc::VoidType::get(mod.getContext()));
	table.add("Float", mlir::rlc::FloatType::get(mod.getContext()));
	table.add("Bool", mlir::rlc::BoolType::get(mod.getContext()));

	for (auto entityDecl : mod.getOps<mlir::rlc::EntityDeclaration>())
		table.add(entityDecl.getName(), entityDecl.getType());

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
					return llvm::None;
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
}

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

static mlir::LogicalResult deduceEntitiesBodies(
		mlir::ModuleOp op, mlir::TypeConverter& typeDeducer)
{
	mlir::IRRewriter rewriter(op.getContext());
	llvm::StringMap<mlir::rlc::EntityDeclaration> decls;
	if (findEntityDecls(op, decls).failed())
		return mlir::failure();

	rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
	for (auto& pair : decls)
	{
		auto decl = pair.second;
		llvm::SmallVector<std::string> names;
		llvm::SmallVector<mlir::Type, 2> types;

		for (const auto& [field, name] :
				 llvm::zip(decl.getMemberTypes(), decl.getMemberNames()))
		{
			auto converted =
					typeDeducer.convertType(field.cast<mlir::TypeAttr>().getValue());
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

static mlir::LogicalResult deduceFunctionTypes(
		mlir::ModuleOp op, mlir::TypeConverter typeDeducer)
{
	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs;
	for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
		funs.push_back(fun);

	for (auto fun : funs)
	{
		rewriter.setInsertionPoint(fun);

		auto deducedType = typeDeducer.convertType(fun.getFunctionType());
		if (deducedType == nullptr)
		{
			fun.emitRemark("in function declaration " + fun.getName());
			return mlir::failure();
		}
		assert(deducedType.isa<mlir::FunctionType>());

		auto newF = rewriter.create<mlir::rlc::FunctionOp>(
				fun.getLoc(),
				fun.getUnmangledName(),
				deducedType.cast<mlir::FunctionType>(),
				fun.getArgNames());
		newF.getBody().takeBody(fun.getBody());
		rewriter.eraseOp(fun);
	}

	return mlir::success();
}

static mlir::LogicalResult deduceOperationTypes(
		mlir::ModuleOp op, mlir::TypeConverter typeDeducer)
{
	mlir::rlc::ValueTable table;
	for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
		table.add(fun.getUnmangledName(), fun);

	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs(
			op.getOps<mlir::rlc::FunctionOp>());
	for (auto fun : funs)
	{
		mlir::rlc::SymbolTable newTable(&table);
		if (mlir::rlc::typeCheck(
						*fun.getOperation(), rewriter, newTable, typeDeducer)
						.failed())
			return mlir::failure();
	}
	return mlir::success();
}

static mlir::LogicalResult emitBuiltinAssignments(
		mlir::ModuleOp op,
		llvm::DenseMap<mlir::Type, mlir::rlc::FunctionOp>& typeToAssign)
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
		typeToAssign[type] = fun;
	}
	return mlir::success();
}

static mlir::LogicalResult emitBuiltinInits(
		mlir::ModuleOp op,
		llvm::DenseMap<mlir::Type, mlir::rlc::FunctionOp>& typeToInit)
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
		typeToInit[type] = fun;
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

static mlir::LogicalResult emitImplicitAssigments(
		mlir::ModuleOp op, mlir::rlc::ValueTable& table)
{
	llvm::DenseMap<mlir::Type, mlir::rlc::FunctionOp> typeToAssign;
	if (emitBuiltinAssignments(op, typeToAssign).failed())
		return mlir::failure();

	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs;

	for (auto entity : op.getOps<mlir::rlc::EntityDeclaration>())
	{
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
		if (auto overloads = mlir::rlc::findOverloads(
						table, "_assign", { entity.getType(), entity.getType() });
				not overloads.empty())
		{
			typeToAssign[entity.getType()] =
					overloads.front().getDefiningOp<mlir::rlc::FunctionOp>();
			overloads.front().dump();
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
		typeToAssign[entity.getType()] = fun;
		table.add("_assign", fun);
	}

	for (auto fun : funs)
	{
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
			rewriter.create<mlir::rlc::CallOp>(
					fun.getLoc(),
					typeToAssign[lhs.getType()],
					mlir::ValueRange({ lhs, rhs }));
		}

		rewriter.create<mlir::rlc::Yield>(
				op.getLoc(), mlir::ValueRange({ block->getArgument(0) }));
	}
	return mlir::success();
}

static mlir::LogicalResult emitImplicitInit(
		mlir::ModuleOp op, mlir::rlc::ValueTable& table)
{
	llvm::DenseMap<mlir::Type, mlir::rlc::FunctionOp> typeToAssign;
	if (emitBuiltinInits(op, typeToAssign).failed())
		return mlir::failure();

	mlir::IRRewriter rewriter(op.getContext());

	llvm::SmallVector<mlir::rlc::FunctionOp, 4> funs;

	for (auto entity : op.getOps<mlir::rlc::EntityDeclaration>())
	{
		if (auto overloads =
						mlir::rlc::findOverloads(table, "_init", { entity.getType() });
				not overloads.empty())
		{
			typeToAssign[entity.getType()] =
					overloads.front().getDefiningOp<mlir::rlc::FunctionOp>();
			overloads.front().dump();
			continue;
		}

		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());

		auto fType =
				mlir::FunctionType::get(op.getContext(), { entity.getType() }, {});

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(), "_init", fType, rewriter.getStrArrayAttr({ "arg0" }));

		funs.emplace_back(fun);
		typeToAssign[entity.getType()] = fun;
		table.add("_init", fun);
	}

	for (auto fun : funs)
	{
		auto type = fun.getArgumentTypes().front().cast<mlir::rlc::EntityType>();
		auto* block = rewriter.createBlock(
				&fun.getBody(), fun.getBody().begin(), { type }, { fun.getLoc() });

		for (auto field : llvm::enumerate(type.getBody()))
		{
			auto lhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(0), field.index());
			rewriter.create<mlir::rlc::CallOp>(
					fun.getLoc(), typeToAssign[lhs.getType()], mlir::ValueRange({ lhs }));
		}

		rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange());
	}
	return mlir::success();
}

void rlc::RLCTypeCheck::runOnOperation()
{
	if (declareEntities(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	mlir::rlc::TypeTable typeTable = getTypeTable(getOperation());
	mlir::TypeConverter converter;
	registerConversions(converter, typeTable);

	if (deduceEntitiesBodies(getOperation(), converter).failed())
	{
		signalPassFailure();
		return;
	}

	if (deduceFunctionTypes(getOperation(), converter).failed())
	{
		signalPassFailure();
		return;
	}

	if (emitBuiltinCasts(getOperation()).failed())
	{
		signalPassFailure();
		return;
	}

	mlir::rlc::ValueTable table;
	for (auto fun : getOperation().getOps<mlir::rlc::FunctionOp>())
		table.add(fun.getUnmangledName(), fun);

	if (emitImplicitInit(getOperation(), table).failed())
	{
		signalPassFailure();
		return;
	}

	if (emitImplicitAssigments(getOperation(), table).failed())
	{
		signalPassFailure();
		return;
	}

	if (deduceOperationTypes(getOperation(), converter).failed())
	{
		signalPassFailure();
		return;
	}
}

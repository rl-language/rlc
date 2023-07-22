#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
	static bool isBuiltinType(mlir::Type type)
	{
		return type.isa<mlir::rlc::IntegerType>() or
					 type.isa<mlir::rlc::FloatType>() or type.isa<mlir::rlc::BoolType>();
	}

	static void emitBuiltinInits(
			mlir::IRRewriter& rewriter, mlir::rlc::ConstructOp op)
	{
		rewriter.setInsertionPoint(op);
		if (auto casted = op.getType().dyn_cast<mlir::rlc::IntegerType>())
		{
			rewriter.replaceOpWithNewOp<mlir::rlc::Constant>(
					op,
					casted,
					rewriter.getIntegerAttr(
							rewriter.getIntegerType(casted.getSize()), 0));
			return;
		}
		if (op.getType().isa<mlir::rlc::FloatType>())
		{
			rewriter.replaceOpWithNewOp<mlir::rlc::Constant>(op, double(0.0));
			return;
		}
		if (op.getType().isa<mlir::rlc::BoolType>())
		{
			rewriter.replaceOpWithNewOp<mlir::rlc::Constant>(op, bool(false));
			return;
		}
	}

	static mlir::Value declareImplicitInit(
			mlir::IRRewriter& rewriter,
			mlir::rlc::ValueTable& table,
			mlir::ModuleOp op,
			mlir::Type type)
	{
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
		mlir::rlc::OverloadResolver resolver(table);
		if (auto overloads = resolver.findOverloads(
						mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(), { type });
				not overloads.empty())
			return overloads.front();

		assert(not isBuiltinType(type));
		auto fType = mlir::FunctionType::get(rewriter.getContext(), { type }, {});

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				rewriter.getUnknownLoc(),
				mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(),
				fType,
				rewriter.getStrArrayAttr({ "arg0" }));

		table.add(mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(), fun);
		return fun;
	}

	static void declareImplicitInits(mlir::ModuleOp op)
	{
		auto table = mlir::rlc::makeValueTable(op);

		mlir::IRRewriter rewriter(op.getContext());
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());

		llvm::SmallVector<mlir::rlc::ConstructOp, 2> ops;
		op.walk([&](mlir::rlc::ConstructOp op) { ops.push_back(op); });

		// emits the needed declarations for each subtypes
		for (auto init : ops)
		{
			const auto emitAllNeedSubtypes = [&](auto subtype) {
				if (isBuiltinType(subtype))
					return;
				if (isTemplateType(subtype).succeeded())
					return;

				declareImplicitInit(rewriter, table, op, subtype);
			};

			if (auto subTypes =
							init.getType().dyn_cast<mlir::SubElementTypeInterface>())
				subTypes.walkSubTypes(emitAllNeedSubtypes);
		}

		// emits the the root tyes and drops the points where they are used in favor
		// of the new function
		for (auto init : ops)
		{
			if (isBuiltinType(init.getType()))
			{
				emitBuiltinInits(rewriter, init);
				continue;
			}

			if (isTemplateType(init.getType()).succeeded())
				continue;

			auto toCall = declareImplicitInit(rewriter, table, op, init.getType());
			rewriter.setInsertionPoint(init);
			assert(toCall);
			auto newOp = rewriter.create<mlir::rlc::ExplicitConstructOp>(
					init.getLoc(), toCall);
			init.replaceAllUsesWith(newOp.getResult());
			init.erase();
		}
	}

	static void emitEntityImplicitInit(
			mlir::rlc::EntityType type,
			mlir::rlc::FunctionOp fun,
			mlir::rlc::ModuleBuilder& builder)
	{
		auto& rewriter = builder.getRewriter();

		for (auto field : llvm::enumerate(type.getBody()))
		{
			auto lhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), fun.getBody().front().getArgument(0), field.index());

			if (isBuiltinType(lhs.getType()))
			{
				auto initialValue =
						builder.getRewriter().create<mlir::rlc::ConstructOp>(
								fun.getLoc(), lhs.getType());
				auto assign = builder.getRewriter().create<mlir::rlc::BuiltinAssignOp>(
						initialValue.getLoc(), lhs, initialValue);
				emitBuiltinInits(rewriter, initialValue);
				rewriter.setInsertionPointAfter(assign);
				continue;
			}

			OverloadResolver resolver(builder.getSymbolTable());
			auto overload = resolver.instantiateOverload(
					rewriter,
					fun.getLoc(),
					builtinOperatorName<mlir::rlc::InitOp>(),
					mlir::ValueRange({ lhs }));
			rewriter.create<mlir::rlc::CallOp>(
					fun.getLoc(), overload, mlir::ValueRange({ lhs }));
		}
	}

	static void emitImplicitInitArray(
			mlir::rlc::ArrayType type,
			mlir::rlc::FunctionOp fun,
			mlir::rlc::ModuleBuilder& builder)
	{
		auto& rewriter = builder.getRewriter();

		auto* block = &fun.getBody().front();
		rewriter.setInsertionPointToEnd(block);
		auto lhs = block->getArgument(0);

		auto decl = rewriter.create<mlir::rlc::DeclarationStatement>(
				fun.getLoc(),
				mlir::rlc::IntegerType::getInt64(fun.getContext()),
				"counter");
		auto* counterInitializerBB =
				rewriter.createBlock(&decl.getBody(), decl.getBody().begin());
		rewriter.setInsertionPoint(
				counterInitializerBB, counterInitializerBB->begin());
		auto zero = rewriter.create<mlir::rlc::Constant>(fun.getLoc(), int64_t(0));
		rewriter.create<mlir::rlc::Yield>(fun.getLoc(), mlir::ValueRange({ zero }));

		rewriter.setInsertionPointAfter(decl);
		auto whileStatemet =
				rewriter.create<mlir::rlc::WhileStatement>(fun.getLoc());
		auto* condition = rewriter.createBlock(
				&whileStatemet.getCondition(), whileStatemet.getCondition().begin());

		rewriter.setInsertionPoint(condition, condition->begin());
		auto arraySize = rewriter.create<mlir::rlc::Constant>(
				fun.getLoc(), int64_t(type.getSize()));
		auto comparison =
				rewriter.create<mlir::rlc::LessOp>(fun.getLoc(), decl, arraySize);
		rewriter.create<mlir::rlc::Yield>(
				fun.getLoc(), mlir::ValueRange({ comparison }));

		auto* loop = rewriter.createBlock(
				&whileStatemet.getBody(), whileStatemet.getBody().begin());
		rewriter.setInsertionPoint(loop, loop->begin());

		auto lhsElem =
				rewriter.create<mlir::rlc::ArrayAccess>(fun.getLoc(), lhs, decl);

		if (isBuiltinType(lhsElem.getType()))
		{
			auto initialValue = builder.getRewriter().create<mlir::rlc::ConstructOp>(
					fun.getLoc(), lhsElem.getType());
			auto assign = builder.getRewriter().create<mlir::rlc::BuiltinAssignOp>(
					initialValue.getLoc(), lhsElem, initialValue);
			emitBuiltinInits(rewriter, initialValue);
			rewriter.setInsertionPointToEnd(loop);
		}
		else if (auto call = builder.emitCall(
								 fun,
								 mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(),
								 mlir::ValueRange({ lhsElem }));
						 not call)
		{
			llvm_unreachable("unrechable");
		}

		auto oneConstant =
				rewriter.create<mlir::rlc::Constant>(fun.getLoc(), int64_t(1));
		auto result =
				rewriter.create<mlir::rlc::AddOp>(fun.getLoc(), decl, oneConstant);
		rewriter.create<mlir::rlc::BuiltinAssignOp>(fun.getLoc(), decl, result);

		rewriter.create<mlir::rlc::Yield>(fun.getLoc(), mlir::ValueRange({}));
		rewriter.setInsertionPointAfter(whileStatemet);
	}

	static void emitImplicitInits(
			mlir::rlc::ModuleBuilder& builder, mlir::ModuleOp op)
	{
		mlir::IRRewriter& rewriter = builder.getRewriter();

		for (auto op : builder.getSymbolTable().get(
						 mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>()))
		{
			auto fun = op.getDefiningOp<mlir::rlc::FunctionOp>();
			if (not fun.getBody().empty())
				continue;

			auto type = fun.getArgumentTypes().front();
			auto* block = rewriter.createBlock(
					&fun.getBody(), fun.getBody().begin(), { type }, { fun.getLoc() });
			rewriter.setInsertionPointToStart(block);
			if (auto entityType = type.dyn_cast<mlir::rlc::EntityType>())
			{
				emitEntityImplicitInit(entityType, fun, builder);
			}
			else if (auto arrayType = type.dyn_cast<mlir::rlc::ArrayType>())
			{
				emitImplicitInitArray(arrayType, fun, builder);
			}
			rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange());
		}
	}

	void emitImplicitInits(mlir::ModuleOp op)
	{
		declareImplicitInits(op);
		mlir::rlc::ModuleBuilder builder(op);
		emitImplicitInits(builder, op);
	}

#define GEN_PASS_DEF_EMITIMPLICITINITPASS
#include "rlc/dialect/Passes.inc"
	struct EmitImplicitInitPass
			: impl::EmitImplicitInitPassBase<EmitImplicitInitPass>
	{
		using impl::EmitImplicitInitPassBase<
				EmitImplicitInitPass>::EmitImplicitInitPassBase;

		void runOnOperation() override { emitImplicitInits(getOperation()); }
	};

}	 // namespace mlir::rlc

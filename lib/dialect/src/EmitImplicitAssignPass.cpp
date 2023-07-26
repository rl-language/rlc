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
	static mlir::rlc::FunctionOp declareImplicitAssign(
			mlir::IRRewriter& rewriter,
			mlir::rlc::ValueTable& table,
			mlir::ModuleOp op,
			mlir::Type type)
	{
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
		mlir::rlc::OverloadResolver resolver(table);
		if (auto overloads = resolver.findOverloads(
						mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
						{ type, type });
				not overloads.empty())
		{
			return overloads.front().getDefiningOp<mlir::rlc::FunctionOp>();
		}

		assert(not isBuiltinType(type));

		auto fType =
				mlir::FunctionType::get(op.getContext(), { type, type }, { type });

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(),
				mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
				fType,
				rewriter.getStrArrayAttr({ "arg0", "arg1" }));

		table.add(mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(), fun);
		return fun;
	}

	static void declareImplicitAssigns(mlir::ModuleOp op)
	{
		mlir::rlc::ModuleBuilder builder(op);

		llvm::SmallVector<mlir::rlc::ImplicitAssignOp, 2> ops;
		op.walk([&](mlir::rlc::ImplicitAssignOp op) { ops.push_back(op); });

		// emits the needed declarations for each subtypes
		for (auto assign : ops)
		{
			assert(assign.getLhs().getType() != nullptr);
			assert(assign.getRhs().getType() != nullptr);
			const auto emitAllNeedSubtypes = [&](auto subtype) {
				if (isBuiltinType(subtype) or
						subtype.template isa<mlir::rlc::OwningPtrType>() or
						isTemplateType(subtype).succeeded() or
						subtype.template isa<mlir::rlc::IntegerLiteralType>())
					return;

				declareImplicitAssign(
						builder.getRewriter(), builder.getSymbolTable(), op, subtype);
			};

			if (auto subTypes =
							assign.getType().dyn_cast<mlir::SubElementTypeInterface>())
				subTypes.walkSubTypes(emitAllNeedSubtypes);
		}

		// emits the the root tyes and drops the points where they are used in favor
		// of the new function
		for (auto assign : ops)
		{
			if (isTemplateType(assign.getType()).succeeded())
				continue;

			auto toCall = declareImplicitAssign(
					builder.getRewriter(),
					builder.getSymbolTable(),
					op,
					assign.getType());

			builder.getRewriter().setInsertionPoint(assign);
			builder.getRewriter().replaceOpWithNewOp<mlir::rlc::CallOp>(
					assign,
					toCall,
					mlir::ValueRange({ assign.getLhs(), assign.getRhs() }));
		}
	}

	static void emitImplicitAssignArray(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getResultTypes().front();
		auto& rewriter = builder.getRewriter();
		auto type = resType.dyn_cast<mlir::rlc::ArrayType>();

		auto* block = &fun.getBody().front();
		rewriter.setInsertionPointToEnd(block);
		auto lhs = block->getArgument(0);
		auto rhs = block->getArgument(1);

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
				fun.getLoc(), int64_t(type.getArraySize()));
		auto comparison =
				rewriter.create<mlir::rlc::LessOp>(fun.getLoc(), decl, arraySize);
		rewriter.create<mlir::rlc::Yield>(
				fun.getLoc(), mlir::ValueRange({ comparison }));

		auto* loop = rewriter.createBlock(
				&whileStatemet.getBody(), whileStatemet.getBody().begin());
		rewriter.setInsertionPoint(loop, loop->begin());

		auto lhsElem =
				rewriter.create<mlir::rlc::ArrayAccess>(fun.getLoc(), lhs, decl);
		auto rhsElem =
				rewriter.create<mlir::rlc::ArrayAccess>(fun.getLoc(), rhs, decl);

		if (auto call = builder.emitCall(
						fun,
						mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
						mlir::ValueRange({ lhsElem, rhsElem }));
				not call)
			return;

		auto oneConstant =
				rewriter.create<mlir::rlc::Constant>(fun.getLoc(), int64_t(1));
		auto result =
				rewriter.create<mlir::rlc::AddOp>(fun.getLoc(), decl, oneConstant);
		rewriter.create<mlir::rlc::BuiltinAssignOp>(fun.getLoc(), decl, result);

		rewriter.create<mlir::rlc::Yield>(fun.getLoc(), mlir::ValueRange({}));
	}

	static void emitImplicitAssignEntity(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getResultTypes().front();
		auto& rewriter = builder.getRewriter();
		auto type = resType.dyn_cast<mlir::rlc::EntityType>();

		auto* block = &fun.getBody().front();
		rewriter.setInsertionPointToEnd(block);
		for (auto field : llvm::enumerate(type.getBody()))
		{
			auto lhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(0), field.index());
			auto rhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(1), field.index());

			builder.emitCall(
					fun,
					mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
					mlir::ValueRange({ lhs, rhs }));
		}
	}

	static void emitImplicitAssign(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto& rewriter = builder.getRewriter();
		if (not fun.getBody().empty())
			return;

		auto resType = fun.getResultTypes().front();

		auto* block = rewriter.createBlock(
				&fun.getBody(),
				fun.getBody().begin(),
				{ resType, resType },
				{ fun.getLoc(), fun.getLoc() });

		if (auto arraytype = resType.dyn_cast<mlir::rlc::ArrayType>())
		{
			emitImplicitAssignArray(builder, fun);
		}
		else if (auto type = resType.dyn_cast<mlir::rlc::EntityType>())
		{
			emitImplicitAssignEntity(builder, fun);
		}
		else
		{
			resType.dump();
			assert(false && "unrechable");
		}

		rewriter.setInsertionPointToEnd(block);
		rewriter.create<mlir::rlc::Yield>(
				fun.getLoc(), mlir::ValueRange({ block->getArgument(0) }));
	}

	static void emitImplicitAssigments(mlir::ModuleOp op)
	{
		mlir::rlc::ModuleBuilder builder(op);
		mlir::IRRewriter& rewriter = builder.getRewriter();

		for (auto decl : builder.getSymbolTable().get(
						 mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>()))
			emitImplicitAssign(builder, decl.getDefiningOp<mlir::rlc::FunctionOp>());
	}

	void emitImplicitAssign(mlir::ModuleOp op)
	{
		declareImplicitAssigns(op);
		emitImplicitAssigments(op);
	}

#define GEN_PASS_DEF_EMITIMPLICITASSIGNPASS
#include "rlc/dialect/Passes.inc"

	struct EmitImplicitAssignPass
			: impl::EmitImplicitAssignPassBase<EmitImplicitAssignPass>
	{
		using impl::EmitImplicitAssignPassBase<
				EmitImplicitAssignPass>::EmitImplicitAssignPassBase;

		void runOnOperation() override { emitImplicitAssign(getOperation()); }
	};

}	 // namespace mlir::rlc

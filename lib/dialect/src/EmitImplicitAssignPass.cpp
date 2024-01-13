/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
*/
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

		if (auto alternative = type.dyn_cast<mlir::rlc::AlternativeType>())
		{
			for (auto field : alternative.getUnderlying())
			{
				auto fType =
						mlir::FunctionType::get(op.getContext(), { type, field }, { type });
				auto fun = rewriter.create<mlir::rlc::FunctionOp>(
						op.getLoc(),
						mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
						fType,
						rewriter.getStrArrayAttr({ "arg0", "arg1" }));

				table.add(mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(), fun);
			}
		}

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
		llvm::SmallVector<mlir::Type, 2> types;
		op.walk([&](mlir::rlc::ImplicitAssignOp op) {
			ops.push_back(op);
			types.push_back(op.getType());
		});
		op.walk([&](mlir::rlc::EntityDeclaration op) {
			types.push_back(op.getType());
		});
		op.walk([&](mlir::rlc::ActionFunction op) {
			types.push_back(op.getEntityType());
		});

		for (auto type : types)
		{
			const auto emitAllNeedSubtypes = [&](mlir::Type subtype) {
				if (isBuiltinType(subtype) or
						subtype.template isa<mlir::rlc::OwningPtrType>() or
						isTemplateType(subtype).succeeded() or
						subtype.template isa<mlir::rlc::IntegerLiteralType>())
					return;

				auto toCall = declareImplicitAssign(
						builder.getRewriter(), builder.getSymbolTable(), op, subtype);
			};

			type.walk(emitAllNeedSubtypes);
			emitAllNeedSubtypes(type);
		}

		// emits the the root tyes and drops the points where they are used in favor
		// of the new function
		for (auto assign : ops)
		{
			assert(assign.getLhs().getType() != nullptr);
			assert(assign.getRhs().getType() != nullptr);
			if (isTemplateType(assign.getType()).succeeded())
				continue;

			builder.getRewriter().setInsertionPoint(assign);
			auto* result = builder.emitCall(
					assign,
					mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
					{ assign.getLhs(), assign.getRhs() });
			builder.getRewriter().replaceOp(assign, result->getResult(0));
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

	// basically emit
	// fun assign(X | Y | Z... arg1, Y arg2):
	//   if not arg1 is Y:
	//     destroy arg1
	//     setActive arg1, Y
	//     casted = upcast Y, arg1
	//     init casted
	//   casted = upcast Y, arg1
	//   arg1 = arg2
	static void emitImplicitAssignAlternatveField(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getResultTypes().front();
		auto& rewriter = builder.getRewriter();
		auto type = resType.dyn_cast<mlir::rlc::AlternativeType>();
		auto* block = &fun.getBody().front();
		auto toAssignType = block->getArgumentTypes()[1];

		size_t outputResultingIndex = std::distance(
				type.getUnderlying().begin(),
				llvm::find(type.getUnderlying(), toAssignType));

		rewriter.setInsertionPointToEnd(block);

		auto ifStatement = rewriter.create<mlir::rlc::IfStatement>(fun.getLoc());
		auto* condition = rewriter.createBlock(&ifStatement.getCondition());
		rewriter.setInsertionPointToEnd(condition);
		auto isThisField = rewriter.create<mlir::rlc::IsOp>(
				fun.getLoc(), block->getArgument(0), toAssignType);

		auto isItNotThisField =
				rewriter.create<mlir::rlc::NotOp>(fun.getLoc(), isThisField);
		rewriter.create<mlir::rlc::Yield>(
				fun.getLoc(), mlir::ValueRange({ isItNotThisField }));

		auto* trueBranch = rewriter.createBlock(&ifStatement.getTrueBranch());
		rewriter.setInsertionPointToEnd(trueBranch);
		rewriter.create<mlir::rlc::DestroyOp>(fun.getLoc(), block->getArgument(0));
		rewriter.create<mlir::rlc::SetActiveEntryOp>(
				fun.getLoc(), block->getArgument(0), outputResultingIndex);
		if (not isBuiltinType(toAssignType))
		{
			auto casted = rewriter.create<mlir::rlc::ValueUpcastOp>(
					fun.getLoc(), toAssignType, block->getArgument(1));
			auto contructed = rewriter.create<mlir::rlc::ConstructOp>(
					fun.getLoc(), casted.getResult().getType());
			assert(isTemplateType(contructed.getType()).failed());
			auto* call = builder.emitCall(
					fun,
					mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
					mlir::ValueRange({ casted, contructed }));
			if (call == nullptr)
				abort();
		}

		rewriter.create<mlir::rlc::Yield>(fun.getLoc());
		auto* elseBranch = rewriter.createBlock(&ifStatement.getElseBranch());
		rewriter.setInsertionPointToEnd(elseBranch);
		rewriter.create<mlir::rlc::Yield>(fun.getLoc());

		rewriter.setInsertionPointToEnd(block);
		auto castedAgain = rewriter.create<mlir::rlc::ValueUpcastOp>(
				fun.getLoc(), toAssignType, block->getArgument(0));

		builder.emitCall(
				fun,
				mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
				mlir::ValueRange({ castedAgain, block->getArgument(1) }));
	}

	// Iterate on every possible alternative of the right and side, if it is the
	// the current active possibility, dispatch to the other emitted assign
	// operation that handles that in the specific
	static void emitImplicitAssignAlternatve(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getResultTypes().front();
		auto& rewriter = builder.getRewriter();
		auto type = resType.dyn_cast<mlir::rlc::AlternativeType>();

		auto* block = &fun.getBody().front();
		for (auto field : type.getUnderlying())
		{
			rewriter.setInsertionPointToEnd(block);
			auto ifStatement = rewriter.create<mlir::rlc::IfStatement>(fun.getLoc());
			auto* condition = rewriter.createBlock(&ifStatement.getCondition());
			rewriter.setInsertionPointToEnd(condition);
			auto isThisField = rewriter.create<mlir::rlc::IsOp>(
					fun.getLoc(), block->getArgument(1), field);
			rewriter.create<mlir::rlc::Yield>(
					fun.getLoc(), mlir::ValueRange({ isThisField }));

			auto* trueBranch = rewriter.createBlock(&ifStatement.getTrueBranch());
			rewriter.setInsertionPointToEnd(trueBranch);
			auto casted = rewriter.create<mlir::rlc::ValueUpcastOp>(
					fun.getLoc(), field, block->getArgument(1));

			builder.emitCall(
					fun,
					mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
					mlir::ValueRange({ block->getArgument(0), casted }));
			rewriter.create<mlir::rlc::Yield>(fun.getLoc());

			auto* elseBranch = rewriter.createBlock(&ifStatement.getElseBranch());
			rewriter.setInsertionPointToEnd(elseBranch);
			rewriter.create<mlir::rlc::Yield>(fun.getLoc());
		}
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
				fun.getArgumentTypes(),
				{ fun.getLoc(), fun.getLoc() });

		if (auto arraytype = resType.dyn_cast<mlir::rlc::ArrayType>())
		{
			emitImplicitAssignArray(builder, fun);
		}
		else if (auto type = resType.dyn_cast<mlir::rlc::EntityType>())
		{
			emitImplicitAssignEntity(builder, fun);
		}
		else if (auto type = resType.dyn_cast<mlir::rlc::AlternativeType>())
		{
			if (type == fun.getType().getInput(1))
				emitImplicitAssignAlternatve(builder, fun);
			else
				emitImplicitAssignAlternatveField(builder, fun);
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

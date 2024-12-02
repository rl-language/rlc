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
						rewriter.getUnknownLoc(),
						true,
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
				auto fType = mlir::FunctionType::get(
						op.getContext(),
						{ type, field },
						{ mlir::rlc::VoidType::get(rewriter.getContext()) });
				auto fun = rewriter.create<mlir::rlc::FunctionOp>(
						op.getLoc(),
						mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
						fType,
						mlir::rlc::FunctionInfoAttr::get(
								fType.getContext(), { "arg0", "arg1" }),
						true);

				table.add(mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(), fun);
			}
		}

		auto fType = mlir::FunctionType::get(
				op.getContext(),
				{ type, type },
				{ mlir::rlc::VoidType::get(rewriter.getContext()) });

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(),
				mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
				fType,
				mlir::rlc::FunctionInfoAttr::get(
						fType.getContext(), { "arg0", "arg1" }),
				true);

		table.add(mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(), fun);
		return fun;
	}

	static mlir::LogicalResult declareImplicitAssigns(mlir::ModuleOp op)
	{
		mlir::rlc::ModuleBuilder builder(op);

		llvm::SmallVector<mlir::rlc::ImplicitAssignOp, 2> ops;
		llvm::SmallVector<mlir::Type, 2> types;
		op.walk([&](mlir::rlc::ImplicitAssignOp op) {
			ops.push_back(op);
			types.push_back(op.getLhs().getType());
		});
		op.walk(
				[&](mlir::rlc::TypeAliasOp op) { types.push_back(op.getAliased()); });
		op.walk(
				[&](mlir::rlc::ClassDeclaration op) { types.push_back(op.getType()); });
		op.walk([&](mlir::rlc::ActionFunction op) {
			types.push_back(op.getClassType());
		});

		for (auto type : types)
		{
			const auto emitAllNeedSubtypes = [&](mlir::Type subtype) {
				if (isBuiltinType(subtype) or
						subtype.template isa<mlir::rlc::OwningPtrType>() or
						isTemplateType(subtype).succeeded() or
						subtype.template isa<mlir::rlc::IntegerLiteralType>() or
						subtype.template isa<mlir::rlc::TraitMetaType>() or
						subtype.template isa<mlir::rlc::VoidType>())
					return;

				auto toCall = declareImplicitAssign(
						builder.getRewriter(), builder.getSymbolTable(), op, subtype);
				if (isTemplateType(toCall.getType()).succeeded())
					builder.getRewriter().create<mlir::rlc::TemplateInstantiationOp>(
							toCall.getLoc(),
							mlir::FunctionType::get(
									type.getContext(),
									{ subtype, subtype },
									{ mlir::rlc::VoidType::get(type.getContext()) }),
							toCall);
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
			if (isTemplateType(assign.getLhs().getType()).succeeded())
				continue;

			if (isBuiltinType(assign.getLhs().getType()) and
					isBuiltinType(assign.getRhs().getType()) and
					assign.getLhs().getType() != assign.getRhs().getType())
			{
				return logError(
						assign,
						"Cannot assign " +
								mlir::rlc::prettyType(assign.getRhs().getType()) + " to " +
								mlir::rlc::prettyType(assign.getLhs().getType()) +
								". Cast it instead.");
			}

			builder.getRewriter().setInsertionPoint(assign);
			auto* result = builder.emitCall(
					assign,
					true,
					mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
					{ assign.getLhs(), assign.getRhs() },
					true,
					false);
			if (!result)
				return mlir::failure();
			assign.erase();
		}
		return mlir::success();
	}

	static void emitMemMove(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getArgumentTypes().front();
		auto& rewriter = builder.getRewriter();

		auto* block = &fun.getBody().front();
		rewriter.setInsertionPointToEnd(block);
		auto lhs = block->getArgument(0);
		auto rhs = block->getArgument(1);

		rewriter.create<mlir::rlc::MemMove>(fun.getLoc(), lhs, rhs);
	}

	static void emitImplicitAssignArray(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getArgumentTypes().front();
		auto& rewriter = builder.getRewriter();
		auto type = resType.dyn_cast<mlir::rlc::ArrayType>();

		auto* block = &fun.getBody().front();
		rewriter.setInsertionPointToEnd(block);
		auto lhs = block->getArgument(0);
		auto rhs = block->getArgument(1);

		auto decl = rewriter.create<mlir::rlc::Constant>(fun.getLoc(), int64_t(0));
		auto zero = rewriter.create<mlir::rlc::Constant>(fun.getLoc(), int64_t(0));
		rewriter.create<mlir::rlc::BuiltinAssignOp>(fun.getLoc(), decl, zero);

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
						true,
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
		auto resType = fun.getArgumentTypes().front();
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
					fun.getLoc(), toAssignType, block->getArgument(0));
			auto contructed = rewriter.create<mlir::rlc::ConstructOp>(
					fun.getLoc(), casted.getResult().getType());
			assert(isTemplateType(contructed.getType()).failed());
			auto* call = builder.emitCall(
					fun,
					true,
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
				true,
				mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
				mlir::ValueRange({ castedAgain, block->getArgument(1) }));
	}

	// Iterate on every possible alternative of the right and side, if it is the
	// the current active possibility, dispatch to the other emitted assign
	// operation that handles that in the specific
	static void emitImplicitAssignAlternatve(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getArgumentTypes().front();
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
					true,
					mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
					mlir::ValueRange({ block->getArgument(0), casted }));
			rewriter.create<mlir::rlc::Yield>(fun.getLoc());

			auto* elseBranch = rewriter.createBlock(&ifStatement.getElseBranch());
			rewriter.setInsertionPointToEnd(elseBranch);
			rewriter.create<mlir::rlc::Yield>(fun.getLoc());
		}
	}

	static void emitImplicitAssignClass(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto resType = fun.getArgumentTypes().front();
		auto& rewriter = builder.getRewriter();
		auto type = resType.dyn_cast<mlir::rlc::ClassType>();

		auto* block = &fun.getBody().front();
		rewriter.setInsertionPointToEnd(block);
		for (auto field : llvm::enumerate(type.getMembers()))
		{
			auto lhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(0), field.index());
			auto rhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), block->getArgument(1), field.index());

			builder.emitCall(
					fun,
					true,
					mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>(),
					mlir::ValueRange({ lhs, rhs }));
		}
	}

	static bool isTriviallyCopiable(mlir::Type t)
	{
		bool leafesAreAllPrimitiveTypes = true;
		t.walk([&](mlir::Type inner) {
			if (inner.isa<mlir::rlc::ClassType>())
				leafesAreAllPrimitiveTypes = false;
		});
		if (t.isa<mlir::rlc::ClassType>())
			leafesAreAllPrimitiveTypes = false;

		return leafesAreAllPrimitiveTypes;
	}

	static void emitImplicitAssign(
			mlir::rlc::ModuleBuilder& builder, mlir::rlc::FunctionOp fun)
	{
		auto& rewriter = builder.getRewriter();
		if (not fun.getBody().empty())
			return;
		if (fun.getType().getNumInputs() != 2)
			return;

		auto lhs = fun.getArgumentTypes().front();

		auto* block = rewriter.createBlock(
				&fun.getBody(),
				fun.getBody().begin(),
				fun.getArgumentTypes(),
				{ fun.getLoc(), fun.getLoc() });

		// if we are assining a field of a alternative to the alternative itself,
		// emit a special assign operator
		if (auto type = lhs.dyn_cast<mlir::rlc::AlternativeType>();
				type and type != fun.getType().getInput(1))
		{
			emitImplicitAssignAlternatveField(builder, fun);
		}
		else if (isTriviallyCopiable(lhs))
		{
			emitMemMove(builder, fun);
		}
		else if (auto arraytype = lhs.dyn_cast<mlir::rlc::ArrayType>())
		{
			emitImplicitAssignArray(builder, fun);
		}
		else if (auto type = lhs.dyn_cast<mlir::rlc::ClassType>())
		{
			emitImplicitAssignClass(builder, fun);
		}
		else if (auto type = lhs.dyn_cast<mlir::rlc::AlternativeType>())
		{
			emitImplicitAssignAlternatve(builder, fun);
		}
		else
		{
			lhs.dump();
			assert(false && "unrechable");
		}

		rewriter.setInsertionPointToEnd(block);
		rewriter.create<mlir::rlc::Yield>(fun.getLoc(), mlir::ValueRange({}));
	}

	static void emitImplicitAssigments(mlir::ModuleOp op)
	{
		mlir::rlc::ModuleBuilder builder(op);
		mlir::IRRewriter& rewriter = builder.getRewriter();

		for (auto decl : builder.getSymbolTable().get(
						 mlir::rlc::builtinOperatorName<mlir::rlc::AssignOp>()))
			emitImplicitAssign(builder, decl.getDefiningOp<mlir::rlc::FunctionOp>());
	}

	mlir::LogicalResult emitImplicitAssign(mlir::ModuleOp op)
	{
		if (declareImplicitAssigns(op).failed())
			return mlir::failure();
		emitImplicitAssigments(op);
		return mlir::success();
	}

#define GEN_PASS_DEF_EMITIMPLICITASSIGNPASS
#include "rlc/dialect/Passes.inc"

	struct EmitImplicitAssignPass
			: impl::EmitImplicitAssignPassBase<EmitImplicitAssignPass>
	{
		using impl::EmitImplicitAssignPassBase<
				EmitImplicitAssignPass>::EmitImplicitAssignPassBase;

		void runOnOperation() override
		{
			if (emitImplicitAssign(getOperation()).failed())
				signalPassFailure();
		}
	};

}	 // namespace mlir::rlc

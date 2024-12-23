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
					 type.isa<mlir::rlc::FloatType>() or
					 type.isa<mlir::rlc::BoolType>() or
					 type.isa<mlir::rlc::OwningPtrType>();
	}

	static mlir::Value emitBuiltinZero(
			mlir::IRRewriter& rewriter, mlir::Type t, mlir::Location loc)
	{
		if (auto casted = t.dyn_cast<mlir::rlc::IntegerType>())
		{
			return rewriter.create<mlir::rlc::Constant>(
					loc,
					casted,
					rewriter.getIntegerAttr(
							rewriter.getIntegerType(casted.getSize()), 0));
		}
		if (t.isa<mlir::rlc::FloatType>())
		{
			return rewriter.create<mlir::rlc::Constant>(loc, double(0.0));
		}
		if (t.isa<mlir::rlc::BoolType>())
		{
			return rewriter.create<mlir::rlc::Constant>(loc, bool(false));
		}
		if (t.isa<mlir::rlc::OwningPtrType>())
		{
			return rewriter.create<mlir::rlc::NullOp>(loc, t);
		}
		t.dump();
		llvm_unreachable("unrechable");
		return nullptr;
	}

	static void emitBuiltinInits(
			mlir::IRRewriter& rewriter, mlir::rlc::ConstructOp op)
	{
		rewriter.setInsertionPoint(op);
		rewriter.replaceOp(
				op, emitBuiltinZero(rewriter, op.getType(), op.getLoc()));
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
						rewriter.getUnknownLoc(),
						true,
						mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(),
						{ type });
				not overloads.empty())
			return overloads.front();

		assert(not isBuiltinType(type));
		auto fType = mlir::FunctionType::get(rewriter.getContext(), { type }, {});

		auto fun = rewriter.create<mlir::rlc::FunctionOp>(
				op.getLoc(),
				mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(),
				fType,
				mlir::rlc::FunctionInfoAttr::get(op.getContext(), { "arg0" }),
				true);

		table.add(mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>(), fun);
		return fun;
	}

	static void declareImplicitInits(mlir::ModuleOp op)
	{
		auto table = mlir::rlc::makeValueTable(op);

		mlir::IRRewriter rewriter(op.getContext());
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());

		llvm::SmallVector<mlir::rlc::ConstructOp, 2> ops;
		llvm::SmallVector<mlir::rlc::InplaceInitializeOp, 2> inplaceInitialize;

		llvm::SmallVector<mlir::Type, 2> types;
		op.walk([&](mlir::rlc::ConstructOp op) {
			ops.push_back(op);
			types.push_back(op.getType());
		});

		op.walk(
				[&](mlir::rlc::TypeAliasOp op) { types.push_back(op.getAliased()); });

		op.walk([&](mlir::rlc::InplaceInitializeOp op) {
			inplaceInitialize.push_back(op);
			types.push_back(op.getValue().getType());
		});

		op.walk(
				[&](mlir::rlc::ClassDeclaration op) { types.push_back(op.getType()); });

		op.walk([&](mlir::rlc::ActionFunction op) {
			types.push_back(op.getClassType());
		});

		llvm::DenseMap<mlir::Type, mlir::Value> typeToFunction;

		// emits the needed declarations for each subtypes
		for (auto type : types)
		{
			const auto emitAllNeedSubtypes = [&](mlir::Type subtype) {
				if (isBuiltinType(subtype))
					return;
				if (isTemplateType(subtype).succeeded())
					return;
				if (subtype.isa<mlir::rlc::TraitMetaType>())
					return;
				if (subtype.isa<mlir::rlc::VoidType>())
					return;
				if (subtype.template isa<mlir::rlc::IntegerLiteralType>())
					return;

				auto toCall = declareImplicitInit(rewriter, table, op, subtype);
				typeToFunction[type] = toCall;
			};

			type.walk(emitAllNeedSubtypes);
			emitAllNeedSubtypes(type);
		}

		// emits the the root tyes and drops the points where they are used in
		// favor of the new function
		for (auto init : ops)
		{
			if (isBuiltinType(init.getType()))
			{
				emitBuiltinInits(rewriter, init);
				continue;
			}

			if (isTemplateType(init.getType()).succeeded())
				continue;

			rewriter.setInsertionPoint(init);
			auto toCall = typeToFunction[init.getType()];
			assert(toCall);

			if (isTemplateType(toCall.getType()).succeeded())
			{
				auto castedType = toCall.getType().cast<mlir::FunctionType>();
				toCall = rewriter.create<mlir::rlc::TemplateInstantiationOp>(
						init.getLoc(),
						mlir::FunctionType::get(
								toCall.getContext(),
								{ init.getType() },
								castedType.getResults()),
						toCall);
			}

			auto newOp = rewriter.create<mlir::rlc::ExplicitConstructOp>(
					init.getLoc(), toCall);
			init.replaceAllUsesWith(newOp.getResult());
			init.erase();
		}

		for (auto init : inplaceInitialize)
		{
			if (isBuiltinType(init.getValue().getType()))
			{
				rewriter.setInsertionPoint(init);
				auto zero =
						emitBuiltinZero(rewriter, init.getValue().getType(), init.getLoc());
				rewriter.create<mlir::rlc::BuiltinAssignOp>(
						init.getLoc(), init.getValue(), zero);

				init.erase();
				continue;
			}

			if (isTemplateType(init.getValue().getType()).succeeded())
				continue;

			rewriter.setInsertionPoint(init);
			auto toCall = typeToFunction[init.getValue().getType()];
			assert(toCall);

			if (isTemplateType(toCall.getType()).succeeded())
			{
				auto castedType = toCall.getType().cast<mlir::FunctionType>();
				toCall = rewriter.create<mlir::rlc::TemplateInstantiationOp>(
						init.getLoc(),
						mlir::FunctionType::get(
								toCall.getContext(),
								{ init.getValue().getType() },
								castedType.getResults()),
						toCall);
			}

			auto newOp = rewriter.create<mlir::rlc::CallOp>(
					init.getLoc(), toCall, true, mlir::ValueRange({ init.getValue() }));
			init.erase();
		}
	}

	static void initValue(
			mlir::Value lhs, ModuleBuilder& builder, mlir::Location loc)
	{
		mlir::IRRewriter& rewriter = builder.getRewriter();
		if (isBuiltinType(lhs.getType()))
		{
			auto initialValue = builder.getRewriter().create<mlir::rlc::ConstructOp>(
					loc, lhs.getType());
			auto assign = builder.getRewriter().create<mlir::rlc::BuiltinAssignOp>(
					initialValue.getLoc(), lhs, initialValue);
			emitBuiltinInits(rewriter, initialValue);
			rewriter.setInsertionPointAfter(assign);
			return;
		}

		auto op = builder.emitCall(
				lhs.getDefiningOp(),
				true,
				builtinOperatorName<mlir::rlc::InitOp>(),
				mlir::ValueRange({ lhs }));
		if (not op)
			abort();
	}

	static void emitClassImplicitInit(
			mlir::rlc::ClassType type,
			mlir::rlc::FunctionOp fun,
			mlir::rlc::ModuleBuilder& builder)
	{
		auto& rewriter = builder.getRewriter();

		for (auto field : llvm::enumerate(type.getMembers()))
		{
			auto lhs = rewriter.create<mlir::rlc::MemberAccess>(
					fun.getLoc(), fun.getBody().front().getArgument(0), field.index());

			initValue(lhs, builder, fun.getLoc());
		}
	}

	static void emitMemSetZero(
			mlir::Type type,
			mlir::rlc::FunctionOp fun,
			mlir::rlc::ModuleBuilder& builder)
	{
		auto& rewriter = builder.getRewriter();

		auto lhs = rewriter.create<mlir::rlc::MemSetZero>(
				fun.getLoc(), fun.getBody().front().getArgument(0));
	}

	static void emitImplicitInitAlternative(
			mlir::rlc::AlternativeType type,
			mlir::rlc::FunctionOp fun,
			mlir::rlc::ModuleBuilder& builder)
	{
		auto& rewriter = builder.getRewriter();
		rewriter.create<mlir::rlc::SetActiveEntryOp>(
				fun.getLoc(),
				fun.getBody().getArgument(0),
				rewriter.getI64IntegerAttr(0));
		auto casted = rewriter.create<mlir::rlc::ValueUpcastOp>(
				fun.getLoc(), type.getUnderlying()[0], fun.getBody().getArgument(0));
		initValue(casted, builder, fun.getLoc());
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

		auto decl = rewriter.create<mlir::rlc::UninitializedConstruct>(
				fun.getLoc(), mlir::rlc::IntegerType::getInt64(fun.getContext()));
		auto zero = rewriter.create<mlir::rlc::Constant>(fun.getLoc(), int64_t(0));
		rewriter.create<mlir::rlc::BuiltinAssignOp>(fun.getLoc(), decl, zero);

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

		initValue(lhsElem, builder, lhsElem.getLoc());

		auto oneConstant =
				rewriter.create<mlir::rlc::Constant>(fun.getLoc(), int64_t(1));
		auto result =
				rewriter.create<mlir::rlc::AddOp>(fun.getLoc(), decl, oneConstant);
		rewriter.create<mlir::rlc::BuiltinAssignOp>(fun.getLoc(), decl, result);

		rewriter.create<mlir::rlc::Yield>(fun.getLoc(), mlir::ValueRange({}));
		rewriter.setInsertionPointAfter(whileStatemet);
	}

	static bool isTriviallConstructible(mlir::Type t)
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

	static void emitImplicitInits(
			mlir::rlc::ModuleBuilder& builder, mlir::ModuleOp op)
	{
		mlir::IRRewriter& rewriter = builder.getRewriter();

		for (auto op : builder.getSymbolTable().get(
						 mlir::rlc::builtinOperatorName<mlir::rlc::InitOp>()))
		{
			auto fun = op.getDefiningOp<mlir::rlc::FunctionOp>();
			assert(fun != nullptr);
			if (not fun.getBody().empty())
				continue;
			if (fun.getArgumentTypes().size() != 1)
				continue;

			auto type = fun.getArgumentTypes().front();
			auto* block = rewriter.createBlock(
					&fun.getBody(), fun.getBody().begin(), { type }, { fun.getLoc() });
			rewriter.setInsertionPointToStart(block);
			if (isTriviallConstructible(type))
			{
				emitMemSetZero(type, fun, builder);
			}
			else if (auto classType = type.dyn_cast<mlir::rlc::ClassType>())
			{
				emitClassImplicitInit(classType, fun, builder);
			}
			else if (auto arrayType = type.dyn_cast<mlir::rlc::ArrayType>())
			{
				emitImplicitInitArray(arrayType, fun, builder);
			}
			else if (
					auto alternativeType = type.dyn_cast<mlir::rlc::AlternativeType>())
			{
				emitImplicitInitAlternative(alternativeType, fun, builder);
			}

			rewriter.create<mlir::rlc::Yield>(op.getLoc(), mlir::ValueRange());
		}
	}

	void emitImplicitInits(mlir::ModuleOp op, mlir::rlc::ModuleBuilder& builder)
	{
		declareImplicitInits(op);
		emitImplicitInits(builder, op);
	}

#define GEN_PASS_DEF_EMITIMPLICITINITPASS
#include "rlc/dialect/Passes.inc"
	struct EmitImplicitInitPass
			: impl::EmitImplicitInitPassBase<EmitImplicitInitPass>
	{
		using impl::EmitImplicitInitPassBase<
				EmitImplicitInitPass>::EmitImplicitInitPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			emitImplicitInits(getOperation(), builder);
		}
	};

}	 // namespace mlir::rlc

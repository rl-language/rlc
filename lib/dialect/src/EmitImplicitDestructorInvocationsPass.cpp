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
#include "mlir/IR/Dominance.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/ReachingDefinitions.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"
#include "rlc/utils/IRange.hpp"

namespace mlir::rlc
{
	static bool isBuiltinType(mlir::Type type)
	{
		return type.isa<mlir::rlc::IntegerType>() or
					 type.isa<mlir::rlc::FloatType>() or type.isa<mlir::rlc::BoolType>();
	}

	static mlir::LogicalResult typeRequiresDestructor(
			mlir::rlc::ModuleBuilder& builder,
			llvm::DenseMap<mlir::Type, bool>& requireDestructor,
			mlir::Type toConsider)
	{
		if (auto iter = requireDestructor.find(toConsider);
				iter != requireDestructor.end())
			return mlir::success(iter->second);

		if (toConsider.isa<mlir::rlc::TraitMetaType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::rlc::TemplateParameterType>())
		{
			requireDestructor[toConsider] = true;
			return mlir::success();
		}

		if (toConsider.isa<mlir::rlc::VoidType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::FunctionType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (auto type = toConsider.dyn_cast<mlir::rlc::AlternativeType>())
		{
			for (auto field : type.getUnderlying())
			{
				if (typeRequiresDestructor(builder, requireDestructor, field).failed())
					continue;

				requireDestructor[toConsider] = true;
				return mlir::success();
			}

			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::rlc::IntegerLiteralType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::rlc::OwningPtrType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::rlc::ContextType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::rlc::FrameType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::rlc::StringLiteralType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (toConsider.isa<mlir::rlc::IntegerType>() or
				toConsider.isa<mlir::rlc::FloatType>() or
				toConsider.isa<mlir::rlc::BoolType>())
		{
			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		if (auto type = toConsider.dyn_cast<mlir::rlc::ArrayType>())
		{
			requireDestructor[toConsider] =
					typeRequiresDestructor(
							builder, requireDestructor, type.getUnderlying())
							.succeeded();
			return mlir::success(requireDestructor[toConsider]);
		}

		if (auto type = toConsider.dyn_cast<mlir::rlc::ClassType>())
		{
			mlir::rlc::OverloadResolver resolver(builder.getSymbolTable());
			auto overload = resolver.findOverloads(
					builder.getRewriter().getUnknownLoc(),
					true,
					"drop",
					mlir::TypeRange({ type }));
			if (not overload.empty())
			{
				requireDestructor[toConsider] = true;
				return mlir::success();
			}

			for (auto field : type.getBody())
			{
				if (typeRequiresDestructor(builder, requireDestructor, field).failed())
					continue;

				requireDestructor[toConsider] = true;
				return mlir::success();
			}

			requireDestructor[toConsider] = false;
			return mlir::failure();
		}

		toConsider.dump();
		llvm_unreachable("unrechable");
		return mlir::success();
	}

	static void discoverAllEndOfLifeTimeInRegion(
			mlir::Value value,
			mlir::Region& region,
			llvm::SmallVector<mlir::Operation*, 2>& out)
	{
		mlir::DominanceInfo dominance;
		// we must emit the destructor before any yield, if the yield is not
		// returning the value. If it returning the value, it is up to the caller to
		// clean up
		for (auto yield : region.getOps<mlir::rlc::Yield>())
		{
			if (not dominance.properlyDominates(value.getDefiningOp(), yield))
				continue;

			if (not llvm::is_contained(yield.getArguments(), value))
				out.push_back(yield);
		}

		// recurr on all return statements, since the control flow actually ends at
		// the yield of the return.
		region.walk([&](mlir::rlc::ReturnStatement returnOp) {
			discoverAllEndOfLifeTimeInRegion(value, returnOp.getRegion(), out);
		});

		// annotate breaks and continues too, making sure not to visit those of
		// inner loops
		region.walk<mlir::WalkOrder::PreOrder>([&](mlir::Operation* current) {
			if (auto continueOp =
							mlir::dyn_cast<mlir::rlc::ContinueStatement>(current))
			{
				discoverAllEndOfLifeTimeInRegion(value, continueOp.getRegion(), out);
				return mlir::WalkResult::advance();
			}
			if (auto breakOp = mlir::dyn_cast<mlir::rlc::BreakStatement>(current))
			{
				discoverAllEndOfLifeTimeInRegion(value, breakOp.getRegion(), out);
				return mlir::WalkResult::advance();
			}
			if (auto returnOp = mlir::dyn_cast<mlir::rlc::WhileStatement>(current))
				return mlir::WalkResult::skip();
			return mlir::WalkResult::advance();
		});

		region.walk([&](mlir::rlc::ActionStatement actionStm) {
			out.push_back(actionStm);
		});

		region.walk([&](mlir::rlc::ActionsStatement actionStm) {
			out.push_back(actionStm);
		});

		region.walk([&](mlir::rlc::SubActionStatement actionStm) {
			out.push_back(actionStm);
		});
	}

	static void emitImplicitDestructorAlternativeType(
			IRRewriter& rewriter,
			mlir::rlc::FunctionOp fun,
			OverloadResolver& resolver)
	{
		for (auto field : fun.getBody()
													.getArgument(0)
													.getType()
													.cast<mlir::rlc::AlternativeType>()
													.getUnderlying())
		{
			if (isBuiltinType(field))
				continue;

			rewriter.setInsertionPointToEnd(&fun.getBody().front());

			auto ifStatement = rewriter.create<mlir::rlc::IfStatement>(fun.getLoc());
			auto* condition = rewriter.createBlock(&ifStatement.getCondition());
			rewriter.setInsertionPointToEnd(condition);

			auto isThisEntry = rewriter.create<mlir::rlc::IsOp>(
					fun.getLoc(), fun.getBody().front().getArgument(0), field);

			rewriter.create<mlir::rlc::Yield>(
					fun.getLoc(), mlir::ValueRange({ isThisEntry }));

			auto* trueBranch = rewriter.createBlock(&ifStatement.getTrueBranch());
			rewriter.setInsertionPointToEnd(trueBranch);

			auto casted = rewriter.create<mlir::rlc::ValueUpcastOp>(
					fun.getLoc(), field, fun.getBody().getArgument(0));

			auto subFunction = resolver.instantiateOverload(
					rewriter, true, fun.getLoc(), "drop", mlir::ValueRange({ casted }));
			rewriter.create<mlir::rlc::CallOp>(
					fun.getLoc(), subFunction, true, mlir::ValueRange({ casted }));

			rewriter.create<mlir::rlc::Yield>(fun.getLoc());

			auto* falseBranch = rewriter.createBlock(&ifStatement.getElseBranch());
			rewriter.setInsertionPointToEnd(falseBranch);
			rewriter.create<mlir::rlc::Yield>(fun.getLoc());
		}
		rewriter.setInsertionPointToEnd(&fun.getBody().front());
	}

	static void emitImplicitDestructors(
			mlir::rlc::ModuleBuilder& builder,
			llvm::DenseMap<mlir::Type, bool>& map,
			mlir::ModuleOp op)
	{
		auto& rewriter = builder.getRewriter();
		mlir::rlc::OverloadResolver resolver(builder.getSymbolTable());
		for (auto fun : op.getOps<mlir::rlc::FunctionOp>())
		{
			if (fun.getUnmangledName() != "drop" or
					fun.getFunctionType().getNumInputs() != 1 or
					not fun.getBody().empty())
				continue;

			auto type = fun.getFunctionType().getInput(0);
			auto* body = rewriter.createBlock(
					&fun.getBody(),
					fun.getBody().begin(),
					fun.getType().getInputs(),
					{ fun.getLoc() });
			rewriter.setInsertionPointToStart(body);
			if (auto casted = type.dyn_cast<mlir::rlc::ClassType>())
			{
				for (auto num : ::rlc::irange(casted.getBody().size()))
				{
					auto fieldType = casted.getBody()[num];
					if (typeRequiresDestructor(builder, map, fieldType).failed())
						continue;

					auto access = rewriter.create<mlir::rlc::MemberAccess>(
							op.getLoc(), body->getArgument(0), num);
					auto subFunction = resolver.instantiateOverload(
							rewriter, true, op.getLoc(), "drop", { fieldType });
					rewriter.create<mlir::rlc::CallOp>(
							op.getLoc(), subFunction, true, mlir::ValueRange({ access }));
				}
			}
			else if (auto casted = type.dyn_cast<mlir::rlc::ArrayType>())
			{
				auto subType = casted.getUnderlying();
				auto subFunction = resolver.instantiateOverload(
						rewriter, true, op.getLoc(), "drop", { subType });
				rewriter.create<mlir::rlc::ArrayCallOp>(
						op.getLoc(),
						subFunction,
						mlir::ValueRange({ body->getArgument(0) }));
			}
			else if (auto casted = type.dyn_cast<mlir::rlc::AlternativeType>())
			{
				emitImplicitDestructorAlternativeType(rewriter, fun, resolver);
			}

			rewriter.create<mlir::rlc::Yield>(fun.getLoc());
		}
	}

	static void declareImplicitDestructors(
			mlir::rlc::ModuleBuilder& builder,
			llvm::DenseMap<mlir::Type, bool>& map,
			mlir::ModuleOp op)
	{
		auto& rewriter = builder.getRewriter();
		rewriter.setInsertionPointToStart(&op.getBodyRegion().front());
		llvm::SmallVector<mlir::Type, 2> destructorsToCreate;
		mlir::rlc::OverloadResolver resolver(builder.getSymbolTable());

		auto collectToCreate = [&](mlir::Type t) {
			if (typeRequiresDestructor(builder, map, t).failed())
				return;

			if (isTemplateType(t).succeeded())
				return;

			destructorsToCreate.push_back(t);
		};

		op.walk([&](mlir::rlc::DestroyOp destroyOp) {
			auto type = destroyOp.getOperand().getType();
			collectToCreate(type);
			type.walk(collectToCreate);
		});

		for (auto type : destructorsToCreate)
		{
			if (auto overload = resolver.findOverload(
							builder.getRewriter().getUnknownLoc(), true, "drop", { type });
					overload != nullptr)
			{
				continue;
			}
			auto destructor = rewriter.create<mlir::rlc::FunctionOp>(
					op.getLoc(),
					"drop",
					mlir::FunctionType::get(
							rewriter.getContext(),
							mlir::TypeRange({ type }),
							mlir::TypeRange()),
					rewriter.getStrArrayAttr({ "to_drop" }),
					true);
			builder.getSymbolTable().add("drop", destructor);
		}
	}

	void lowerDestructors(
			llvm::DenseMap<mlir::Type, bool>& requireDestructor,
			mlir::rlc::ModuleBuilder& builder,
			mlir::Operation* op)
	{
		mlir::rlc::OverloadResolver resolver(builder.getSymbolTable());
		llvm::SmallVector<mlir::rlc::DestroyOp, 2> toReplace;
		op->walk([&](mlir::rlc::DestroyOp destroyOp) {
			toReplace.push_back(destroyOp);
		});

		for (auto destroyOp : toReplace)
		{
			builder.getRewriter().setInsertionPoint(destroyOp);
			if (typeRequiresDestructor(
							builder, requireDestructor, destroyOp.getOperand().getType())
							.failed())
			{
				builder.getRewriter().eraseOp(destroyOp);
				continue;
			}

			auto function = resolver.instantiateOverload(
					builder.getRewriter(),
					true,
					destroyOp.getLoc(),
					"drop",
					mlir::TypeRange({ destroyOp.getOperand().getType() }));
			if (not function)
				continue;
			builder.getRewriter().replaceOpWithNewOp<mlir::rlc::CallOp>(
					destroyOp,
					function,
					true,
					mlir::ValueRange({ destroyOp.getOperand() }));
		}
	}

#define GEN_PASS_DEF_LOWERDESTRUCTORSPASS
#include "rlc/dialect/Passes.inc"
	struct LowerDestructorsPass
			: impl::LowerDestructorsPassBase<LowerDestructorsPass>
	{
		using impl::LowerDestructorsPassBase<
				LowerDestructorsPass>::LowerDestructorsPassBase;

		void runOnOperation() override
		{
			llvm::DenseMap<mlir::Type, bool> requireDestructor;
			mlir::rlc::ModuleBuilder builder(getOperation());
			lowerDestructors(requireDestructor, builder, getOperation());
		}
	};

#define GEN_PASS_DEF_EMITIMPLICITDESTRUCTORSPASS
#include "rlc/dialect/Passes.inc"
	struct EmitImplictDestructorsPass
			: impl::EmitImplicitDestructorsPassBase<EmitImplictDestructorsPass>
	{
		using impl::EmitImplicitDestructorsPassBase<
				EmitImplictDestructorsPass>::EmitImplicitDestructorsPassBase;

		void runOnOperation() override
		{
			llvm::DenseMap<mlir::Type, bool> requireDestructor;
			mlir::rlc::ModuleBuilder builder(getOperation());
			declareImplicitDestructors(builder, requireDestructor, getOperation());
			emitImplicitDestructors(builder, requireDestructor, getOperation());
		}
	};

	// sets the rewriter insertion point to be at the location where destructor
	// invocations must be inserted, for yields it is the block inside the yeild,
	// for actions it is just before the action
	static mlir::Operation* setRewriterToInsertionPoint(
			mlir::IRRewriter& rewriter, mlir::Operation* op)
	{
		auto casted = mlir::dyn_cast<mlir::rlc::Yield>(op);
		if (casted)
		{
			if (casted.getOnEnd().empty())
			{
				rewriter.createBlock(&casted.getOnEnd(), casted.getOnEnd().begin());
				rewriter.create<mlir::rlc::Yield>(
						casted.getLoc(), mlir::ValueRange({}));
			}
			return casted.getOnEnd().front().getTerminator();
		}

		return op;
	}

#define GEN_PASS_DEF_EMITIMPLICITDESTRUCTORINVOCATIONSPASS
#include "rlc/dialect/Passes.inc"
	struct EmitImplictDestructoInvocationsPass
			: impl::EmitImplicitDestructorInvocationsPassBase<
						EmitImplictDestructoInvocationsPass>
	{
		using impl::EmitImplicitDestructorInvocationsPassBase<
				EmitImplictDestructoInvocationsPass>::
				EmitImplicitDestructorInvocationsPassBase;

		void runOfFunction(mlir::rlc::ModuleBuilder& builder, mlir::Operation* fun)
		{
			mlir::IRRewriter& rewriter = builder.getRewriter();

			llvm::SmallVector<mlir::Value, 3> toEmitDestroy;
			llvm::DenseMap<mlir::Type, bool> requireDestructor;

			fun->walk([&](mlir::Operation* op) {
				if (not mlir::isa<mlir::rlc::DeclarationStatement>(op))
					return;
				for (mlir::Value result : op->getResults())
					if (typeRequiresDestructor(
									builder, requireDestructor, result.getType())
									.succeeded())
						toEmitDestroy.push_back(result);
			});
			fun->walk([&](mlir::rlc::CallOp op) {
				if (op.getNumResults() == 0 or
						op.getCalleeType().getResult(0).isa<mlir::rlc::ReferenceType>())
					return;
				for (mlir::Value result : op->getResults())
					if (typeRequiresDestructor(
									builder, requireDestructor, result.getType())
									.succeeded())
						toEmitDestroy.push_back(result);
			});
			llvm::SmallVector<std::pair<mlir::Value, mlir::Operation*>>
					valueDestructionPairs;
			mlir::rlc::ReachingDefinitions analysis(fun);
			for (auto value : toEmitDestroy)
			{
				llvm::SmallVector<mlir::Operation*, 2> destructionPoints;
				auto* parentScope = value.getDefiningOp()->getParentRegion();
				discoverAllEndOfLifeTimeInRegion(
						value, *parentScope, destructionPoints);
				llvm::erase_if(destructionPoints, [&](mlir::Operation* lifeTimeEnd) {
					return not analysis.reachesOperation(value, lifeTimeEnd);
				});

				for (auto* lifeTimeEnd : destructionPoints)
				{
					auto insertionPoint =
							setRewriterToInsertionPoint(rewriter, lifeTimeEnd);
					valueDestructionPairs.push_back(
							std::make_pair(value, insertionPoint));
				}
			}

			for (auto& pair : valueDestructionPairs)
			{
				rewriter.setInsertionPoint(pair.second);
				rewriter.create<mlir::rlc::DestroyOp>(
						pair.second->getLoc(), pair.first);
			}
		}

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			for (auto fun : getOperation().getOps<mlir::rlc::FunctionOp>())
				runOfFunction(builder, fun);

			for (auto fun : getOperation().getOps<mlir::rlc::ActionFunction>())
				runOfFunction(builder, fun);
		}
	};

#define GEN_PASS_DEF_TESTREACHINGDEFINITIONSPASS
#include "rlc/dialect/Passes.inc"
	struct TestReachingDefinitionsPass
			: impl::TestReachingDefinitionsPassBase<TestReachingDefinitionsPass>
	{
		using impl::TestReachingDefinitionsPassBase<
				TestReachingDefinitionsPass>::TestReachingDefinitionsPassBase;

		void runOnFunction(mlir::Operation* op)
		{
			ReachingDefinitions analysis(op);
			op->walk([&](mlir::rlc::DeclarationStatement stm) {
				op->walk([&](mlir::Operation* inner) {
					if (analysis.reachesOperation(stm, inner))
					{
						inner->setAttr(
								stm.getSymName(),
								mlir::StringAttr::get(inner->getContext(), "true"));
					}
				});
			});

			int i = 0;

			op->walk([&](mlir::rlc::CallOp stm) {
				if (stm.getNumResults() == 0)
					return;
				op->walk([&](mlir::Operation* inner) {
					if (analysis.reachesOperation(stm.getResult(0), inner))
					{
						inner->setAttr(
								("call_n" + llvm::Twine(i)).str(),
								mlir::StringAttr::get(inner->getContext(), "true"));
					}
				});
				i++;
			});
		}

		void runOnOperation() override
		{
			for (auto op : getOperation().getOps<mlir::rlc::FunctionOp>())
				runOnFunction(op);
			for (auto op : getOperation().getOps<mlir::rlc::ActionFunction>())
				runOnFunction(op);
		}
	};
}	 // namespace mlir::rlc

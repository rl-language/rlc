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
#include "mlir/IR/PatternMatch.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
	using Frames = std::pair<ActionFrameContent, ActionFrameContent>;
	static mlir::Type getHiddenFrameType(mlir::rlc::ActionFunction fun)
	{
		return mlir::rlc::EntityType::getIdentified(
				fun.getContext(),
				(fun.getEntityType().getName() + "_shadow").str(),
				{});
	}

	static void replaceAllAccessesWithIndirectOnes(
			mlir::rlc::ActionFunction action,
			mlir::rlc::ModuleBuilder& builder,
			const Frames& content)
	{
		mlir::IRRewriter rewriter(action.getContext());

		auto* region = &action.getBody();
		auto argument =
				region->front().addArgument(action.getEntityType(), action.getLoc());

		auto argument2 = region->front().addArgument(
				getHiddenFrameType(action), action.getLoc());

		rewriter.setInsertionPointToStart(&region->front());

		for (auto& arg : llvm::drop_end(region->front().getArguments(), 2))
		{
			llvm::SmallVector<mlir::OpOperand*, 4> operands;
			for (auto& operand : arg.getUses())
			{
				operands.push_back(&operand);
			}
			for (auto& use : operands)
			{
				rewriter.setInsertionPoint(use->getOwner());
				if (arg.getType().isa<mlir::rlc::FrameType>())
				{
					auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
							use->getOwner()->getLoc(),
							argument,
							content.first.indexOf(arg) + 1);
					use->set(refToMember);
				}
				else
				{
					auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
							use->getOwner()->getLoc(),
							argument2,
							content.second.indexOf(arg));
					auto dereffed = rewriter.create<mlir::rlc::DerefOp>(
							refToMember.getLoc(), refToMember);
					use->set(dereffed);
				}
			}
		}

		while (region->front().getNumArguments() != 2)
			region->front().eraseArgument(static_cast<size_t>(0));
	}

	static void replaceAllDeclarationsWithIndirectOnes(
			mlir::rlc::ActionFunction action,
			mlir::rlc::ModuleBuilder& builder,
			const Frames& frames)
	{
		mlir::IRRewriter& rewriter = builder.getRewriter();

		auto refToEntity = action.getBlocks().front().getArgument(0);
		auto type = refToEntity.getType().cast<mlir::rlc::EntityType>();

		llvm::SmallVector<mlir::rlc::DeclarationStatement, 4> decls;
		action.walk([&](mlir::rlc::DeclarationStatement statement) {
			decls.push_back(statement);
		});

		for (auto decl : decls)
		{
			if (not decl.getType().isa<mlir::rlc::FrameType>())
				continue;
			size_t memberIndex = frames.first.indexOf(decl) + 1;
			auto underlyingType =
					decl.getType().cast<mlir::rlc::FrameType>().getUnderlying();
			// we have replaced the real location of the variable, we can now decay it
			// into a regular type
			decl.getResult().setType(underlyingType);

			llvm::SmallVector<mlir::OpOperand*, 4> uses;
			for (auto& operand : decl.getResult().getUses())
				uses.push_back(&operand);

			for (auto* operand : uses)
			{
				rewriter.setInsertionPoint(operand->getOwner());

				auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
						decl.getLoc(), refToEntity, memberIndex);

				operand->set(refToMember.getResult());
			}

			rewriter.setInsertionPointAfter(decl);
			auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
					decl.getLoc(), refToEntity, memberIndex);
			assert(decl.getType() != nullptr);
			auto* call = builder.emitCall(
					decl,
					true,
					builtinOperatorName<mlir::rlc::AssignOp>(),
					mlir::ValueRange({ refToMember, decl }));
			assert(call != nullptr);
			builder.emitCall(decl, true, "drop", mlir::ValueRange({ decl }), false);
		}
	}

	// we have stolen the precondition from the actionStatement in the original
	// function, we need to redirect all operands of all operations that still
	// point to a argument of that original root function with the new frame
	// argument
	static void redirectAccessesToNewFrame(
			mlir::rlc::ActionFunction root, mlir::rlc::FunctionOp action)
	{
		mlir::IRRewriter rewiter(root.getContext());
		llvm::SmallVector<mlir::Operation*, 4> ops;
		for (auto& ins : action.getPrecondition().front().getOperations())
		{
			ops.push_back(&ins);
		}
		for (auto& ins : ops)
		{
			for (auto& operand : ins->getOpOperands())
			{
				if (root.getBody().front().getArgument(0) == operand.get())
					operand.set(action.getPrecondition().getArgument(0));
				else if (root.getBody().front().getArgument(1) == operand.get())
				{
					// if there is a access to the hidden frame in the precondition
					// of a action it must be because it is trying to access a context
					// variable, thus let us make sure it is a member access and then
					// redirect it to the argument with the same index. This is valid
					// since the cntext variables are always the leftmosts of the
					// function, and the first ones of the hidden frames so the index
					// is in common
					auto casted = mlir::cast<mlir::rlc::MemberAccess>(ins);
					rewiter.setInsertionPointAfter(casted);
					auto dereffed = rewiter.create<mlir::rlc::MakeRefOp>(
							casted.getLoc(),
							action.getPrecondition().getArgument(
									casted.getMemberIndex() + 1));
					casted.replaceAllUsesWith(dereffed.getResult());
					casted.erase();
					break;
				}
			}
		}
	}

	// add the precondition "actionEntity.resumptionPoint ==
	// actionStatement.resumptionIndex" to the subActionFunction.
	static void addResumptionPointPrecondition(
			mlir::Block* block,
			mlir::rlc::ActionStatement actionStatement,
			mlir::IRRewriter& rewriter)
	{
		auto& yield = block->back();
		rewriter.setInsertionPoint(&yield);
		auto savedResumptionIndex = rewriter.create<MemberAccess>(
				actionStatement.getLoc(), block->getArgument(0), 0);
		auto expectedResumptionIndex = rewriter.create<Constant>(
				actionStatement.getLoc(),
				(int64_t) actionStatement.getResumptionPoint());
		auto eq = rewriter.create<EqualOp>(
				actionStatement->getLoc(),
				savedResumptionIndex,
				expectedResumptionIndex);
		yield.insertOperands(
				yield.getNumOperands(), ValueRange({ eq.getResult() }));
	}

	static void emitActionArgsAssignsToFrame(
			mlir::rlc::ActionFunction action,
			mlir::rlc::ActionStatement subAction,
			mlir::IRRewriter& rewriter,
			mlir::rlc::EntityType entityType,
			mlir::rlc::FunctionOp subF,
			mlir::rlc::UninitializedConstruct hiddenFrame,
			const Frames& frames)

	{
		for (auto arg : llvm::enumerate(subAction.getResults()))
		{
			if (arg.value().getType().isa<mlir::rlc::FrameType>())
			{
				auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
						action.getLoc(),
						subF.getBlocks().front().getArgument(0),
						frames.first.indexOf(arg.value()) + 1);

				rewriter.create<mlir::rlc::AssignOp>(
						action.getLoc(),
						addressOfArgInFrame,
						subF.getBlocks().front().getArgument(arg.index() + 1));
			}
			else
			{
				mlir::Value addressOfArgInFrame;
				if (arg.value().getType().isa<mlir::rlc::ContextType>())
				{
					// Context argumets are guaranteed to be the first of the list
					addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
							action.getLoc(), hiddenFrame, arg.index());
				}
				else
				{
					addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
							action.getLoc(), hiddenFrame, frames.second.indexOf(arg.value()));
				}

				auto refToArg = rewriter.create<mlir::rlc::MakeRefOp>(
						addressOfArgInFrame.getLoc(),
						subF.getBlocks().front().getArgument(arg.index() + 1));

				rewriter.create<mlir::rlc::BuiltinAssignOp>(
						action.getLoc(), addressOfArgInFrame, refToArg);
			}
		}
	}

	// emits:
	// if expected_resume_index == frame.resume_index:
	//   frame.args... = actual_args...
	//   frame.resumption_index = correct_resumptiom_index
	static void emitDispatchToImpl(
			mlir::rlc::ActionFunction action,
			mlir::rlc::ActionStatement subAction,
			mlir::IRRewriter& rewriter,
			mlir::rlc::EntityType entityType,
			mlir::rlc::FunctionOp subF,
			const Frames& frames)
	{
		auto hiddenEntityType = getHiddenFrameType(action);
		auto ifStatement = rewriter.create<mlir::rlc::IfStatement>(subF.getLoc());
		auto* condition = rewriter.createBlock(&ifStatement.getCondition());

		auto savedResumptionIndex = rewriter.create<MemberAccess>(
				subF.getLoc(), subF.getBody().front().getArgument(0), 0);
		auto expectedResumptionIndex = rewriter.create<Constant>(
				subF.getLoc(), (int64_t) subAction.getResumptionPoint());
		auto eq = rewriter.create<EqualOp>(
				subF->getLoc(), savedResumptionIndex, expectedResumptionIndex);
		rewriter.create<mlir::rlc::Yield>(subF.getLoc(), mlir::ValueRange({ eq }));

		auto* trueBranch = rewriter.createBlock(&ifStatement.getTrueBranch());
		auto frameHidden = rewriter.create<mlir::rlc::UninitializedConstruct>(
				action.getLoc(), hiddenEntityType);
		emitActionArgsAssignsToFrame(
				action, subAction, rewriter, entityType, subF, frameHidden, frames);

		// the resumption point stored in the frame is not enough to discrimated
		// among alternative resumption points, so we have to store the actual id
		// of the selected action so it can resume
		auto resumptionPointSelector = rewriter.create<mlir::rlc::Constant>(
				subF.getLoc(), static_cast<int64_t>(subAction.getId()));
		auto candidatesResumptionPoint = rewriter.create<mlir::rlc::MemberAccess>(
				subF.getLoc(), subF.getBody().getArgument(0), 0);
		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				subF.getLoc(), candidatesResumptionPoint, resumptionPointSelector);

		rewriter.create<mlir::rlc::CallOp>(
				subF.getLoc(),
				mlir::TypeRange(),
				action.getResult(),
				false,
				mlir::ValueRange(
						{ subF.getBlocks().front().getArgument(0), frameHidden }));
		rewriter.create<mlir::rlc::Yield>(subF.getLoc());

		auto* elseBranch = rewriter.createBlock(&ifStatement.getElseBranch());
		auto lastYield = rewriter.create<mlir::rlc::Yield>(subF.getLoc());
		rewriter.setInsertionPoint(lastYield);
	}

	static mlir::FunctionType decayFunctionType(mlir::FunctionType t)
	{
		llvm::SmallVector<mlir::Type, 4> decayedArgs;
		for (auto arg : t.getInputs())
		{
			if (auto casted = arg.dyn_cast<mlir::rlc::FrameType>())
				decayedArgs.push_back(casted.getUnderlying());
			else if (auto casted = arg.dyn_cast<mlir::rlc::ContextType>())
				decayedArgs.push_back(casted.getUnderlying());
			else
				decayedArgs.push_back(arg);
		}
		return mlir::FunctionType::get(t.getContext(), decayedArgs, t.getResults());
	}

	// all preconditions for all possible sub actions that can be executed for
	// this wrapper are in their on basic blocks, this puts them in a or
	// expression
	void mergePreconditionBlocksIntoOne(mlir::rlc::FunctionOp subF)
	{
		mlir::IRRewriter rewriter(subF.getContext());
		llvm::SmallVector<mlir::rlc::Yield, 4> yields;
		for (auto yield : subF.getPrecondition().getOps<mlir::rlc::Yield>())
		{
			yields.push_back(yield);
		}

		llvm::SmallVector<mlir::Value, 4> singleActionPrerequirements;
		// put all previous yieled requirements in a single and
		for (auto yield : yields)
		{
			rewriter.setInsertionPoint(yield);

			mlir::Value lastValue = yield.getOperand(0);
			for (mlir::Value value : llvm::drop_begin(yield.getOperands()))
				lastValue =
						rewriter.create<mlir::rlc::AndOp>(yield.getLoc(), lastValue, value);

			singleActionPrerequirements.push_back(lastValue);
			yield.erase();
		}

		while (subF.getPrecondition().getBlocks().size() > 1)
		{
			rewriter.mergeBlocks(
					&*(++subF.getPrecondition().begin()),
					&subF.getPrecondition().front(),
					subF.getPrecondition().front().getArguments());
		}

		rewriter.setInsertionPointToEnd(&subF.getPrecondition().front());
		mlir::Value lastValue = singleActionPrerequirements[0];
		for (mlir::Value value : llvm::drop_begin(singleActionPrerequirements))
			lastValue =
					rewriter.create<mlir::rlc::OrOp>(subF.getLoc(), lastValue, value);
		rewriter.create<mlir::rlc::Yield>(
				subF.getLoc(), mlir::ValueRange({ lastValue }));
	}

	static void decayRegionTypes(mlir::Region& region)
	{
		for (auto& arg : region.front().getArguments())
		{
			if (auto casted = arg.getType().dyn_cast<mlir::rlc::FrameType>())
				arg.setType(casted.getUnderlying());
			else if (auto casted = arg.getType().dyn_cast<mlir::rlc::ContextType>())
				arg.setType(casted.getUnderlying());
		}
	}

	// emits a function declaration such as
	// def action(frame, args...):
	//   for possible_resumption_point
	//     if can_action(frame, args...):
	//  		frame.resumption_index = action_resumption_index
	//  		frame.args... = args...
	//  		call action(frame)
	static void emitSingleActionStatementWrapper(
			mlir::rlc::ActionFunction action,
			mlir::rlc::ModuleBuilder& builder,
			mlir::ArrayRef<mlir::Operation*> subActions,
			size_t subActionIndex,
			const Frames& frames)
	{
		mlir::IRRewriter rewriter(action.getContext());
		rewriter.setInsertionPoint(action);
		auto type = decayFunctionType(action.getActions()[subActionIndex]
																			.getType()
																			.cast<mlir::FunctionType>());
		auto entityType = action.getEntityType();

		auto firstStatement = llvm::cast<mlir::rlc::ActionStatement>(subActions[0]);
		auto subF = rewriter.create<mlir::rlc::FunctionOp>(
				action.getLoc(),
				firstStatement.getName(),
				type,
				firstStatement.getDeclaredNames(),
				true);
		action.getActions()[subActionIndex].replaceAllUsesWith(subF);

		// steals the precondition of all possible actions to take from here and
		// adds a frame argument to the new function and adds the requirement
		// regarding the resumption index
		for (auto* subAct : subActions)
		{
			auto subAction = mlir::cast<mlir::rlc::ActionStatement>(subAct);
			rewriter.inlineRegionBefore(
					subAction.getPrecondition(),
					subF.getPrecondition(),
					subF.getPrecondition().end());

			subF.getPrecondition().back().insertArgument(
					static_cast<unsigned int>(0),
					subF.getArgumentTypes().front(),
					subF.getLoc());
			addResumptionPointPrecondition(
					&subF.getPrecondition().back(), subAction, rewriter);
		}

		mergePreconditionBlocksIntoOne(subF);
		decayRegionTypes(subF.getPrecondition());

		redirectAccessesToNewFrame(action, subF);

		// creates a block to hold the dispatching mechanism
		llvm::SmallVector<mlir::Location, 2> locs;
		for (size_t i = 0; i < type.getInputs().size(); i++)
			locs.push_back(action.getLoc());
		rewriter.createBlock(
				&subF.getBody(), subF.getBody().begin(), type.getInputs(), locs);

		for (auto* subAct : subActions)
		{
			auto subAction = mlir::cast<mlir::rlc::ActionStatement>(subAct);
			emitDispatchToImpl(action, subAction, rewriter, entityType, subF, frames);
		}

		rewriter.setInsertionPointToEnd(&subF.getBody().front());
		rewriter.create<mlir::rlc::Yield>(subF.getLoc());
	}

	static void emitActionWrapperCalls(
			mlir::rlc::ActionFunction action,
			mlir::rlc::ModuleBuilder& builder,
			const Frames& frames)
	{
		mlir::IRRewriter rewriter(action.getContext());

		rewriter.setInsertionPoint(action);
		auto f = rewriter.create<mlir::rlc::FunctionOp>(
				action.getLoc(),
				action.getUnmangledName(),
				decayFunctionType(action.getType().cast<mlir::FunctionType>()),
				action.getArgNames(),
				false);

		f.getPrecondition().takeBody(action.getPrecondition());
		decayRegionTypes(f.getPrecondition());

		llvm::SmallVector<mlir::Location, 2> locs;
		for (size_t i = 0; i < f.getFunctionType().getInputs().size(); i++)
			locs.push_back(action.getLoc());

		rewriter.createBlock(
				&f.getBody(),
				f.getBody().begin(),
				f.getFunctionType().getInputs(),
				locs);

		action.getOperation()->getResult(0).replaceAllUsesWith(f);

		auto entityType =
				builder.typeOfAction(action).cast<mlir::rlc::EntityType>();
		auto initFunction = builder.getInitFunctionOf(entityType);
		auto frame = rewriter.create<mlir::rlc::ExplicitConstructOp>(
				action.getLoc(), initFunction);

		auto frameHidden = rewriter.create<mlir::rlc::UninitializedConstruct>(
				action.getLoc(), getHiddenFrameType(action));

		auto ptrToIndex =
				rewriter.create<mlir::rlc::MemberAccess>(action.getLoc(), frame, 0);

		auto constantZero =
				rewriter.create<mlir::rlc::Constant>(action.getLoc(), int64_t(0));

		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				action.getLoc(), ptrToIndex, constantZero);

		size_t explitFrameIndex = 1;
		size_t hiddenFrameIndex = 0;

		for (const auto& argument : llvm::enumerate(action.getArgNames()))
		{
			auto arg = action.getType().getInputs()[argument.index()];
			if (arg.isa<mlir::rlc::FrameType>())
			{
				auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
						action.getLoc(), frame, explitFrameIndex++);

				rewriter.create<mlir::rlc::AssignOp>(
						action.getLoc(),
						addressOfArgInFrame,
						f.getBlocks().front().getArgument(argument.index()));
			}
			else
			{
				auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
						action.getLoc(), frameHidden, hiddenFrameIndex++);

				auto refToArg = rewriter.create<mlir::rlc::MakeRefOp>(
						addressOfArgInFrame.getLoc(),
						f.getBlocks().front().getArgument(argument.index()));

				rewriter.create<mlir::rlc::BuiltinAssignOp>(
						action.getLoc(), addressOfArgInFrame, refToArg);
			}
		}

		rewriter.create<mlir::rlc::CallOp>(
				f.getLoc(),
				mlir::TypeRange(),
				action.getResult(),
				false,
				mlir::ValueRange({ frame, frameHidden }));

		rewriter.create<mlir::rlc::Yield>(f.getLoc(), mlir::Value({ frame }));

		for (const auto& subAction : llvm::enumerate(action.getActions()))
		{
			emitSingleActionStatementWrapper(
					action,
					builder,
					builder.actionFunctionValueToActionStatement(subAction.value()),
					subAction.index(),
					frames);
		}
	}

	// redirects all uses of a variable introduced by a action to the variable in
	// the coroutine frame
	static void redirectActionArgumentsToFrame(
			mlir::rlc::ActionFunction fun, const Frames& frames)
	{
		mlir::IRRewriter rewriter(fun.getContext());
		fun.walk([&](mlir::rlc::ActionStatement op) {
			size_t index = 0;
			for (const auto& [res, name] :
					 llvm::zip(op.getResults(), op.getDeclaredNames()))
			{
				llvm::SmallVector<mlir::OpOperand*, 4> uses;
				decayRegionTypes(op.getPrecondition());
				for (auto& use : res.getUses())
				{
					uses.push_back(&use);
				}
				for (auto* use : uses)
				{
					rewriter.setInsertionPoint(use->getOwner());
					auto frame = fun.getBlocks().front().getArgument(0);
					auto frameHidden = fun.getBlocks().front().getArgument(1);
					auto entityType = frame.getType().cast<mlir::rlc::EntityType>();

					if (res.getType().isa<mlir::rlc::FrameType>())
					{
						auto ref = rewriter.create<mlir::rlc::MemberAccess>(
								op.getLoc(), frame, frames.first.indexOf(res) + 1);
						use->set(ref);
					}
					else if (res.getType().isa<mlir::rlc::ContextType>())
					{
						auto ref = rewriter.create<mlir::rlc::MemberAccess>(
								op.getLoc(), frameHidden, index);
						auto dereffed =
								rewriter.create<mlir::rlc::DerefOp>(op.getLoc(), ref);
						use->set(dereffed);
					}
					else
					{
						auto ref = rewriter.create<mlir::rlc::MemberAccess>(
								op.getLoc(), frameHidden, frames.second.indexOf(res));
						auto dereffed =
								rewriter.create<mlir::rlc::DerefOp>(op.getLoc(), ref);
						use->set(dereffed);
					}
				}
				index++;
			}
		});
	}

	void moveCastsNearUses(mlir::ModuleOp module)
	{
		llvm::SmallVector<mlir::rlc::StorageCast, 4> casts;
		module.walk([&](mlir::rlc::StorageCast cast) { casts.push_back(cast); });
		for (auto cast : casts)
		{
			mlir::IRRewriter rewriter(module.getContext());
			if (cast.getResult().getUses().empty())
			{
				cast.erase();
				continue;
			}

			while (not cast.getResult().hasOneUse())
			{
				auto& use = *cast.getResult().getUses().begin();
				rewriter.setInsertionPoint(use.getOwner());
				auto* cloned = rewriter.clone(*cast);
				use.set(cloned->getResult(0));
			}

			cast->moveBefore(cast.getResult().getUses().begin()->getOwner());
		}
	}

#define GEN_PASS_DEF_LOWERACTIONPASS
#include "rlc/dialect/Passes.inc"

	struct LowerActionPass: impl::LowerActionPassBase<LowerActionPass>
	{
		using impl::LowerActionPassBase<LowerActionPass>::LowerActionPassBase;

		void runOnOperation() override
		{
			moveCastsNearUses(getOperation());

			mlir::rlc::ModuleBuilder builder(getOperation());
			llvm::SmallVector<mlir::rlc::ActionFunction, 4> acts(
					getOperation().getOps<mlir::rlc::ActionFunction>());
			mlir::IRRewriter rewriter(getOperation().getContext());

			for (auto action : acts)
			{
				const Frames& frames = action.getFrameLists();
				replaceAllAccessesWithIndirectOnes(action, builder, frames);
				replaceAllDeclarationsWithIndirectOnes(action, builder, frames);
				redirectActionArgumentsToFrame(action, frames);
				emitActionWrapperCalls(action, builder, frames);

				rewriter.setInsertionPoint(action);

				auto isDoneFunction = rewriter.create<mlir::rlc::FunctionOp>(
						action.getLoc(),
						"is_done",
						action.getIsDoneFunctionType(),
						rewriter.getStrArrayAttr({ "frame" }),
						true);

				auto* block = rewriter.createBlock(
						&isDoneFunction.getBody(),
						isDoneFunction.getBody().begin(),
						action.getEntityType(),
						{ action.getLoc() });
				rewriter.setInsertionPoint(block, block->begin());
				auto index = rewriter.create<mlir::rlc::MemberAccess>(
						action.getLoc(), block->getArgument(0), 0);
				auto constant = rewriter.create<mlir::rlc::Constant>(
						action.getLoc(), static_cast<int64_t>(-1));
				auto toReturn = rewriter.create<mlir::rlc::EqualOp>(
						action.getLoc(), index, constant);
				auto retStatement = rewriter.create<mlir::rlc::ReturnStatement>(
						action.getLoc(), isDoneFunction.getFunctionType().getResult(0));
				rewriter.createBlock(&retStatement.getBody());
				rewriter.create<mlir::rlc::Yield>(
						action.getLoc(), mlir::ValueRange({ toReturn }));
				action.getIsDoneFunction().replaceAllUsesWith(isDoneFunction);

				rewriter.setInsertionPoint(action);
				auto actionType = builder.typeOfAction(action);
				auto loweredToFun = rewriter.create<mlir::rlc::FunctionOp>(
						action.getLoc(),
						("_" + action.getUnmangledName() + "_impl").str(),
						mlir::FunctionType::get(
								action.getContext(),
								{ actionType, getHiddenFrameType(action) },
								{}),
						rewriter.getStrArrayAttr({ "frame", "hidden_frame" }),
						true);
				loweredToFun.getBody().takeBody(action.getBody());
				action.getResult().replaceAllUsesWith(loweredToFun);
				rewriter.eraseOp(action);
			}

			getOperation().walk([](mlir::rlc::StorageCast cast) {
				assert(cast.getType() == cast.getOperand().getType());
				cast.replaceAllUsesWith(cast.getOperand());
				cast.erase();
			});
		}
	};
}	 // namespace mlir::rlc

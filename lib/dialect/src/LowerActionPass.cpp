#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/PatternMatch.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{

	static void replaceAllAccessesWithIndirectOnes(
			mlir::rlc::ActionFunction action, mlir::rlc::ModuleBuilder& builder)
	{
		mlir::IRRewriter rewriter(action.getContext());

		for (auto* region : { &action.getBody(), &action.getPrecondition() })
		{
			auto argument = region->front().addArgument(
					builder.typeOfAction(action), action.getLoc());

			rewriter.setInsertionPointToStart(&region->front());

			for (const auto& arg :
					 llvm::enumerate(llvm::drop_end(region->front().getArguments())))
			{
				llvm::SmallVector<mlir::OpOperand*, 4> operands;
				for (auto& operand : arg.value().getUses())
				{
					operands.push_back(&operand);
				}
				for (auto& use : operands)
				{
					rewriter.setInsertionPoint(use->getOwner());
					auto refToMember = rewriter.create<mlir::rlc::MemberAccess>(
							use->getOwner()->getLoc(), argument, arg.index() + 1);
					use->set(refToMember);
				}
			}

			while (region->front().getNumArguments() != 1)
				region->front().eraseArgument(static_cast<size_t>(0));
		}
	}

	static void replaceAllDeclarationsWithIndirectOnes(
			mlir::rlc::ActionFunction action, mlir::rlc::ModuleBuilder& builder)
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
			const auto* maybeEntry =
					llvm::find(type.getFieldNames(), decl.getSymName());
			if (maybeEntry == type.getFieldNames().end())
				continue;
			size_t memberIndex =
					std::distance(type.getFieldNames().begin(), maybeEntry);

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
					builtinOperatorName<mlir::rlc::AssignOp>(),
					mlir::ValueRange({ refToMember, decl }));
			assert(call != nullptr);
		}
	}

	// we have stolen the precondition from the actionStatement in the original
	// function, we need to redirect all operands of all operations that still
	// point to a argument of that original root function with the new frame
	// argument
	static void redirectAccessesToNewFrame(
			mlir::rlc::ActionFunction root,
			mlir::FunctionType actionType,
			mlir::rlc::FunctionOp action,
			mlir::rlc::ModuleBuilder& builder)
	{
		for (auto& ins : action.getPrecondition().front().getOperations())
		{
			for (auto& operand : ins.getOpOperands())
			{
				if (root.getBody().front().getArgument(0) == operand.get())
					operand.set(action.getPrecondition().getArgument(0));
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
			mlir::rlc::FunctionOp subF)
	{
		for (auto arg : llvm::enumerate(subAction.getDeclaredNames()))
		{
			size_t fieldIndex = std::distance(
					entityType.getFieldNames().begin(),
					llvm::find(
							entityType.getFieldNames(),
							arg.value().cast<mlir::StringAttr>().str()));

			auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
					action.getLoc(), subF.getBlocks().front().getArgument(0), fieldIndex);

			rewriter.create<mlir::rlc::AssignOp>(
					action.getLoc(),
					addressOfArgInFrame,
					subF.getBlocks().front().getArgument(arg.index() + 1));
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
			mlir::rlc::FunctionOp subF)
	{
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
		emitActionArgsAssignsToFrame(action, subAction, rewriter, entityType, subF);

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
				mlir::Value({ subF.getBlocks().front().getArgument(0) }));
		rewriter.create<mlir::rlc::Yield>(subF.getLoc());

		auto* elseBranch = rewriter.createBlock(&ifStatement.getElseBranch());
		auto lastYield = rewriter.create<mlir::rlc::Yield>(subF.getLoc());
		rewriter.setInsertionPoint(lastYield);
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
			size_t subActionIndex)
	{
		mlir::IRRewriter rewriter(action.getContext());
		rewriter.setInsertionPoint(action);
		auto type = action.getActions()[subActionIndex]
										.getType()
										.cast<mlir::FunctionType>();
		auto entityType =
				builder.typeOfAction(action).cast<mlir::rlc::EntityType>();

		auto firstStatement = llvm::cast<mlir::rlc::ActionStatement>(subActions[0]);
		auto subF = rewriter.create<mlir::rlc::FunctionOp>(
				action.getLoc(),
				firstStatement.getName(),
				type,
				firstStatement.getDeclaredNames());
		action.getActions()[subActionIndex].replaceAllUsesWith(subF);

		// steals the precondition of all possible actions to be take from here and
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

		redirectAccessesToNewFrame(action, type, subF, builder);

		// creates a block to hold the dispatching mechanism
		llvm::SmallVector<mlir::Location, 2> locs;
		for (size_t i = 0; i < type.getInputs().size(); i++)
			locs.push_back(action.getLoc());
		rewriter.createBlock(
				&subF.getBody(), subF.getBody().begin(), type.getInputs(), locs);

		for (auto* subAct : subActions)
		{
			auto subAction = mlir::cast<mlir::rlc::ActionStatement>(subAct);
			emitDispatchToImpl(action, subAction, rewriter, entityType, subF);
		}

		rewriter.setInsertionPointToEnd(&subF.getBody().front());
		rewriter.create<mlir::rlc::Yield>(subF.getLoc());
	}

	static void emitActionWrapperCalls(
			mlir::rlc::ActionFunction action, mlir::rlc::ModuleBuilder& builder)
	{
		mlir::IRRewriter rewriter(action.getContext());

		rewriter.setInsertionPoint(action);
		auto f = rewriter.create<mlir::rlc::FunctionOp>(
				action.getLoc(),
				action.getUnmangledName(),
				action.getType().cast<mlir::FunctionType>(),
				action.getArgNames());

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

		auto ptrToIndex =
				rewriter.create<mlir::rlc::MemberAccess>(action.getLoc(), frame, 0);

		auto constantZero =
				rewriter.create<mlir::rlc::Constant>(action.getLoc(), int64_t(0));

		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				action.getLoc(), ptrToIndex, constantZero);

		for (const auto& argument : llvm::enumerate(action.getArgNames()))
		{
			auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
					action.getLoc(), frame, argument.index() + 1);

			rewriter.create<mlir::rlc::AssignOp>(
					action.getLoc(),
					addressOfArgInFrame,
					f.getBlocks().front().getArgument(argument.index()));
		}

		rewriter.create<mlir::rlc::CallOp>(
				f.getLoc(),
				mlir::TypeRange(),
				action.getResult(),
				mlir::Value({ frame }));

		rewriter.create<mlir::rlc::Yield>(f.getLoc(), mlir::Value({ frame }));

		for (const auto& subAction : llvm::enumerate(action.getActions()))
		{
			emitSingleActionStatementWrapper(
					action,
					builder,
					builder.actionFunctionValueToActionStatement(subAction.value()),
					subAction.index());
		}
	}

#define GEN_PASS_DEF_LOWERACTIONPASS
#include "rlc/dialect/Passes.inc"

	struct LowerActionPass: impl::LowerActionPassBase<LowerActionPass>
	{
		using impl::LowerActionPassBase<LowerActionPass>::LowerActionPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());
			llvm::SmallVector<mlir::rlc::ActionFunction, 4> acts(
					getOperation().getOps<mlir::rlc::ActionFunction>());
			mlir::IRRewriter rewriter(getOperation().getContext());

			for (auto action : acts)
			{
				replaceAllAccessesWithIndirectOnes(action, builder);
				replaceAllDeclarationsWithIndirectOnes(action, builder);
				emitActionWrapperCalls(action, builder);

				rewriter.setInsertionPoint(action);
				auto isDoneFunction = rewriter.create<mlir::rlc::FunctionOp>(
						action.getLoc(),
						"is_done",
						action.getIsDoneFunctionType(),
						rewriter.getStrArrayAttr({ "frame" }));

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
						(action.getUnmangledName() + "_impl").str(),
						mlir::FunctionType::get(action.getContext(), { actionType }, {}),
						rewriter.getStrArrayAttr({ "frame" }));
				loweredToFun.getBody().takeBody(action.getBody());
				action.getResult().replaceAllUsesWith(loweredToFun);
				rewriter.eraseOp(action);
			}
		}
	};
}	 // namespace mlir::rlc

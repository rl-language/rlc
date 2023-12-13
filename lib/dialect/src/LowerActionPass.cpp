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

	static void addFrameArgumentToPrecondition(
			mlir::rlc::ActionFunction root,
			mlir::FunctionType actionType,
			mlir::rlc::FunctionOp action,
			mlir::rlc::ModuleBuilder& builder)
	{
		mlir::IRRewriter rewriter(action.getContext());
		rewriter.setInsertionPoint(root);

		action.getPrecondition().front().insertArgument(
				static_cast<unsigned int>(0),
				action.getArgumentTypes().front(),
				action.getLoc());

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
			mlir::rlc::FunctionOp subactionFunction,
			mlir::rlc::ActionStatement actionStatement,
			mlir::IRRewriter& rewriter)
	{
		auto& yield =
				subactionFunction.getPrecondition().getBlocks().front().back();
		rewriter.setInsertionPoint(&yield);
		auto savedResumptionIndex = rewriter.create<MemberAccess>(
				actionStatement.getLoc(),
				subactionFunction.getPrecondition().getBlocks().front().getArgument(0),
				0);
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

		for (const auto& subAction :
				 llvm::enumerate(builder.actionStatementsOfAction(action)))
		{
			rewriter.setInsertionPoint(action);
			auto subAct = mlir::cast<mlir::rlc::ActionStatement>(*subAction.value());
			auto type = action.getActions()[subAction.index()]
											.getType()
											.cast<mlir::FunctionType>();
			auto subF = rewriter.create<mlir::rlc::FunctionOp>(
					action.getLoc(), subAct.getName(), type, subAct.getDeclaredNames());
			subF.getPrecondition().takeBody(subAct.getPrecondition());
			addFrameArgumentToPrecondition(action, type, subF, builder);

			addResumptionPointPrecondition(subF, subAct, rewriter);

			action.getActions()[subAction.index()].replaceAllUsesWith(subF);

			llvm::SmallVector<mlir::Location, 2> locs;
			for (size_t i = 0; i < type.getInputs().size(); i++)
				locs.push_back(action.getLoc());

			rewriter.createBlock(
					&subF.getBody(), subF.getBody().begin(), type.getInputs(), locs);

			for (auto arg : llvm::enumerate(subAct.getDeclaredNames()))
			{
				size_t fieldIndex = std::distance(
						entityType.getFieldNames().begin(),
						llvm::find(
								entityType.getFieldNames(),
								arg.value().cast<mlir::StringAttr>().str()));

				auto addressOfArgInFrame = rewriter.create<mlir::rlc::MemberAccess>(
						action.getLoc(),
						subF.getBlocks().front().getArgument(0),
						fieldIndex);

				rewriter.create<mlir::rlc::AssignOp>(
						action.getLoc(),
						addressOfArgInFrame,
						subF.getBlocks().front().getArgument(arg.index() + 1));
			}

			// the resumption point stored in the frame is not enough to discrimated
			// among alternative resumption points, so we have to store the actual id
			// of the selected action so it can resume
			auto resumptionPointSelector = rewriter.create<mlir::rlc::Constant>(
					subF.getLoc(), static_cast<int64_t>(subAct.getId()));
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

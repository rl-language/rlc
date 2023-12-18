#include "rlc/dialect/ActionLiveness.hpp"

namespace mlir::rlc
{
	bool ActionLiveness::isUsedAcrossActions(mlir::Value val)
	{
		if (cache.contains(val))
			return cache.at(val);

		bool isUsedAcrossActions = false;
		fun.walk([&, this](mlir::rlc::ActionStatement subAction) {
			// the action defining the value is allowed have uses after it, of
			// course
			if (val.getDefiningOp() == subAction)
				return;

			if (not liveness.isDeadAfter(val, subAction))
				isUsedAcrossActions = true;
		});

		cache[val] = isUsedAcrossActions;
		return isUsedAcrossActions;
	}

	const ActionLiveness::Partition<mlir::rlc::DeclarationStatement>&
	ActionLiveness::getDeclarationsUsedAcrossActions()
	{
		if (declsStatements.has_value())
			return *declsStatements;

		declsStatements = Partition<mlir::rlc::DeclarationStatement>();
		fun.walk([&](mlir::rlc::DeclarationStatement statement) {
			// declarations do not go in the hidden frame, they can stay on the stack
			// since they are not inputs from outside
			if (isUsedAcrossActions(statement.getResult()))
			{
				declsStatements->usedAcrossActions.push_back(statement);
				valueToSurvivingFrameIndex[statement] =
						valueToSurvivingFrameIndex.size();
			}
		});
		return *declsStatements;
	}

	const ActionLiveness::Partition<mlir::OpResult>&
	ActionLiveness::getStatementsArgsUsedAcrossActions()
	{
		if (operands.has_value())
			return *operands;

		operands = Partition<mlir::OpResult>();
		fun.walk([&](mlir::rlc::ActionStatement statement) {
			for (auto result : statement.getResults())
				if (isUsedAcrossActions(result))
				{
					operands->usedAcrossActions.push_back(result);
					valueToSurvivingFrameIndex[result] =
							valueToSurvivingFrameIndex.size();
				}
				else
				{
					operands->notUsedAcrossActions.push_back(result);
					valueToHiddenFrameIndex[result] = valueToHiddenFrameIndex.size();
				}
		});

		return *operands;
	}

	const ActionLiveness::Partition<mlir::BlockArgument>&
	ActionLiveness::getArgumentsUsedAcrossActions()
	{
		if (mainFunArgs.has_value())
			return *mainFunArgs;

		mainFunArgs = Partition<mlir::BlockArgument>();
		for (auto arg : fun.getBody().front().getArguments())
		{
			if (isUsedAcrossActions(arg))
			{
				mainFunArgs->usedAcrossActions.push_back(arg);
				valueToSurvivingFrameIndex[arg] = valueToSurvivingFrameIndex.size();
				mainFunctionArgToFrameIndex[arg.getArgNumber()] =
						std::pair<bool, size_t>(true, valueToSurvivingFrameIndex[arg]);
			}
			else
			{
				mainFunArgs->notUsedAcrossActions.push_back(arg);
				valueToHiddenFrameIndex[arg] = valueToHiddenFrameIndex.size();
				mainFunctionArgToFrameIndex[arg.getArgNumber()] =
						std::pair<bool, size_t>(false, valueToHiddenFrameIndex[arg]);
			}
		}

		return *mainFunArgs;
	}

	ActionLiveness::ActionFrames ActionLiveness::getFrames()
	{
		ActionFrames toReturn;

		auto funArgs = getArgumentsUsedAcrossActions();
		for (auto arg : funArgs.usedAcrossActions)
		{
			std::string name =
					fun.getArgNames()[arg.getArgNumber()].cast<mlir::StringAttr>().str();
			toReturn.usedAcrossActions.emplace_back(std::move(name), arg.getType());
		}
		for (auto arg : funArgs.notUsedAcrossActions)
		{
			std::string name =
					fun.getArgNames()[arg.getArgNumber()].cast<mlir::StringAttr>().str();
			toReturn.notUsedAcrossActions.emplace_back(
					std::move(name), arg.getType());
		}

		auto actionDecl = getStatementsArgsUsedAcrossActions();
		for (auto arg : actionDecl.usedAcrossActions)
		{
			auto statement = arg.getDefiningOp<mlir::rlc::ActionStatement>();
			std::string name = statement.getDeclaredNames()[arg.getResultNumber()]
														 .cast<mlir::StringAttr>()
														 .str();
			toReturn.usedAcrossActions.emplace_back(std::move(name), arg.getType());
		}
		for (auto arg : actionDecl.notUsedAcrossActions)
		{
			auto statement = arg.getDefiningOp<mlir::rlc::ActionStatement>();
			std::string name = statement.getDeclaredNames()[arg.getResultNumber()]
														 .cast<mlir::StringAttr>()
														 .str();
			toReturn.notUsedAcrossActions.emplace_back(
					std::move(name), arg.getType());
		}

		auto decls = getDeclarationsUsedAcrossActions();
		for (auto decl : decls.usedAcrossActions)
			toReturn.usedAcrossActions.emplace_back(
					decl.getName().str(), decl.getType());

		for (auto decl : decls.notUsedAcrossActions)
			toReturn.notUsedAcrossActions.emplace_back(
					decl.getName().str(), decl.getType());

		return toReturn;
	}

}	 // namespace mlir::rlc

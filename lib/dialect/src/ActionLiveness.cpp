#include "rlc/dialect/ActionLiveness.hpp"

#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/DenseAnalysis.h"

namespace mlir::rlc
{
	namespace detail
	{
		class ActionValueLivenessLattice
				: public mlir::dataflow::AbstractDenseLattice
		{
			public:
			MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ActionValueLivenessLattice);
			explicit ActionValueLivenessLattice(mlir::ProgramPoint point)
					: mlir::dataflow::AbstractDenseLattice(point)
			{
			}

			mlir::ChangeResult meet(
					const mlir::dataflow::AbstractDenseLattice& val) override
			{
				assert(false);
				return mlir::ChangeResult::NoChange;
			}

			mlir::ChangeResult join(
					const mlir::dataflow::AbstractDenseLattice& val) override
			{
				const auto& r = *static_cast<const ActionValueLivenessLattice*>(&val);

				bool changed = false;
				for (auto entry : r.content)
				{
					if (content.contains(entry))
						continue;
					changed = true;
					content.insert(entry);
				}

				if (changed)
					return mlir::ChangeResult::Change;
				return mlir::ChangeResult::NoChange;
			}

			void print(llvm::raw_ostream& os) const override
			{
				os << "VAL = [";
				for (auto value : content)
					value.dump();
				os << "]\n";
			}

			mlir::ChangeResult joinAndMark(
					const ActionValueLivenessLattice& after,
					const mlir::DenseSet<mlir::Value>& justUsed,
					const mlir::DenseSet<mlir::Value>& justDefined)
			{
				bool changed = false;
				if (join(after) == mlir::ChangeResult::Change)
					changed = true;
				for (auto used : justUsed)
				{
					if (not content.contains(used))
					{
						content.insert(used);
						changed = true;
					}
				}
				for (auto defined : justDefined)
				{
					if (content.contains(defined))
					{
						content.erase(defined);
						changed = true;
					}
				}
				if (changed)
					return mlir::ChangeResult::Change;

				return mlir::ChangeResult::NoChange;
			}

			bool isAlive(mlir::Value val) { return content.contains(val); }

			private:
			llvm::DenseSet<mlir::Value> content;
		};

		class ActionValueLivenessAnalysis
				: public mlir::dataflow::DenseBackwardDataFlowAnalysis<
							ActionValueLivenessLattice>
		{
			public:
			using mlir::dataflow::DenseBackwardDataFlowAnalysis<
					ActionValueLivenessLattice>::DenseBackwardDataFlowAnalysis;

			private:
			static bool isOfInterest(mlir::Operation* op)
			{
				// if nobody defined it, it is a block argument, only functions have
				// block argument, thus is a function argument
				if (op == nullptr)
					return true;

				if (mlir::isa<mlir::rlc::DeclarationStatement>(op))
					return true;

				if (mlir::isa<mlir::rlc::ActionStatement>(op))
					return true;

				return false;
			}

			static DenseSet<mlir::Value> getJustUsed(mlir::Operation* op)
			{
				llvm::DenseSet<mlir::Value> toAdd;
				for (auto operand : op->getOperands())
				{
					auto* definingOp = operand.getDefiningOp();
					if (isOfInterest(definingOp))
					{
						toAdd.insert(operand);
					}
				}
				return toAdd;
			}

			static DenseSet<mlir::Value> getJustDefined(mlir::Operation* op)
			{
				llvm::DenseSet<mlir::Value> toKill;
				if (isOfInterest(op))
				{
					for (auto result : op->getResults())
						toKill.insert(result);
				}
				return toKill;
			}

			void visitOperation(
					mlir::Operation* op,
					const ActionValueLivenessLattice& after,
					ActionValueLivenessLattice* before) override
			{
				propagateIfChanged(
						before,
						before->joinAndMark(after, getJustUsed(op), getJustDefined(op)));
			}

			void visitCallControlFlowTransfer(
					CallOpInterface call,
					mlir::dataflow::CallControlFlowAction action,
					const ActionValueLivenessLattice& after,
					ActionValueLivenessLattice* before) final
			{
				propagateIfChanged(
						before,
						before->joinAndMark(
								after, getJustUsed(call), getJustDefined(call)));
			}

			void visitRegionBranchControlFlowTransfer(
					mlir::RegionBranchOpInterface branch,
					RegionBranchPoint regionFrom,
					RegionBranchPoint regionTo,
					const ActionValueLivenessLattice& after,
					ActionValueLivenessLattice* before) override
			{
				bool changed = false;
				if (regionTo.isParent())
				{
					if (before->joinAndMark(after, {}, getJustDefined(branch)) ==
							mlir::ChangeResult::Change)
						changed = true;
				}

				if (regionFrom.isParent())
				{
					if (before->joinAndMark(after, getJustUsed(branch), {}) ==
							mlir::ChangeResult::Change)
						changed = true;
				}

				if (not regionFrom.isParent() and not regionTo.isParent())
				{
					if (before->join(after) == mlir::ChangeResult::Change)
						changed = true;
				}
				propagateIfChanged(
						before,
						changed ? mlir::ChangeResult::Change
										: mlir::ChangeResult::NoChange);
			}

			void setToExitState(ActionValueLivenessLattice* lattice) override {}

			public:
			bool isDeadAfter(mlir::Value val, mlir::Operation* op)
			{
				auto* lattice = op != nullptr ? getLattice(mlir::ProgramPoint(op))
																			: getLattice(op->getBlock());
				return not lattice->isAlive(val);
			}
		};

		class ActionValueLivenessImpl
		{
			public:
			explicit ActionValueLivenessImpl(mlir::rlc::ActionFunction action)
					: entry(action), config()
			{
				config.setInterprocedural(false);
				solver = std::make_unique<DataFlowSolver>(config);
				solver->load<mlir::dataflow::DeadCodeAnalysis>();
				solver->load<mlir::dataflow::SparseConstantPropagation>();
				analysis = solver->load<ActionValueLivenessAnalysis>(symbolTable);
				auto res = solver->initializeAndRun(action);
			}

			bool isDeadAfter(
					mlir::rlc::ActionStatement toCheck,
					mlir::rlc::ActionStatement after) const
			{
				for (auto res : toCheck.getResults())
				{
					if (not analysis->isDeadAfter(res, after))
						return false;
				}
				return true;
			}

			bool isDeadAfter(
					mlir::rlc::DeclarationStatement statement,
					mlir::rlc::ActionStatement after) const
			{
				return analysis->isDeadAfter(statement.getResult(), after);
			}

			bool isDeadAfter(
					mlir::Value value, mlir::rlc::ActionStatement after) const
			{
				return analysis->isDeadAfter(value, after);
			}

			private:
			mlir::SymbolTableCollection symbolTable;
			mlir::rlc::ActionFunction entry;
			DataFlowConfig config;
			std::unique_ptr<DataFlowSolver> solver;
			ActionValueLivenessAnalysis* analysis;
		};
	}	 // namespace detail

	ActionValueLiveness::ActionValueLiveness(mlir::rlc::ActionFunction action)
			: impl(new detail::ActionValueLivenessImpl(action))
	{
	}

	ActionValueLiveness::~ActionValueLiveness() { delete impl; }

	bool ActionValueLiveness::isDeadAfter(
			mlir::Value value, mlir::rlc::ActionStatement statement)
	{
		return impl->isDeadAfter(value, statement);
	}

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

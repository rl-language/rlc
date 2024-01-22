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
#include "rlc/dialect/ActionLiveness.hpp"

#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/DenseAnalysis.h"
#include "mlir/Pass/Pass.h"

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

		class ActionLiveness
		{
			public:
			explicit ActionLiveness(mlir::rlc::ActionFunction action)
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

	static bool isUsedAcrossActions(
			mlir::rlc::ActionFunction fun,
			mlir::Value val,
			detail::ActionLiveness& liveness)
	{
		bool isUsedAcrossActions = false;
		fun.walk([&](mlir::rlc::ActionStatement subAction) {
			// the action defining the value is allowed have uses after it, of
			// course
			if (val.getDefiningOp() == subAction)
				return;

			if (not liveness.isDeadAfter(val, subAction))
				isUsedAcrossActions = true;
		});

		return isUsedAcrossActions;
	}

#define GEN_PASS_DEF_VALIDATESTORAGEQUALIFIERSPASS
#include "rlc/dialect/Passes.inc"

	struct ValidateStorageQualifiersPass
			: impl::ValidateStorageQualifiersPassBase<ValidateStorageQualifiersPass>
	{
		using impl::ValidateStorageQualifiersPassBase<
				ValidateStorageQualifiersPass>::ValidateStorageQualifiersPassBase;

		void runOnOperation() override
		{
			moveCastsNearUses(getOperation());
			for (auto fun : getOperation().getOps<mlir::rlc::ActionFunction>())
			{
				detail::ActionLiveness liveness(fun);
				for (auto arg : fun.getBody().front().getArguments())
				{
					if (not arg.getType().isa<mlir::rlc::FrameType>() and
							isUsedAcrossActions(fun, arg, liveness))
					{
						auto _ = mlir::rlc::logError(
								fun,
								"Action function argument declared as a local variable, but it "
								"is "
								"used in different actions. Rewrite it as frm <name> if this "
								"was intended, this will move it to the action frame.");
						signalPassFailure();
					}
				}

				fun.walk([&, this](mlir::rlc::DeclarationStatement statemenet) {
					if (not statemenet.getType().isa<mlir::rlc::FrameType>() and
							isUsedAcrossActions(fun, statemenet, liveness))
					{
						auto _ = mlir::rlc::logError(
								statemenet,
								"Declaration statement declared as a local variable, but it is "
								"used in different actions. Rewrite it as frm <name> if this "
								"was intended, this will move it to the action frame.");
						signalPassFailure();
					}
				});

				fun.walk([&, this](mlir::rlc::ActionStatement statemenet) {
					for (auto res : statemenet.getResults())
						if (not res.getType().isa<mlir::rlc::FrameType>() and
								isUsedAcrossActions(fun, res, liveness))
						{
							auto _ = mlir::rlc::logError(
									statemenet,
									"Action statement declaration is local, but it "
									"is "
									"used in different actions. Rewrite it as frm <name> if this "
									"was intended, this will move it to the action frame.");
							signalPassFailure();
						}
				});
			}
		}
	};

}	 // namespace mlir::rlc

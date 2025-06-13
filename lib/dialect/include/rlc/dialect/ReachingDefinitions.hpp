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
#pragma once
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/DenseAnalysis.h"
#include "mlir/Pass/Pass.h"
#include "rlc/dialect/Operations.hpp"

namespace mlir
{
	namespace rlc
	{
		class ReachingDefinitionsLattice
				: public mlir::dataflow::AbstractDenseLattice
		{
			public:
			MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ReachingDefinitionsLattice);
			using dataflow::AbstractDenseLattice::AbstractDenseLattice;

			mlir::ChangeResult meet(
					const mlir::dataflow::AbstractDenseLattice& val) override
			{
				assert(false);
				return mlir::ChangeResult::NoChange;
			}

			mlir::ChangeResult join(
					const mlir::dataflow::AbstractDenseLattice& val) override
			{
				const auto& r = *static_cast<const ReachingDefinitionsLattice*>(&val);

				bool changed = false;
				for (auto entry : r.liveValues)
				{
					if (liveValues.contains(entry))
						continue;
					changed = true;
					liveValues.insert(entry);
				}

				if (changed)
					return mlir::ChangeResult::Change;
				return mlir::ChangeResult::NoChange;
			}

			mlir::ChangeResult insert(mlir::Value value)
			{
				if (liveValues.contains(value))
					return ChangeResult::NoChange;
				if (value.getType().isa<mlir::rlc::ContextType>() or
						value.getType().isa<mlir::rlc::FrameType>())
					return ChangeResult::NoChange;
				liveValues.insert(value);
				return ChangeResult::Change;
			}

			mlir::ChangeResult clear()
			{
				if (liveValues.size() == 0)
					return ChangeResult::NoChange;
				liveValues.clear();
				return ChangeResult::Change;
			}

			bool contains(mlir::Value val) { return liveValues.contains(val); }

			void print(raw_ostream& os) const override
			{
				for (auto& value : liveValues)
					value.dump();
			}

			private:
			llvm::DenseSet<mlir::Value> liveValues;
		};

		class ReachingDefinitionsAnalysis
				: public mlir::dataflow::DenseForwardDataFlowAnalysis<
							ReachingDefinitionsLattice>
		{
			public:
			using mlir::dataflow::DenseForwardDataFlowAnalysis<
					ReachingDefinitionsLattice>::DenseForwardDataFlowAnalysis;

			mlir::LogicalResult visitOperation(
					mlir::Operation* op,
					const ReachingDefinitionsLattice& before,
					ReachingDefinitionsLattice* after) override
			{
				bool changed = false;
				auto endLifetime = [&](auto stm) {
					if (after->clear() == ChangeResult::Change)
						changed = true;
				};
				llvm::TypeSwitch<mlir::Operation*>(op)
						.Case<mlir::rlc::DeclarationStatement, mlir::rlc::CallOp>(
								[&](mlir::Operation* op) {
									if (after->join(before) == mlir::ChangeResult::Change)
										changed = true;

									if (after->insert(op->getResult(0)) == ChangeResult::Change)
										changed = true;
								})
						.Case<
								mlir::rlc::ActionStatement,
								mlir::rlc::SubActionStatement,
								mlir::rlc::ActionsStatement>(endLifetime)
						.Default([&](mlir::Operation*) {
							if (after->join(before) == mlir::ChangeResult::Change)
								changed = true;
						});

				if (changed)
					propagateIfChanged(after, ChangeResult::Change);
				else
					propagateIfChanged(after, ChangeResult::NoChange);
				return success();
			}

			void visitCallControlFlowTransfer(
					CallOpInterface call,
					mlir::dataflow::CallControlFlowAction action,
					const ReachingDefinitionsLattice& before,
					ReachingDefinitionsLattice* after) final
			{
				assert(mlir::dataflow::CallControlFlowAction::ExternalCallee == action);
				if (call->getNumResults() == 0)
				{
					propagateIfChanged(after, after->join(before));
				}
				else
				{
					auto _ = visitOperation(call, before, after);
				}
			}
			void visitRegionBranchControlFlowTransfer(
					mlir::RegionBranchOpInterface branch,
					std::optional<unsigned> regionFrom,
					std::optional<unsigned> regionTo,
					const ReachingDefinitionsLattice& before,
					ReachingDefinitionsLattice* after) override
			{
				// when we are leaving the operation, we add our just defined operation
				// to the list of things to destroy
				auto _ = visitOperation(branch, before, after);
			}

			void setToEntryState(ReachingDefinitionsLattice* lattice) override {}

			bool reachesOperation(mlir::Value value, mlir::Operation* op)
			{
				auto* lattice = getLattice(LatticeAnchor(getProgramPointBefore(op)));
				if (value.getType().isa<mlir::rlc::ContextType>() or
						value.getType().isa<mlir::rlc::FrameType>())
					return true;
				return lattice->contains(value);
			}
		};

		class ReachingDefinitions
		{
			public:
			explicit ReachingDefinitions(Operation* action): config()
			{
				config.setInterprocedural(false);
				solver = std::make_unique<DataFlowSolver>(config);
				solver->load<mlir::dataflow::DeadCodeAnalysis>();
				solver->load<mlir::dataflow::SparseConstantPropagation>();
				analysis = solver->load<ReachingDefinitionsAnalysis>();
				auto res = solver->initializeAndRun(action);
			}

			bool reachesOperation(mlir::Value val, mlir::Operation* op)
			{
				return analysis->reachesOperation(val, op);
			}

			private:
			DataFlowConfig config;
			std::unique_ptr<DataFlowSolver> solver;
			ReachingDefinitionsAnalysis* analysis;
		};
	}	 // namespace rlc
}	 // namespace mlir

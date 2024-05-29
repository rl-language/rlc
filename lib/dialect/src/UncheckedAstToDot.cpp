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
#include <set>

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/DenseAnalysis.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
	namespace
	{
		// rapresents a set of action statements that were the last to be executed
		// before reaching a certain point of the program
		class LastActionsTakenLattice: public mlir::dataflow::AbstractDenseLattice
		{
			public:
			MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(LastActionsTakenLattice);
			explicit LastActionsTakenLattice(mlir::ProgramPoint point)
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
				const auto& r = *static_cast<const LastActionsTakenLattice*>(&val);

				auto copy = content;
				for (auto entry : r.content)
					content.insert(entry);

				if (content == copy)
					return mlir::ChangeResult::NoChange;
				return mlir::ChangeResult::Change;
			}

			mlir::ChangeResult copy(const LastActionsTakenLattice& before)
			{
				if (content == before.content)
					return mlir::ChangeResult::NoChange;
				content = before.content;
				return mlir::ChangeResult::Change;
			}

			void print(raw_ostream& os) const override
			{
				os << "LastActions = {";
				for (auto* entry : content)
				{
					entry->print(os);
					os << ",";
				}

				os << "}\n";
			}

			bool operator==(const LastActionsTakenLattice& other) const
			{
				return content == other.content;
			}

			mlir::ChangeResult visitAction(mlir::Operation* statement)
			{
				if (content.contains(statement) and content.size() == 1)
					return mlir::ChangeResult::NoChange;

				content.clear();
				content.insert(statement);
				return mlir::ChangeResult::Change;
			}

			[[nodiscard]] llvm::SmallVector<mlir::Operation*, 4> getPredecessors()
					const
			{
				return llvm::SmallVector<mlir::Operation*, 4>(
						content.begin(), content.end());
			}

			private:
			std::set<mlir::Operation*> content;
		};

		class LastActionAnalysis
				: public mlir::dataflow::DenseForwardDataFlowAnalysis<
							LastActionsTakenLattice>
		{
			public:
			using mlir::dataflow::DenseForwardDataFlowAnalysis<
					LastActionsTakenLattice>::DenseForwardDataFlowAnalysis;

			private:
			void visitOperation(
					mlir::Operation* op,
					const LastActionsTakenLattice& before,
					LastActionsTakenLattice* after) override
			{
				assert(after != nullptr);
				if (auto action = mlir::dyn_cast<mlir::rlc::ActionStatement>(op))
					propagateIfChanged(after, after->visitAction(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionFunction>(op))
					propagateIfChanged(after, after->visitAction(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionsStatement>(op))
					propagateIfChanged(after, after->visitAction(action));
				else if (
						auto action = mlir::dyn_cast<mlir::rlc::SubActionStatement>(op))
					propagateIfChanged(after, after->visitAction(action));
				else
					propagateIfChanged(after, after->copy(before));
			}

			void visitCallControlFlowTransfer(
					CallOpInterface call,
					mlir::dataflow::CallControlFlowAction action,
					const LastActionsTakenLattice& before,
					LastActionsTakenLattice* after) final
			{
				propagateIfChanged(after, after->copy(before));
			}

			void visitRegionBranchControlFlowTransfer(
					mlir::RegionBranchOpInterface branch,
					std::optional<unsigned> regionFrom,
					std::optional<unsigned> regionTo,
					const LastActionsTakenLattice& before,
					LastActionsTakenLattice* after) override
			{
				if (auto action = mlir::dyn_cast<mlir::rlc::ActionStatement>(
								branch.getOperation()))
					propagateIfChanged(after, after->visitAction(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionFunction>(
										 branch.getOperation());
								 action && not regionFrom.has_value())
					propagateIfChanged(after, after->visitAction(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionsStatement>(
										 branch.getOperation());
								 action && not regionFrom.has_value())
					propagateIfChanged(after, after->visitAction(action));
				else if (
						auto action = mlir::dyn_cast<mlir::rlc::SubActionStatement>(
								branch.getOperation()))
					propagateIfChanged(after, after->visitAction(action));
				else
					propagateIfChanged(after, after->join(before));
			}

			// no action is executed at the starting entry point
			void setToEntryState(LastActionsTakenLattice* lattice) override
			{
				// propagateIfChanged(lattice, lattice->visitAction(nullptr));
			}

			public:
			llvm::SmallVector<mlir::Operation*, 4> getPredecessors(
					mlir::Operation* op)
			{
				auto* lattice = op->getPrevNode() != nullptr
														? getLattice(mlir::ProgramPoint(op->getPrevNode()))
														: getLattice(op->getBlock());
				return lattice->getPredecessors();
			}
		};

		class ActionFlowNode
		{
			public:
			ActionFlowNode(mlir::Operation* current)
					: currentOperation(current), successors()
			{
			}

			mlir::Operation* getOperation() const { return currentOperation; }

			void addSuccessor(const ActionFlowNode& other)
			{
				successors.insert(&other);
			}

			void removeSuccessor(const ActionFlowNode& other)
			{
				successors.erase(&other);
			}

			auto begin() const { return successors.begin(); }
			auto end() const { return successors.end(); }

			void dump() const { print(llvm::errs()); }

			void print(llvm::raw_ostream& os) const
			{
				os << "\"" << currentOperation << "\"[shape=" << shape() << ", label=\""
					 << name() << "\", style=" << style() << "]\n";
				for (auto* successor : successors)
				{
					os << "\"" << currentOperation << "\" -> \""
						 << successor->currentOperation << "\"\n";
				}
			}

			void setOperation(mlir::Operation* op) { this->currentOperation = op; }

			private:
			std::string shape() const
			{
				if (llvm::isa<mlir::rlc::ActionFunction>(currentOperation))
					return "ellipse";
				if (llvm::isa<mlir::rlc::ActionsStatement>(currentOperation))
					return "diamond";
				if (llvm::isa<mlir::rlc::ActionStatement>(currentOperation))
					return "box";
				if (llvm::isa<mlir::rlc::SubActionStatement>(currentOperation))
					return "box";
				if (llvm::isa<mlir::rlc::Yield>(currentOperation))
					return "ellipse";

				currentOperation->dump();
				abort();
			}

			std::string style() const
			{
				if (llvm::isa<mlir::rlc::SubActionStatement>(currentOperation))
					return "dotted";
				return "solid";
			}

			std::string name() const
			{
				if (auto casted =
								llvm::dyn_cast<mlir::rlc::ActionFunction>(currentOperation))
					return casted.getUnmangledName().str();
				if (auto casted =
								llvm::dyn_cast<mlir::rlc::ActionsStatement>(currentOperation))
					return "";
				if (auto casted =
								llvm::dyn_cast<mlir::rlc::ActionStatement>(currentOperation))
					return casted.getName().str();
				if (auto casted =
								llvm::dyn_cast<mlir::rlc::SubActionStatement>(currentOperation))
					return casted.getName().str();
				if (auto casted = llvm::dyn_cast<mlir::rlc::Yield>(currentOperation))
					return "ret";
				currentOperation->dump();
				abort();
			}

			mlir::Operation* currentOperation;
			llvm::DenseSet<const ActionFlowNode*> successors;
		};

		class ActionFlowGraph
		{
			public:
			explicit ActionFlowGraph(mlir::rlc::ActionFunction action): entry(action)
			{
				mlir::IRRewriter rewriter(action.getContext());
				rewriter.setInsertionPointToStart(&action.getBody().front());
				auto proxyEntry = rewriter.create<mlir::rlc::ActionStatement>(
						action.getLoc(),
						mlir::TypeRange({}),
						"proxy",
						llvm::ArrayRef<std::string>({}),
						0,
						0);

				DataFlowConfig config;
				config.setInterprocedural(false);
				DataFlowSolver solver(config);
				solver.load<mlir::dataflow::DeadCodeAnalysis>();
				solver.load<mlir::dataflow::SparseConstantPropagation>();
				auto* analsyis = solver.load<mlir::rlc::LastActionAnalysis>();
				auto res = solver.initializeAndRun(action);

				action.walk([&](mlir::Operation* op) {
					if (mlir::isa<mlir::rlc::ActionStatement>(op) or
							mlir::isa<mlir::rlc::ActionsStatement>(op) or
							mlir::isa<mlir::rlc::SubActionStatement>(op))
						makeNode(op);
				});

				auto* end = makeNode(action.getBody().front().getTerminator());

				for (auto& pair : nodes)
				{
					auto* successor = pair.first;
					auto pred = analsyis->getPredecessors(successor);

					for (auto* predecessor : pred)
						nodes[predecessor]->addSuccessor(*nodes[successor]);
				}

				// connect the predeccessors of return statements to the exit node
				action.walk([&](mlir::rlc::ReturnStatement op) {
					for (auto* predecessor : analsyis->getPredecessors(op))
						nodes[predecessor]->addSuccessor(*end);
				});

				auto* begin = makeNode(action);
				const auto isEdgeToBegin = [&](const ActionFlowNode* successor) {
					return successor->getOperation() == proxyEntry;
				};
				for (Content::iterator::value_type& node : nodes)
				{
					if (llvm::any_of(*node.second, isEdgeToBegin))
					{
						node.second->addSuccessor(*begin);
					}
				}
				for (const ActionFlowNode* succ : *nodes[proxyEntry])
				{
					begin->addSuccessor(*succ);
				}

				eraseNode(proxyEntry);
				proxyEntry.erase();
			}

			void eraseNode(mlir::Operation* op)
			{
				for (auto& pair : nodes)
					pair.second->removeSuccessor(*nodes[op]);
				nodes.erase(op);
			}

			[[nodiscard]] const ActionFlowNode& getEntryNode() const
			{
				return *nodes.at(entry);
			}

			auto begin() const { return nodes.begin(); }

			auto end() const { return nodes.end(); }

			void dump() const { print(llvm::errs()); }

			void print(llvm::raw_ostream& os) const
			{
				llvm::outs() << "digraph g {\n";
				for (auto& node : nodes)
					node.second->print(os);
				llvm::outs() << "}\n";
			}

			private:
			using Content =
					std::map<mlir::Operation*, std::unique_ptr<ActionFlowNode>>;
			ActionFlowNode* makeNode(mlir::Operation* current)
			{
				assert(not nodes.contains(current));
				nodes[current] = std::make_unique<ActionFlowNode>(current);
				return nodes[current].get();
			}

			mlir::rlc::ActionFunction entry;
			Content nodes;
		};
	}	 // namespace

#define GEN_PASS_DEF_UNCHECKEDASTTODOTPASS
#include "rlc/dialect/Passes.inc"

	struct UncheckedAstToDotPass
			: impl::UncheckedAstToDotPassBase<UncheckedAstToDotPass>
	{
		using impl::UncheckedAstToDotPassBase<
				UncheckedAstToDotPass>::UncheckedAstToDotPassBase;

		void runOnOperation() override
		{
			std::map<
					mlir::rlc::ActionStatement,
					llvm::SmallVector<mlir::rlc::ActionStatement, 4>>
					dependencies;

			*OS << " digraph g {\n";

			for (auto op : getOperation().getOps<mlir::rlc::ActionFunction>())
			{
				ActionFlowGraph graph(op);
				for (auto& node : graph)
					node.second->print(*OS);
			}

			*OS << " }";
		}
	};
}	 // namespace mlir::rlc

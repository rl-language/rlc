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

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/DirectedGraph.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Regex.h"
#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/DenseAnalysis.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/ProgramGraph.hpp"
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
			using dataflow::AbstractDenseLattice::AbstractDenseLattice;

			mlir::ChangeResult meet(
					const mlir::dataflow::AbstractDenseLattice &val) override
			{
				assert(false);
				return mlir::ChangeResult::NoChange;
			}

			mlir::ChangeResult join(
					const mlir::dataflow::AbstractDenseLattice &val) override
			{
				const auto &r = *static_cast<const LastActionsTakenLattice *>(&val);

				auto copy = content;
				for (auto entry : r.content)
					content.insert(entry);

				if (content == copy)
					return mlir::ChangeResult::NoChange;
				return mlir::ChangeResult::Change;
			}

			mlir::ChangeResult copy(const LastActionsTakenLattice &before)
			{
				if (content == before.content)
					return mlir::ChangeResult::NoChange;
				content = before.content;
				return mlir::ChangeResult::Change;
			}

			void print(raw_ostream &os) const override
			{
				os << "LastActions = {";
				for (auto *entry : content)
				{
					entry->print(os);
					os << ",";
				}

				os << "}\n";
			}

			bool operator==(const LastActionsTakenLattice &other) const
			{
				return content == other.content;
			}

			mlir::ChangeResult visitAction(mlir::Operation *statement)
			{
				if (content.contains(statement) and content.size() == 1)
					return mlir::ChangeResult::NoChange;

				content.clear();
				content.insert(statement);
				return mlir::ChangeResult::Change;
			}

			[[nodiscard]] llvm::SmallVector<mlir::Operation *, 4> getPredecessors()
					const
			{
				return llvm::SmallVector<mlir::Operation *, 4>(
						content.begin(), content.end());
			}

			private:
			std::set<mlir::Operation *> content;
		};

		class LastActionAnalysis
				: public mlir::dataflow::DenseForwardDataFlowAnalysis<
							LastActionsTakenLattice>
		{
			public:
			using mlir::dataflow::DenseForwardDataFlowAnalysis<
					LastActionsTakenLattice>::DenseForwardDataFlowAnalysis;

			private:
			mlir::LogicalResult visitOperation(
					mlir::Operation *op,
					const LastActionsTakenLattice &before,
					LastActionsTakenLattice *after) override
			{
				assert(after != nullptr);
				if (auto action = mlir::dyn_cast<mlir::rlc::ActionStatement>(op))
					propagateIfChanged(after, after->visitAction(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionFunction>(op))
					propagateIfChanged(after, after->visitAction(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionsStatement>(op))
					propagateIfChanged(after, after->visitAction(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::SubActionInfo>(op))
					propagateIfChanged(after, after->visitAction(action));
				else
					propagateIfChanged(after, after->copy(before));
				return mlir::success();
			}

			void visitCallControlFlowTransfer(
					CallOpInterface call,
					mlir::dataflow::CallControlFlowAction action,
					const LastActionsTakenLattice &before,
					LastActionsTakenLattice *after) final
			{
				propagateIfChanged(after, after->copy(before));
			}

			void visitRegionBranchControlFlowTransfer(
					mlir::RegionBranchOpInterface branch,
					std::optional<unsigned> regionFrom,
					std::optional<unsigned> regionTo,
					const LastActionsTakenLattice &before,
					LastActionsTakenLattice *after) override
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
						auto action =
								mlir::dyn_cast<mlir::rlc::SubActionInfo>(branch.getOperation()))
					propagateIfChanged(after, after->visitAction(action));
				else
					propagateIfChanged(after, after->join(before));
			}

			// no action is executed at the starting entry point
			void setToEntryState(LastActionsTakenLattice *lattice) override
			{
				// propagateIfChanged(lattice, lattice->visitAction(nullptr));
			}

			public:
			llvm::SmallVector<mlir::Operation *, 4> getPredecessors(
					mlir::Operation *op)
			{
				auto *lattice = getLattice(LatticeAnchor(getProgramPointBefore(op)));
				return lattice->getPredecessors();
			}
		};

		llvm::SmallVector<ActionFlowNode *> findPredecessors(
				ActionFlowGraph &Graph, const ActionFlowNode *Target)
		{
			llvm::SmallVector<ActionFlowNode *> Preds;
			for (const auto &Node : Graph)
			{
				auto &Candidate = *Node;
				if (Candidate.hasSuccessor(*Target))
					Preds.push_back(&Candidate);
			}
			return Preds;
		}

		using InliningCache =
				std::unordered_map<const ActionFlowNode *, ActionFlowNode *>;
		ActionFlowNode *cloneCalleeCFG(
				const ActionFlowNode &ToInlineEntry,
				ActionFlowGraph &IntoGraph,
				InliningCache &cache)
		{
			ActionFlowNode *newExit = nullptr;
			for (const ActionFlowNode *node : llvm::depth_first(&ToInlineEntry))
			{
				auto &clone = *IntoGraph.addNode(*node);
				cache[node] = &clone;
				if (clone.getKind() == ActionFlowNode::Kind::exit)
					newExit = &clone;
			}

			return newExit;
		}

		void redirectClonedEdges(
				const ActionFlowNode &ToInlineEntry, InliningCache &cache)
		{
			for (const ActionFlowNode *node : llvm::depth_first(&ToInlineEntry))
			{
				ActionFlowNode *Clone = cache.at(node);	 // must exist

				llvm::SmallVector<const ActionFlowNode *, 8> oldSuccessors(
						Clone->begin(), Clone->end());
				Clone->clearSuccessors();

				/* 2. rebuild it with re-mapped targets -------------------------- */
				for (const ActionFlowNode *oldSuccessor : oldSuccessors)
					Clone->addSuccessor(*cache.at(oldSuccessor));
			}
		}

		void cloneFunction(
				InliningCache &entryToExit,
				InliningCache &originalToClone,
				const ActionFlowNode &callSite,
				ActionFlowGraph &callerGraph)
		{
			for (const auto &callee : callSite.callees_range())
				entryToExit[callee.first] =
						cloneCalleeCFG(*callee.first, callerGraph, originalToClone);
			for (const auto &callee : callSite.callees_range())
				redirectClonedEdges(*callee.first, originalToClone);
		}

		bool isToErase(const ActionFlowGraph &graph, const ActionFlowNode &node)
		{
			return (&graph.getEntryNode() != &node and
							node.getKind() == ActionFlowNode::Kind::entry) or
						 (node.getKind() == ActionFlowNode::Kind::exit and
							node.numSuccessors() != 0);
		}

		bool isNonAction(const ActionFlowGraph &graph, const ActionFlowNode &node)
		{
			return node.getKind() != ActionFlowNode::Kind::action and
						 node.getKind() != ActionFlowNode::Kind::exit;
		}

		void eraseNodesIf(ActionFlowGraph &graph, auto &&filter)
		{
			llvm::DenseSet<const ActionFlowNode *> toErase;
			for (const auto &pair : graph)
			{
				ActionFlowNode &node = *pair;

				llvm::DenseSet<const ActionFlowNode *> successorSuccessors;
				for (const ActionFlowNode *successor : node)
				{
					if (filter(graph, *successor))
					{
						toErase.insert(successor);
						for (const ActionFlowNode *successorSuccessor : *successor)
							successorSuccessors.insert(successorSuccessor);
					}
				}

				for (const ActionFlowNode *successorSuccessor : successorSuccessors)
					node.addSuccessor(*successorSuccessor);
			}

			for (const ActionFlowNode *node : toErase)
				graph.unlinkAndErase(*node);
		}

		void dropUselessBeginAndEnd(ActionFlowGraph &graph)
		{
			eraseNodesIf(graph, isToErase);
		}

		void dropNonActions(ActionFlowGraph &graph)
		{
			eraseNodesIf(graph, isNonAction);
		}

		void inlineCalls(ActionFlowGraph &callerGraph)
		{
			llvm::SmallVector<const ActionFlowNode *, 4> callSites;
			for (const auto &pair : callerGraph)
			{
				const ActionFlowNode &node = *pair;
				if (node.getKind() != ActionFlowNode::Kind::call)
					continue;
				callSites.push_back(&node);
			}

			for (const ActionFlowNode *callSite : callSites)
			{
				InliningCache originalToClone;
				InliningCache entryToExit;
				cloneFunction(entryToExit, originalToClone, *callSite, callerGraph);

				// connect inlined entry to predecessor of callsite
				auto predecessors = findPredecessors(callerGraph, callSite);
				for (ActionFlowNode *predecessor : predecessors)
				{
					predecessor->removeSuccessor(*callSite);
					for (const auto &callee : callSite->callees_range())
						predecessor->addSuccessor(*originalToClone[callee.first]);
				}

				// connect inlined exit to successors of callsite
				for (const auto &callee : callSite->callees_range())
					for (const ActionFlowNode *successor : *callSite)
						entryToExit[callee.first]->addSuccessor(*successor);

				callerGraph.eraseNode(*callSite);
				dropUselessBeginAndEnd(callerGraph);
			}
		}

		void inlineRegularCalls(ActionFlowForest &Forest)
		{
			Forest.cacheCallees();
			llvm::SmallVector<ActionFlowGraph *, 4> sorted;
			llvm::DenseSet<ActionFlowGraph *> visited;

			for (auto &graph : Forest)
				for (auto *graph : llvm::post_order(&graph.second))
				{
					if (not visited.contains(graph))
						sorted.push_back(graph);
					visited.insert(graph);
				}

			for (auto &graph : sorted)
				inlineCalls(*graph);
		}
	}	 // namespace
	ActionFlowGraph::ActionFlowGraph(mlir::rlc::ActionFunction action)
			: entry(action)
	{
		mlir::IRRewriter rewriter(action.getContext());
		rewriter.setInsertionPointToStart(&action.getBody().front());
		auto proxyEntry = rewriter.create<mlir::rlc::ActionStatement>(
				action.getLoc(),
				mlir::TypeRange({}),
				"proxy",
				mlir::rlc::FunctionInfoAttr::get(action.getContext()),
				0,
				0);

		DataFlowConfig config;
		config.setInterprocedural(false);
		DataFlowSolver solver(config);
		solver.load<mlir::dataflow::DeadCodeAnalysis>();
		solver.load<mlir::dataflow::SparseConstantPropagation>();
		auto *analsyis = solver.load<mlir::rlc::LastActionAnalysis>();
		auto res = solver.initializeAndRun(action);

		action.walk([&](mlir::Operation *op) {
			if (mlir::isa<mlir::rlc::ActionStatement>(op) or
					mlir::isa<mlir::rlc::ActionsStatement>(op) or
					mlir::isa<mlir::rlc::SubActionInfo>(op))
				makeNode(op);
		});

		auto *end = makeNode(action.getBody().front().getTerminator());

		for (auto &pair : nodes)
		{
			const ActionFlowNode &successor = *pair;
			auto pred = analsyis->getPredecessors(successor.getOperation());

			for (auto *predecessor : pred)
				get(predecessor).addSuccessor(get(successor.getOperation()));
		}

		// connect the predeccessors of return statements to the exit node
		action.walk([&](mlir::rlc::ReturnStatement op) {
			for (auto *predecessor : analsyis->getPredecessors(op))
				get(predecessor).addSuccessor(*end);
		});

		const auto isEdgeToBegin = [&](const ActionFlowNode *successor) {
			return successor->getOperation() == proxyEntry;
		};
		auto *begin = makeNode(action);
		nodes.insert(nodes.begin(), std::move(nodes.back()));
		nodes.pop_back();

		for (auto &node : nodes)
		{
			if (llvm::any_of(*node, isEdgeToBegin))
			{
				node->addSuccessor(*begin);
			}
		}
		for (const ActionFlowNode *succ : get(proxyEntry))
		{
			begin->addSuccessor(*succ);
		}

		eraseNode(get(proxyEntry));
		proxyEntry.erase();
	}

	void ActionFlowForest::print(llvm::raw_ostream &OS, llvm::StringRef regex)
	{
		llvm::Regex r(regex);
		OS << " digraph g {\n";
		for (auto &pair : map)
		{
			if (not r.match(pair.second.actionName()))
				continue;
			for (const auto &node : pair.second)
				node->print(OS);
		}

		OS << " }";
	}

	void ActionFlowNode::printParsable(llvm::raw_ostream &os) const
	{
		os << kindToString() << " " << this << " " << name() << "\n";
		for (const auto *successor : successors)
		{
			os << "s " << successor << "\n";
		}

		for (const auto &callee : callees)
		{
			os << "c " << callee.first << "\n";
		}
	}

	void ActionFlowForest::printParsable(
			llvm::raw_ostream &OS, llvm::StringRef regexFilter)
	{
		llvm::Regex r(regexFilter);
		for (auto &pair : map)
		{
			if (not r.match(pair.second.actionName()))
				continue;
			for (const auto &node : pair.second)
				node->printParsable(OS);
		}
	}

	void dropNonActions(ActionFlowForest &forest)
	{
		for (auto &graph : forest)
		{
			dropNonActions(graph.second);
		}
	}

#define GEN_PASS_DEF_UNCHECKEDASTTODOTPASS
#include "rlc/dialect/Passes.inc"

	struct UncheckedAstToDotPass
			: impl::UncheckedAstToDotPassBase<UncheckedAstToDotPass>
	{
		using impl::UncheckedAstToDotPassBase<
				UncheckedAstToDotPass>::UncheckedAstToDotPassBase;

		void runOnOperation() override
		{
			// drop the content of sub actions so we don't see them
			getOperation().walk([](mlir::rlc::SubActionInfo info) {
				info.getBody().front().erase();
			});
			mlir::rlc::ActionFlowForest forest(getOperation());
			if (inline_calls)
				inlineRegularCalls(forest);
			if (drop_non_actions)
				dropNonActions(forest);
			if (print_parsable)
				forest.printParsable(*OS, regex_filter);
			else
				forest.print(*OS, regex_filter);
		}
	};
}	 // namespace mlir::rlc

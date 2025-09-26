#pragma once
/*
Copyright 2025 Massimo Fioravanti

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

#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/iterator.h"
#include "mlir/IR/Operation.h"
#include "rlc/dialect/Operations.hpp"

namespace mlir::rlc
{

	class ActionFlowGraph;
	class ActionFlowNode
	{
		public:
		enum class Kind
		{
			entry,
			exit,
			call,
			call_once,
			alternative,
			action
		};
		using SuccessorContainer = llvm::DenseSet<const ActionFlowNode*>;
		using CalleesContainer = llvm::DenseSet<
				std::pair<const ActionFlowNode*, mlir::rlc::ActionFunction>>;
		ActionFlowNode(mlir::Operation* current)
				: currentOperation(current), kind(kindFromOp(current))
		{
		}

		[[nodiscard]] Kind getKind() const { return kind; }

		// return true if the node is artificial and does not describe a real node
		// of the graph. (for example inlining can duplicate the graph and
		// generate a artificial node)
		[[nodiscard]] bool isArtificial() const
		{
			return currentOperation == nullptr;
		}

		ActionFlowNode(Kind kind): currentOperation(nullptr), kind(kind) {}

		mlir::Operation* getOperation() const { return currentOperation; }

		void addCallee(ActionFunction actFun, const ActionFlowNode& other)
		{
			callees.insert(std::make_pair(&other, actFun));
		}

		[[nodiscard]] size_t numSuccessors() const { return successors.size(); }

		void addSuccessor(const ActionFlowNode& other)
		{
			successors.insert(&other);
		}

		bool hasSuccessor(const ActionFlowNode& other) const
		{
			return successors.contains(&other);
		}

		void clearSuccessors() { successors.clear(); }
		void clearCallees() { callees.clear(); }

		void removeSuccessor(const ActionFlowNode& other)
		{
			successors.erase(&other);
		}

		auto begin() { return successors.begin(); }
		auto end() { return successors.end(); }

		auto begin() const { return successors.begin(); }
		auto end() const { return successors.end(); }

		auto callees_begin() const { return callees.begin(); }
		auto callees_end() const { return callees.end(); }

		auto callees_begin() { return callees.begin(); }
		auto callees_end() { return callees.end(); }

		auto callees_range()
		{
			return llvm::make_range(callees_begin(), callees_end());
		}
		auto callees_range() const
		{
			return llvm::make_range(callees_begin(), callees_end());
		}

		void dump() const { print(llvm::errs()); }

		void print(llvm::raw_ostream& os) const
		{
			os << "\"" << this << "\"[shape=" << shape() << ", label=\"" << name()
				 << "\", style=" << style() << "]\n";
			for (const auto* successor : successors)
			{
				os << "\"" << this << "\" -> \"" << successor << "\"\n";
			}

			for (const auto& callee : callees)
			{
				os << "\"" << this << "\"" << " -> "
					 << "\"" << callee.first << "\"[style=\"dashed\"]";
			}
		}

		void printParsable(llvm::raw_ostream& os) const;

		void setOperation(mlir::Operation* op) { this->currentOperation = op; }

		[[nodiscard]] llvm::SmallVector<mlir::Type, 4> getInvokedSubActions()
		{
			llvm::SmallVector<mlir::Type, 4> toReturn;
			auto subAction = dyn_cast<SubActionInfo>(currentOperation);
			if (not subAction)
				return {};
			for (auto typeAttr : subAction.getTypes())
			{
				auto type =
						decayCtxFrmType(mlir::cast<mlir::TypeAttr>(typeAttr).getValue());
				if (auto alternative = mlir::dyn_cast<mlir::rlc::AlternativeType>(type))
					for (auto entry : alternative.getUnderlying())
						toReturn.push_back(decayCtxFrmType(entry));
				else
					toReturn.push_back(decayCtxFrmType(type));
			}
			return toReturn;
		}

		ActionFlowGraph* getParent() { return parent; }
		ActionFlowGraph* getParent() const { return parent; }

		void setParent(ActionFlowGraph* newParent) { parent = newParent; }

		private:
		static Kind kindFromOp(mlir::Operation* op)
		{
			if (mlir::isa<mlir::rlc::ActionFunction>(op))
				return Kind::entry;
			if (mlir::isa<mlir::rlc::ActionsStatement>(op))
				return Kind::alternative;
			if (mlir::isa<mlir::rlc::ActionStatement>(op))
				return Kind::action;
			if (auto casted = mlir::dyn_cast<mlir::rlc::SubActionInfo>(op))
				return casted.getRunOnce() ? Kind::call_once : Kind::call;
			if (mlir::isa<mlir::rlc::Yield>(op))
				return Kind::exit;
			if (mlir::isa<mlir::rlc::ReturnStatement>(op))
				return Kind::exit;

			op->dump();
			abort();
		}

		std::string kindToString() const
		{
			switch (kind)
			{
				case Kind::entry:
					return "entry";
				case Kind::alternative:
					return "alternative";
				case Kind::action:
					return "action";
				case Kind::call_once:
					return "call";
				case Kind::call:
					return "call*";
				case Kind::exit:
					return "exit";
			}

			abort();
		}
		std::string shape() const
		{
			switch (kind)
			{
				case Kind::entry:
				case Kind::exit:
					return "ellipse";
				case Kind::alternative:
					return "diamond";
				case Kind::action:
				case Kind::call_once:
				case Kind::call:
					return "box";
			}

			abort();
		}

		std::string style() const
		{
			switch (kind)
			{
				case Kind::entry:
				case Kind::exit:
				case Kind::alternative:
				case Kind::action:
					return "solid";
				case Kind::call_once:
				case Kind::call:
					return "dotted";
			}
		}

		std::string name() const
		{
			if (currentOperation == nullptr)
				return "";
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
							llvm::dyn_cast<mlir::rlc::SubActionInfo>(currentOperation))
			{
				std::string result;
				for (auto type : casted.getTypes())
					result += prettyType(decayCtxFrmType(
												mlir::cast<mlir::TypeAttr>(type).getValue())) +
										" ";
				return result;
			}

			if (auto casted = llvm::dyn_cast<mlir::rlc::Yield>(currentOperation))
				return "ret";
			if (auto casted =
							llvm::dyn_cast<mlir::rlc::ReturnStatement>(currentOperation))
				return "ret";
			currentOperation->dump();
			abort();
		}

		mlir::Operation* currentOperation;
		Kind kind;
		SuccessorContainer successors;
		CalleesContainer callees;
		ActionFlowGraph* parent;
	};

	class ActionFlowGraph
	{
		public:
		explicit ActionFlowGraph(mlir::rlc::ActionFunction action);

		void unlinkAndErase(const ActionFlowNode& toErase)
		{
			auto* iter = llvm::find_if(
					nodes, [&toErase](auto& node) { return node.get() == &toErase; });
			if (iter == nodes.end())
				return;
			assert(iter != nodes.end());
			for (auto& pair : nodes)
				if (pair->hasSuccessor(**iter))
					pair->removeSuccessor(**iter);
			nodes.erase(iter);
		}

		void eraseNode(const ActionFlowNode& toRemove)
		{
			llvm::erase_if(
					nodes, [&toRemove](auto& node) { return &toRemove == node.get(); });
		}

		ActionFlowNode* addNode(const ActionFlowNode& node)
		{
			nodes.push_back(std::make_unique<ActionFlowNode>(node));
			nodes.back()->setParent(this);
			return nodes.back().get();
		}

		[[nodiscard]] const ActionFlowNode& getEntryNode() const
		{
			return *nodes.front();
		}

		auto begin() const { return nodes.begin(); }

		auto end() const { return nodes.end(); }

		auto callee_begin() const { return _cacheCallees.begin(); }
		auto callee_end() const { return _cacheCallees.end(); }

		auto callee_begin() { return _cacheCallees.begin(); }
		auto callee_end() { return _cacheCallees.end(); }

		void dump() const { print(llvm::errs()); }

		void print(llvm::raw_ostream& os) const
		{
			llvm::outs() << "digraph g {\n";
			for (auto& node : nodes)
				node->print(os);
			llvm::outs() << "}\n";
		}

		ActionFlowNode& get(mlir::Operation* op) { return *opToMapCache.at(op); }
		const ActionFlowNode& get(mlir::Operation* op) const
		{
			return *opToMapCache.at(op);
		}

		using Content = llvm::SmallVector<std::unique_ptr<ActionFlowNode>, 4>;

		llvm::StringRef actionName() const { return entry.getUnmangledName(); }

		void cacheCallees(std::map<const void*, ActionFlowGraph>& typeToGraphMap)
		{
			for (auto& node : nodes)
			{
				for (auto& callSite : node->callees_range())
				{
					_cacheCallees.insert(&typeToGraphMap.at(
							callSite.second.getClassType().getAsOpaquePointer()));
				}
			}
		}

		ActionFunction getActionFunction() { return entry; }

		using CalleeCacheType = llvm::DenseSet<ActionFlowGraph*>;

		private:
		ActionFlowNode* makeNode(mlir::Operation* current)
		{
			nodes.push_back(std::make_unique<ActionFlowNode>(current));
			opToMapCache[current] = nodes.back().get();
			nodes.back()->setParent(this);
			return nodes.back().get();
		}

		mutable mlir::rlc::ActionFunction entry;
		Content nodes;
		CalleeCacheType _cacheCallees;
		llvm::DenseMap<Operation*, ActionFlowNode*> opToMapCache;
	};

	class ActionFlowForest
	{
		public:
		ActionFlowForest(mlir::ModuleOp op)
		{
			for (auto op : op.getOps<mlir::rlc::ActionFunction>())
				map.emplace(op.getClassType().getAsOpaquePointer(), op);

			populateCallGraph();
		}

		void populateCallGraph()
		{
			for (auto& pair : map)
			{
				for (const auto& node : pair.second)
				{
					ActionFlowNode& current = *node;
					for (mlir::Type type : current.getInvokedSubActions())
					{
						auto& pair = map.at(decayCtxFrmType(type).getAsOpaquePointer());
						const ActionFlowNode& target = pair.getEntryNode();

						current.addCallee(pair.getActionFunction(), target);
					}
				}
			}
		}

		void print(llvm::raw_ostream& OS, llvm::StringRef regex);

		void printParsable(llvm::raw_ostream& OS, llvm::StringRef regexFilter);

		auto begin() { return map.begin(); }
		auto end() { return map.end(); }

		auto begin() const { return map.begin(); }
		auto end() const { return map.end(); }

		void cacheCallees()
		{
			for (auto& node : map)
			{
				node.second.cacheCallees(map);
			}
		}

		private:
		std::map<const void*, ActionFlowGraph> map;
	};

}	 // namespace mlir::rlc

namespace llvm
{

	template<>
	struct GraphTraits<mlir::rlc::ActionFlowNode*>
	{
		using NodeRef = mlir::rlc::ActionFlowNode*;
		using ChildIteratorType =
				mlir::rlc::ActionFlowNode::SuccessorContainer::Iterator;

		static NodeRef getEntryNode(NodeRef N) { return N; }
		static ChildIteratorType child_begin(NodeRef N) { return (*N).begin(); }
		static ChildIteratorType child_end(NodeRef N) { return (*N).end(); }
	};

	template<>
	struct GraphTraits<const mlir::rlc::ActionFlowNode*>
	{
		using NodeRef = const mlir::rlc::ActionFlowNode*;
		using ChildIteratorType =
				mlir::rlc::ActionFlowNode::SuccessorContainer::ConstIterator;

		static NodeRef getEntryNode(NodeRef N) { return N; }
		static ChildIteratorType child_begin(NodeRef N) { return (*N).begin(); }
		static ChildIteratorType child_end(NodeRef N) { return (*N).end(); }
	};

}	 // end namespace llvm

namespace llvm
{

	template<>
	struct GraphTraits<mlir::rlc::ActionFlowGraph*>
	{
		using NodeRef = mlir::rlc::ActionFlowGraph*;
		using ChildIteratorType =
				mlir::rlc::ActionFlowGraph::CalleeCacheType::Iterator;

		static NodeRef getEntryNode(mlir::rlc::ActionFlowGraph* Forest)
		{
			return Forest;
		}
		static ChildIteratorType child_begin(NodeRef actionFunction)
		{
			return actionFunction->callee_begin();
		}
		static ChildIteratorType child_end(NodeRef actionFunction)
		{
			return actionFunction->callee_end();
		}
	};

}	 // end namespace llvm

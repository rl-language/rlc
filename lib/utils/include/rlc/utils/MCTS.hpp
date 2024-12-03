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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

AnyGameAction action;
auto actions = enumerate(action);
auto actions_count = actions.size();

// Interface for the state of the game
class GameState
{
	private:
	mutable Game state;
	mutable int64_t owner_id;
	mutable int64_t lastActionTaken = 0;

	public:
	using Action = int64_t;
	GameState()
	{
		auto new_state = play();
		state = new_state;
		owner_id = get_current_player(state);
	}

	GameState(Game& initial)
	{
		state = initial;
		owner_id = get_current_player(state);
	}

	Game& getPayload() { return state; }

	int64_t getAction() const { return lastActionTaken; }

	std::vector<int64_t> getLegalActions() const
	{
		std::vector<int64_t> to_return;
		for (int64_t i = 0; i < actions_count; i++)
		{
			if (can_apply_impl(*actions.get(i), state))
			{
				to_return.push_back(i);
			}
		}

		return to_return;
	}
	int64_t getCurrentPlayer() const { return owner_id; }
	void applyAction(int64_t action)
	{
		lastActionTaken = action;
		assert(can_apply_impl(*actions.get(action), state));
		apply(*actions.get(action), state);
		owner_id = get_current_player(state);
	}
	bool isTerminal() const { return state.is_done(); }
	double getReward() const
	{
		int64_t val = 0;
		return score(state, val);
	}

	void dump() const { pretty_print(state); }
};

// MCTS Node
template<typename State>
class MCTSNode
{
	public:
	MCTSNode(const State& state, MCTSNode* parent = nullptr)
			: state(state), parent(parent), visits(0), reward(0.0)
	{
	}

	MCTSNode* select();
	void expand();
	double simulate(uint64_t num_threads);
	void simulate_woker(double* result);
	void backpropagate(double reward);

	State state;
	MCTSNode* parent;
	std::vector<std::unique_ptr<MCTSNode>> children;
	int visits;
	double reward;
	std::vector<typename State::Action> untriedActions;

	void dumpDot()
	{
		std::cout << "\"" << this << "\"" << "[label=\"" << state.getAction()
							<< " visits=" << visits << ", average_value=" << (reward / visits)
							<< ", player " << state.getCurrentPlayer() << "\"];\n";
		for (auto& child : children)
		{
			std::cout << "\"" << this << "\"" << " -> " << "\"" << child.get() << "\""
								<< "\n";
			child->dumpDot();
		}
	}
};

template<typename State>
MCTSNode<State>* MCTSNode<State>::select()
{
	MCTSNode* node = this;

	while (!node->children.empty() and node->untriedActions.empty())
	{
		node = (*std::max_element(
								node->children.begin(),
								node->children.end(),
								[&](const std::unique_ptr<MCTSNode>& a,
										const std::unique_ptr<MCTSNode>& b) {
									return ((a->reward) / double(a->visits)) +
														 std::sqrt(
																 2 * std::log(a->parent->visits) / a->visits) <
												 ((b->reward) / double(b->visits)) +
														 std::sqrt(
																 2 * std::log(b->parent->visits) / b->visits);
								}))
							 .get();
	}
	return node;
}

template<typename State>
void MCTSNode<State>::expand()
{
	if (untriedActions.empty())
	{
		untriedActions = state.getLegalActions();
	}
	assert(not untriedActions.empty() or state.isTerminal());
	if (!untriedActions.empty())
	{
		typename State::Action action = untriedActions.back();
		untriedActions.pop_back();
		State newState = state;
		newState.applyAction(action);
		children.push_back(std::make_unique<MCTSNode>(newState, this));
	}
}

template<typename State>
void MCTSNode<State>::simulate_woker(double* result)
{
	std::random_device r;
	State currentState = state;
	std::default_random_engine generator(r());
	size_t count = 0;
	while (!currentState.isTerminal())
	{
		count++;
		auto legalActions = currentState.getLegalActions();
		std::uniform_int_distribution<int> distribution(0, legalActions.size() - 1);
		currentState.applyAction(legalActions[distribution(generator)]);
		if (count == 100)
		{
			*result = 0;
			return;
		}
	}
	*result = currentState.getReward();
}

template<typename State>
double MCTSNode<State>::simulate(uint64_t num_threads)
{
	if (state.isTerminal())
		return state.getReward();
	double to_return;
	simulate_woker(&to_return);
	return to_return;

	double results[num_threads];
	std::thread threads[num_threads];
	for (uint64_t i = 0; i != num_threads; i++)
	{
		threads[i] =
				std::thread([&results, i, this]() { simulate_woker(&results[i]); });
	}

	for (uint64_t i = 0; i != num_threads; i++)
	{
		threads[i].join();
	}

	return std::accumulate(results, results + num_threads, 0.0) / num_threads;
}

template<typename State>
void MCTSNode<State>::backpropagate(double reward)
{
	MCTSNode* node = this;
	while (node)
	{
		node->visits++;
		int choiseMaker =
				node->parent == nullptr ? 0 : node->parent->state.getCurrentPlayer();
		if (1 == choiseMaker)
			node->reward -= reward;
		// node->reward += reward;
		else
			// node->reward -= reward;
			node->reward += reward;
		node = node->parent;
		// reward = reward * 0.95;
	}
}

// MCTS Algorithm
template<typename State>
class MCTS
{
	public:
	typename State::Action search(int iterations, uint64_t num_threads);
	void promoteBest();
	void promoteNth(int64_t action);
	void promoteRandom();
	std::unique_ptr<MCTSNode<State>>& getBestNode();
	void setState(const State& initialState);
	const State& getState() const { return (*root).state; }

	private:
	std::unique_ptr<MCTSNode<State>> root;
	void worker(MCTSNode<State>* node, uint64_t num_threads);

	public:
	void dumpDot()
	{
		std::cout << "digraph g {\n";
		root->dumpDot();
		std::cout << "}\n";
	}
};

template<typename State>
void MCTS<State>::worker(MCTSNode<State>* node, uint64_t num_threads)
{
	MCTSNode<State>* selectedNode = node->select();
	if (!selectedNode->state.isTerminal())
	{
		selectedNode->expand();
		selectedNode = selectedNode->children.back().get();
	}
	double reward = selectedNode->simulate(num_threads);
	selectedNode->backpropagate(reward);
}

template<typename State>
void MCTS<State>::setState(const State& initialState)
{
	root = std::make_unique<MCTSNode<State>>(initialState);
}

template<typename State>
std::unique_ptr<MCTSNode<State>>& MCTS<State>::getBestNode()
{
	// if (root->state.getObserver() == root->state.getCurrentPlayer())
	return (*std::max_element(
			root->children.begin(),
			root->children.end(),
			[](const std::unique_ptr<MCTSNode<State>>& a,
				 const std::unique_ptr<MCTSNode<State>>& b) {
				return a->visits < b->visits;
			}));
}

template<typename State>
void MCTS<State>::promoteBest()
{
	// auto best = std::move();
	setState(getBestNode()->state);
	// root = std::move(best);
	//(*root).parent = nullptr;
}

template<typename State>
void MCTS<State>::promoteNth(int64_t action)
{
	auto state = getState();
	state.applyAction(action);
	setState(state);
	return;
}

template<typename State>
void MCTS<State>::promoteRandom()
{
	std::random_device r;
	std::default_random_engine generator(r());
	std::uniform_int_distribution<int> distribution(0, root->children.size() - 1);
	auto best = std::move(root->children[distribution(generator)]);
	root = std::move(best);
	(*root).parent = nullptr;
}

template<typename State>
typename State::Action MCTS<State>::search(int iterations, uint64_t num_threads)
{
	while (root->visits < iterations)
	{
		worker(root.get(), num_threads);
	}

	return getBestNode()->state.getAction();
}

bool playGame(size_t iterations, size_t iterations2)
{
	// Create an initial game state
	GameState initialState;

	// MCTS<GameState> mcts;
	//  int64_t bestAction =
	//  mcts.search(initialState, 100, 4);	// 1000 iterations, 4 threads
	//  mcts.dumpDot();

	// return 0;
	// initialState.dump();
	MCTS<GameState> mcts;
	mcts.setState(initialState);
	int i = 0;
	while (!mcts.getState().isTerminal())
	{
		// Perform MCTS search
		// threads if (i++ == 6)
		//{
		// mcts.dumpDot();
		// return 0;
		//}
		int64_t bestAction;
		if (mcts.getState().getCurrentPlayer() == 1)
		{
			mcts.search(iterations2, 16);
			// std::cout << "random\n";
			// mcts.promoteRandom();
			mcts.promoteBest();
		}
		else
		{
			mcts.search(iterations, 16);
			mcts.promoteBest();
			//
			// auto acts = mcts.getState().getLegalActions();
			// for (int64_t i = 0; i != acts.size(); i++)
			//{
			// printf("%ld: ", i);
			// print(*actions.get(acts[i]));
			//}
			// std::cout.flush();
			// scanf("%ld", &bestAction);
			// bestAction = acts[bestAction];
			// mcts.promoteNth(bestAction);
		}

		i++;
		bestAction = mcts.getState().getAction();
		//  system("clear");
		// print(*actions.get(bestAction));
		initialState.applyAction(bestAction);
		// initialState.dump();
	}
	printf("turn count %d\n", i);
	return mcts.getState().getReward() == 1.0;
}

// int main()
//{
// for (auto y : { 100000 })
// for (auto i : { 100000 })
//{
// std::thread threads[30];
// for (int t = 0; t != 30; t++)
// threads[t] = std::thread([=]() {
// for (int game = 0; game != 3; game++)
//{
// bool p1Win = playGame(i, y);
// std::cout << i << " " << y << " g: " << game + (t * 3)
//<< ", did p1 win? " << p1Win << "\n";
// std::cout.flush();
//}
//});
// for (int t = 0; t != 30; t++)
// threads[t].join();
//}
// return 0;
//}

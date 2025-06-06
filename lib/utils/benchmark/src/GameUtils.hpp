#pragma once
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

#include "benchmark/benchmark.h"

constexpr std::size_t minRange = 1 << 2;
constexpr std::size_t maxRange = 1 << 10;

template<typename Actions>
static void run_game(Actions& actions, std::vector<int64_t>& actionsIndex)
{
	auto game = play();
	std::vector<int64_t> out;
	for (int64_t i = 0; i < (int64_t) actionsIndex.size(); i++)
	{
		apply(*actions.get(actionsIndex[i]), game);
		out.push_back(actionsIndex[i]);
	}
}

template<typename T>
static void genGame(std::vector<int64_t>& out, std::mt19937& gen)
{
	auto state = play();
	T action;
	auto actions = enumerate(action);
	while (!state.is_done())
	{
		std::vector<int64_t> validActions;
		for (int64_t i = 0; i < actions.size(); i++)
		{
			if (can_apply(*actions.get(i), state))
				validActions.push_back(i);
		}

		std::uniform_int_distribution<> distrib(0, validActions.size() - 1);
		out.push_back(validActions[distrib(gen)]);
		apply(*actions.get(out.back()), state);
	}
}

template<typename T>
static void runBench(benchmark::State& state)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::vector<std::vector<int64_t>> playOuts;
	T action;
	auto actions = enumerate(action);

	for (int i = 0; i < state.range(); i++)
	{
		playOuts.emplace_back();
		genGame<T>(playOuts.back(), gen);
	}

	for (auto _ : state)
	{
		for (int i = 0; i < state.range(); i++)
			run_game(actions, playOuts[i]);
	}
	state.SetComplexityN(state.range(0));
}

BENCHMARK(runBench<AnyGameAction>)
		->Name(name)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK_MAIN();

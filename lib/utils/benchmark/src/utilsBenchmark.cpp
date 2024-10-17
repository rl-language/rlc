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

#include <utility>

#include "benchmark/benchmark.h"

constexpr size_t minRange = 1 << 2;
constexpr size_t maxRange = 1 << 10;

static void forCycleBenchmark(benchmark::State& state)
{
	int a = 0;
	for (auto _ : state)
	{
		for (int i = 0; i < state.range(); i++)
			benchmark::DoNotOptimize(a += i);
	}
	state.SetComplexityN(state.range(0));
}

BENCHMARK(forCycleBenchmark)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK_MAIN();
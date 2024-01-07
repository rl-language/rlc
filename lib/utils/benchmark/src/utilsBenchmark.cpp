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


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

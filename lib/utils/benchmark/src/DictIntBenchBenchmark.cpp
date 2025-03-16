/*
Copyright 2024 Samuele Pasini

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

#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <random>
#include <unordered_set>
#include <vector>

#include "DictIntBench.h"

static std::random_device rd;
static std::mt19937 gen(rd());

constexpr std::size_t minRange = 1 << 2;
constexpr std::size_t maxRange = 1 << 10;

// Helper function to generate random unique integers
static std::vector<int64_t> GenerateRandomIntegers(int n)
{
	std::uniform_int_distribution<int64_t> distrib(-n, n);
	std::unordered_set<int64_t> unique_numbers;

	int attempts = 0;
	const int maxAttempts = n * 10;

	while (unique_numbers.size() < static_cast<size_t>(n) &&
				 attempts < maxAttempts)
	{
		unique_numbers.insert(distrib(gen));
		attempts++;
	}

	if (unique_numbers.size() < static_cast<size_t>(n))
	{
		for (int i = 0; unique_numbers.size() < static_cast<size_t>(n); ++i)
		{
			unique_numbers.insert(n * 2 + i);
		}
	}

	return std::vector<int64_t>(unique_numbers.begin(), unique_numbers.end());
}

// Sequential insert benchmark
template<typename Map>
static void Insert_Sequential(benchmark::State& state)
{
	Map dict;

	for (auto _ : state)
	{
		for (int64_t i = 0; i < state.range(); i++)
		{
			auto key = dict.to_key(i);
			benchmark::DoNotOptimize(dict.insert(key, i));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Random insert benchmark
template<typename Map>
static void Insert_Random(benchmark::State& state)
{
	std::vector<int64_t> numbers = GenerateRandomIntegers(state.range(0));
	Map dict;

	for (auto _ : state)
	{
		for (int64_t i = 0; i < state.range(0); i++)
		{
			int64_t intkey = numbers[i];
			int64_t value = i;
			auto key = dict.to_key(intkey);
			benchmark::DoNotOptimize(dict.insert(key, value));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Sequential get benchmark
template<typename Map>
static void Get_Sequential(benchmark::State& state)
{
	const auto n = state.range(0);

	// Setup dictionary once outside the benchmark loop
	Map dict;
	for (int64_t i = 0; i < n; i++)
	{
		auto key = dict.to_key(i);
		dict.insert(key, i);
	}

	for (auto _ : state)
	{
		for (int64_t i = 0; i < n; i++)
		{
			auto key = dict.to_key(i);
			benchmark::DoNotOptimize(dict.get(key));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Random get benchmark
template<typename Map>
static void Get_Random(benchmark::State& state)
{
	const auto n = state.range(0);

	// Setup dictionary and generate random numbers once outside the benchmark
	// loop
	std::vector<int64_t> numbers = GenerateRandomIntegers(n);
	Map dict;
	for (int64_t i = 0; i < n; i++)
	{
		auto key = dict.to_key(numbers[i]);
		dict.insert(key, i);
	}

	for (auto _ : state)
	{
		for (size_t i = 0; i < numbers.size(); i++)
		{
			int64_t intkey = numbers[i];
			auto key = dict.to_key(intkey);
			benchmark::DoNotOptimize(dict.get(key));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Sequential contains benchmark
template<typename Map>
static void Contains_Sequential(benchmark::State& state)
{
	const auto n = state.range(0);

	// Setup dictionary once outside the benchmark loop
	Map dict;
	for (int64_t i = 0; i < n; i++)
	{
		auto key = dict.to_key(i);
		dict.insert(key, i);
	}

	for (auto _ : state)
	{
		for (int64_t i = 0; i < n; i++)
		{
			auto key = dict.to_key(i);
			benchmark::DoNotOptimize(dict.contains(key));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Random contains benchmark
template<typename Map>
static void Contains_Random(benchmark::State& state)
{
	const auto n = state.range(0);

	// Setup dictionary and generate random numbers once outside the benchmark
	// loop
	std::vector<int64_t> numbers = GenerateRandomIntegers(n);
	Map dict;
	for (int64_t i = 0; i < n; i++)
	{
		auto key = dict.to_key(numbers[i]);
		dict.insert(key, i);
	}

	for (auto _ : state)
	{
		for (size_t i = 0; i < numbers.size(); i++)
		{
			int64_t intkey = numbers[i];
			auto key = dict.to_key(intkey);
			benchmark::DoNotOptimize(dict.contains(key));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Sequential remove benchmark
template<typename Map>
static void Remove_Sequential(benchmark::State& state)
{
	const auto n = state.range(0);
	Map dict;
	for (int64_t i = 0; i < n; i++)
	{
		auto key = dict.to_key(i);
		dict.insert(key, i);
	}

	for (auto _ : state)
	{
		for (int64_t i = 0; i < n; i++)
		{
			auto key = dict.to_key(i);
			benchmark::DoNotOptimize(dict.remove(key));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Random remove benchmark
template<typename Map>
static void Remove_Random(benchmark::State& state)
{
	const auto n = state.range(0);

	std::vector<int64_t> numbers = GenerateRandomIntegers(n);
	Map dict;
	for (int64_t i = 0; i < n; i++)
	{
		auto key = dict.to_key(numbers[i]);
		dict.insert(key, i);
	}

	for (auto _ : state)
	{
		for (size_t i = 0; i < numbers.size(); i++)
		{
			int64_t intkey = numbers[i];
			auto key = dict.to_key(intkey);
			benchmark::DoNotOptimize(dict.remove(key));
		}
	}
	state.SetComplexityN(state.range(0));
}

// Register benchmarks
BENCHMARK(Insert_Sequential<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Insert_Random<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Get_Sequential<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Get_Random<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Contains_Sequential<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Contains_Random<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Remove_Sequential<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Remove_Random<DictIntInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Insert_Sequential<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Insert_Random<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Get_Sequential<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Get_Random<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Contains_Sequential<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Contains_Random<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Remove_Sequential<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK(Remove_Random<DictLargeKeyInt>)
		->RangeMultiplier(2)
		->Range(minRange, maxRange)
		->Complexity();

BENCHMARK_MAIN();

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

// #include <utility>
// #include <iostream>
// #include <string>
// #include <unordered_map>
// using namespace std;

// #include "benchmark/benchmark.h"

// constexpr size_t minRange = 1 << 2;
// constexpr size_t maxRange = 1 << 10;

// template<typename T> static void BM_SubscriptOperatorInsert(benchmark::State& state)
// {
//     T dictionary;

// 	for (auto _ : state)
// 	{
// 		for (int i = 0; i < state.range(); i++)
// 			benchmark::DoNotOptimize(dictionary[i] = i);
// 	}
// 	state.SetComplexityN(state.range(0));
// }

// template<typename T> static void BM_Insert(benchmark::State& state)
// {
//     T dictionary;

// 	for (auto _ : state)
// 	{
// 		for (int i = 0; i < state.range(); i++)
// 			benchmark::DoNotOptimize(dictionary.insert(make_pair(i,i)));
// 	}
// 	state.SetComplexityN(state.range(0));}


// template<typename T> static void BM_SubscriptorOperatorFind(benchmark::State& state)
// {
//     T dictionary;
// 	for (int i = 0; i < state.range(); i++)
// 			dictionary.insert(make_pair(i,i));

// 	for (auto _ : state)
// 	{
// 		for (int i = 0; i < state.range(); i++)
// 			benchmark::DoNotOptimize(dictionary[i]);
// 	}
// 	state.SetComplexityN(state.range(0));
// }

// template<typename T> static void BM_Find(benchmark::State& state)
// {
//     T dictionary;
// 	for (int i = 0; i < state.range(); i++)
// 			dictionary.insert(make_pair(i,i));

// 	for (auto _ : state)
// 	{
// 		for (int i = 0; i < state.range(); i++)
// 			benchmark::DoNotOptimize(dictionary.find(i));
// 	}
// 	state.SetComplexityN(state.range(0));
// }

// template<typename T> static void BM_Erase(benchmark::State& state)
// {
//     T dictionary;
// 	for (int i = 0; i < state.range(); i++)
// 			dictionary.insert(make_pair(i,i));

// 	for (auto _ : state)
// 	{
// 		for (int i = 0; i < state.range(); i++)
// 			benchmark::DoNotOptimize(dictionary.erase(i));
// 	}
// 	state.SetComplexityN(state.range(0));
// }


// BENCHMARK(BM_SubscriptOperatorInsert<unordered_map<int,int>>)
// 	->RangeMultiplier(2)
// 	->Range(minRange, maxRange)
// 	->Complexity();


// BENCHMARK(BM_SubscriptOperatorInsert<map<int,int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_Insert<unordered_map<int, int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_Insert<map<int, int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_SubscriptorOperatorFind<unordered_map<int,int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_SubscriptorOperatorFind<map<int,int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_Find<unordered_map<int,int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_Find<map<int,int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_Erase<unordered_map<int, int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK(BM_Erase<map<int, int>>)
// 		->RangeMultiplier(2)
// 		->Range(minRange, maxRange)
// 		->Complexity();

// BENCHMARK_MAIN();

#include <benchmark/benchmark.h>
#include <unordered_map>
#include <map>
#include <string>
#include <iostream>
#include <utility>

constexpr size_t minRange = 1 << 2;
constexpr size_t maxRange = 1 << 10;

// Template function for benchmarking
template <typename PassedType>
static void BM_SubsciptorOperatorInsert(benchmark::State& state) {
    PassedType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary[i] = i);
    }
    state.SetComplexityN(state.range(0));
}

// Template function for benchmarking
template <typename PassedType>
static void BM_Insert(benchmark::State& state) {
    PassedType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.insert(std::make_pair(i,i)));
    }
    state.SetComplexityN(state.range(0));
}

// Template function for benchmarking
template <typename PassedType>
static void BM_SubscriptorOperatorFind(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[i] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary[i]);
    }
    state.SetComplexityN(state.range(0));
}

// Template function for benchmarking
template <typename PassedType>
static void BM_Find(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[i] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.find(i));
    }
    state.SetComplexityN(state.range(0));
}

// Template function for benchmarking
template <typename PassedType>
static void BM_Erase(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[i] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.erase(i));
    }
    state.SetComplexityN(state.range(0));
}

// Register template with a specific class based on a string input
template <typename PassedType>
void RegisterBenchmarkSuite(const std::string& name) {
    std::string benchmark_name = "BM_SubscriptOperatorInsert_" + name;
    BENCHMARK_TEMPLATE(BM_SubsciptorOperatorInsert, PassedType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();

	benchmark_name = "BM_Insert_" + name;
    BENCHMARK_TEMPLATE(BM_Insert, PassedType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();

	benchmark_name = "BM_SubscriptorOperatorFind_" + name;
    BENCHMARK_TEMPLATE(BM_SubscriptorOperatorFind, PassedType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();
	
	benchmark_name = "BM_Find_" + name;
    BENCHMARK_TEMPLATE(BM_Find, PassedType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();
	
	benchmark_name = "BM_Erase_" + name;
    BENCHMARK_TEMPLATE(BM_Erase, PassedType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();
}

int main(int argc, char** argv) {
    // Default to "all" if no argument is provided
    std::string choice = (argc > 1) ? argv[argc-1] : "all";

    // Register the benchmark based on user input
	if (choice == "all" or choice[0] == '-') {
		RegisterBenchmarkSuite<std::unordered_map<int, int>>("unordered_map");
		RegisterBenchmarkSuite<std::map<int, int>>("map");
	}else if (choice == "unordered_map") {
        RegisterBenchmarkSuite<std::unordered_map<int, int>>("unordered_map");
    } else if (choice == "map") {
        RegisterBenchmarkSuite<std::map<int, int>>("map");
    } else {
        std::cerr << "Invalid choice. Use <executable> [flags] [all|unordered_map|map].\n";
        return 1;
    }

    // Initialize and run benchmarks
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
	::benchmark::Shutdown();  
}
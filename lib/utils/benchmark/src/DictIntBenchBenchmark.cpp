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

#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include <cstdint>
#include "DictIntBench.h"

#include <benchmark/benchmark.h>
#include <random>
#include <unordered_set>
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());

constexpr size_t minRange = 1 << 2;
constexpr size_t maxRange = 1 << 10;

// Helper function to generate random unique integers
std::vector<int64_t> GenerateRandomIntegers(int n) {
    std::uniform_int_distribution<int64_t> distrib(-n, n);
    std::unordered_set<int64_t> unique_numbers;
    
    while (unique_numbers.size() < static_cast<size_t>(n)) {
        unique_numbers.insert(distrib(gen));
    }
    
    return std::vector<int64_t>(unique_numbers.begin(), unique_numbers.end());
}

// Sequential insert benchmark
static void BM_DictIntInt_Insert_Sequential(benchmark::State& state) {
   DictIntInt dict;
   
   for (auto _ : state) {

        
        for (int64_t i = 0; i < state.range(); i++) {
            benchmark::DoNotOptimize(dict.insert(i, i));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random insert benchmark
static void BM_DictIntInt_Insert_Random(benchmark::State& state) {
    std::vector<int64_t> numbers = GenerateRandomIntegers(state.range());
    DictIntInt dict;
    
    for (auto _ : state) {

        
        for (int64_t i = 0; i < state.range(); i++) {
            int64_t key = numbers[i];
            int64_t value = i;
            benchmark::DoNotOptimize(dict.insert(key, value));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Sequential get benchmark
static void BM_DictIntInt_Get_Sequential(benchmark::State& state) {
    DictIntInt dict;
    for (int64_t i = 0; i < state.range(); i++) {
        dict.insert(i, i);
    }
    
    for (auto _ : state) {
        for (int64_t i = 0; i < state.range(); i++) {
            benchmark::DoNotOptimize(dict.get(i));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random get benchmark
static void BM_DictIntInt_Get_Random(benchmark::State& state) {
    std::vector<int64_t> numbers = GenerateRandomIntegers(state.range());
    DictIntInt dict;
    for (int64_t i = 0; i < state.range(); i++) {
        dict.insert(numbers[i], i);
    }
    
    for (auto _ : state) {
        for (const auto& num : numbers) {
            int64_t key = num;
            benchmark::DoNotOptimize(dict.get(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Sequential contains benchmark
static void BM_DictIntInt_Contains_Sequential(benchmark::State& state) {
    DictIntInt dict;
    for (int64_t i = 0; i < state.range(); i++) {
        dict.insert(i, i);
    }
    
    for (auto _ : state) {
        for (int64_t i = 0; i < state.range(); i++) {
            benchmark::DoNotOptimize(dict.contains(i));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random contains benchmark
static void BM_DictIntInt_Contains_Random(benchmark::State& state) {
    std::vector<int64_t> numbers = GenerateRandomIntegers(state.range());
    DictIntInt dict;
    for (int64_t i = 0; i < state.range(); i++) {
        dict.insert(numbers[i], i);
    }
    
    for (auto _ : state) {
        for (const auto& num : numbers) {
            int64_t key = num;
            benchmark::DoNotOptimize(dict.contains(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Sequential remove benchmark
static void BM_DictIntInt_Remove_Sequential(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        DictIntInt dict;
        for (int64_t i = 0; i < state.range(); i++) {
            dict.insert(i, i);
        }
        state.ResumeTiming();
        
        for (int64_t i = 0; i < state.range(); i++) {
            benchmark::DoNotOptimize(dict.remove(i));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random remove benchmark
static void BM_DictIntInt_Remove_Random(benchmark::State& state) {
    std::vector<int64_t> numbers = GenerateRandomIntegers(state.range());
    
    for (auto _ : state) {
        state.PauseTiming();
        DictIntInt dict;
        for (int64_t i = 0; i < state.range(); i++) {
            dict.insert(numbers[i], i);
        }
        state.ResumeTiming();
        
        for (const auto& num : numbers) {
            int64_t key = num;
            benchmark::DoNotOptimize(dict.remove(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Register benchmarks
BENCHMARK(BM_DictIntInt_Insert_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictIntInt_Insert_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictIntInt_Get_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictIntInt_Get_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictIntInt_Contains_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictIntInt_Contains_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictIntInt_Remove_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictIntInt_Remove_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK_MAIN(); 
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
#include <cstddef>
#include <cstdint>
#include "DictLargeKeyBench.h"

#include <benchmark/benchmark.h>
#include <random>
#include <unordered_set>
#include <vector>

static std::random_device rd;
static std::mt19937 gen(rd());

constexpr std::size_t minRange = 1 << 2;
constexpr std::size_t maxRange = 1 << 10;

// Helper function to generate random unique integers
static std::vector<int64_t> GenerateRandomIntegers(int n) {
    std::uniform_int_distribution<int64_t> distrib(-n, n);
    std::unordered_set<int64_t> unique_numbers;
    
    int attempts = 0;
    const int maxAttempts = n * 10;
    
    while (unique_numbers.size() < static_cast<size_t>(n) && attempts < maxAttempts) {
        unique_numbers.insert(distrib(gen));
        attempts++;
    }
    
    if (unique_numbers.size() < static_cast<size_t>(n)) {
        for (int i = 0; unique_numbers.size() < static_cast<size_t>(n); ++i) {
            unique_numbers.insert(n * 2 + i);
        }
    }
    
    return std::vector<int64_t>(unique_numbers.begin(), unique_numbers.end());
}

// Sequential insert benchmark
static void BM_DictLargeKeyInt_Insert_Sequential(benchmark::State& state) {
   DictLargeKeyInt dict;
   
   for (auto _ : state) {
        for (int64_t i = 0; i < state.range(); i++) {
            LargeKey key(i);
            benchmark::DoNotOptimize(dict.insert(key, i));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random insert benchmark
static void BM_DictLargeKeyInt_Insert_Random(benchmark::State& state) {
    std::vector<int64_t> numbers = GenerateRandomIntegers(state.range(0));
    DictLargeKeyInt dict;
    
    for (auto _ : state) {
        for (int64_t i = 0; i < state.range(0); i++) {
            LargeKey key(numbers[i]);
            int64_t value = i;
            benchmark::DoNotOptimize(dict.insert(key, value));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Sequential get benchmark
static void BM_DictLargeKeyInt_Get_Sequential(benchmark::State& state) {
    const auto n = state.range(0);
    
    // Setup dictionary once outside the benchmark loop
    DictLargeKeyInt dict;
    for (int64_t i = 0; i < n; i++) {
        LargeKey key(i);
        dict.insert(key, i);
    }
    
    for (auto _ : state) {
        for (int64_t i = 0; i < n; i++) {
            LargeKey key(i);
            benchmark::DoNotOptimize(dict.get(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random get benchmark
static void BM_DictLargeKeyInt_Get_Random(benchmark::State& state) { 
    const auto n = state.range(0);
    
    // Setup dictionary and generate random numbers once outside the benchmark loop
    std::vector<int64_t> numbers = GenerateRandomIntegers(n);
    DictLargeKeyInt dict;
    for (int64_t i = 0; i < n; i++) {
        LargeKey key(numbers[i]);
        dict.insert(key, i);
    }
    
    for (auto _ : state) {
        for (size_t i = 0; i < numbers.size(); i++) {
            LargeKey key(numbers[i]);
            benchmark::DoNotOptimize(dict.get(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Sequential contains benchmark
static void BM_DictLargeKeyInt_Contains_Sequential(benchmark::State& state) {
    const auto n = state.range(0);
    
    // Setup dictionary once outside the benchmark loop
    DictLargeKeyInt dict;
    for (int64_t i = 0; i < n; i++) {
        LargeKey key(i);
        dict.insert(key, i);
    }
    
    for (auto _ : state) {
        for (int64_t i = 0; i < n; i++) {
            LargeKey key(i);
            benchmark::DoNotOptimize(dict.contains(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random contains benchmark
static void BM_DictLargeKeyInt_Contains_Random(benchmark::State& state) {
    const auto n = state.range(0);
    
    // Setup dictionary and generate random numbers once outside the benchmark loop
    std::vector<int64_t> numbers = GenerateRandomIntegers(n);
    DictLargeKeyInt dict;
    for (int64_t i = 0; i < n; i++) {
        LargeKey key(numbers[i]);
        dict.insert(key, i);
    }
    
    for (auto _ : state) {
        for (size_t i = 0; i < numbers.size(); i++) {
            LargeKey key(numbers[i]);
            benchmark::DoNotOptimize(dict.contains(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Sequential remove benchmark
static void BM_DictLargeKeyInt_Remove_Sequential(benchmark::State& state) {
    const auto n = state.range(0);
    DictLargeKeyInt dict;
    for (int64_t i = 0; i < n; i++) {
        LargeKey key(i);
        dict.insert(key, i);
    }
    
    for (auto _ : state) {
        for (int64_t i = 0; i < n; i++) {
            LargeKey key(i);
            benchmark::DoNotOptimize(dict.remove(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Random remove benchmark
static void BM_DictLargeKeyInt_Remove_Random(benchmark::State& state) {
    const auto n = state.range(0);
    
    std::vector<int64_t> numbers = GenerateRandomIntegers(n);
    DictLargeKeyInt dict;
    for (int64_t i = 0; i < n; i++) {
        LargeKey key(numbers[i]);
        dict.insert(key, i);
    }
    
    for (auto _ : state) {
        for (size_t i = 0; i < numbers.size(); i++) {
            LargeKey key(numbers[i]);
            benchmark::DoNotOptimize(dict.remove(key));
        }
    }
    state.SetComplexityN(state.range(0));
}

// Register benchmarks
BENCHMARK(BM_DictLargeKeyInt_Insert_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictLargeKeyInt_Insert_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictLargeKeyInt_Get_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictLargeKeyInt_Get_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictLargeKeyInt_Contains_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictLargeKeyInt_Contains_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictLargeKeyInt_Remove_Sequential)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK(BM_DictLargeKeyInt_Remove_Random)
    ->RangeMultiplier(2)
    ->Range(minRange, maxRange)
    ->Complexity();

BENCHMARK_MAIN(); 
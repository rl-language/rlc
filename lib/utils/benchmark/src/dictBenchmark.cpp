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

#include <benchmark/benchmark.h>

#include <algorithm> // For std::shuffle
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <utility>
#include <random>

#include "absl/container/flat_hash_map.h"

std::random_device rd;
std::mt19937 gen(rd());

constexpr size_t minRange = 1 << 2;
constexpr size_t maxRange = 1 << 10;

class LargeKey  {
public:
  int content;
  int dont_care[10];
  LargeKey(int key) : content(key) {}
// Define operator ==
bool operator==(const LargeKey& other) const {
    return content == other.content;
}

// Define operator <
bool operator<(const LargeKey& other) const {
    return content < other.content;
}
};

// Specialize std::hash for LargeKey
namespace std {
    template <>
    struct hash<LargeKey> {
        std::size_t operator()(const LargeKey& key) const {
            return std::hash<int>()(key.content); // Hash only the 'content' field
        }
    };
}

std::vector<int> GenerateNRandomIntegers(int n){
    std::uniform_int_distribution<> distrib(-n, n);

    std::unordered_set<int> unique_numbers;

    // Generate unique numbers
    while (unique_numbers.size() < static_cast<size_t>(n)) {
        unique_numbers.insert(distrib(gen));
    }

    std::vector<int> result(unique_numbers.begin(), unique_numbers.end());

    return result;
}


template <typename DictType, typename KeyType>
static void BM_SubsciptOperatorInsert(benchmark::State& state) {
    DictType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			dictionary[KeyType(i)] = i;
    }
    state.SetComplexityN(state.range(0));
}

template <typename DictType, typename KeyType>
static void BM_SubsciptOperatorInsertRandom(benchmark::State& state) {
    DictType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			dictionary[KeyType(unique_numbers[i])] = i;
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_Insert(benchmark::State& state) {
    PassedType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.insert(std::make_pair(KeyType(i),i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_InsertRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.insert(std::make_pair(KeyType(unique_numbers[i]),i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_SubscriptOperatorFind(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[KeyType(i)] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary[KeyType(i)]);
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_SubscriptOperatorFindRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[KeyType(unique_numbers[i])] = i;
    }

    std::shuffle(unique_numbers.begin(), unique_numbers.end(), gen);

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary[KeyType(unique_numbers[i])]);
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_Find(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[KeyType(i)] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.find(KeyType(i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_FindRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[KeyType(unique_numbers[i])] = i;
    }

    std::shuffle(unique_numbers.begin(), unique_numbers.end(), gen);

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.find(KeyType(unique_numbers[i])));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_Erase(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[KeyType(i)] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.erase(KeyType(i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_EraseRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[KeyType(unique_numbers[i])] = i;
    }

    std::shuffle(unique_numbers.begin(), unique_numbers.end(), gen);

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.erase(KeyType(unique_numbers[i])));
    }
    state.SetComplexityN(state.range(0));
}

// Macros to register benchmarks
#define REGISTER_BENCHMARK(BenchmarkFunction, DictType, KeyType, NameSuffix) \
    BENCHMARK_TEMPLATE(BenchmarkFunction, DictType, KeyType)                \
        ->Name(#BenchmarkFunction "_" NameSuffix)                           \
        ->RangeMultiplier(2)                                                \
        ->Range(minRange, maxRange)                                         \
        ->Complexity();

#define REGISTER_BENCHMARK_SUITE(DictType, KeyType, NameSuffix)        \
    REGISTER_BENCHMARK(BM_Insert, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_InsertRandom, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_SubsciptOperatorInsert, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_SubsciptOperatorInsertRandom, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_SubscriptOperatorFind, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_SubscriptOperatorFindRandom, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_Find, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_FindRandom, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_Erase, DictType, KeyType, NameSuffix) \
    REGISTER_BENCHMARK(BM_EraseRandom, DictType, KeyType, NameSuffix) \

using UnorderedMapIntInt = std::unordered_map<int, int>;
using UnorderedMapLargeKeyInt = std::unordered_map<LargeKey, int>;
using MapIntInt = std::map<int, int>;
using MapLargeKeyInt = std::map<LargeKey, int>;
using FlatHashMapIntInt = absl::flat_hash_map<int, int>;
using FlatHashMapLargeKeyInt = absl::flat_hash_map<LargeKey, int>;

// Register benchmark suites
REGISTER_BENCHMARK_SUITE(UnorderedMapIntInt, int, "UnorderedMap_Small")
REGISTER_BENCHMARK_SUITE(UnorderedMapLargeKeyInt, LargeKey, "UnorderedMap_Large")
REGISTER_BENCHMARK_SUITE(MapIntInt, int, "Map_Small")
REGISTER_BENCHMARK_SUITE(MapLargeKeyInt, LargeKey, "Map_Large")
REGISTER_BENCHMARK_SUITE(FlatHashMapIntInt, int, "FlatHashMap_Small")
REGISTER_BENCHMARK_SUITE(FlatHashMapLargeKeyInt, LargeKey, "FlatHashMap_Large")

BENCHMARK_MAIN();

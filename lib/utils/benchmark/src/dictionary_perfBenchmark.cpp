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
#include <random>
#include <utility>
#include <cstdint>
#include <unordered_map>
#include <map>

#include <benchmark/benchmark.h>

#include <algorithm> // For std::shuffle
#include <unordered_set>
#include <vector>
#include <string>
#include <iostream>

#include "RLDictWrapper.hpp"

std::random_device rd;
std::mt19937 gen(rd());

constexpr size_t minRange = 1 << 2;
constexpr size_t maxRange = 1 << 10;

// C++ LargeKey for std containers
class CPPLargeKey {
public:
    int content;
    int dont_care[10];
    CPPLargeKey(int key) : content(key) {}
    // Define operator ==
    bool operator==(const CPPLargeKey& other) const {
        return content == other.content;
    }
    // Define operator <
    bool operator<(const CPPLargeKey& other) const {
        return content < other.content;
    }
};

// Specialize std::hash for CPPLargeKey
namespace std {
    template <>
    struct hash<CPPLargeKey> {
        std::size_t operator()(const CPPLargeKey& key) const {
            return std::hash<int>()(key.content);
        }
    };
}

// Helper function to create a LargeKey from int for RL
inline void init_large_key(LargeKey* key, int value) {
    rl_m_init__LargeKey(key);
    key->content.content = value;
}

// Helper function to convert int to the appropriate key type
template<typename T>
T make_key(int value) {
    if constexpr (std::is_same_v<T, int>) {
        return value;
    } else if constexpr (std::is_same_v<T, CPPLargeKey>) {
        return CPPLargeKey(value);
    } else {
        LargeKey key;
        init_large_key(&key, value);
        return key;
    }
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
 			dictionary[make_key<KeyType>(i)] = i;
    }
    state.SetComplexityN(state.range(0));
}

template <typename DictType, typename KeyType>
static void BM_SubsciptOperatorInsertRandom(benchmark::State& state) {
    DictType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			dictionary[make_key<KeyType>(unique_numbers[i])] = i;
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_Insert(benchmark::State& state) {
    PassedType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.insert(std::make_pair(make_key<KeyType>(i),i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_InsertRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.insert(std::make_pair(make_key<KeyType>(unique_numbers[i]),i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_SubscriptOperatorFind(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[make_key<KeyType>(i)] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary[make_key<KeyType>(i)]);
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_SubscriptOperatorFindRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[make_key<KeyType>(unique_numbers[i])] = i;
    }

    std::shuffle(unique_numbers.begin(), unique_numbers.end(), gen);

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary[make_key<KeyType>(unique_numbers[i])]);
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_Find(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[make_key<KeyType>(i)] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.find(make_key<KeyType>(i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_FindRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[make_key<KeyType>(unique_numbers[i])] = i;
    }

    std::shuffle(unique_numbers.begin(), unique_numbers.end(), gen);

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.find(make_key<KeyType>(unique_numbers[i])));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_Erase(benchmark::State& state) {
    PassedType dictionary;
    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[make_key<KeyType>(i)] = i;
    }

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.erase(make_key<KeyType>(i)));
    }
    state.SetComplexityN(state.range(0));
}

template <typename PassedType, typename KeyType>
static void BM_EraseRandom(benchmark::State& state) {
    PassedType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    // Setup the dictionary
    for (int i = 0; i < state.range(0); ++i) {
        dictionary[make_key<KeyType>(unique_numbers[i])] = i;
    }

    std::shuffle(unique_numbers.begin(), unique_numbers.end(), gen);

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.erase(make_key<KeyType>(unique_numbers[i])));
    }
    state.SetComplexityN(state.range(0));
}

// Macros to register benchmarks
#define REGISTER_BENCHMARK(BenchmarkFunction, DictType, KeyType, NameSuffix) \
    BENCHMARK_TEMPLATE(BenchmarkFunction, DictType, KeyType)                \
        ->Name(#BenchmarkFunction "_" NameSuffix)                           \
        ->RangeMultiplier(2)                                                \
        ->Range(minRange, maxRange)                                         \
        ->Complexity()

#define REGISTER_BENCHMARK_SUITE(DictType, KeyType, NameSuffix)             \
    REGISTER_BENCHMARK(BM_SubsciptOperatorInsert, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_SubsciptOperatorInsertRandom, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_Insert, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_InsertRandom, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_SubscriptOperatorFind, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_SubscriptOperatorFindRandom, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_Find, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_FindRandom, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_Erase, DictType, KeyType, NameSuffix); \
    REGISTER_BENCHMARK(BM_EraseRandom, DictType, KeyType, NameSuffix)

using UnorderedMapIntInt = std::unordered_map<int, int>;
using UnorderedMapLargeKeyInt = std::unordered_map<CPPLargeKey, int>;
using MapIntInt = std::map<int, int>;
using MapLargeKeyInt = std::map<CPPLargeKey, int>;

// Register benchmark suites
REGISTER_BENCHMARK_SUITE(UnorderedMapIntInt, int, "UnorderedMap_Small");
REGISTER_BENCHMARK_SUITE(UnorderedMapLargeKeyInt, CPPLargeKey, "UnorderedMap_Large");
REGISTER_BENCHMARK_SUITE(MapIntInt, int, "Map_Small");
REGISTER_BENCHMARK_SUITE(MapLargeKeyInt, CPPLargeKey, "Map_Large");

// Register RLDict benchmarks
REGISTER_BENCHMARK_SUITE(RLDictWrapper<int, int>, int, "RLDict_Small");
REGISTER_BENCHMARK_SUITE(RLDictWrapper<LargeKey, int>, LargeKey, "RLDict_Large");

BENCHMARK_MAIN();

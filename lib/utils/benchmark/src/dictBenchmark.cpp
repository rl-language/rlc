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

std::random_device rd;
std::mt19937 gen(rd());

constexpr size_t minRange = 1 << 2;
constexpr size_t maxRange = 1 << 10;

class LargeKey  {
public:
  int content;
  int dont_care[10];
  LargeKey(int key) : content(key) {}
// Define equality operator
bool operator==(const LargeKey& other) const {
    return content == other.content;
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
static void BM_SubsciptorOperatorInsert(benchmark::State& state) {
    DictType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			dictionary[KeyType(i)] = i;
    }
    state.SetComplexityN(state.range(0));
}

template <typename DictType, typename KeyType>
static void BM_SubsciptorOperatorInsertRandom(benchmark::State& state) {
    DictType dictionary;
    std::vector<int> unique_numbers = GenerateNRandomIntegers(state.range());

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			dictionary[KeyType(unique_numbers[i])] = i;
    }
    state.SetComplexityN(state.range(0));
}

// Template function for benchmarking
template <typename PassedType, typename KeyType>
static void BM_Insert(benchmark::State& state) {
    PassedType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			benchmark::DoNotOptimize(dictionary.insert(std::make_pair(KeyType(i),i)));
    }
    state.SetComplexityN(state.range(0));
}

// Template function for benchmarking
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

// Template function for benchmarking
template <typename PassedType, typename KeyType>
static void BM_SubscriptorOperatorFind(benchmark::State& state) {
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

// Template function for benchmarking
template <typename PassedType, typename KeyType>
static void BM_SubscriptorOperatorFindRandom(benchmark::State& state) {
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

// Template function for benchmarking
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

// Template function for benchmarking
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

// Template function for benchmarking
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

// Template function for benchmarking
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

template <typename Func>
void RegisterBenchmark(Func func, const std::string& benchmark_name) {
    benchmark::RegisterBenchmark(benchmark_name.c_str(), func)
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();
}




// Register template with a specific class based on a string input
template <typename DictType, typename KeyType>
void RegisterBenchmarkSuite(const std::string& name) {
    RegisterBenchmark(BM_SubsciptorOperatorInsert<DictType, KeyType>,"BM_SubscriptOperatorInsert_" + name);
    RegisterBenchmark(BM_SubsciptorOperatorInsertRandom<DictType, KeyType>, "BM_SubsciptorOperatorInsert_Random_" + name);
    RegisterBenchmark(BM_Insert<DictType, KeyType>, "BM_Insert_" + name);
    RegisterBenchmark(BM_InsertRandom<DictType, KeyType>, "BM_Insert_Random_" + name);
    RegisterBenchmark(BM_SubscriptorOperatorFind<DictType, KeyType>, "BM_SubscriptorOperatorFind_" + name);
    RegisterBenchmark(BM_SubscriptorOperatorFindRandom<DictType, KeyType>, "BM_SubscriptorOperatorFind_Random_" + name);
    RegisterBenchmark(BM_Find<DictType, KeyType>, "BM_Find_" + name);
    RegisterBenchmark(BM_FindRandom<DictType, KeyType>, "BM_Find_Random_" + name);
    RegisterBenchmark(BM_Erase<DictType, KeyType>, "BM_Erase_" + name);
    RegisterBenchmark(BM_EraseRandom<DictType, KeyType>, "BM_Erase_Random_" + name);
    
}

int main(int argc, char** argv) {
    std::string typeChoice = "unordered_map";
    std::string keyChoice = "small";

    if (argc > 1){
        for(int i = 1; i < argc - 1; i++){
            std::string flag = argv[i];
            if(flag == "--type"){
                typeChoice = argv[i+1];
                i++;
            } else if(flag == "--key"){
                keyChoice = argv[i+1];
                i++;
            }
        }
    }


    // Register the benchmark based on user input
    if (typeChoice == "unordered_map") {
        if(keyChoice == "large"){
            RegisterBenchmarkSuite<std::unordered_map<LargeKey, int>, LargeKey>("UnorderedMap_Large");
        } else if (keyChoice == "small"){
            RegisterBenchmarkSuite<std::unordered_map<int, int>, int>("UnorderedMap_Small");
        } else {
            std::cerr << "Invalid key choice. Use --key (small|large).\n";
            return 1;
        }
    } else if (typeChoice == "map") {
        if(keyChoice == "large"){
            RegisterBenchmarkSuite<std::unordered_map<LargeKey, int>, LargeKey>("Map_Large");
        } else if (keyChoice == "small"){
            RegisterBenchmarkSuite<std::unordered_map<int, int>, int>("Map_Small");
        } else {
            std::cerr << "Invalid key choice. Use --key (small|large).\n";
            return 1;
        }
    } else {
        std::cerr << "Invalid type choice. Use --type (unordered_map|map).\n";
        return 1;
    }

    // Initialize and run benchmarks
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
	::benchmark::Shutdown();  
}

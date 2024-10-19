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
#include <unordered_map>
#include <map>
#include <string>
#include <iostream>
#include <utility>

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

// Template function for benchmarking
template <typename DictType, typename KeyType>
static void BM_SubsciptorOperatorInsert(benchmark::State& state) {
    DictType dictionary;

    for (auto _ : state) {
		for (int i = 0; i < state.range(); i++)
 			dictionary[KeyType(i)] = i;
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


// Register template with a specific class based on a string input
template <typename PassedType, typename KeyType>
void RegisterBenchmarkSuite(const std::string& name) {
    std::string benchmark_name = "BM_SubscriptOperatorInsert_" + name;
    BENCHMARK_TEMPLATE(BM_SubsciptorOperatorInsert, PassedType, KeyType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();

	benchmark_name = "BM_Insert_" + name;
    BENCHMARK_TEMPLATE(BM_Insert, PassedType, KeyType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();

	benchmark_name = "BM_SubscriptorOperatorFind_" + name;
    BENCHMARK_TEMPLATE(BM_SubscriptorOperatorFind, PassedType, KeyType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();
	
	benchmark_name = "BM_Find_" + name;
    BENCHMARK_TEMPLATE(BM_Find, PassedType, KeyType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();
	
	benchmark_name = "BM_Erase_" + name;
    BENCHMARK_TEMPLATE(BM_Erase, PassedType, KeyType)
        ->Name(benchmark_name.c_str())
        ->RangeMultiplier(2)
        ->Range(minRange, maxRange)
        ->Complexity();
}

int main(int argc, char** argv) {
    // Default to "all" if no argument is provided
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
            RegisterBenchmarkSuite<std::unordered_map<LargeKey, int>, LargeKey>("unordered_map_large");
        } else if (keyChoice == "small"){
            RegisterBenchmarkSuite<std::unordered_map<int, int>, int>("unordered_map_small");
        } else {
            std::cerr << "Invalid key choice. Use --key (small|large).\n";
            return 1;
        }
    } else if (typeChoice == "map") {
        if(keyChoice == "large"){
            RegisterBenchmarkSuite<std::unordered_map<LargeKey, int>, LargeKey>("map_large");
        } else if (keyChoice == "small"){
            RegisterBenchmarkSuite<std::unordered_map<int, int>, int>("map_small");
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

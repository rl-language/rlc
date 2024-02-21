/*
Copyright 2024 Cem Cebeci

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

#include <cmath>
#include <cstdlib>
#include "stdio.h"
#include <math.h>
#include <algorithm>

// This is implemented by RLC.
extern "C" void rl_fuzzer_fuzz_action_function_();


int byte_offset;
int bit_offset;
int fuzz_input_length;
const char *fuzz_input;

int64_t consume_bits(const char *Data, int num_bits, int &byte_offset, int &bit_offset) {
    int64_t result = 0;
    int remaining_bits = num_bits;

    while (true) {
        int to_consume_from_current_byte = std::min(remaining_bits, 8 - bit_offset);
        int shift_count = (8 - bit_offset - remaining_bits);
        int mask = ((1u << to_consume_from_current_byte) - 1) << shift_count;
        int data = (*(Data + byte_offset) & mask) >> shift_count;
        result = (result << to_consume_from_current_byte) | data;

        if(remaining_bits >= (8 - bit_offset)) {
            byte_offset ++;
            remaining_bits -= (8 - bit_offset);
            bit_offset = 0;
        } else {
            bit_offset = bit_offset + remaining_bits;
            return result;
        }
    }
}

// TODO this is not completely uniform since the number of possible inputs is not a power of two, think about whether or not that's a problem.
extern "C" void rl_fuzzer_get_input__int64_t_r_int64_t(__int64_t *result, const __int64_t *max) {
    printf("Generating input in range [0, %ld)\n", *max);
    int num_bits = ceil(log2(*max));
    *result = consume_bits(fuzz_input, num_bits, byte_offset, bit_offset) % *max;
}

extern "C" void rl_fuzzer_pick_argument__int64_t_int64_t_r_int64_t(__int64_t *result, const __int64_t *min, __int64_t *max) {
    printf("Picking an integer argument in range [%ld, %ld]\n", *min, *max);
    if(*min > *max) {
        // The current implementation of constraint analysis allows this to happen. When it does, it's not a fault
        //  of the program under analysis but a fault of the analysis itself. It shouldn't cause an error. Returning
        //  any arbitrary value is safe here since any value is either valid by pure chance, or will be discarded by the fuzzer.
        // It might be worth tweaking the fuzz target not to call this function in such a case, but I see no justification for that
        //  effort at the moment. 
        printf("Invalid range. Picked 0.\n");
        *result = 0;
        return;
    }

    int num_bits = ceil(log2(*max - *min + 1));
    *result = std::abs(consume_bits(fuzz_input, num_bits, byte_offset, bit_offset)) % (*max - *min + 1) + *min;
    printf("Picked %ld\n", *result);
}

extern "C" void rl_fuzzer_is_input_long_enough__r_bool(__int8_t *result) {
    // printf("fuzz_input_length: %d, byte_offfset: %d, bit_offset: %d\n", fuzz_input_length, byte_offset, bit_offset);
    *result = (fuzz_input_length - byte_offset) > 10; // TODO handle this better.
}

extern "C" void rl_fuzzer_print__int64_t_r_void(const __int64_t *message) {
    // printf("Message: %ld \n", *message);
}

extern "C" void rl_fuzzer_skip_input__r_void() {
    // printf("skipping the current fuzz input!\n");
}


extern "C" int LLVMFuzzerTestOneInput(const char *Data, size_t Size) {
    byte_offset = 0;
    bit_offset = 0;
    fuzz_input = Data;
    fuzz_input_length = Size;
    
    rl_fuzzer_fuzz_action_function_();
    return 0;
}
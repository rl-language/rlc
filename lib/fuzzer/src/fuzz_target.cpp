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

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <math.h>

#include "stdio.h"

struct VectorByte
{
	const char* data;
	int64_t size;
	int64_t capacity;
};

// This is implemented by RLC.
extern "C" void rl_fuzz__VectorTint8_tT(VectorByte* input);

extern "C" int LLVMFuzzerTestOneInput(const char* Data, size_t Size)
{
	// this is a struc with the same layout as rl Vector<Byte>, so we can send it
	// as a parameter to rl functions using it
	VectorByte vector;
	vector.data = Data;
	vector.capacity = static_cast<int64_t>(Size);
	vector.size = static_cast<int64_t>(Size);

	rl_fuzz__VectorTint8_tT(&vector);
	return 0;
}

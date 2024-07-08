#pragma once
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

#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#	define EXPORT __declspec(dllexport)
#else
#	define EXPORT
#endif

typedef struct String String;

// fun init(String self)
EXPORT void rl_m_init__String(String* self);

// fun append(String self, Int to_append)
EXPORT void rl_append_to_string__int64_t_String(int64_t* member, String* out);
// fun append(String self, Byte to_append)
EXPORT void rl_append_to_string__int8_t_String(int8_t* member, String* out);
// fun append(String self, Float to_append)
EXPORT void rl_append_to_string__double_String(double* member, String* out);

// fun print(String self)
EXPORT void rl_print__String(String* s);
// fun print(StringLiteral self)
EXPORT void rl_print_string__strlit(char** s);

// fun parse_string(Int result, String buffer, Int index) -> Bool
EXPORT void rl_parse_string__int64_t_String_int64_t_r_bool(
		bool* return_value, int64_t* result, String* buffer, int64_t* current);

// fun parse_string(Float result, String buffer, Int index) -> Bool
EXPORT void rl_parse_string__double_String_int64_t_r_bool(
		bool* return_value, double* result, String* buffer, int64_t* current);

// fun parse_string(Byte result, String buffer, Int index) -> Bool
EXPORT void rl_parse_string__int8_t_String_int64_t_r_bool(
		bool* return_value, int8_t* result, String* buffer, int64_t* current);

// fun parse_string(Byte result, String buffer, Int index) -> Bool
EXPORT void rl_is_alphanumeric__int8_t_r_bool(
		bool* return_value, int8_t* input_char);

// fun load_file(String file_path, String out) -> Bool
EXPORT void rl_load_file__String_r_String(
		int8_t* result, String* file_name, String* out);

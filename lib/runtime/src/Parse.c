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

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>

#include "rlc/runtime/Runtime.h"

void rl_append_to_string__int64_t_String(int64_t* toConvert, String* out)
{
	char buffer[256];
	sprintf(buffer, "%" PRId64, *toConvert);
	char* addres = buffer;
	rl_m_append__String_strlit(out, &addres);
}

void rl_append_to_string__int8_t_String(int8_t* toConvert, String* out)
{
	char buffer[256];
	sprintf(buffer, "%" PRId8, *toConvert);
	char* addres = buffer;
	rl_m_append__String_strlit(out, &addres);
}

void rl_append_to_string__double_String(double* toConvert, String* out)
{
	char buffer[256];
	sprintf(buffer, "%f", *toConvert);
	char* addres = buffer;
	rl_m_append__String_strlit(out, &addres);
}

void rl_print_string__String(String* s)
{
	int8_t* start = 0;
	int64_t index = 0;
	rl_m_get__String_int64_t_r_int8_tRef(&start, s, &index);
	puts((char*) start);
	fflush(stdout);
}

void rl_print_string_lit__strlit(char** s)
{
	puts(*s);
	fflush(stdout);
}

// fun parse_string(Int result, String buffer, Int index)
void rl_parse_string__int64_t_String_int64_t_r_bool(
		bool* return_value, int64_t* result, String* buffer, int64_t* current)
{
	int8_t* to_read = 0;
	rl_m_get__String_int64_t_r_int8_tRef(&to_read, buffer, current);

	int c;
	int scanned = sscanf((char*) to_read, "%" SCNd64 "%n", result, &c);
	*return_value = scanned == 1;
	*current += (int64_t) (c);
}

// fun parse_string(Float result, String buffer, Int index)
void rl_parse_string__double_String_int64_t(
		int64_t* member, String* buffer, int64_t* current)
{
}

// fun parse_string(Byte result, String buffer, Int index)
void rl_parse_string__int8_t_String_int64_t_r_bool(
		bool* return_value, int8_t* result, String* buffer, int64_t* current)
{
	int8_t* to_read = 0;
	rl_m_get__String_int64_t_r_int8_tRef(&to_read, buffer, current);

	int c;
	int scanned = sscanf((char*) to_read, "%" SCNd8 "%n", result, &c);
	*return_value = scanned == 1;
	*current += (int64_t) (c);
}

// fun parse_string(Float result, String buffer, Int index) -> Bool
void rl_parse_string__double_String_int64_t_r_bool(
		bool* return_value, double* result, String* buffer, int64_t* current)
{
	int8_t* to_read = 0;
	rl_m_get__String_int64_t_r_int8_tRef(&to_read, buffer, current);

	int c;
	int scanned = sscanf((char*) to_read, "%lf%n", result, &c);
	*return_value = scanned == 1;
	*current += (int64_t) (c);
}

void rl_is_alphanumeric__int8_t_r_bool(bool* return_value, int8_t* input_char)
{
	*return_value = isalpha(*input_char) || isdigit(*input_char);
}

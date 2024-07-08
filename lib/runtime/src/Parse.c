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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "rlc/runtime/Runtime.h"

struct String {
  char* str;
  int64_t size;
  int64_t capacity;
};

// fun get(String self, Int index) -> ref Byte 
static void rl_m_get__String_int64_t_r_int8_tRef(
		int8_t** out, String* self, int64_t* index) {
	*out = (int8_t*) &(self->str[*index]);

}

// fun append(String self, StringLiteral l)
static void rl_m_append__String_strlit(String* self, char** to_append) {
	size_t selflen = self->size; // size includes terminator
	size_t to_copy_len = strlen(*to_append); // size does not include terminator

	if (selflen + to_copy_len >= self->capacity) {
	   size_t new_capacity = (selflen + to_copy_len) * 2;
	   self->str = realloc(self->str, new_capacity);  
	   self->capacity = new_capacity; 
	}

	memcpy(self->str + selflen - 1, *to_append, to_copy_len + 1);
	self->size =  selflen + to_copy_len; 
}

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

void rl_load_file__String_String_r_bool(
		int8_t* result, String* file_name, String* out)
{
	int8_t* start = 0;
	int64_t index = 0;
	rl_m_get__String_int64_t_r_int8_tRef(&start, file_name, &index);

	FILE* f = fopen((char*) start, "r");
	if (!f)
	{
		*result = 0;
		return;
	}

	char read[1024];
	char* c = read;
	read[1023] = '\0';
	while (fgets(read, 1024, f))
		rl_m_append__String_strlit(out, &c);

	fclose(f);
	*result = 1;
}

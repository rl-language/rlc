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

#include <stdint.h>

typedef struct String String;
void rl_m_init__String(String* self);
void rl_m_get__String_int64_t_r_int8_tRef(
		int8_t** out, String* self, int64_t* index);
void rl_m_append__String_strlit(String* self, char* to_append);
void rl_append_to_string__int64_t_String(int64_t* member, String* out);
void rl_append_to_string__int8_t_String(int8_t* member, String* out);
void rl_append_to_string__double_String(double* member, String* out);
void rl_print__String(String* s);

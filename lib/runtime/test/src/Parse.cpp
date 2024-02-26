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

#include "gtest/gtest.h"
#include <string>

struct String
{
	std::string content;
};

extern "C"
{
#include "rlc/runtime/Runtime.h"

	void rl_m_append__String_strlit(String* self, char* to_append)
	{
		self->content.append(to_append);
	}

	void rl_m_init__String(String* self) {}

	void rl_m_get__String_int64_t_r_int8_tRef(
			int8_t** out, String* self, int64_t* index)
	{
		*out = ((int8_t*) &self->content[*index]);
	}
}

TEST(parseTest, parseInt)
{
	String result;
	int64_t to_parse = -258;
	rl_append_to_string__int64_t_String(&to_parse, &result);
	EXPECT_EQ(result.content, "-258");
}

TEST(parseTest, parseInt8)
{
	String result;
	int8_t to_parse = -4;
	rl_append_to_string__int8_t_String(&to_parse, &result);
	EXPECT_EQ(result.content, "-4");
}

TEST(parseTest, parseDouble)
{
	String result;
	double to_parse = -42.3;
	rl_append_to_string__double_String(&to_parse, &result);
	EXPECT_EQ(result.content, "-42.300000");
}

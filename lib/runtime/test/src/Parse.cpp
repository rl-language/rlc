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
	char* str;
	int64_t size;
	int64_t capacity;

	String(): str(new char[4]), size(1), capacity(4) { str[0] = '\0'; }
	~String() { delete[] str; }
};

extern "C"
{
#include "rlc/runtime/Runtime.h"
}

TEST(parseTest, printInt64)
{
	String result;
	int64_t to_parse = -258;
	rl_append_to_string__int64_t_String(&to_parse, &result);
	EXPECT_STREQ(result.str, "-258");
}

TEST(parseTest, printInt8)
{
	String result;
	int8_t to_parse = -4;
	rl_append_to_string__int8_t_String(&to_parse, &result);
	EXPECT_STREQ(result.str, "-4");
}

TEST(parseTest, printDouble)
{
	String result;
	double to_parse = -42.3;
	rl_append_to_string__double_String(&to_parse, &result);
	EXPECT_STREQ(result.str, "-42.300000");
}

TEST(parseTest, parseInt64)
{
	String to_parse;
	char buffer[] = "-42 asd";
	char* ref = buffer;
	impl_rl_m_append__String_strlit(&to_parse, &ref);
	int64_t result = 0;
	int64_t index = 0;
	bool result_value = false;
	rl_parse_string__int64_t_String_int64_t_r_bool(
			&result_value, &result, &to_parse, &index);
	EXPECT_EQ(result, -42);
	EXPECT_EQ(result_value, true);
	EXPECT_EQ(index, 3);
}

TEST(parseTest, parseInt8)
{
	String to_parse;
	char buffer[] = "-42 asd";
	char* ref = buffer;
	impl_rl_m_append__String_strlit(&to_parse, &ref);
	int8_t result = 0;
	int64_t index = 0;
	bool result_value = false;
	rl_parse_string__int8_t_String_int64_t_r_bool(
			&result_value, &result, &to_parse, &index);
	EXPECT_EQ(result, -42);
	EXPECT_EQ(result_value, true);
	EXPECT_EQ(index, 3);
}

TEST(parseTest, parseDouble)
{
	String to_parse;
	char buffer[] = "-42.3 asd";
	char* ref = buffer;
	impl_rl_m_append__String_strlit(&to_parse, &ref);
	double result = 0;
	int64_t index = 0;
	bool result_value = false;
	rl_parse_string__double_String_int64_t_r_bool(
			&result_value, &result, &to_parse, &index);
	EXPECT_EQ(result, -42.3);
	EXPECT_EQ(result_value, true);
	EXPECT_EQ(index, 5);
}

#include "gtest/gtest.h"
#include <bits/stdint-intn.h>

#include "rlc/ast/Constant.hpp"

using namespace rlc;

TEST(ConstantTest, createLongConstant)
{
	auto c = ScalarConstant::longC(4);
	EXPECT_TRUE(c.isA<int64_t>());
	EXPECT_EQ(c.get<int64_t>(), 4);
	EXPECT_EQ(c.as<double>(), 4.0);
}

TEST(ConstantTest, createBoolConstant)
{
	auto c = ScalarConstant::boolC(true);
	EXPECT_TRUE(c.isA<bool>());
	EXPECT_FALSE(c.isA<double>());
	EXPECT_EQ(c.get<bool>(), 1);
	EXPECT_EQ(c.as<double>(), 1);
}

TEST(ConstantTest, createDoubleConstant)
{
	auto c = ScalarConstant::doubleC(1.4);
	EXPECT_TRUE(c.isA<double>());
	EXPECT_EQ(c.get<double>(), 1.4);
	EXPECT_EQ(c.as<int64_t>(), 1);
}

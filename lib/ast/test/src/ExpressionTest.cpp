#include "gtest/gtest.h"
#include <bits/stdint-intn.h>

#include "rlc/ast/Constant.hpp"
#include "rlc/ast/Expression.hpp"
#include "rlc/ast/Reference.hpp"

using namespace rlc;

TEST(ExpressionTest, referenceShouldBeConstructable)
{
	auto exp = Expression::reference("name");
	EXPECT_TRUE(exp.isA<Reference>());
	EXPECT_EQ(exp.get<Reference>().getName(), "name");
}

TEST(ExpressionTest, constantShouldBeConstructable)
{
	auto exp = Expression::int64Constant(4);
	EXPECT_TRUE(exp.isA<ScalarConstant>());
	EXPECT_EQ(exp.get<ScalarConstant>().get<int64_t>(), 4);
}

TEST(ExpressionTest, callShoulBeConstructable)
{
	auto exp = Expression::call(Expression::scalarConstant(0.0));
	EXPECT_TRUE(exp.isA<Call>());
	EXPECT_TRUE(exp.get<Call>().getFunctionExpression().isA<ScalarConstant>());
}

TEST(ExpressionTest, nestedExpressionsShouldBeIteratable)
{
	using E = Expression;
	auto exp = E::call(E::scalarConstant(0.0), { E::scalarConstant<int64_t>(1) });
	int elCount = 0;
	for (auto& subExp : exp)
		elCount++;
	EXPECT_EQ(elCount, 2);
}

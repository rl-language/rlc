#include "gtest/gtest.h"
#include <bits/stdint-intn.h>
#include <iterator>

#include "rlc/ast/Constant.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/utils/SourcePosition.hpp"

using namespace llvm;
using namespace std;
using namespace rlc;

TEST(StatementTest, ExpressionStatement)
{
	ExpressionStatement stat(Expression::boolConstant(false));
	EXPECT_TRUE(stat.getExpression().isA<ScalarConstant>());
	EXPECT_EQ(distance(stat.begin(), stat.end()), 1);
}

TEST(StatementTest, Expression)
{
	auto at = Statement::expStatement(Expression::boolConstant(false));
	EXPECT_TRUE(at.isA<ExpressionStatement>());
	EXPECT_EQ(at.subExpCount(), 1);
	EXPECT_EQ(at.subStatmentCount(), 0);
}

TEST(StatementTest, ExpressionIterator)
{
	auto at = Statement::expStatement(Expression::boolConstant(false));
	for (const auto& c : at.expRange())
		EXPECT_TRUE(c.isA<ScalarConstant>());
}

TEST(StatementTest, IfStatement)
{
	auto exp = Expression::boolConstant(false);
	auto ifE = Statement::ifStatment(exp, Statement::expStatement(exp));
	EXPECT_TRUE(ifE.isIfStat());
	EXPECT_TRUE(ifE.getSubStatement(0).isExpStat());
	EXPECT_TRUE(ifE.getSubExp(0).isA<ScalarConstant>());
}

TEST(StatementTest, WhileStatement)
{
	auto exp = Expression::boolConstant(false);
	auto ifE = Statement::whileStatement(exp, Statement::expStatement(exp));
	EXPECT_TRUE(ifE.isWhileStat());
	EXPECT_TRUE(ifE.getSubStatement(0).isExpStat());
	EXPECT_TRUE(ifE.getSubExp(0).isA<ScalarConstant>());
}

TEST(StatementTest, StatementList)
{
	auto exp = Expression::boolConstant(false);
	auto stat = Statement::expStatement(exp);
	auto ifE = Statement::statmentList({ stat, stat });
	EXPECT_TRUE(ifE.isStatmentList());
	EXPECT_TRUE(ifE.getSubStatement(0).isExpStat());
	EXPECT_EQ(ifE.subExpCount(), 0);
}

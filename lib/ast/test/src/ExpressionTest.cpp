#include "gtest/gtest.h"
#include <bits/stdint-intn.h>

#include "rlc/ast/Constant.hpp"
#include "rlc/ast/Entity.hpp"
#include "rlc/ast/Expression.hpp"
#include "rlc/ast/FunctionDeclaration.hpp"
#include "rlc/ast/Reference.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"

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

TEST(ExpressionTest, referenceTypeCheck)
{
	DeclarationStatement d("var", Expression::int64Constant(3));

	SymbolTable table;
	TypeDB db;

	auto exp = Expression::reference("var");
	if (auto e = d.deduceTypes(table, db); e)
		FAIL();
	if (auto e = exp.deduceType(table, db); e)
		FAIL();

	EXPECT_EQ(exp.getType(), db.getLongType());
}

TEST(ExpressionTest, callTypeCheck)
{
	DeclarationStatement d("var", Expression::int64Constant(3));
	FunctionDeclaration fun(
			"duplicate",
			SingleTypeUse::scalarType("Int"),
			{ ArgumentDeclaration("name", SingleTypeUse::scalarType("Int")) });

	SymbolTable table;
	table.insert(fun);
	TypeDB db;

	Entity i("Int", {});
	if (i.createType(table, db))
		FAIL();
	if (i.deduceType(table, db))
		FAIL();

	auto exp = Expression::call(
			Expression::reference("duplicate"), { Expression::reference("var") });
	if (auto e = fun.deduceType(table, db); e)
		FAIL();
	if (auto e = d.deduceTypes(table, db); e)
		FAIL();
	if (auto e = exp.deduceType(table, db); e)
		FAIL();

	EXPECT_EQ(exp.getType(), db.getLongType());
	EXPECT_EQ(fun.getReturnType(), db.getLongType());
}

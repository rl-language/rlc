#include "gtest/gtest.h"
#include <iterator>

#include "rlc/ast/Entity.hpp"
#include "rlc/ast/FunctionDefinition.hpp"
#include "rlc/ast/Statement.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/ast/TypeUse.hpp"

using namespace rlc;

TEST(EntityTest, entityShouldDeduceCorrectBuiltinType)
{
	Entity b("Bool", {});
	Entity e("example", { EntityField(SingleTypeUse::scalarType("Bool"), "f") });
	SymbolTable tb;
	TypeDB db;
	if (b.createType(tb, db))
		FAIL();
	if (b.deduceType(tb, db))
		FAIL();

	if (e.createType(tb, db))
		FAIL();
	if (e.deduceType(tb, db))
		FAIL();
	EXPECT_EQ(db.getUserDefinedType("example"), e.getType());
	EXPECT_EQ(e.getType()->getContainedType(0), db.getBoolType());
}

TEST(EntityTest, rangeShouldHaveOneEntity)
{
	Entity b("bool", {});
	SymbolTable tb;
	TypeDB db;
	if (b.createType(tb, db))
		FAIL();
	if (b.deduceType(tb, db))
		FAIL();

	auto r = tb.range<Entity>("bool");
	auto d = std::distance(r.begin(), r.end());
	EXPECT_EQ(d, 1);
}

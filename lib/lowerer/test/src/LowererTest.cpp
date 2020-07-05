#include "gtest/gtest.h"

#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/lowerer/Lowerer.hpp"

using namespace rlc;

TEST(LowererTest, structLowererTest)
{
	System s("testSystem");

	s.addEntity(EntityDeclaration(
			Entity("testEntity", { EntityField("testEntity", "fieldName") })));

	TypeDB db;
	if (s.typeCheck(SymbolTable(), db))
		FAIL();

	Lowerer lowerer;
	if (lowerer.lowerSystem(s))
		FAIL();

	auto t = lowerer.rlcToLlvmType(db.getUserDefinedType("testEntity"));

	EXPECT_TRUE(t->isPointerTy());
	auto strType = t->getContainedType(0);
	EXPECT_TRUE(strType->isStructTy());
	EXPECT_EQ(strType->getNumContainedTypes(), 1);
	EXPECT_EQ(t, strType->getContainedType(0));
}

#include "gtest/gtest.h"

#include "rlc/ast/Entity.hpp"
#include "rlc/ast/EntityDeclaration.hpp"
#include "rlc/ast/SymbolTable.hpp"
#include "rlc/ast/System.hpp"
#include "rlc/ast/Type.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/lowerer/Lowerer.hpp"

using namespace rlc;

TEST(LowererTest, structLowererTest)
{
	System s("testSystem");

	s.addEntity(EntityDeclaration(Entity(
			"testEntity",
			{ EntityField(SingleTypeUse::scalarType("testEntity"), "fieldName") })));

	TypeDB db;
	if (s.typeCheck(SymbolTable(), db))
		FAIL();

	mlir::MLIRContext ctx;
	Lowerer lowerer(ctx, db);
	mlir::DialectRegistry Registry;
	Registry.insert<mlir::rlc::RLCDialect>();
	lowerer.getContext().appendDialectRegistry(Registry);
	lowerer.getContext().loadAllAvailableDialects();
	if (lowerer.lowerSystem(s))
		FAIL();

	auto t = lowerer.rlcToLlvmType(db.getUserDefinedType("testEntity"));

	EXPECT_TRUE(t);
	auto strType = t.dyn_cast<mlir::rlc::EntityType>();
	EXPECT_TRUE(strType);
	EXPECT_EQ(strType.getBody().size(), 1);
	EXPECT_EQ(t, strType.getBody()[0]);
}

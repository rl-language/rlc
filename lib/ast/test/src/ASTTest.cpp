#include "gtest/gtest.h"

#include "rlc/ast/Type.hpp"

using namespace rlc;

TEST(TypeTest, createLongType)
{
	TypeDB db;

	auto longType = db.getLongType();
	EXPECT_EQ(longType->isBuiltin(), true);
	EXPECT_EQ(longType->getBuiltinType(), BuiltinType::LONG);
}

TEST(TypeTest, twoLongTypeShouldBeEquals)
{
	TypeDB db;

	EXPECT_EQ(db.getLongType(), db.getLongType());
}

TEST(TypeTest, longAndBoolShouldNotBeEqual)
{
	TypeDB db;

	EXPECT_NE(db.getLongType(), db.getDoubleType());
}

TEST(TypeTest, createArrayType)
{
	TypeDB db;
	auto t = db.getLongType();
	auto arraType = db.getArrayType(t, 5);
	EXPECT_EQ(arraType->arrayLenght(), 5);
	EXPECT_EQ(arraType->getContainedType(0), t);
}

TEST(TypeTest, arrayTypeShouldBeIterable)
{
	TypeDB db;
	auto t = db.getLongType();
	auto t2 = db.getArrayType(t, 4);
	for (auto subT : *t2)
		EXPECT_EQ(subT, t);
}

TEST(TypeTest, equalArrayTypesShouldBeEqual)
{
	TypeDB db;
	auto t = db.getLongType();
	EXPECT_EQ(db.getArrayType(t, 4), db.getArrayType(t, 4));
}

TEST(TypeTest, notEqualArrayTypesShouldNotBeEqual)
{
	TypeDB db;
	auto t = db.getLongType();
	EXPECT_NE(db.getArrayType(t, 5), db.getArrayType(t, 4));
}

TEST(TypeTest, functionType)
{
	TypeDB db;
	auto t = db.getLongType();
	auto ft = db.getFunctionType(t, db.getVoidType());
	EXPECT_EQ(ft->getArgCount(), 1);
	EXPECT_EQ(ft->getReturnType(), t);
	EXPECT_EQ(ft->getArgumentType(0), db.getVoidType());
}

TEST(TypeTest, equalFunctionShouldBeEqual)
{
	TypeDB db;
	auto t = db.getLongType();
	auto ft = db.getFunctionType(t, db.getVoidType());
	auto ft2 = db.getFunctionType(t, db.getVoidType());
	EXPECT_EQ(ft, ft2);
}

TEST(TypeTest, notEqualFunctionShouldBeEqual)
{
	TypeDB db;
	auto t = db.getLongType();
	auto ft = db.getFunctionType(t, db.getVoidType());
	auto ft2 = db.getFunctionType(t, db.getBoolType());
	EXPECT_NE(ft, ft2);
}

TEST(TypeTest, userDefinedTypes)
{
	TypeDB db;
	auto t = db.getLongType();
	auto ft = db.createUserDefinedType("name", t);
	EXPECT_EQ(ft->getName(), "name");
	EXPECT_EQ(ft->containedTypesCount(), 1);
	EXPECT_EQ(ft->getContainedType(0), t);
}

TEST(TypeTest, equalUserDefinedTypesShouldBeEqual)
{
	TypeDB db;
	auto t = db.getLongType();
	auto ft = db.createUserDefinedType("name", t);
	auto ft2 = db.createUserDefinedType("name", t);
	EXPECT_EQ(ft2, nullptr);
	EXPECT_EQ(ft, db.getUserDefined("name"));
}

#include "rlc/ast/Constant.hpp"

#include <cstdint>
#include <string>

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Type.hpp"

using namespace rlc;
using namespace llvm;
using namespace std;

void ScalarConstant::dump() const { print(outs()); }

void ScalarConstant::print(raw_ostream& OS) const
{
	visit([&OS](auto c) { OS << to_string(c); });
}

template<typename T>
Type* rlcTypeFromCppType(TypeDB& db);

template<>
Type* rlcTypeFromCppType<int64_t>(TypeDB& db)
{
	return db.getLongType();
}

template<>
Type* rlcTypeFromCppType<double>(TypeDB& db)
{
	return db.getDoubleType();
}

template<>
Type* rlcTypeFromCppType<bool>(TypeDB& db)
{
	return db.getBoolType();
}

Type* ScalarConstant::type(TypeDB& db) const
{
	return visit([&db](auto c) { return rlcTypeFromCppType<decltype(c)>(db); });
}

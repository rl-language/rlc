rlcAddLibrary(ast SHARED
	src/Type.cpp
	src/Constant.cpp
	src/Call.cpp
	src/Expression.cpp
	src/ArrayAccess.cpp
	src/ArgumentDeclaration.cpp
	src/ActionDeclaration.cpp
	src/ActionDefinition.cpp
	src/Entity.cpp
	src/Statement.cpp
	src/FunctionDefinition.cpp
	src/FunctionDeclaration.cpp
	src/TypeUse.cpp
	src/System.cpp
	src/EntityDeclaration.cpp
	src/SymbolTable.cpp
	src/BuiltinFunctions.cpp
	src/BuiltinEntities.cpp
	src/MemberAccess.cpp)

target_link_libraries(ast PUBLIC rlc::utils)


rlcAddLibrary(ast SHARED 
	src/Type.cpp
	src/Constant.cpp
	src/Call.cpp
	src/Expression.cpp
	src/Entity.cpp
	src/Statement.cpp
	src/FunctionDefinition.cpp
	src/TypeUse.cpp
	src/System.cpp
	src/EntityDeclaration.cpp)

target_link_libraries(ast PUBLIC rlc::utils)


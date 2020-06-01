rlcAddLibrary(ast SHARED 
	src/Type.cpp
	src/Constant.cpp
	src/Call.cpp
	src/Expression.cpp
	src/Entity.cpp
	src/Statement.cpp)

target_link_libraries(ast PUBLIC rlc::utils)


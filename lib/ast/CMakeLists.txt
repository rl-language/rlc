rlcAddLibrary(ast SHARED 
	src/Type.cpp
	src/Constant.cpp)
target_link_libraries(ast PUBLIC rlc::utils)


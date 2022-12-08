rlcAddLibrary(parser SHARED src/Lexer.cpp src/Parser.cpp)
target_link_libraries(parser PUBLIC
	rlc::utils
	rlc::dialect)


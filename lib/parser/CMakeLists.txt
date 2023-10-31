rlcAddLibrary(parser src/Lexer.cpp src/Parser.cpp src/MultiFileParser.cpp)
target_link_libraries(parser PUBLIC
	rlc::utils
	rlc::dialect)


rlcAddLibrary(parser SHARED src/Lexer.cpp)
target_link_libraries(parser PUBLIC 
	rlc::utils 
	rlc::ast)


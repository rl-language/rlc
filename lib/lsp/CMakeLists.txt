rlcAddLibrary(lsp src/LSP.cpp src/LSPContext.cpp src/Protocol.cpp)
target_link_libraries(lsp PUBLIC MLIRLspServerLib rlc::parser)


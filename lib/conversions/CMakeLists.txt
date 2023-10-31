rlcAddLibrary(conversions src/RLCToPython.cpp src/RLCToC.cpp src/RLCToCPass.cpp)
llvm_map_components_to_libnames(llvm_utils_libs Support)
target_link_libraries(conversions PUBLIC ${llvm_utils_libs} rlc::dialect rlc::python)


rlcAddLibrary(driver src/Driver.cpp)
llvm_map_components_to_libnames(llvm_utils_libs Support)
target_link_libraries(driver PUBLIC ${llvm_utils_libs} rlc::dialect rlc::parser rlc::backend rlc::conversions   )


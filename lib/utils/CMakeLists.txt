rlcAddLibrary(utils SHARED src/Utils.cpp)
llvm_map_components_to_libnames(llvm_utils_libs Support)
target_link_libraries(utils PUBLIC ${llvm_utils_libs})


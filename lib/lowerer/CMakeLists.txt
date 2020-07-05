rlcAddLibrary(lowerer SHARED src/Lowerer.cpp)
llvm_map_components_to_libnames(llvm_lowerer_libs Core ${LLVM_TARGETS_TO_BUILD})
target_link_libraries(lowerer PUBLIC ${llvm_lowerer_libs} rlc::ast)


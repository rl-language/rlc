get_property(translation_libs GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)
rlcAddTool(rlc-tblgen
    MLIRTblgenLib
    LLVMTableGen
    MLIRTableGen
    ${dialect_libs}
    ${llvm_libs}
    ${translation_libs})
target_include_directories(rlc-tblgen PUBLIC ${MLIR_INCLUDE_DIRS})

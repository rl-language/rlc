rlcAddLibrary(python src/Dialect.cpp src/Types.cpp src/Operations.cpp src/Interfaces.cpp)
target_link_libraries(python PUBLIC MLIRSupport MLIRDialect MLIRLLVMDialect MLIRLLVMIRTransforms MLIRControlFlowDialect)

set(tblgen ${LLVM_BINARY_DIR}/bin/mlir-tblgen)
add_rlc_dialect(python)


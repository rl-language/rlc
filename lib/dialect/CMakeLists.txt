rlcAddLibrary(dialect SHARED src/Dialect.cpp src/Types.cpp src/Operations.cpp src/Conversion.cpp src/EmitMain.cpp src/TypeCheck.cpp src/Interfaces.cpp src/SymbolTable.cpp)
target_link_libraries(dialect PUBLIC MLIRSupport MLIRDialect MLIRLLVMDialect MLIRLLVMIRTransforms MLIRControlFlowDialect)

set(tblgen ${LLVM_BINARY_DIR}/bin/mlir-tblgen)

#Dialect
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td
	COMMAND ${tblgen} --gen-dialect-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Dialect.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td
	COMMAND ${tblgen} --gen-dialect-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Dialect.inc)

add_custom_target(DialectInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc)
add_custom_target(DialectIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Dialect.inc)

#types
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Types.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td
	COMMAND ${tblgen} --gen-typedef-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Types.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Types.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td
	COMMAND ${tblgen} --gen-typedef-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Types.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(TypesInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Types.inc)
add_custom_target(TypesIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Types.inc)

#operations
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Operations.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td
	COMMAND ${tblgen} --gen-op-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Operations.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Operations.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td
	COMMAND ${tblgen} --gen-op-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Operations.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(OperationsInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Operations.inc)
add_custom_target(OperationsIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Operations.inc)

#interfaces
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Interfaces.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td
	COMMAND ${tblgen} --gen-op-interface-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Interfaces.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Interfaces.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td
	COMMAND ${tblgen} --gen-op-interface-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Interfaces.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(InterfacesInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Interfaces.inc)
add_custom_target(InterfacesIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/dialect/Interfaces.inc)

add_dependencies(dialect DialectInc DialectIncludeInc TypesInc TypesIncludeInc OperationsInc OperationsIncludeInc InterfacesInc InterfacesIncludeInc)


rlcAddLibrary(dialect src/Dialect.cpp src/Types.cpp src/Operations.cpp src/Conversion.cpp src/EmitMain.cpp src/TypeCheck.cpp src/Interfaces.cpp src/SymbolTable.cpp src/ActionArgumentAnalysis.cpp src/LowerActionPass.cpp src/LowerArrayCalls.cpp src/LowerToCf.cpp src/ActionStatementsToCoro.cpp src/OverloadResolver.cpp src/LowerIsOperationsPass.cpp src/InstantiateTemplatesPass.cpp src/LowerAssignPass.cpp src/EmitImplicitAssignPass.cpp src/LowerConstructOpPass.cpp src/EmitImplicitInitPass.cpp src/EmitImplicitDestructorInvocationsPass.cpp src/LowerForFieldOpPass.cpp)
target_link_libraries(dialect PUBLIC rlc::utils MLIRSupport MLIRDialect MLIRLLVMDialect MLIRLLVMIRTransforms MLIRControlFlowDialect)

set(tblgen ${LLVM_BINARY_DIR}/bin/mlir-tblgen)

macro(add_rlc_dialect dialectName)
#Dialect
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td
	COMMAND ${tblgen} --gen-dialect-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Dialect.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td
	COMMAND ${tblgen} --gen-dialect-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Dialect.inc)

add_custom_target(${dialectName}DialectInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc)
add_custom_target(${dialectName}DialectIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Dialect.inc)

#types
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Types.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td
	COMMAND ${tblgen} --gen-typedef-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Types.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Types.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td
	COMMAND ${tblgen} --gen-typedef-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Types.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(${dialectName}TypesInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Types.inc)
add_custom_target(${dialectName}TypesIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Types.inc)

#operations
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Operations.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td
	COMMAND ${tblgen} --gen-op-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Operations.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Operations.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td
	COMMAND ${tblgen} --gen-op-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Operations.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(${dialectName}OperationsInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Operations.inc)
add_custom_target(${dialectName}OperationsIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Operations.inc)

#interfaces
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Interfaces.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td
	COMMAND ${tblgen} --gen-op-interface-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Interfaces.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Interfaces.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td
	COMMAND ${tblgen} --gen-op-interface-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Interfaces.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target(${dialectName}InterfacesInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Interfaces.inc)
add_custom_target(${dialectName}InterfacesIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Interfaces.inc)

#interfaces
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Passes.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Passes.td
	COMMAND ${tblgen} --gen-pass-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Passes.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Passes.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src -name ${dialectName})

add_custom_target(${dialectName}PassesIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Passes.inc)

add_dependencies(${dialectName} ${dialectName}PassesIncludeInc ${dialectName}DialectInc ${dialectName}DialectIncludeInc ${dialectName}TypesInc ${dialectName}TypesIncludeInc ${dialectName}OperationsInc ${dialectName}OperationsIncludeInc ${dialectName}InterfacesInc ${dialectName}InterfacesIncludeInc)
endmacro(add_rlc_dialect)

add_rlc_dialect(dialect)

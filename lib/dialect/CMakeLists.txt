rlcAddLibrary(dialect src/Dialect.cpp  src/Types.cpp src/Operations.cpp src/Conversion.cpp src/EmitMain.cpp src/TypeCheck.cpp src/Interfaces.cpp src/SymbolTable.cpp src/ActionArgumentAnalysis.cpp src/LowerActionPass.cpp src/LowerArrayCalls.cpp src/LowerToCf.cpp src/ActionStatementsToCoro.cpp src/OverloadResolver.cpp src/LowerIsOperationsPass.cpp src/InstantiateTemplatesPass.cpp src/LowerAssignPass.cpp src/EmitImplicitAssignPass.cpp src/LowerConstructOpPass.cpp src/EmitImplicitInitPass.cpp src/EmitImplicitDestructorInvocationsPass.cpp src/LowerForFieldOpPass.cpp src/EmitEnumEntitiesPass.cpp src/SortTypeDeclarationsPass.cpp src/AddOutOfBoundsCheckPass.cpp src/PrintIRPass.cpp src/ExtractPreconditionPass.cpp src/LowerAssertsPass.cpp src/AddPreconditionsCheckPass.cpp src/ActionLiveness.cpp src/UncheckedAstToDot.cpp src/RewriteCallSignaturesPass.cpp src/RemoveUselessAllocaPass.cpp src/MembeFunctionsToRegularFunctionsPass.cpp src/LowerInitializerListsPass.cpp src/Enums.cpp src/HoistAllocaPass.cpp src/RemoveUninitConstructsPass.cpp src/ConstraintsAnalysis.cpp src/TypeInterface.cpp src/SerializeRLPass.cpp src/Attrs.cpp)
target_link_libraries(dialect PUBLIC rlc::utils MLIRSupport MLIRDialect MLIRLLVMDialect MLIRLLVMIRTransforms MLIRControlFlowDialect)

set(tblgen ${LLVM_BINARY_DIR}/bin/mlir-tblgen)

macro(add_rlc_dialect dialectName)
# Custom Builder
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Builder.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td
    COMMAND rlc-tblgen --print-builder-decls -I ${LLVM_INCLUDE_DIRS} -I ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/Operations.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Builder.inc)
add_custom_target(${dialectName}BuilderInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Builder.inc rlc-tblgen)

#Dialect
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td
	COMMAND ${tblgen} --gen-dialect-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Dialect.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td
	COMMAND ${tblgen} --gen-dialect-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Dialect.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Dialect.inc)

add_custom_target(${dialectName}DialectInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Dialect.inc)
add_custom_target(${dialectName}DialectIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Dialect.inc)

#attrs
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Attrs.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Attrs.td
    COMMAND ${tblgen} --gen-attrdef-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Attrs.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Attrs.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Attrs.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Attrs.td
    COMMAND ${tblgen} --gen-attrdef-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Attrs.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Attrs.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(${dialectName}AttrsInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Attrs.inc)
add_custom_target(${dialectName}AttrsIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Attrs.inc)


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

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/TypeInterfaces.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td
	COMMAND ${tblgen} --gen-type-interface-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/TypeInterfaces.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/TypeInterfaces.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td
	COMMAND ${tblgen} --gen-type-interface-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Interfaces.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/TypeInterfaces.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target(${dialectName}TypeInterfacesInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/TypeInterfaces.inc)
add_custom_target(${dialectName}TypeInterfacesIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/TypeInterfaces.inc)

#interfaces
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Passes.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Passes.td
	COMMAND ${tblgen} --gen-pass-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Passes.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Passes.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src -name ${dialectName})

add_custom_target(${dialectName}PassesIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Passes.inc)

# enums
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Enums.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Enums.td
    COMMAND ${tblgen} --gen-enum-decls -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Enums.td -o ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Enums.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src -name ${dialectName})

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/Enums.inc DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/Enums.td
    COMMAND ${tblgen} --gen-enum-defs -I ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src/Enums.td -o ${CMAKE_CURRENT_BINARY_DIR}/src/Enums.inc -I ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(${dialectName}EnumIncludeInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/${dialectName}/Enums.inc)
add_custom_target(${dialectName}EnumInc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/Enums.inc)

#####
add_dependencies(${dialectName} ${dialectName}PassesIncludeInc ${dialectName}DialectInc ${dialectName}DialectIncludeInc ${dialectName}TypesInc ${dialectName}TypesIncludeInc ${dialectName}OperationsInc ${dialectName}OperationsIncludeInc ${dialectName}TypeInterfacesInc ${dialectName}TypeInterfacesIncludeInc ${dialectName}InterfacesInc ${dialectName}InterfacesIncludeInc ${dialectName}EnumIncludeInc ${dialectName}EnumInc ${dialectName}AttrsIncludeInc ${dialectName}AttrsInc ${dialectName}BuilderInc)

endmacro(add_rlc_dialect)


add_rlc_dialect(dialect)

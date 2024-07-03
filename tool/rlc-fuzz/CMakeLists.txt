IF (WIN32)
ELSE()
get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(translation_libs GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)
rlcAddTool(rlc-fuzz
    rlc::driver
    rlc::backend
    MLIRLLVMToLLVMIRTranslation
	MLIRLLVMCommonConversion
	MLIRTargetLLVMIRExport
	MLIRTranslateLib
	MLIRSPIRVDialect
	${dialect_libs}
	${translation_libs}
    )

target_link_options(rlc-fuzz PUBLIC -fsanitize=fuzzer)
ENDIF()

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(translation_libs GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)
llvm_map_components_to_libnames(llvm_libs core ipo vectorize instcombine target scalaropts objcarcopts ${LLVM_TARGETS_TO_BUILD})
rlcAddTool(rlc
	rlc::parser
	rlc::conversions
	rlc::backend
	rlc::driver
	MLIRTargetLLVMIRExport
	LLVMCodeGen
	LLVMAnalysis
	LLVMMC
	LLVMTransformUtils
	LLVMPasses
	LLVMInstrumentation
	LLVMAggressiveInstCombine
	MLIRLLVMToLLVMIRTranslation
	MLIRLLVMCommonConversion
	MLIRTargetLLVMIRExport
	MLIRTranslateLib
	MLIRSPIRVDialect
	${dialect_libs}
	${translation_libs}
	${llvm_libs})

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rlc/Fuzzer.hpp INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Fuzzer.hpp.in)
target_include_directories(rlc PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
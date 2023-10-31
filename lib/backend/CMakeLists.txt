rlcAddLibrary(backend src/BackEnd.cpp)
llvm_map_components_to_libnames(llvm_utils_libs Support)
llvm_map_components_to_libnames(llvm_libs core ipo vectorize instcombine target scalaropts objcarcopts ${LLVM_TARGETS_TO_BUILD})
target_link_libraries(backend
	PRIVATE
	rlc::parser
	rlc::conversions
	rlc::dialect
	${llvm_utils_libs}
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
	${llvm_libs}
	)


get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(translation_libs GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)
llvm_map_components_to_libnames(llvm_libs core ipo vectorize instcombine target scalaropts objcarcopts)
rlcAddTool(rlc
	rlc::parser
	rlc::lowerer
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


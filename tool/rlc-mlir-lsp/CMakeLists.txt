get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
rlcAddTool(rlc-mlir-lsp
	rlc::parser
	rlc::backend
	rlc::conversions
	rlc::dialect
    MLIRLspServerLib
    ${dialect_libs})

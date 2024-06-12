get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
rlcAddTool(rlc-opt
	rlc::parser
	rlc::backend
	rlc::conversions
	rlc::dialect
	MLIROptLib
    ${dialect_libs})

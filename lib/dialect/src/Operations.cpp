#include "rlc/dialect/Operations.hpp"

#include "rlc/dialect/Dialect.h"
#define GET_OP_CLASSES
#include "./Operations.inc"

void mlir::rlc::RLCDialect::registerOperations()
{
	addOperations<
#define GET_OP_LIST
#include "./Operations.inc"
			>();
}

::mlir::LogicalResult mlir::rlc::CallOp::verifySymbolUses(
		::mlir::SymbolTableCollection &symbolTable)
{
	return LogicalResult::success();
}

#pragma once

#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/Dialect.h"
#include "rlc/dialect/Dialect.inc"

namespace mlir::rlc
{
	mlir::LogicalResult logRemark(mlir::Operation *op, llvm::Twine twine);
	mlir::LogicalResult logError(mlir::Operation *op, llvm::Twine twine);
}	 // namespace mlir::rlc

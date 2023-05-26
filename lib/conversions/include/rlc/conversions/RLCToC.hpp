#pragma once

#include "mlir/IR/BuiltinOps.h"

namespace rlc
{
	void rlcToCHeader(mlir::ModuleOp Module, llvm::raw_ostream& OS);
	void rlcToGodot(mlir::ModuleOp Module, llvm::raw_ostream& OS);
}	 // namespace rlc

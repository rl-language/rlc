#pragma once

#include <variant>

#include "llvm/ADT/ArrayRef.h"
#include "mlir/IR/FunctionInterfaces.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "rlc/python/Interfaces.hpp"
#include "rlc/python/Types.hpp"

namespace mlir::rlc::detail
{

}	 // namespace mlir::rlc::detail
#define GET_OP_CLASSES
#include "rlc/python/Operations.inc"

namespace rlc
{
}

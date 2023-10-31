#pragma once

#include "mlir/Pass/Pass.h"
#include "rlc/python/Dialect.h"

namespace mlir::python
{
#define GEN_PASS_DECL
#include "rlc/python/Passes.inc"

#define GEN_PASS_REGISTRATION
#include "rlc/python/Passes.inc"

}	 // namespace mlir::python

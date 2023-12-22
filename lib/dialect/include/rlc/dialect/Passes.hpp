#pragma once

#include "mlir/Pass/Pass.h"
#include "rlc/dialect/Dialect.h"

namespace mlir::rlc
{
	struct TargetInfo;
#define GEN_PASS_DECL
#include "rlc/dialect/Passes.inc"

#define GEN_PASS_REGISTRATION
#include "rlc/dialect/Passes.inc"

}	 // namespace mlir::rlc

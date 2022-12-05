#pragma once

#include "llvm/ADT/ArrayRef.h"
#include "mlir/IR/FunctionInterfaces.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "rlc/dialect/Types.hpp"
#define GET_OP_CLASSES
#include <variant>

#include "rlc/dialect/Operations.inc"

namespace rlc
{
	using StatementTypes = std::variant<
			mlir::rlc::StatementList,
			mlir::rlc::ExpressionStatement,
			mlir::rlc::DeclarationStatement,
			mlir::rlc::IfStatement,
			mlir::rlc::ReturnStatement,
			mlir::rlc::WhileStatement,
			mlir::rlc::ActionStatement>;
}

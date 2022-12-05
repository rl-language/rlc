
#include "rlc/dialect/Dialect.h"

#include "Dialect.inc"
#include "rlc/dialect/Types.hpp"

void mlir::rlc::RLCDialect::initialize()
{
	registerTypes();
	registerOperations();
}

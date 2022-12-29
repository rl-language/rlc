#include "rlc/python/Types.hpp"

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/AsmParser/AsmParser.h"
#include "mlir/AsmParser/AsmParserState.h"
#include "mlir/AsmParser/CodeComplete.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/StorageUniquerSupport.h"
#include "mlir/Parser/Parser.h"
#include "rlc/python/Dialect.h"

#define GET_TYPEDEF_CLASSES
#include "Types.inc"

void mlir::rlc::python::RLCPython::registerTypes()
{
	addTypes<
#define GET_TYPEDEF_LIST
#include "Types.inc"
			>();
}

using namespace mlir::rlc;

#include <cassert>
#include <strings.h>
#include "llvm/ADT/SmallVector.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/Location.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Region.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/Support/LLVM.h"
#include "rlc/dialect/Operations.hpp"


struct DeducedConstraints {
    mlir::Value min;
    mlir::Value max;
};

enum TermType {
    DEPENDS_ON_ARG,
    DEPENDS_ON_OTHER_UNKNOWNS,
    KNOWN_VALUE
};

class DynamicArgumentAnalysis
{
    public:
    explicit DynamicArgumentAnalysis(mlir::rlc::FunctionOp op, mlir::ValueRange knownArgs, mlir::OpBuilder builder, mlir::Location loc);
    DeducedConstraints deduceConstraints(int argIndex);

    private:
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> expandToDNF(mlir::Value constraint);
    TermType decideTermType(mlir::Value term, mlir::Value argument);
    mlir::Value compute(mlir::Value expression);
    DeducedConstraints findImposedConstraints(mlir::Value constraint, mlir::Value arg);
    DeducedConstraints findImposedConstraints(mlir::Operation *binaryOperation, mlir::Value arg);
    DeducedConstraints findImposedConstraints(mlir::rlc::CallOp call, mlir::Value arg);

    mlir::rlc::FunctionOp function;
    mlir::Region& precondition;
    mlir::ValueRange knownArgs;
    mlir::OpBuilder builder;
    mlir::Location loc;
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> conjunctions;
};

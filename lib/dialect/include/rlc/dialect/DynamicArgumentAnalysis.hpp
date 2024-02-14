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

/*
    Given a function and a set of known (bound) arguments for the function, we try to find the
        minimum and maximum values the unknown (unbound) arguments can take while satisfying the
        function's precondition.

    We first normalize the precondition to be a disjunction of conjunction of terms, where each 
        term is a value produced by something other than an AndOp or an OrOp. i.e. we express the
        precondition in the form (t_1 AND t_2 AND ...) OR (t_3 AND t_4 AND ...).

    Then, to find the minimum and the maximum for a given argument "arg", we emit code as follows:
    We emit declarations for aggregate_min and aggregate_max, initialized to minus and plus infinity.

    For each conjunction (t_1 AND t_2 AND ...) we classify the terms as
        - conditions: terms depending only on known values 
        - constraints: terms depending on "arg" and no other unknown values
        - others: terms depending on other unknown values.
    
    We analyse the minimum and maximum values each constraint implies on "arg" in terms of known 
        dynamic values. Currently, this returns [-infinity, +infinity] for anything but <, >, >=, <=, ==
        constraints where one of the sides is "arg", and calls to CanOp results.

    Then, we emit an if statement in the form of
        if(condition_1 & condition_2....) {
            current_min = -inf
            if(imposed_min(constraint_1) > current_min)
                current_min = imposed_min(constraint_1)
            if(imposed_min(constraint_2) > current_min)
                current_min = imposed_min(constraint_2)
            ...
            if ( uninitialized(aggregate_min) || current_min < aggregate_min)
                aggregate_min = current_min
        }

    In other words, we compute
        aggregate_min = max([
            min([imposed_min(term) for term in conjunction.constraints])
            for conjunction in constraint
            where evaluate(conjunction.conditions) == true
        ])

    and similarly for the maximum.
*/
class DynamicArgumentAnalysis
{
    public:
    explicit DynamicArgumentAnalysis(mlir::rlc::FunctionOp op, mlir::ValueRange knownArgs, mlir::Value argPicker, mlir::OpBuilder builder, mlir::Location loc);
    mlir::Value pickArg(int argIndex);

    private:
    DeducedConstraints deduceConstraints(int argIndex);
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> expandToDNF(mlir::Value constraint);
    TermType decideTermType(mlir::Value term, mlir::Value argument);
    mlir::Value compute(mlir::Value expression);
    DeducedConstraints findImposedConstraints(mlir::Value constraint, mlir::Value arg);
    DeducedConstraints findImposedConstraints(mlir::Operation *binaryOperation, mlir::Value arg);
    DeducedConstraints findImposedConstraints(mlir::rlc::CallOp call, mlir::Value arg);

    mlir::rlc::FunctionOp function;
    mlir::Region& precondition;
    mlir::ValueRange knownArgs;
    mlir::Value argPicker;
    mlir::OpBuilder builder;
    mlir::Location loc;
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> conjunctions;
};

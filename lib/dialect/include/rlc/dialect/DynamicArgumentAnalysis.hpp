#include <cassert>
#include <cstdint>
#include <ranges>
#include <strings.h>
#include "llvm/ADT/DenseMap.h"
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
    DEPENDS_ON_UNBOUND,
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


/*
    memberAddress is the "path" from the arg to the value we want to pick.
    Example: arg = arg2, memberAddress = [2, 1] maps to MemberAccess(MemberAccess(arg,2), 1)
*/
struct UnboundValue {
    mlir::Value argument;
    llvm::SmallVector<uint64_t> memberAddress;

    /*
        Returns whether this unbound value corressponds to the term.
    */
    bool matches(mlir::Value term) {
        mlir::Value current = term;
        // walk the member address in reverse, test if it leads to the argument.
        for(uint64_t & index : std::ranges::reverse_view(memberAddress)) {
            auto definingOp = current.getDefiningOp();
            if( not llvm::detail::isPresent(definingOp))
                return false;
            if(auto memberAccess = mlir::dyn_cast<mlir::rlc::MemberAccess>(definingOp)) {
                if (memberAccess.getMemberIndex() != index) {
                    return false;
                }
                current = memberAccess.getValue();
            } else {
                return false;
            }
        }
        return current == argument;
    }

    mlir::Type getType() {
        auto type = argument.getType();
        for (auto index : memberAddress) {
            type = type.cast<mlir::rlc::EntityType>().getBody()[index];
        }
        return type;
    }
};

class DynamicArgumentAnalysis
{
    public:
    explicit DynamicArgumentAnalysis(mlir::rlc::FunctionOp op, mlir::ValueRange boundArgs, mlir::Value argPicker, mlir::OpBuilder builder, mlir::Location loc);
    mlir::Value pickArg(int argIndex);

    private:
    DeducedConstraints deduceIntegerUnboundValueConstraints(UnboundValue unbound);
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> expandToDNF(mlir::Value constraint);
    TermType decideTermType(mlir::Value term, UnboundValue unbound);
    mlir::Value compute(mlir::Value expression);
    DeducedConstraints findImposedConstraints(mlir::Value constraint, UnboundValue unbound);
    DeducedConstraints findImposedConstraints(mlir::Operation *binaryOperation, UnboundValue unbound);
    DeducedConstraints findImposedConstraints(mlir::rlc::CallOp call, UnboundValue unbound);

    mlir::Value pickIntegerUnboundValue(UnboundValue unbound);
    mlir::Value pickUnboundValue(UnboundValue unbound);

    mlir::rlc::FunctionOp function;
    mlir::Region& precondition;

    llvm::SmallVector<std::pair<UnboundValue, mlir::Value>> bindings;
    /*
        If the passed value matches an UnboundValue that has a binding,
            return the value bound to it.
        Otherwise returns nullptr.
    */
    mlir::Value getBoundValue(mlir::Value expr) {
        for(auto binding : bindings) {
            if(binding.first.matches(expr))
                return binding.second;
        }
        return nullptr;
    }
    
    mlir::Value argPicker;
    mlir::OpBuilder builder;
    mlir::Location loc;
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> conjunctions;
};

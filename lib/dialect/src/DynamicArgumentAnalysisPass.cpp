#include <cassert>
#include <cstdint>
#include <strings.h>
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Location.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Region.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Support/LLVM.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/DynamicArgumentAnalysis.hpp"

DynamicArgumentAnalysis::DynamicArgumentAnalysis(mlir::rlc::FunctionOp op, mlir::ValueRange knownArgs, mlir::Value argPicker, mlir::OpBuilder builder, mlir::Location loc): 
        function(op),
        precondition(op.getPrecondition()),
        knownArgs(knownArgs),
        argPicker(argPicker),
        builder(builder),
        loc(loc) {
    auto yield = mlir::dyn_cast<mlir::rlc::Yield>(precondition.getBlocks().front().back());
    assert(yield->getNumOperands() == 1);
    conjunctions = expandToDNF(yield->getOperand(0));
}


void helper(
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> &conjunctions,
    llvm::SmallVector<mlir::Value> currentConjunction,
    mlir::Region &precondition
    ) {
    for (auto expr : llvm::enumerate(currentConjunction)) {
        auto definingOp = expr.value().getDefiningOp();

        // the expr is defined outside of the precondition, it doesn't need to be expanded.
        if(definingOp->getParentRegion() != precondition) {
            continue;
        }

        // TODO add other stopping conditions?

        if(auto andOp = llvm::dyn_cast<mlir::rlc::AndOp>(definingOp)) {
            currentConjunction.erase(currentConjunction.begin() + expr.index());
            currentConjunction.append({andOp.getLhs(), andOp.getRhs()});
            helper(conjunctions, currentConjunction, precondition);
            return;
        }

        if(auto orOp = llvm::dyn_cast<mlir::rlc::OrOp>(definingOp)) {
            currentConjunction.erase(currentConjunction.begin() + expr.index());
            auto rhs = orOp.getRhs();
            auto lhs = orOp.getLhs();

            currentConjunction.emplace_back(rhs);
            helper(conjunctions, currentConjunction, precondition);

            currentConjunction.pop_back();
            currentConjunction.emplace_back(lhs);
            helper(conjunctions, currentConjunction, precondition);
            return;
        }
    }
    conjunctions.emplace_back(currentConjunction);
}

/*
    Expands the given value into a disjunction of conjunctions of terms.
    A term is a value that's not the result of on AndOp or an OrOp.
*/
llvm::SmallVector<llvm::SmallVector<mlir::Value>> DynamicArgumentAnalysis::expandToDNF(mlir::Value constraint) {
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> conjunctions;
    helper(conjunctions, {constraint}, precondition);
    return conjunctions;
}

TermType DynamicArgumentAnalysis::decideTermType(mlir::Value term, mlir::Value argument) {
    if(term == argument)
        return DEPENDS_ON_ARG;

    if(auto arg = llvm::dyn_cast<mlir::BlockArgument>(term)) {
        if(arg.getArgNumber() < knownArgs.size())
            return KNOWN_VALUE;
        
        return DEPENDS_ON_OTHER_UNKNOWNS;
    }

    auto definingOp = term.getDefiningOp();

    if(auto casted = llvm::dyn_cast<mlir::rlc::Constant>(definingOp))
        return KNOWN_VALUE;

    if(definingOp->getParentRegion() != precondition)
        return KNOWN_VALUE;

    bool dependsOnArg = false;
    bool dependsOnOtherUnkowns = false;

    for(auto operand : definingOp->getOperands()) {
        auto operandType = decideTermType(operand, argument);
        if(operandType == DEPENDS_ON_ARG)
            dependsOnArg = true;
        else if (operandType == DEPENDS_ON_OTHER_UNKNOWNS)
            dependsOnOtherUnkowns = true;
    }

    if (dependsOnArg) { return DEPENDS_ON_ARG; }
    if (dependsOnOtherUnkowns) { return DEPENDS_ON_OTHER_UNKNOWNS; };
    return KNOWN_VALUE;
}

mlir::Value DynamicArgumentAnalysis::compute(mlir::Value expression) {
    if(auto arg = llvm::dyn_cast<mlir::BlockArgument>(expression)) {
        assert(arg.getArgNumber() < knownArgs.size());
        return knownArgs[arg.getArgNumber()];
    }

    if (expression.getDefiningOp()->getParentRegion() != precondition)
        return expression;

    int resultIndex = -1;
    for(auto result : llvm::enumerate(expression.getDefiningOp()->getResults())) {
        if(result.value() == expression)
            resultIndex = result.index();
    }
    assert(resultIndex != -1);

    auto newOp = builder.clone(*expression.getDefiningOp());
    builder.setInsertionPoint(newOp);
    for(auto operand : llvm::enumerate(newOp->getOperands())) {
        auto newOperand = compute(operand.value());
        newOp->setOperand(operand.index(), newOperand);
    }
    builder.setInsertionPointAfter(newOp);
    return newOp->getResult(resultIndex);
}

//TODO 
const int64_t min_int = -800;
const int64_t max_int = 800;

DeducedConstraints DynamicArgumentAnalysis::findImposedConstraints(mlir::Operation *binaryOperation, mlir::Value arg) {
    if (binaryOperation->getOperand(0) == arg) {
            auto rhs = compute(binaryOperation->getOperand(1));

            if (mlir::isa<mlir::rlc::LessOp>(binaryOperation))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::Constant>(loc, min_int),
                    builder.create<mlir::rlc::SubOp>(loc, rhs, one)
                };
            }

            if (mlir::isa<mlir::rlc::LessEqualOp>(binaryOperation))
            {
                return {
                    builder.create<mlir::rlc::Constant>(loc, min_int),
                    rhs
                };
            }

            if (mlir::isa<mlir::rlc::GreaterOp>(binaryOperation))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::AddOp>(loc, rhs, one),
                    builder.create<mlir::rlc::Constant>(loc, max_int)
                };
            }

            if (mlir::isa<mlir::rlc::GreaterEqualOp>(binaryOperation))
            {
                return {
                    rhs,
                    builder.create<mlir::rlc::Constant>(loc, max_int)
                };
            }

            if (mlir::isa<mlir::rlc::EqualOp>(binaryOperation))
            {
                return {
                    rhs,
                    rhs
                };
            }
        }

        if(binaryOperation->getOperand(1) == arg) {
            auto lhs = compute(binaryOperation->getOperand(0));

            if (mlir::isa<mlir::rlc::LessOp>(binaryOperation))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::AddOp>(loc, lhs, one),
                    builder.create<mlir::rlc::Constant>(loc, max_int),
                };
            }

            if (mlir::isa<mlir::rlc::LessEqualOp>(binaryOperation))
            {
                return {
                    lhs,
                    builder.create<mlir::rlc::Constant>(loc, max_int),
                };
            }

            if (mlir::isa<mlir::rlc::GreaterOp>(binaryOperation))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::Constant>(loc, min_int),
                    builder.create<mlir::rlc::SubOp>(loc, lhs, one)
                };
            }

            if (mlir::isa<mlir::rlc::GreaterEqualOp>(binaryOperation))
            {
                return {
                    builder.create<mlir::rlc::Constant>(loc, min_int),
                    lhs
                };
            }

            if (mlir::isa<mlir::rlc::EqualOp>(binaryOperation))
            {
                return {
                    lhs,
                    lhs
                };
            }
        }
    return {
        builder.create<mlir::rlc::Constant>(loc, min_int),
        builder.create<mlir::rlc::Constant>(loc, max_int)
    };
}

DeducedConstraints DynamicArgumentAnalysis::findImposedConstraints(mlir::rlc::CallOp call, mlir::Value arg) {
    if( mlir::rlc::CanOp can = llvm::dyn_cast<mlir::rlc::CanOp>(call.getCallee().getDefiningOp())) {
        auto underlyingFunction = llvm::dyn_cast<mlir::rlc::FunctionOp>(*can.getCallee().getDefiningOp());
        llvm::SmallVector<mlir::Value> knownArgsOfUnderlyingFunction;

        int argIndex = -1;
        for(auto current : llvm::enumerate(call.getArgs())) {
            // decide which arguments are known (assuming (in general) known arguments are before unknown arguments.)
            if( not current.value().isa<mlir::BlockArgument>()) {
                auto newExpr = compute(current.value());
                knownArgsOfUnderlyingFunction.emplace_back(newExpr);
                continue;
            }
            // and find the index of the argument we're interested in.
            if(current.value() == arg) {
                argIndex = current.index();
                break;
            }
        }
        assert(argIndex != -1 && "Expected to find the argument.");
        DynamicArgumentAnalysis analysis(underlyingFunction, knownArgsOfUnderlyingFunction, argPicker, builder, loc);
        return analysis.deduceConstraints(argIndex);
    }
    return {
        builder.create<mlir::rlc::Constant>(loc, min_int),
        builder.create<mlir::rlc::Constant>(loc, max_int)
    };
}


DeducedConstraints DynamicArgumentAnalysis::findImposedConstraints(mlir::Value constraint, mlir::Value arg) {
    auto *definingOp = constraint.getDefiningOp();
    if (definingOp->getOperands().size() == 2) {
        return findImposedConstraints(definingOp, arg);
    }

    if( mlir::rlc::CallOp call = llvm::dyn_cast<mlir::rlc::CallOp>(definingOp)) {
        return  findImposedConstraints(call, arg);
	}

    return {
        builder.create<mlir::rlc::Constant>(loc, min_int),
        builder.create<mlir::rlc::Constant>(loc, max_int)
    };
}

/*
    Emits `if (value > target) target = value`
*/
void assignIfGreaterthan(mlir::Value value, mlir::Value target, mlir::OpBuilder builder, mlir::Location loc) {
    auto ifStatement = builder.create<mlir::rlc::IfStatement>(loc);
    builder.createBlock(&ifStatement.getElseBranch());
    builder.create<mlir::rlc::Yield>(loc);

    builder.createBlock(&ifStatement.getCondition());
    auto g = builder.create<mlir::rlc::GreaterOp>(loc, value, target);
    builder.create<mlir::rlc::Yield>(loc, g.getResult());

    builder.createBlock(&ifStatement.getTrueBranch());
    builder.create<mlir::rlc::BuiltinAssignOp>(loc, target, value);
    builder.create<mlir::rlc::Yield>(loc);
    builder.setInsertionPointAfter(ifStatement);
}

/*
    Emits `if (value < target) target = value`
*/
void assignIfLessThan(mlir::Value value, mlir::Value target, mlir::OpBuilder builder, mlir::Location loc) {
    auto ifStatement = builder.create<mlir::rlc::IfStatement>(loc);
    builder.createBlock(&ifStatement.getElseBranch());
    builder.create<mlir::rlc::Yield>(loc);

    builder.createBlock(&ifStatement.getCondition());
    auto g = builder.create<mlir::rlc::LessOp>(loc, value, target);
    builder.create<mlir::rlc::Yield>(loc, g.getResult());

    builder.createBlock(&ifStatement.getTrueBranch());
    builder.create<mlir::rlc::BuiltinAssignOp>(loc, target, value);
    builder.create<mlir::rlc::Yield>(loc);
    builder.setInsertionPointAfter(ifStatement);
}

void maybeAssignMin(mlir::Value currentMin, mlir::Value aggregateMin, mlir::Value minInt, mlir::OpBuilder builder, mlir::Location loc) {
    auto maybeAssignMin = builder.create<mlir::rlc::IfStatement>(loc);
    builder.createBlock(&maybeAssignMin.getElseBranch());
    builder.create<mlir::rlc::Yield>(loc);

    builder.createBlock(&maybeAssignMin.getCondition());
    auto minUninitialized = builder.create<mlir::rlc::EqualOp>(loc, aggregateMin, minInt);
    auto currentMinIsUninitialized = builder.create<mlir::rlc::EqualOp>(loc, currentMin, minInt);
    auto currentMinIsInitialized = builder.create<mlir::rlc::NotOp>(loc, currentMinIsUninitialized.getResult());
    auto currentMinIsSmaller = builder.create<mlir::rlc::LessOp>(loc, currentMin, aggregateMin);
    auto cond = builder.create<mlir::rlc::OrOp>(loc, minUninitialized, currentMinIsSmaller);
    auto cond2 = builder.create<mlir::rlc::AndOp>(loc, currentMinIsInitialized, cond);
    builder.create<mlir::rlc::Yield>(loc, cond2.getResult());

    builder.createBlock(&maybeAssignMin.getTrueBranch());
    builder.create<mlir::rlc::BuiltinAssignOp>(loc, aggregateMin, currentMin);
    builder.create<mlir::rlc::Yield>(loc);
    builder.setInsertionPointAfter(maybeAssignMin);
}

void maybeAssignMax(mlir::Value currentMax, mlir::Value aggregateMax, mlir::Value maxInt, mlir::OpBuilder builder, mlir::Location loc) {
    auto maybeAssignMax= builder.create<mlir::rlc::IfStatement>(loc);
    builder.createBlock(&maybeAssignMax.getElseBranch());
    builder.create<mlir::rlc::Yield>(loc);

    builder.createBlock(&maybeAssignMax.getCondition());
    auto maxUninitialized = builder.create<mlir::rlc::EqualOp>(loc, aggregateMax, maxInt);
    auto currentMaxIsUninitialized = builder.create<mlir::rlc::EqualOp>(loc, currentMax, maxInt);
    auto currentMaxIsInitialized = builder.create<mlir::rlc::NotOp>(loc, currentMaxIsUninitialized.getResult());
    auto currentMaxIsGreater = builder.create<mlir::rlc::GreaterOp>(loc, currentMax, aggregateMax);
    auto cond = builder.create<mlir::rlc::OrOp>(loc, maxUninitialized, currentMaxIsGreater);
    auto cond2 = builder.create<mlir::rlc::AndOp>(loc, currentMaxIsInitialized, cond);
    builder.create<mlir::rlc::Yield>(loc, cond2.getResult());

    builder.createBlock(&maybeAssignMax.getTrueBranch());
    builder.create<mlir::rlc::BuiltinAssignOp>(loc, aggregateMax, currentMax);
    builder.create<mlir::rlc::Yield>(loc);
    builder.setInsertionPointAfter(maybeAssignMax);
}

DeducedConstraints DynamicArgumentAnalysis::deduceConstraints(int argIndex) {
    auto arg = function.getPrecondition().getArgument(argIndex);
    assert(arg.getType().isa<mlir::rlc::IntegerType>() && "Expected an integer.");

    auto minVal = builder.create<mlir::rlc::UninitializedConstruct>(loc, arg.getType());
    auto minInt =  builder.create<mlir::rlc::Constant>(loc, min_int);
    builder.create<mlir::rlc::BuiltinAssignOp>(loc, minVal, minInt);

    auto maxVal = builder.create<mlir::rlc::UninitializedConstruct>(loc, arg.getType());
    auto maxInt =  builder.create<mlir::rlc::Constant>(loc, max_int);
    builder.create<mlir::rlc::BuiltinAssignOp>(loc, maxVal, maxInt);

    for (auto conjunction :  conjunctions) {
        llvm::SmallVector<mlir::Value> constraints;
        llvm::SmallVector<mlir::Value> conditions;
        // categorize the terms in the conjunction
        for(auto term : conjunction) {
            TermType type = decideTermType(term, arg);
            if(type == DEPENDS_ON_ARG) {
                constraints.emplace_back(term);
            } else if (type == KNOWN_VALUE){
                conditions.emplace_back(term);
            }
        }

        // if there are any constraints on unknown args, emit an if statement for this conjunction.
        if(constraints.size() > 0) {
            auto minForThisConjunction = builder.create<mlir::rlc::UninitializedConstruct>(loc, arg.getType());
            builder.create<mlir::rlc::BuiltinAssignOp>(loc, minForThisConjunction, minInt);

            auto maxForThisConjunction = builder.create<mlir::rlc::UninitializedConstruct>(loc, arg.getType());
            builder.create<mlir::rlc::BuiltinAssignOp>(loc, maxForThisConjunction, maxInt);

            // if the conditions are met, the constraints are active.
            auto ifStatement = builder.create<mlir::rlc::IfStatement>(loc);
            builder.createBlock(&ifStatement.getElseBranch());
            builder.create<mlir::rlc::Yield>(loc);

            builder.createBlock(&ifStatement.getCondition());
            mlir::Value condition = builder.create<mlir::rlc::Constant>(loc, true);
            for( auto c : conditions) {
                auto computed = compute(c);
                condition = builder.create<mlir::rlc::AndOp>(loc, condition, computed);
            }

            builder.create<mlir::rlc::Yield>(loc, condition);

            builder.createBlock(&ifStatement.getTrueBranch());
            for(auto  constraint : constraints) {
                auto imposedConstraints = findImposedConstraints(constraint, arg);      

                // if the minimum imposed by this constraint is greater than the current minimum, set the current minimum.
                assignIfGreaterthan(imposedConstraints.min, minForThisConjunction, builder, loc);

                // if the maximum imposed by this constraint is less than the current maximum, set the current maximum.
                assignIfLessThan(imposedConstraints.max, maxForThisConjunction, builder, loc);
            }

            // if minVal is not initialized, or if the min imposed by this conjunction is smaller than it, set minVal 
            //  to the min imposed by this conjunction.
            maybeAssignMin(minForThisConjunction, minVal, minInt, builder, loc);

            // similarly for maxVal.
            maybeAssignMax(maxForThisConjunction, maxVal, maxInt, builder, loc);

            builder.create<mlir::rlc::Yield>(loc);
            builder.setInsertionPointAfter(ifStatement);
        }
    }
    return {minVal, maxVal};
}

mlir::Value DynamicArgumentAnalysis::pickArg(int argIndex) {
    auto deduced = deduceConstraints(argIndex);
    auto call = builder.create<mlir::rlc::CallOp>(
        loc,
        argPicker,
        false,
        mlir::ValueRange({deduced.min, deduced.max})
    );
    return call.getResult(0);
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_DYNAMICARGUMENTANALYSISPASS
#include "rlc/dialect/Passes.inc"
	struct DynamicArgumentAnalysisPass
			: public impl::DynamicArgumentAnalysisPassBase<DynamicArgumentAnalysisPass>
	{
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}

		void runOnOperation() override
		{
			ModuleOp module = getOperation();

            mlir::Value argPicker; 
            for (auto op : module.getOps<mlir::rlc::FunctionOp>()) {
                if(op.getUnmangledName().equals("fuzzer_pick_argument")) {
                    argPicker = op.getResult();
                    break;
                }
            }

            llvm::SmallVector<mlir::rlc::PickedArgOp, 4> pickedArgOps;
			module.walk([&](mlir::rlc::PickedArgOp op) {
				pickedArgOps.emplace_back(op);
			});

			for (auto pickedArgOp : pickedArgOps)
			{
                mlir::OpBuilder builder(pickedArgOp);
                mlir::Location loc = pickedArgOp->getLoc();

                auto function = llvm::dyn_cast<mlir::rlc::FunctionOp>(pickedArgOp.getFunction().getDefiningOp());
                assert( function  && "Expected a FunctionOp");
                DynamicArgumentAnalysis analysis(function, pickedArgOp.getKnownArgs(), argPicker, builder, loc);
                auto pickedArg = analysis.pickArg(pickedArgOp.getArgumentIndex());

                pickedArgOp.getResult().replaceAllUsesWith(pickedArg);
                pickedArgOp->erase();
			}
		}
	};
}	 // namespace mlir::rlc
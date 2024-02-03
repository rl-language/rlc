#include <cassert>
#include <cstdint>
#include <strings.h>
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Location.h"
#include "mlir/IR/Region.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Support/LLVM.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"

struct DeducedConstraints {
    mlir::Value min;
    mlir::Value max;
};

enum TermType {
    DEPENDS_ON_ARG,
    DEPENDS_ON_OTHER_UNKNOWNS,
    KNOWN_VALUE
};

class ArgumentAnalysis
{
    public:
    explicit ArgumentAnalysis(mlir::rlc::FunctionOp op);
    
    DeducedConstraints deduceConstraints(int argIndex, mlir::ValueRange knownArgs, mlir::OpBuilder builder, mlir::Location loc);

    private:
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> conjunctions;
    mlir::rlc::FunctionOp function;
};


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
    Expands the given value into a disjunction of conjunctions.
*/
llvm::SmallVector<llvm::SmallVector<mlir::Value>> ExpandToDNF(mlir::Value constraint, mlir::Region &precondition) {
    llvm::SmallVector<llvm::SmallVector<mlir::Value>> conjunctions;
    helper(conjunctions, {constraint}, precondition);
    return conjunctions;
}


ArgumentAnalysis::ArgumentAnalysis(mlir::rlc::FunctionOp op) {
    auto yield = mlir::dyn_cast<mlir::rlc::Yield>(op.getPrecondition().getBlocks().front().back());
    assert(yield->getNumOperands() == 1);
    function = op;
    conjunctions = ExpandToDNF(yield->getOperand(0), op.getPrecondition());
    llvm::dbgs() << "CONJUNCTIONS: \n";
    for(auto c :conjunctions) {
        llvm::dbgs() << "{";
        for (auto t : c) {
            llvm::dbgs() << "(  ";
            t.print(llvm::dbgs());
            llvm::dbgs() << "   ),";
        }
        llvm::dbgs() << "}\n";
    }

}

TermType decideTermType(mlir::Value term, mlir::Value argument, mlir::ValueRange knownArgs, mlir::Region &precondition) {
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
        auto operandType = decideTermType(operand, argument, knownArgs, precondition);
        if(operandType == DEPENDS_ON_ARG)
            dependsOnArg = true;
        else if (operandType == DEPENDS_ON_OTHER_UNKNOWNS)
            dependsOnOtherUnkowns = true;
    }

    if (dependsOnArg) { return DEPENDS_ON_ARG; }
    if (dependsOnOtherUnkowns) { return DEPENDS_ON_OTHER_UNKNOWNS; };
    return KNOWN_VALUE;
}

mlir::Value recompute(mlir::Value expression, mlir::ValueRange knownArgs, mlir::OpBuilder builder, mlir::Region &precondition) {
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
        auto newOperand = recompute(operand.value(), knownArgs, builder, precondition);
        newOp->setOperand(operand.index(), newOperand);
    }
    builder.setInsertionPointAfter(newOp);
    return newOp->getResult(resultIndex);
}

//TODO 
const int64_t min_int = -800;
const int64_t max_int = 800;


DeducedConstraints findImposedConstraints(mlir::Value constraint, mlir::Value arg, mlir::ValueRange knownArgs, mlir::OpBuilder builder, mlir::Location loc, mlir::Region &precondition) {
    auto *definingOp = constraint.getDefiningOp();
    if (definingOp->getOperands().size() == 2) {
        if (definingOp->getOperand(0) == arg) {
            auto rhs = recompute(definingOp->getOperand(1), knownArgs, builder, precondition);

            if (mlir::isa<mlir::rlc::LessOp>(definingOp))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::Constant>(loc, min_int),
                    builder.create<mlir::rlc::SubOp>(loc, rhs, one)
                };
            }

            if (mlir::isa<mlir::rlc::LessEqualOp>(definingOp))
            {
                return {
                    rhs,
                    builder.create<mlir::rlc::Constant>(loc, max_int)
                };
            }

            if (mlir::isa<mlir::rlc::GreaterOp>(definingOp))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::AddOp>(loc, rhs, one),
                    builder.create<mlir::rlc::Constant>(loc, max_int)
                };
            }

            if (mlir::isa<mlir::rlc::GreaterEqualOp>(definingOp))
            {
                return {
                    rhs,
                    builder.create<mlir::rlc::Constant>(loc, max_int)
                };
            }

            if (mlir::isa<mlir::rlc::EqualOp>(definingOp))
            {
                return {
                    rhs,
                    rhs
                };
            }
        }

        if(definingOp->getOperand(1) == arg) {
            auto lhs = recompute(definingOp->getOperand(0), knownArgs, builder, precondition);

            if (mlir::isa<mlir::rlc::LessOp>(definingOp))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::AddOp>(loc, lhs, one),
                    builder.create<mlir::rlc::Constant>(loc, max_int),
                };
            }

            if (mlir::isa<mlir::rlc::LessEqualOp>(definingOp))
            {
                return {
                    lhs,
                    builder.create<mlir::rlc::Constant>(loc, max_int),
                };
            }

            if (mlir::isa<mlir::rlc::GreaterOp>(definingOp))
            {
                auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
                return {
                    builder.create<mlir::rlc::Constant>(loc, min_int),
                    builder.create<mlir::rlc::SubOp>(loc, lhs, one)
                };
            }

            if (mlir::isa<mlir::rlc::GreaterEqualOp>(definingOp))
            {
                return {
                    builder.create<mlir::rlc::Constant>(loc, min_int),
                    lhs
                };
            }

            if (mlir::isa<mlir::rlc::EqualOp>(definingOp))
            {
                return {
                    lhs,
                    lhs
                };
            }
        }
    }

    if( mlir::rlc::CallOp call = llvm::dyn_cast<mlir::rlc::CallOp>(definingOp))
		{
			if( mlir::rlc::CanOp can = llvm::dyn_cast<mlir::rlc::CanOp>(call.getCallee().getDefiningOp())) {
				llvm::dbgs() << "Underlying function\n"; 
				auto underlyingFunction = llvm::dyn_cast<mlir::rlc::FunctionOp>(*can.getCallee().getDefiningOp());
				underlyingFunction.dump();

                llvm::SmallVector<mlir::Value> knownArgsOfUnderlyingFunction;
                int argIndex = -1;
                for(auto current : llvm::enumerate(call.getArgs())) {
                    // decide which arguments are known (assuming (in general) known arguments are before unknown arguments.)
                    if( not current.value().isa<mlir::BlockArgument>()) {
                        auto newExpr = recompute(current.value(), knownArgs, builder, precondition);
                        knownArgsOfUnderlyingFunction.emplace_back(newExpr);
                        continue;
                    }
                    // and find the index of the argument we're interested in.
                    if(current.value() == arg) {
                        argIndex = current.index();
                        break;
                    }
                }
                assert(argIndex != -1 && "Expecteed to find the argument.");
                ArgumentAnalysis analysis(underlyingFunction);
                return analysis.deduceConstraints(argIndex, knownArgsOfUnderlyingFunction, builder, loc);
			}
		}


    return {
        builder.create<mlir::rlc::Constant>(loc, min_int),
        builder.create<mlir::rlc::Constant>(loc, max_int)
    };
}

DeducedConstraints ArgumentAnalysis::deduceConstraints(int argIndex, mlir::ValueRange knownArgs, mlir::OpBuilder builder, mlir::Location loc) {
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
        for(auto term : conjunction) {
            TermType type = decideTermType(term, arg, knownArgs, function.getPrecondition());
            if(type == DEPENDS_ON_ARG) {
                constraints.emplace_back(term);
            } else if (type == KNOWN_VALUE){
                conditions.emplace_back(term);
            }
        }

        if(constraints.size() > 0) {
            llvm::dbgs() << "Well here we are \n";
            for (auto t : constraints) {
                llvm::dbgs() << "(  ";
                t.print(llvm::dbgs());
                llvm::dbgs() << "   ),";
            }
            llvm::dbgs() << "\n";

            auto ifStatement = builder.create<mlir::rlc::IfStatement>(loc);
            builder.createBlock(&ifStatement.getElseBranch());
            builder.create<mlir::rlc::Yield>(loc);

            builder.createBlock(&ifStatement.getCondition());
            mlir::Value condition = builder.create<mlir::rlc::Constant>(loc, true);
            for( auto c : conditions) {
                auto recomputed = recompute(c, knownArgs, builder, function.getPrecondition());
                condition = builder.create<mlir::rlc::AndOp>(loc, condition, recomputed);
            }

            builder.create<mlir::rlc::Yield>(loc, condition);

            builder.createBlock(&ifStatement.getTrueBranch());
            for(auto  constraint : constraints) {
                auto imposedConstraints = findImposedConstraints(constraint, arg, knownArgs, builder, loc, function.getPrecondition());
               
                // if the minimum imposed by this constraint is greater than the current minimum, set the current minimum.
                auto minIfStatement = builder.create<mlir::rlc::IfStatement>(loc);
                builder.createBlock(&minIfStatement.getElseBranch());
                builder.create<mlir::rlc::Yield>(loc);

                builder.createBlock(&minIfStatement.getCondition());
                auto g = builder.create<mlir::rlc::GreaterOp>(loc, imposedConstraints.min, minVal);
                builder.create<mlir::rlc::Yield>(loc, g.getResult());

                builder.createBlock(&minIfStatement.getTrueBranch());
                builder.create<mlir::rlc::BuiltinAssignOp>(loc, minVal, imposedConstraints.min);
                builder.create<mlir::rlc::Yield>(loc);
                builder.setInsertionPointAfter(minIfStatement);

                // if the maximum imposed by this constraint is less than the current maximum, set the current maximum.
                auto maxIfStatement = builder.create<mlir::rlc::IfStatement>(loc);
                builder.createBlock(&maxIfStatement.getElseBranch());
                builder.create<mlir::rlc::Yield>(loc);

                builder.createBlock(&maxIfStatement.getCondition());
                auto l = builder.create<mlir::rlc::LessOp>(loc, imposedConstraints.max, maxVal);
                builder.create<mlir::rlc::Yield>(loc, l.getResult());

                builder.createBlock(&maxIfStatement.getTrueBranch());
                builder.create<mlir::rlc::BuiltinAssignOp>(loc, maxVal, imposedConstraints.max);
                builder.create<mlir::rlc::Yield>(loc);
                builder.setInsertionPointAfter(maxIfStatement);
            }
            builder.create<mlir::rlc::Yield>(loc);
            builder.setInsertionPointAfter(ifStatement);
        }
    }
    return {minVal, maxVal};
}

namespace mlir::rlc
{
#define GEN_PASS_DEF_ARGUMENTANALYSISPASS
#include "rlc/dialect/Passes.inc"
	struct ArgumentAnalysisPass
			: public impl::ArgumentAnalysisPassBase<ArgumentAnalysisPass>
	{
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}

		void runOnOperation() override
		{
			ModuleOp module = getOperation();


            llvm::SmallVector<mlir::rlc::ArgConstraintsOp, 4> argConstraintsOps;
			module.walk([&](mlir::rlc::ArgConstraintsOp op) {
				argConstraintsOps.emplace_back(op);
			});

			for (auto argConstraints: argConstraintsOps)
			{
                mlir::OpBuilder builder(argConstraints);
                mlir::Location loc = argConstraints->getLoc();

                auto function = llvm::dyn_cast<mlir::rlc::FunctionOp>(argConstraints.getFunction().getDefiningOp());
                assert( function  && "Expected a FunctionOp");
                ArgumentAnalysis analysis(function);
                auto deduced = analysis.deduceConstraints(argConstraints.getArgumentIndex(), argConstraints.getKnownArgs(), builder, loc);
                argConstraints.getMin().replaceAllUsesWith(deduced.min);
                argConstraints.getMax().replaceAllUsesWith(deduced.max);
                argConstraints.erase();
			}
		}
	};

}	 // namespace mlir::rlc
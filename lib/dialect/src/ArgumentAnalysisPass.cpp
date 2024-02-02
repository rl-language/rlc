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
#include "mlir/Pass/Pass.h"
#include "mlir/Support/LLVM.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Types.hpp"

class ArgumentAnalysis
{
    public:
    explicit ArgumentAnalysis(mlir::rlc::FunctionOp op);
    
    mlir::Value emitMinimumValue(int argIndex, mlir::Value actionEntity, mlir::OpBuilder builder, mlir::Location loc);

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

bool dependsOn(mlir::Value term, mlir::Value argument, mlir::Region &precondition) {
    if(term == argument)
        return true;

    if(term.isa<mlir::BlockArgument>())
        return false;

    auto definingOp = term.getDefiningOp();

    if(auto casted = llvm::dyn_cast<mlir::rlc::Constant>(definingOp))
        return false;

    if(definingOp->getParentRegion() != precondition)
        return false;

    for(auto operand : definingOp->getOperands()) {
        if(dependsOn(operand, argument, precondition))
            return true;
    }
    return false;
}

mlir::Value recompute(mlir::Value expression, mlir::Value actionEntity, mlir::OpBuilder builder, mlir::Region &precondition) {
    if(auto arg = llvm::dyn_cast<mlir::BlockArgument>(expression)) {
        assert(arg.getArgNumber() == 0);
        return actionEntity;
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
        auto newOperand = recompute(operand.value(), actionEntity, builder, precondition);
        newOp->setOperand(operand.index(), newOperand);
    }
    builder.setInsertionPointAfter(newOp);
    return newOp->getResult(resultIndex);
}

//TODO 
const int64_t min_int = -800;

mlir::Value findMinConstraint(mlir::Value constraint, mlir::Value arg, mlir::Value actionEntity, mlir::OpBuilder builder, mlir::Location loc, mlir::Region &precondition) {
    auto *definingOp = constraint.getDefiningOp();
    if (definingOp->getOperands().size() != 2)
        return builder.create<mlir::rlc::Constant>(loc, min_int);

    if (definingOp->getOperand(0) == arg) {
        auto rhs = recompute(definingOp->getOperand(1), actionEntity, builder, precondition);

        if (mlir::isa<mlir::rlc::LessOp>(definingOp))
        {
            // could set max here.
        }

        if (mlir::isa<mlir::rlc::LessEqualOp>(definingOp))
        {
            // could set max here.
        }

        if (mlir::isa<mlir::rlc::GreaterOp>(definingOp))
        {
            auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
            return builder.create<mlir::rlc::AddOp>(loc, rhs, one);
        }

        if (mlir::isa<mlir::rlc::GreaterEqualOp>(definingOp))
        {
            return rhs;
        }

        if (mlir::isa<mlir::rlc::EqualOp>(definingOp))
        {
            return rhs;
        }
    }

    if(definingOp->getOperand(1) == arg) {
        auto lhs = recompute(definingOp->getOperand(0), actionEntity, builder, precondition);

        if (mlir::isa<mlir::rlc::LessOp>(definingOp))
        {
            auto one = builder.create<mlir::rlc::Constant>(loc, (int64_t)1);
            return builder.create<mlir::rlc::AddOp>(loc, lhs, one);
        }

        if (mlir::isa<mlir::rlc::LessEqualOp>(definingOp))
        {
            return lhs;
        }

        if (mlir::isa<mlir::rlc::GreaterOp>(definingOp))
        {
            // could set max here.
        }

        if (mlir::isa<mlir::rlc::GreaterEqualOp>(definingOp))
        {
            // could set max here.
        }

        if (mlir::isa<mlir::rlc::EqualOp>(definingOp))
        {
            return lhs;
        }
    }

    return builder.create<mlir::rlc::Constant>(loc, min_int);
}

mlir::Value ArgumentAnalysis::emitMinimumValue(int argIndex, mlir::Value actionEntity, mlir::OpBuilder builder, mlir::Location loc) {
    auto arg = function.getPrecondition().getArgument(argIndex);
    assert(arg.getType().isa<mlir::rlc::IntegerType>() && "Expected an integer.");
    auto minVal = builder.create<mlir::rlc::UninitializedConstruct>(loc, arg.getType());
    auto minInt =  builder.create<mlir::rlc::Constant>(loc, min_int);
    builder.create<mlir::rlc::BuiltinAssignOp>(loc, minVal, minInt);

    for (auto conjunction :  conjunctions) {
        llvm::SmallVector<mlir::Value> constraints;
        llvm::SmallVector<mlir::Value> conditions;
        for(auto term : conjunction) {
            if(dependsOn(term, arg, function.getPrecondition())) {
                constraints.emplace_back(term);
            } else {
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
                auto recomputed = recompute(c, actionEntity, builder, function.getPrecondition());
                condition = builder.create<mlir::rlc::AndOp>(loc, condition, recomputed);
            }

            builder.create<mlir::rlc::Yield>(loc, condition);

            builder.createBlock(&ifStatement.getTrueBranch());
            for(auto  constraint : constraints) {
                auto imposed_min = findMinConstraint(constraint, arg, actionEntity, builder, loc, function.getPrecondition());
                auto innerIfStatement = builder.create<mlir::rlc::IfStatement>(loc);
                builder.createBlock(&innerIfStatement.getElseBranch());
                builder.create<mlir::rlc::Yield>(loc);

                builder.createBlock(&innerIfStatement.getCondition());
                auto g = builder.create<mlir::rlc::GreaterOp>(loc, imposed_min, minVal);
                builder.create<mlir::rlc::Yield>(loc, g.getResult());

                builder.createBlock(&innerIfStatement.getTrueBranch());
                builder.create<mlir::rlc::BuiltinAssignOp>(loc, minVal, imposed_min);
                builder.create<mlir::rlc::Yield>(loc);
                builder.setInsertionPointAfter(innerIfStatement);
            }
            builder.create<mlir::rlc::Yield>(loc);
            builder.setInsertionPointAfter(ifStatement);
        }
    }
    return minVal;
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

            llvm::SmallVector<mlir::rlc::MinValOp, 4> minValOps;
			module.walk([&](mlir::rlc::MinValOp op) {
				minValOps.emplace_back(op);
			});

			for (auto minVal : minValOps)
			{
                auto function = llvm::dyn_cast<mlir::rlc::FunctionOp>(minVal.getFunction().getDefiningOp());
                assert( function  && "Expected a FunctionOp");
                ArgumentAnalysis analysis(function);
                auto min = analysis.emitMinimumValue(minVal.getArgumentIndex(), minVal.getActionEntity(), mlir::OpBuilder(minVal), minVal->getLoc());
                minVal.replaceAllUsesWith(min);
                minVal.erase();
			}

            //module.dump();
		}
	};

}	 // namespace mlir::rlc
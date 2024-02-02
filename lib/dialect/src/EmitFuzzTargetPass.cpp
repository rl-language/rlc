/*
Copyright 2024 Cem Cebeci

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <cassert>
#include <cstdint>
#include <string>
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Location.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/OperationSupport.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/IR/Region.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Support/LLVM.h"
#include "rlc/dialect/ActionArgumentAnalysis.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/SymbolTable.h"
#include "rlc/dialect/Types.hpp"

static mlir::Value findFunction(mlir::ModuleOp module, llvm::StringRef functionName) {
    for (auto op : module.getOps<mlir::rlc::FunctionOp>())
        if(op.getUnmangledName().equals(functionName))
            return op.getResult();

    assert(0 && "failed to find the function");
    return nullptr;
}

/*
    let actionEntity = action()
    TODO handle action function arguments.
*/
static mlir::Value emitActionEntityDeclaration(
        mlir::rlc::ActionFunction action,
        mlir::rlc::FunctionOp fuzzActionFunction,
        mlir::OpBuilder builder
) {
    auto ip = builder.saveInsertionPoint();

    auto call = builder.create<mlir::rlc::CallOp>(
        fuzzActionFunction->getLoc(),
        // the first result of the ActionFunction op is the function that initializes the entity.
        action->getResults().front(),
        true,
        mlir::ValueRange({}) // TODO Assuming the action has no args for now.
    );
    builder.restoreInsertionPoint(ip);
    return call->getResult(0);
}

/*
    not is_done_action(actionEntity) and not stop and isInputLongEnough
*/
static void emitLoopCondition(
    mlir::rlc::ActionFunction action,
    mlir::Region *condition,
    mlir::Value actionEntity,
    mlir::Value stopFlag,
    mlir::OpBuilder builder
) {
    
    auto ip = builder.saveInsertionPoint(); 

    auto isInputLongEnough = findFunction(action->getParentOfType<mlir::ModuleOp>(), "fuzzer_is_input_long_enough");    
    builder.createBlock(condition);
    auto actionIsDone = builder.create<mlir::rlc::CallOp>(
        action->getLoc(),
        action->getResult(1), // the second result of the actionFunction is the isDoneFunction.
        true,
        mlir::ValueRange({actionEntity})
    );
    auto longEnough = builder.create<mlir::rlc::CallOp>(
        action->getLoc(),
        isInputLongEnough,
        false,
        mlir::ValueRange({})
    );
    auto neg = builder.create<mlir::rlc::NotOp>(action->getLoc(), actionIsDone.getResult(0));
    auto neg2 = builder.create<mlir::rlc::NotOp>(action->getLoc(), stopFlag);
    auto conj = builder.create<mlir::rlc::AndOp>(action->getLoc(), neg, neg2);
    auto conj2 = builder.create<mlir::rlc::AndOp>(action.getLoc(), conj.getResult(), longEnough->getResult(0));
    builder.create<mlir::rlc::Yield>(action->getLoc(), conj2.getResult());
    builder.restoreInsertionPoint(ip);
}

/*
    Emits:

        let availableSubactions = Vector<Int>
        if(subAction0.resumptionIndex == actionEntity.resumptionIndex)
            availableSubactions.push(0)
        if(subAction1.resumptionIndex == actionEntity.resumptionIndex)
            availableSubactions.push(1)
        ...
        let index = getInput(availableSubactions.size)
        let chosenAction = availableSubactions.get(index)

    We use the helper functions init_available_subactions, add_available_subaction, pick_subaction
        defined in fuzzer.utils to avoid dealing with templates here.
*/
static mlir::Value emitChosenActionDeclaration(
    mlir::rlc::ActionFunction action,
    mlir::Value actionEntity,
    mlir::Value availableSubactionsVector,
    mlir::rlc::ModuleBuilder &moduleBuilder,
    mlir::OpBuilder builder
) {
    auto ip = builder.saveInsertionPoint();

    auto clearAvailableSubactions = findFunction(action->getParentOfType<mlir::ModuleOp>(), "fuzzer_clear_available_subactions");
    auto addAvailableSubaction = findFunction(action->getParentOfType<mlir::ModuleOp>(), "fuzzer_add_available_subaction");
    auto pickSubaction = findFunction(action->getParentOfType<mlir::ModuleOp>(), "fuzzer_pick_subaction");

    // availableSubactions.clear()
    builder.create<mlir::rlc::CallOp>(
        action->getLoc(),
        clearAvailableSubactions,
        false,
        mlir::ValueRange({availableSubactionsVector})
    );

    // for each subaction,
    //      if(subAction.resumptionIndex == actionEntity.resumptionIndex)
    //          availableSubactions.push(subactionID)
    int64_t index = 0;
    for(auto subactionFunction : action.getActions()) {
        auto ifStatement = builder.create<mlir::rlc::IfStatement>(action->getLoc());
        builder.createBlock(&ifStatement.getCondition());
        auto storedResumptionPoint = builder.create<mlir::rlc::MemberAccess>(
            action->getLoc(),
            actionEntity,
            0
        );

        // the subactionFunction is available if the stored resumptionIndex matches any of its acitonStatements' resumptionIndex.
        auto actionStatements = moduleBuilder.actionFunctionValueToActionStatement(subactionFunction);
        mlir::Value lastOperand =
			builder.create<mlir::rlc::Constant>(action.getLoc(), false);
        for(auto *actionStatement : actionStatements) {
            auto cast = mlir::dyn_cast<mlir::rlc::ActionStatement>(actionStatement);
            auto subactionResumptionPoint = builder.create<mlir::rlc::Constant>(action.getLoc(), (int64_t) cast.getResumptionPoint());
            auto eq = builder.create<mlir::rlc::EqualOp>(action->getLoc(), storedResumptionPoint, subactionResumptionPoint);
            lastOperand = builder.create<mlir::rlc::OrOp>(action.getLoc(), lastOperand, eq.getResult());
        }
        
        builder.create<mlir::rlc::Yield>(ifStatement.getLoc(), lastOperand);

        builder.createBlock(&ifStatement.getTrueBranch());
        auto subactionIndex = builder.create<mlir::rlc::Constant>(action->getLoc(), index);
        builder.create<mlir::rlc::CallOp>(
            action->getLoc(),
            addAvailableSubaction,
            false,
            mlir::ValueRange{availableSubactionsVector, subactionIndex.getResult()}
        );
        builder.create<mlir::rlc::Yield>(action->getLoc());

        // construct the false branch that does nothing
        auto *falseBranch = builder.createBlock(&ifStatement.getElseBranch());
        builder.create<mlir::rlc::Yield>(ifStatement.getLoc());
        
        builder.setInsertionPointAfter(ifStatement);
        index++;
    }

    // let chosenAction = pick_subaction(availableSubactions)
    auto chosenAction = builder.create<mlir::rlc::CallOp>(
        action->getLoc(),
        pickSubaction,
        false,
        mlir::ValueRange{availableSubactionsVector}
    )->getResult(0);

    builder.restoreInsertionPoint(ip);
    return chosenAction;
}

/*
    let arg1 = pickArgument(arg_1_size)
    let arg2 = pickArgument(arg_2_size)
    ...

    where arg1,arg2,... are the arguments of the subaction.
*/
static llvm::SmallVector<mlir::Value, 2> emitSubactionArgumentDeclarations(
    mlir::Value subactionFunction,
    mlir::Value actionEntity,
    mlir::Value pickArgument,
    mlir::Value print,
    mlir::Location loc,
    mlir::OpBuilder builder,
    mlir::rlc::ModuleBuilder &moduleBuilder
) {
    auto ip = builder.saveInsertionPoint();
    llvm::SmallVector<mlir::Value, 2> arguments;
    
    // declare the arguments
    auto inputs = mlir::dyn_cast<mlir::FunctionType>(subactionFunction.getType()).getInputs();
    // The first input is the actionEntity, which does not need to be declared here.
    auto inputsExcludingActionEntity = llvm::drop_begin(llvm::enumerate(inputs));
    for(auto inputType : inputsExcludingActionEntity) {
        assert(inputType.value().isa<mlir::rlc::IntegerType>() && "Fuzzing can only handle integer arguments for now.");

        auto input_min = builder.create<mlir::rlc::MinValOp>(
            loc,
            mlir::rlc::IntegerType::getInt64(builder.getContext()),
            subactionFunction,
            (uint8_t)inputType.index(),
            actionEntity
        );
        auto input_max = builder.create<mlir::rlc::Constant>(
            loc,
            (int64_t)10 // TODO
        );
        auto call = builder.create<mlir::rlc::CallOp>(
            loc,
            pickArgument,
            false,
            mlir::ValueRange({input_min.getResult(), input_max.getResult()})
        );
        // print the value picked for the argument for debugging purposes.
        builder.create<mlir::rlc::CallOp>(loc, print, false, call.getResult(0));
        arguments.emplace_back(call->getResult(0));
    }

    builder.restoreInsertionPoint(ip);
    return arguments;
}

/*
    For each subaction, emits the statements:
    if(chosenAction == SUBACTION_INDEX)
    {
        let arg1 = pickArgument(arg_1_size)
        let arg2 = pickArgument(arg_2_size)
        ...
        if( !can_subaction_function(arg1, arg2, ...))
            stop = true
        else 
            subaction_function(actionEntity, arg1, arg2, ...)
    }
*/
static void emitSubactionCases(
    mlir::rlc::ActionFunction action,
    mlir::Region *parentRegion,
    mlir::Value actionEntity,
    mlir::Value chosenAction,
    mlir::Value stopFlag,
    mlir::rlc::ModuleBuilder &moduleBuilder,
    mlir::OpBuilder builder
) {
    auto ip = builder.saveInsertionPoint();
    auto pickArgument = findFunction(action->getParentOfType<mlir::ModuleOp>(), "fuzzer_pick_argument");
    auto print = findFunction(action->getParentOfType<mlir::ModuleOp>(),"fuzzer_print");
    auto skipFuzzInput = findFunction(action->getParentOfType<mlir::ModuleOp>(),"fuzzer_skip_input");
    
    for(auto subaction : llvm::enumerate(action.getActions())) {
        auto ifActionIsChosen = builder.create<mlir::rlc::IfStatement>(action->getLoc());

        // if chosenAction == i:
        builder.createBlock(&ifActionIsChosen.getCondition());
        auto subactionIndex = builder.create<mlir::rlc::Constant>(action.getLoc(), (int64_t)subaction.index());
        auto eq = builder.create<mlir::rlc::EqualOp>(action->getLoc(), subactionIndex.getResult(), chosenAction);
        builder.create<mlir::rlc::Yield>(action.getLoc(), eq.getResult());

        //      let arg1 = pickArgument(arg_1_size)
        //      ...
        builder.createBlock(&ifActionIsChosen.getTrueBranch());
        auto args = emitSubactionArgumentDeclarations(subaction.value(), actionEntity, pickArgument, print, action->getLoc(), builder, moduleBuilder);
        args.insert(args.begin(), actionEntity); // the first argument should be the entity itself.

        //      if( !can_subaction_function(arg1, arg2, ...))
        auto ifInputIsInvalid = builder.create<mlir::rlc::IfStatement>(action->getLoc());
        builder.createBlock(&ifInputIsInvalid.getCondition());
        auto can = builder.create<mlir::rlc::CanOp>(action->getLoc(), subaction.value());
        auto can_call = builder.create<mlir::rlc::CallOp>(action->getLoc(), can.getResult(), false, args);
        auto neg = builder.create<mlir::rlc::NotOp>(action->getLoc(), can_call->getResult(0));
        builder.create<mlir::rlc::Yield>(action -> getLoc(), neg.getResult());
        //          stop = true      
        builder.createBlock(&ifInputIsInvalid.getTrueBranch());
        builder.create<mlir::rlc::CallOp>(action->getLoc(), skipFuzzInput, false, mlir::ValueRange({}));
        auto t = builder.create<mlir::rlc::Constant>(action.getLoc(), true);
        builder.create<mlir::rlc::AssignOp>(action.getLoc(), stopFlag, t.getResult());
        builder.create<mlir::rlc::Yield>(action->getLoc());
        //      else 
        //          subaction_function(actionEntity, arg1, arg2, ...)
        auto *falseBranch = builder.createBlock(&ifInputIsInvalid.getElseBranch());
        builder.create<mlir::rlc::CallOp>(
            action.getLoc(),
            subaction.value(),
            false,
            args
        );
        builder.create<mlir::rlc::Yield>(action->getLoc());

        builder.createBlock(&ifActionIsChosen.getElseBranch());
        builder.create<mlir::rlc::Yield>(action->getLoc());

        builder.setInsertionPointAfter(ifInputIsInvalid);
        builder.create<mlir::rlc::Yield>(action.getLoc());
        builder.setInsertionPointAfter(ifActionIsChosen);
    }
    builder.restoreInsertionPoint(ip);
}

/*
    fun fuzzer_fuzz_action_function():
        let actionEntity = play()
        let stop = false
        let availableSubactions = Vector<Int>
        while not is_done_action(actionEntity) and not stop and isInputLongEnough():
            availableSubactions.clear()
            if(subAction0.resumptionIndex == actionEntity.resumptionIndex)
                availableSubactions.push(0)
            if(subAction1.resumptionIndex == actionEntity.resumptionIndex)
                availableSubactions.push(1)
            ...

            let index = getInput(availableSubactions.size)
            let chosenAction = availableSubactions.get(index)
            switch chosenAction:
                case subaction1:
                    let arg1 = pickArgument(arg_1_size)
                    let arg2 = pickArgument(arg_2_size)
                    ...

                    subaction_function(actionEntity, arg1, arg2, ...)
                case subaction2:
                    ...
                ...
*/
static void emitFuzzActionFunction(mlir::rlc::ActionFunction action) {
    auto loc = action.getLoc();
    mlir::OpBuilder builder(action);
    mlir::rlc::ModuleBuilder moduleBuilder(action->getParentOfType<mlir::ModuleOp>());
    
    auto fuzzActionFunctionType = mlir::FunctionType::get(action.getContext(), {}, {});
    auto fuzzActionFunction = builder.create<mlir::rlc::FunctionOp>(
        loc,
        llvm::StringRef("fuzzer_fuzz_action_function"),
        fuzzActionFunctionType,
        builder.getStrArrayAttr({}),
        false
    );
    builder.createBlock(&fuzzActionFunction.getBody()); 

    auto entityDeclaration = emitActionEntityDeclaration(action, fuzzActionFunction, builder);

    auto stopFlag = builder.create<mlir::rlc::ConstructOp>(
        fuzzActionFunction->getLoc(),
        mlir::rlc::BoolType::get(builder.getContext()));
    auto f = builder.create<mlir::rlc::Constant>(fuzzActionFunction->getLoc(), false);
    builder.create<mlir::rlc::AssignOp>(loc, stopFlag, f);

    // let availableSubactions = Vector<Int>
    auto intVectorType = mlir::rlc::EntityType::getIdentified(
        builder.getContext(),
        "Vector",
        {mlir::rlc::IntegerType::getInt64(builder.getContext())}
    );
    auto initAvailableSubactions = findFunction(action->getParentOfType<mlir::ModuleOp>(), "fuzzer_init_available_subactions");
    auto availableSubactions = builder.create<mlir::rlc::CallOp>(
        action->getLoc(),
        initAvailableSubactions,
        false,
        mlir::ValueRange({})
    )->getResult(0);

    auto whileStmt = builder.create<mlir::rlc::WhileStatement>(loc);
    emitLoopCondition(action, &whileStmt.getCondition(), entityDeclaration, stopFlag.getResult(), builder);
    builder.createBlock(&whileStmt.getBody());
    auto chosenAction = emitChosenActionDeclaration(action, entityDeclaration, availableSubactions, moduleBuilder, builder);
    emitSubactionCases(action, &whileStmt.getBody(), entityDeclaration, chosenAction, stopFlag.getResult(), moduleBuilder, builder);
    builder.create<mlir::rlc::Yield>(loc);
    
    builder.setInsertionPointAfter(whileStmt);
    builder.create<mlir::rlc::Yield>(loc);
}

namespace mlir::rlc
{
#define GEN_PASS_DECL_EMITFUZZTARGETPASS
#define GEN_PASS_DEF_EMITFUZZTARGETPASS
#include "rlc/dialect/Passes.inc"
	struct EmitFuzzTargetPass: public impl::EmitFuzzTargetPassBase<EmitFuzzTargetPass>
	{
        using impl::EmitFuzzTargetPassBase<EmitFuzzTargetPass>::EmitFuzzTargetPassBase;
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}

		void runOnOperation() override
		{   
			ModuleOp module = getOperation();
            mlir::IRRewriter rewriter(module->getContext());

            // invoke emitFuzzActionFunction on the ActionFunction with the correct unmangledName
            for(auto op :module.getOps<ActionFunction>()) {
                if(op.getUnmangledName().str() == actionToFuzz)
				    emitFuzzActionFunction(op);
			}
		}
	};

}
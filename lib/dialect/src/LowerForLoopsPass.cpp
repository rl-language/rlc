/*
Copyright 2024 Massimo Fioravanti

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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/PatternMatch.h"
#include "rlc/dialect/IRBuilder.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{

#define GEN_PASS_DEF_LOWERFORLOOPSPASS
#include "rlc/dialect/Passes.inc"

	struct LowerForLoopsPass: impl::LowerForLoopsPassBase<LowerForLoopsPass>
	{
		using impl::LowerForLoopsPassBase<LowerForLoopsPass>::LowerForLoopsPassBase;

		void runOnOperation() override
		{
			mlir::rlc::ModuleBuilder builder(getOperation());

			llvm::SmallVector<mlir::rlc::ForLoopStatement, 4> loops;
			getOperation().walk(
					[&](mlir::rlc::ForLoopStatement loop) { loops.push_back(loop); });

			mlir::rlc::IRBuilder rewriter(&getContext());
			for (auto loop : loops)
			{
				auto tmpVar = mlir::dyn_cast<mlir::rlc::ForLoopVarDeclOp>(
						loop.getBody().front().front());
				rewriter.setInsertionPoint(loop);
				// var = 0
				auto index = rewriter.createUninitializedConstruct(
						loop.getLoc(), mlir::rlc::IntegerType::getInt64(&getContext()));
				auto zero = rewriter.createConstant(
						loop.getLoc(),
						mlir::rlc::IntegerType::getInt64(&getContext()),
						rewriter.getI64IntegerAttr(0));
				auto one = rewriter.createConstant(
						loop.getLoc(),
						mlir::rlc::IntegerType::getInt64(&getContext()),
						rewriter.getI64IntegerAttr(1));

				rewriter.createBuiltinAssignOp(loop.getLoc(), index, zero);

				// target = exp.size()
				builder.getRewriter().restoreInsertionPoint(
						rewriter.saveInsertionPoint());
				auto size =
						builder.emitCall(loop, true, "size", { loop.getExpression() });
				assert(size->getNumResults() == 1);

				auto whileStmt = rewriter.createWhileStatement(loop.getLoc());
				// while var != target
				auto bb = rewriter.createBlock(
						&whileStmt.getCondition(), whileStmt.getCondition().begin());
				auto equal = rewriter.createNotEqualOp(
						loop.getLoc(), index.getResult(), size->getResult(0));
				rewriter.createYield(loop.getLoc(), { equal });

				// var = exp.get(index)
				whileStmt.getBody().takeBody(loop.getBody());
				rewriter.setInsertionPointToStart(&whileStmt.getBody().front());

				builder.getRewriter().restoreInsertionPoint(
						rewriter.saveInsertionPoint());
				auto var = builder.emitCall(
						loop, true, "get", { loop.getExpression(), index });
				assert(var->getNumResults() == 1);
				mlir::Value value = var->getResult(0);

				tmpVar.getResult().replaceAllUsesWith(value);

				rewriter.restoreInsertionPoint(
						builder.getRewriter().saveInsertionPoint());

				// index = index + 1
				auto increased = rewriter.createAddOp(loop.getLoc(), one, index);
				rewriter.createBuiltinAssignOp(loop.getLoc(), index, increased);
				tmpVar.erase();

				loop.erase();
			}
		}
	};

}	 // namespace mlir::rlc

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
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/SymbolTable.h"

namespace mlir::rlc
{
#define GEN_PASS_DEF_LOWERASSERTSPASS
#include "rlc/dialect/Passes.inc"
	struct LowerAssertsPass: public impl::LowerAssertsPassBase<LowerAssertsPass>
	{
		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}

		void runOnOperation() override
		{
			ModuleOp module = getOperation();
			ModuleBuilder builder(module);

			llvm::SmallVector<mlir::rlc::AssertOp, 4> assertOps;
			module.walk([&](mlir::rlc::AssertOp op) { assertOps.emplace_back(op); });
			for (auto op : assertOps)
			{
				lower(op, builder.getRewriter());
			}
		}

		void lower(mlir::rlc::AssertOp &op, IRRewriter &rewriter)
		{
			rewriter.setInsertionPoint(op);
			auto ifStatement = rewriter.create<mlir::rlc::IfStatement>(op->getLoc());

			auto *conditionBlock = rewriter.createBlock(&ifStatement.getCondition());
			rewriter.create<mlir::rlc::Yield>(
					ifStatement.getLoc(), op.getAssertion());

			// construct the true branch that does nothing
			auto *trueBranch = rewriter.createBlock(&ifStatement.getTrueBranch());
			rewriter.create<mlir::rlc::Yield>(ifStatement.getLoc());

			// construct the false branch that aborts
			auto *falseBranch = rewriter.createBlock(&ifStatement.getElseBranch());
			rewriter.create<mlir::rlc::AbortOp>(ifStatement.getLoc());
			rewriter.eraseOp(op);
		}
	};
}	 // namespace mlir::rlc
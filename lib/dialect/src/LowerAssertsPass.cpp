/*
This file is part of the RLC project.

RLC is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License version 2 as published by the Free Software
Foundation.

RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
RLC. If not, see <https://www.gnu.org/licenses/>.
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
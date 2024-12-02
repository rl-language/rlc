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
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{

#define GEN_PASS_DEF_LOWERINITIALIZERLISTSPASS
#define GEN_PASS_DEF_CONSTANTARRAYTOGLOBALPASS
#include "rlc/dialect/Passes.inc"

	static mlir::Value rewriteAsInitialization(
			mlir::rlc::InitializerListOp initializerList, mlir::IRRewriter& rewriter)
	{
		auto variable = rewriter.create<mlir::rlc::UninitializedConstruct>(
				initializerList.getLoc(), initializerList.getResult().getType());

		auto yield = mlir::cast<mlir::rlc::Yield>(
				initializerList.getBody().front().getTerminator());
		while (initializerList.getBody().front().getOperations().size() != 1)
		{
			auto& op = initializerList.getBody().front().front();
			op.moveBefore(variable);
		}
		for (int64_t i = 0; i < static_cast<int64_t>(yield.getArguments().size());
				 i++)
		{
			auto index =
					rewriter.create<mlir::rlc::Constant>(initializerList.getLoc(), i);
			auto member = rewriter.create<mlir::rlc::ArrayAccess>(
					initializerList.getLoc(), variable, index);
			rewriter.create<mlir::rlc::AssignOp>(
					initializerList.getLoc(), member, yield.getArguments()[i]);
		}
		return variable;
	}

	static mlir::rlc::FlatConstantGlobalOp rewriteAsGlobal(
			llvm::StringRef name, mlir::rlc::Constant op, mlir::IRRewriter& rewriter)
	{
		rewriter.setInsertionPoint(op);
		auto global = rewriter.create<mlir::rlc::FlatConstantGlobalOp>(
				op.getLoc(),
				op.getResult().getType(),
				op.getValue().cast<mlir::ArrayAttr>(),
				name);

		for (auto& use : op.getResult().getUses())
		{
			rewriter.setInsertionPoint(use.getOwner());
			use.assign(rewriter.create<mlir::rlc::Reference>(
					op.getLoc(), global.getType(), global.getName()));
		}

		op.erase();
		return global;
	}

	struct ConstantArrayToGlobalPass
			: impl::ConstantArrayToGlobalPassBase<ConstantArrayToGlobalPass>
	{
		using impl::ConstantArrayToGlobalPassBase<
				ConstantArrayToGlobalPass>::ConstantArrayToGlobalPassBase;

		void runOnOperation() override
		{
			llvm::SmallVector<mlir::rlc::Constant, 4> toGlobals;
			mlir::IRRewriter rewriter(&getContext());
			size_t emittedGlobals = 0;

			getOperation().walk([&](mlir::rlc::Constant initializer) {
				if (initializer.getType().isa<mlir::rlc::ArrayType>())
					toGlobals.push_back(initializer);
			});

			for (auto op : toGlobals)
			{
				auto global = rewriteAsGlobal(
						("constant_array_" + llvm::Twine(emittedGlobals)).str(),
						op,
						rewriter);
				global->moveBefore(
						getOperation().getBody(), getOperation().getBody()->begin());
				emittedGlobals++;
			}
		}
	};

	struct LowerInitializerListsPass
			: impl::LowerInitializerListsPassBase<LowerInitializerListsPass>
	{
		using impl::LowerInitializerListsPassBase<
				LowerInitializerListsPass>::LowerInitializerListsPassBase;

		void runOnOperation() override
		{
			llvm::SmallVector<mlir::rlc::InitializerListOp, 4> initializerLists;
			mlir::IRRewriter rewriter(&getContext());
			size_t emittedGlobals = 0;

			getOperation().walk([&](mlir::rlc::InitializerListOp initializer) {
				initializerLists.push_back(initializer);
			});

			for (auto op :
					 llvm::make_range(initializerLists.rbegin(), initializerLists.rend()))
			{
				rewriter.setInsertionPoint(op);
				rewriter.replaceOp(op, rewriteAsInitialization(op, rewriter));
			}
		}
	};
}	 // namespace mlir::rlc

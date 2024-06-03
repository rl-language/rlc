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
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_EMITENUMENTITIESPASS
#include "rlc/dialect/Passes.inc"

	static void emitAsIntFunction(
			IRRewriter& rewriter, mlir::rlc::EnumDeclarationOp enumOp)
	{
		rewriter.setInsertionPoint(enumOp);
		auto argType =
				mlir::rlc::ScalarUseType::get(rewriter.getContext(), enumOp.getName());
		auto returnType = mlir::rlc::IntegerType::getInt64(rewriter.getContext());
		auto argName = "self";

		auto f = rewriter.create<mlir::rlc::FunctionOp>(
				enumOp.getLoc(),
				"as_int",
				mlir::FunctionType::get(
						rewriter.getContext(), { argType }, { returnType }),
				rewriter.getStrArrayAttr({ argName }),
				false);

		auto* bb = rewriter.createBlock(
				&f.getBody(), f.getBody().begin(), { argType }, { enumOp.getLoc() });
		rewriter.setInsertionPointToStart(bb);

		auto returnStatement = rewriter.create<mlir::rlc::ReturnStatement>(
				enumOp.getLoc(), returnType);
		auto* bbReturn = rewriter.createBlock(
				&returnStatement.getBody(), returnStatement.getBody().begin());
		rewriter.setInsertionPointToStart(bbReturn);

		auto trueOp = rewriter.create<mlir::rlc::UnresolvedMemberAccess>(
				enumOp.getLoc(),
				mlir::rlc::UnknownType::get(rewriter.getContext()),
				f.getBody().front().getArgument(0),
				"value");
		rewriter.create<mlir::rlc::Yield>(
				enumOp.getLoc(), mlir::ValueRange({ trueOp }));
	}

	static void emitMaxMemberFunction(
			IRRewriter& rewriter, mlir::rlc::EnumDeclarationOp enumOp)
	{
		rewriter.setInsertionPoint(enumOp);
		auto argType =
				mlir::rlc::ScalarUseType::get(rewriter.getContext(), enumOp.getName());
		auto returnType = mlir::rlc::IntegerType::getInt64(rewriter.getContext());
		auto argName = "self";

		auto f = rewriter.create<mlir::rlc::FunctionOp>(
				enumOp.getLoc(),
				"max",
				mlir::FunctionType::get(
						rewriter.getContext(), { argType }, { returnType }),
				rewriter.getStrArrayAttr({ argName }),
				false);

		auto* bb = rewriter.createBlock(
				&f.getBody(), f.getBody().begin(), { argType }, { enumOp.getLoc() });
		rewriter.setInsertionPointToStart(bb);

		auto returnStatement = rewriter.create<mlir::rlc::ReturnStatement>(
				enumOp.getLoc(), returnType);
		auto* bbReturn = rewriter.createBlock(
				&returnStatement.getBody(), returnStatement.getBody().begin());
		rewriter.setInsertionPointToStart(bbReturn);

		auto trueOp = rewriter.create<mlir::rlc::Constant>(
				enumOp.getLoc(), int64_t(enumOp.getEnumNames().size() - 1));
		rewriter.create<mlir::rlc::Yield>(
				enumOp.getLoc(), mlir::ValueRange({ trueOp }));
	}

	static void emitIsEnumMemberFunction(
			IRRewriter& rewriter, mlir::rlc::EnumDeclarationOp enumOp)
	{
		rewriter.setInsertionPoint(enumOp);
		auto argType =
				mlir::rlc::ScalarUseType::get(rewriter.getContext(), enumOp.getName());
		auto returnType = mlir::rlc::BoolType::get(rewriter.getContext());
		auto argName = "self";

		auto f = rewriter.create<mlir::rlc::FunctionOp>(
				enumOp.getLoc(),
				"is_enum",
				mlir::FunctionType::get(
						rewriter.getContext(), { argType }, { returnType }),
				rewriter.getStrArrayAttr({ argName }),
				false);

		auto* bb = rewriter.createBlock(
				&f.getBody(), f.getBody().begin(), { argType }, { enumOp.getLoc() });
		rewriter.setInsertionPointToStart(bb);

		auto returnStatement = rewriter.create<mlir::rlc::ReturnStatement>(
				enumOp.getLoc(), returnType);
		auto* bbReturn = rewriter.createBlock(
				&returnStatement.getBody(), returnStatement.getBody().begin());
		rewriter.setInsertionPointToStart(bbReturn);

		auto trueOp = rewriter.create<mlir::rlc::Constant>(enumOp.getLoc(), true);
		rewriter.create<mlir::rlc::Yield>(
				enumOp.getLoc(), mlir::ValueRange({ trueOp }));
	}

	static void emitImplicitEnumFunctions(
			IRRewriter& rewriter, mlir::rlc::EnumDeclarationOp enumOp)
	{
		emitIsEnumMemberFunction(rewriter, enumOp);
		emitMaxMemberFunction(rewriter, enumOp);
		emitAsIntFunction(rewriter, enumOp);
	}

	struct EmitEnumEntitiesPass
			: impl::EmitEnumEntitiesPassBase<EmitEnumEntitiesPass>
	{
		using impl::EmitEnumEntitiesPassBase<
				EmitEnumEntitiesPass>::EmitEnumEntitiesPassBase;

		void runOnOperation() override
		{
			mlir::IRRewriter rewriter(getOperation().getContext());
			llvm::SmallVector<mlir::rlc::EnumDeclarationOp, 2> ops;
			for (auto declaration :
					 getOperation().getOps<mlir::rlc::EnumDeclarationOp>())
				ops.push_back(declaration);

			llvm::StringMap<mlir::rlc::EnumDeclarationOp> enums;
			llvm::DenseMap<mlir::rlc::EnumDeclarationOp, mlir::rlc::EntityType>
					enumsToType;
			for (auto declaration : ops)
			{
				rewriter.setInsertionPoint(declaration);
				auto op = rewriter.create<mlir::rlc::EntityDeclaration>(
						declaration.getLoc(),
						mlir::rlc::UnknownType::get(getOperation().getContext()),
						declaration.getName(),
						rewriter.getTypeArrayAttr({ mlir::rlc::ScalarUseType::get(
								getOperation().getContext(), "Int") }),
						rewriter.getStrArrayAttr({ "value" }),
						rewriter.getTypeArrayAttr({}));
				op.getBody().takeBody(declaration.getBody());
				enums[declaration.getName()] = declaration;
			}

			for (auto declaration : ops)
				emitImplicitEnumFunctions(rewriter, declaration);

			llvm::SmallVector<mlir::rlc::UncheckedEnumUse, 2> uses;
			getOperation().walk(
					[&](mlir::rlc::UncheckedEnumUse op) { uses.push_back(op); });

			for (auto use : uses)
			{	

				
				rewriter.setInsertionPoint(use);
				if (enums.count(use.getEnumName()) == 0)
				{
					auto _ = logError(use, "Enum use does not name a declared enum");
					signalPassFailure();
					return;
				}
				
				auto enumDeclaration = enums[use.getEnumName()];

				bool failed = true;
				for (size_t i = 0; i != enumDeclaration.getEnumNames().size(); i++)
				{	
					
					if (enumDeclaration.getEnumNames()[i].cast<mlir::StringAttr>() !=
							use.getEnumValue())
						continue;
					failed = false;

					auto type = mlir::rlc::EntityType::getIdentified(
							getOperation().getContext(), use.getEnumName(), {});
					rewriter.replaceOpWithNewOp<mlir::rlc::EnumUse>(
							use, type, rewriter.getI64IntegerAttr(i));
					break;
				}
				if (failed)
				{
					auto _ = logError(
							use,
							"Cannot find member " + use.getEnumValue() + " in enum " +
									use.getEnumName());
					signalPassFailure();
				}
			}
		}
	};
}	 // namespace mlir::rlc

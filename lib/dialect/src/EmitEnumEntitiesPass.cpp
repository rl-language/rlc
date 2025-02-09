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
#include <set>

#include "mlir/IR/IRMapping.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_EMITENUMENTITIESPASS
#include "rlc/dialect/Passes.inc"

	static void emitFromIntFunction(
			IRRewriter& rewriter, mlir::rlc::EnumDeclarationOp enumOp)
	{
		rewriter.setInsertionPoint(enumOp);
		auto argType =
				mlir::rlc::ScalarUseType::get(rewriter.getContext(), enumOp.getName());
		auto intType = mlir::rlc::IntegerType::getInt64(rewriter.getContext());
		llvm::StringRef argName = "self";
		llvm::StringRef valueName = "new_value";

		auto f = rewriter.create<mlir::rlc::FunctionOp>(
				enumOp.getLoc(),
				"from_int",
				mlir::FunctionType::get(
						rewriter.getContext(), { argType, intType }, {}),
				mlir::rlc::FunctionInfoAttr::get(
						intType.getContext(), { argName, valueName }),
				false);

		auto* bb = rewriter.createBlock(
				&f.getBody(),
				f.getBody().begin(),
				{ argType, intType },
				{ enumOp.getLoc(), enumOp.getLoc() });
		rewriter.setInsertionPointToStart(bb);

		auto value = rewriter.create<mlir::rlc::UnresolvedMemberAccess>(
				enumOp.getLoc(),
				mlir::rlc::UnknownType::get(rewriter.getContext()),
				f.getBody().front().getArgument(0),
				"value");
		rewriter.create<mlir::rlc::BuiltinAssignOp>(
				enumOp.getLoc(), value, f.getBody().front().getArgument(1));
		rewriter.create<mlir::rlc::Yield>(enumOp.getLoc());
	}

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
				mlir::rlc::FunctionInfoAttr::get(enumOp.getContext(), { argName }),
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
				mlir::rlc::FunctionInfoAttr::get(enumOp.getContext(), { argName }),
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
				enumOp.getLoc(), int64_t(enumOp.countFields() - 1));
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
				mlir::rlc::FunctionInfoAttr::get(enumOp.getContext(), { argName }),
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

	static void emitAsStringLiteralFunction(
			IRRewriter& rewriter, mlir::rlc::EnumDeclarationOp enumOp)
	{
		// Set initial insertion point
		rewriter.setInsertionPoint(enumOp);

		auto argType =
				mlir::rlc::ScalarUseType::get(rewriter.getContext(), enumOp.getName());
		auto returnType = mlir::rlc::StringLiteralType::get(rewriter.getContext());
		auto argName = "self";

		// Create function signature
		auto f = rewriter.create<mlir::rlc::FunctionOp>(
				enumOp.getLoc(),
				"as_string_literal",
				mlir::FunctionType::get(
						rewriter.getContext(), { argType }, { returnType }),

				mlir::rlc::FunctionInfoAttr::get(enumOp.getContext(), { argName }),
				false);

		// Create function block and set insertion point
		auto* bb = rewriter.createBlock(
				&f.getBody(), f.getBody().begin(), { argType }, { enumOp.getLoc() });
		rewriter.setInsertionPointToStart(bb);

		// Iterate over the fields of the enum
		int64_t currentFieldIndex = 0;
		for (auto field :
				 enumOp.getRegion().front().getOps<mlir::rlc::EnumFieldDeclarationOp>())
		{
			// Generate If statement. Is it field.getLoc() or enumOp.getLoc()?
			auto ifStatement =
					rewriter.create<mlir::rlc::IfStatement>(field.getLoc());

			// Generate If confition
			rewriter.createBlock(&ifStatement.getCondition());
			auto selfOp = rewriter.create<mlir::rlc::UnresolvedReference>(
					field.getLoc(),
					mlir::rlc::UnknownType::get(enumOp.getContext()),
					"self");
			auto accessSelf = rewriter.create<mlir::rlc::UnresolvedMemberAccess>(
					field.getLoc(),
					mlir::rlc::UnknownType::get(field.getContext()),
					selfOp,
					"value");
			auto value = rewriter.create<mlir::rlc::Constant>(
					field.getLoc(), currentFieldIndex);
			auto areEqual = rewriter.create<mlir::rlc::EqualOp>(
					field.getLoc(), value, accessSelf);
			rewriter.create<mlir::rlc::Yield>(
					field.getLoc(), mlir::ValueRange({ areEqual }));

			// Generate True branch to be filled with return command
			auto trueBB = rewriter.createBlock(&ifStatement.getTrueBranch());
			rewriter.setInsertionPointToStart(trueBB);

			// Generate return statement with constant string as value
			auto retStm = rewriter.create<mlir::rlc::ReturnStatement>(
					ifStatement.getLoc(), returnType);
			auto* bbReturn =
					rewriter.createBlock(&retStm.getBody(), retStm.getBody().begin());
			rewriter.setInsertionPointToStart(bbReturn);

			// Generate constant return value
			auto stringConstant = rewriter.create<mlir::rlc::StringLiteralOp>(
					field.getLoc(), field.getName().str());
			rewriter.create<mlir::rlc::Yield>(
					enumOp.getLoc(), mlir::ValueRange({ stringConstant }));

			// Generate empty Else branch
			rewriter.createBlock(&ifStatement.getElseBranch());
			rewriter.create<mlir::rlc::Yield>(field.getLoc());

			rewriter.setInsertionPointToEnd(bb);
			currentFieldIndex++;
		}

		// Generate return statement with constant string as value
		auto finalRetStm = rewriter.create<mlir::rlc::ReturnStatement>(
				enumOp.getLoc(), returnType);
		auto* bbFinalReturn = rewriter.createBlock(
				&finalRetStm.getBody(), finalRetStm.getBody().begin());
		rewriter.setInsertionPointToStart(bbFinalReturn);

		// Generate default constant return value: the empty string
		auto defautStringLiteral =
				rewriter.create<mlir::rlc::StringLiteralOp>(enumOp.getLoc(), "");
		// Yield that value
		rewriter.create<mlir::rlc::Yield>(
				enumOp.getLoc(), mlir::ValueRange({ defautStringLiteral }));
	}

	static void emitImplicitEnumFunctions(
			IRRewriter& rewriter, mlir::rlc::EnumDeclarationOp enumOp)
	{
		emitIsEnumMemberFunction(rewriter, enumOp);
		emitMaxMemberFunction(rewriter, enumOp);
		emitAsIntFunction(rewriter, enumOp);
		emitFromIntFunction(rewriter, enumOp);
		emitAsStringLiteralFunction(rewriter, enumOp);
	}

	static void moveAllFunctionDeclsToNewClass(
			mlir::rlc::EnumDeclarationOp declaration,
			mlir::rlc::ClassDeclaration classOp,
			mlir::IRRewriter& rewriter)
	{
		auto classBody = rewriter.createBlock(&classOp.getBody());
		llvm::SmallVector<mlir::Operation*> toMoveOut;
		if (declaration.getBody().empty())
			return;
		for (mlir::Operation& decl : declaration.getBody().front())
			if (not mlir::isa<mlir::rlc::EnumFieldDeclarationOp>(decl))
				toMoveOut.push_back(&decl);

		for (auto decl : toMoveOut)
		{
			decl->remove();
			classBody->push_back(decl);
		}
	}

	// in rlc we allow to write enums like
	// enum A:
	//   b:
	//    Int c = 0
	//   d:
	//    Int c = 1
	//
	//  which is translated to
	//  enum A:
	//    b
	//    d
	//
	//  fun c(A self) -> Int:
	//    if self == A::b:
	//       return 0
	//    if self == A::d:
	//       return 1
	//
	//  this function does this transformation by taking away the
	//  expressions seen in the above code, and places them in
	//  the below code of the example
	static mlir::LogicalResult moveAllMethodExpressionToNewClassFunctions(
			mlir::rlc::EnumDeclarationOp declaration,
			mlir::rlc::ClassDeclaration classOp,
			mlir::IRRewriter& rewriter)
	{
		llvm::StringMap<mlir::rlc::FunctionOp> funs;
		if (declaration.getRegion()
						.front()
						.getOps<mlir::rlc::EnumFieldDeclarationOp>()
						.empty())
		{
			return logError(declaration, "enum must have at least one field");
		}

		auto firstField = *declaration.getRegion()
													 .front()
													 .getOps<mlir::rlc::EnumFieldDeclarationOp>()
													 .begin();

		if (firstField.getRegion().empty())
			return mlir::success();

		for (auto expression : firstField.getRegion()
															 .front()
															 .getOps<mlir::rlc::EnumFieldExpressionOp>())
		{
			rewriter.setInsertionPointToEnd(&classOp.getBody().front());
			auto op = rewriter.create<mlir::rlc::FunctionOp>(
					declaration.getLoc(),
					expression.getName(),
					mlir::FunctionType::get(
							rewriter.getContext(), {}, { expression.getResult().getType() }),

					mlir::rlc::FunctionInfoAttr::get(rewriter.getContext()),
					true);
			rewriter.createBlock(&op.getBody());
			auto retStm = rewriter.create<mlir::rlc::ReturnStatement>(
					op.getLoc(), mlir::rlc::UnknownType::get(op.getContext()));
			mlir::IRMapping mapping;
			rewriter.cloneRegionBefore(
					expression.getBody(),
					retStm.getBody(),
					retStm.getBody().begin(),
					mapping);

			rewriter.createBlock(&op.getPrecondition());
			rewriter.create<mlir::rlc::Yield>(declaration.getLoc());
			funs[expression.getName()] = op;
		}

		int64_t currentFieldIndex = 0;
		for (auto field : declaration.getRegion()
													.front()
													.getOps<mlir::rlc::EnumFieldDeclarationOp>())
		{
			if (field.getRegion().empty())
			{
				return emitError(
						field.getLoc(),
						"Enum field expected to have a enum field declarations");
			}
			std::set<llvm::StringRef> seenNames;
			for (auto expression :
					 field.getRegion().front().getOps<mlir::rlc::EnumFieldExpressionOp>())
			{
				if (seenNames.contains(expression.getName()))
				{
					return emitError(
							expression.getLoc(),
							("Multiple definitions of " + expression.getName() +
							 " in enum field " + field.getName().str()));
				}

				if (!funs.contains(expression.getName()))
				{
					return emitError(
							declaration.getLoc(),
							"Enum field " + declaration.getName() +
									" is missing expression " + expression.getName());
				}

				seenNames.insert(expression.getName());
				rewriter.setInsertionPoint(
						&funs[expression.getName()].getBody().front().back());

				auto ifStatement =
						rewriter.create<mlir::rlc::IfStatement>(expression.getLoc());

				rewriter.createBlock(&ifStatement.getCondition());
				auto selfOp = rewriter.create<mlir::rlc::UnresolvedReference>(
						expression.getLoc(),
						mlir::rlc::UnknownType::get(expression.getContext()),
						"self");

				auto accessSelf = rewriter.create<mlir::rlc::UnresolvedMemberAccess>(
						expression.getLoc(),
						mlir::rlc::UnknownType::get(expression.getContext()),
						selfOp,
						"value");

				auto value = rewriter.create<mlir::rlc::Constant>(
						expression.getLoc(), currentFieldIndex);
				auto areEqual = rewriter.create<mlir::rlc::EqualOp>(
						expression.getLoc(), value, accessSelf);
				rewriter.create<mlir::rlc::Yield>(
						expression.getLoc(), mlir::ValueRange({ areEqual }));
				rewriter.createBlock(&ifStatement.getElseBranch());
				rewriter.create<mlir::rlc::Yield>(expression.getLoc());
				auto trueBB = rewriter.createBlock(&ifStatement.getTrueBranch());
				rewriter.setInsertionPointToStart(trueBB);

				auto retStm = rewriter.create<mlir::rlc::ReturnStatement>(
						ifStatement.getLoc(),
						mlir::rlc::UnknownType::get(ifStatement.getContext()));

				retStm.getBody().takeBody(expression.getBody());
			}
			currentFieldIndex++;
		}

		return mlir::success();
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
			llvm::DenseMap<mlir::rlc::EnumDeclarationOp, mlir::rlc::ClassType>
					enumsToType;
			for (auto declaration : ops)
			{
				rewriter.setInsertionPoint(declaration);
				llvm::SmallVector<mlir::rlc::ClassFieldAttr, 1> fields{
					mlir::rlc::ClassFieldAttr::get(
							"value",
							mlir::rlc::ScalarUseType::get(getOperation().getContext(), "Int"))
				};
				assert(declaration.getTypeLocation().has_value());
				auto shugarType = mlir::rlc::ShugarizedTypeAttr::get(
						declaration.getContext(),
						*declaration.getTypeLocation(),
						fields.back().getType());
				llvm::SmallVector<mlir::rlc::ClassFieldDeclarationAttr, 1> fieldsDecl{
					mlir::rlc::ClassFieldDeclarationAttr::get(
							declaration.getContext(), fields.back(), shugarType)
				};
				auto op = rewriter.create<mlir::rlc::ClassDeclaration>(
						declaration.getLoc(),
						declaration.getNameAttr(),
						fieldsDecl,
						mlir::ArrayRef<mlir::Type>({}),
						*declaration.getTypeLocation());
				moveAllFunctionDeclsToNewClass(declaration, op, rewriter);
				if (moveAllMethodExpressionToNewClassFunctions(
								declaration, op, rewriter)
								.failed())
				{
					signalPassFailure();
					return;
				}
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
				auto fields = enumDeclaration.getBody()
													.front()
													.getOps<mlir::rlc::EnumFieldDeclarationOp>();
				for (auto [i, field] : llvm::enumerate(fields))
				{
					if (field.getName() != use.getEnumValue())
						continue;
					failed = false;

					auto type = mlir::rlc::ClassType::getIdentified(
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

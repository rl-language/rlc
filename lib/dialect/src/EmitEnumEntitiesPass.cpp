#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_EMITENUMENTITIESPASS
#include "rlc/dialect/Passes.inc"

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
				enums[declaration.getName()] = declaration;
			}

			llvm::SmallVector<mlir::rlc::UncheckedEnumUse, 2> uses;
			getOperation().walk(
					[&](mlir::rlc::UncheckedEnumUse op) { uses.push_back(op); });

			for (auto use : uses)
			{
				rewriter.setInsertionPoint(use);
				if (enums.count(use.getEnumName()) == 0)
				{
					use.emitError("enum use does not name a declared enum");
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
				}
				if (failed)
				{
					use.emitError(
							"cannot find member " + use.getEnumValue() + " in enum " +
							use.getEnumName());
					signalPassFailure();
				}
			}
		}
	};
}	 // namespace mlir::rlc

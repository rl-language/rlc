#include <cstddef>
#include <cstdint>
#include "llvm/Support/Casting.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Value.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"
#include "rlc/dialect/Operations.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_ADDOUTOFBOUNDSCHECKPASS
#include "rlc/dialect/Passes.inc"
	struct AddOutOfBoundsCheckPass: public impl::AddOutOfBoundsCheckPassBase<AddOutOfBoundsCheckPass>
	{
		void getDependentDialects(mlir::DialectRegistry& registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}

		void runOnOperation() override
		{
			ModuleOp module = getOperation();

			module->walk([&](mlir::rlc::ArrayAccess op) {
				addOutOfBoundsCheck(op);
			});
		}

		void addOutOfBoundsCheck(mlir::rlc::ArrayAccess &op) {

			// if the accessed object is not an array, do nothing.
			auto array = op.getValue().getType().dyn_cast<mlir::rlc::ArrayType>();
			if(array == nullptr) {
				return;
			}

			mlir::OpBuilder builder(op);
			auto ifStatement = builder.create<mlir::rlc::IfStatement>(op->getLoc());

			// construct the condition
			auto *conditionBlock = builder.createBlock(&ifStatement.getCondition());
			auto arraySizeConst = builder.create<mlir::rlc::Constant>(ifStatement->getLoc(), array.getArraySize());
			auto ge = builder.create<mlir::rlc::GreaterEqualOp>(ifStatement->getLoc(), op.getMemberIndex(), arraySizeConst.getResult());

			auto zero = builder.create<mlir::rlc::Constant>(ifStatement->getLoc(), (int64_t) 0);
			auto lt = builder.create<mlir::rlc::LessOp>(ifStatement->getLoc(), op.getMemberIndex(), zero);

			auto disj = builder.create<OrOp>(ifStatement->getLoc(), ge, lt);
			builder.create<mlir::rlc::Yield>(ifStatement.getLoc(), disj.getResult());


			// construct the true branch
			auto *trueBranch = builder.createBlock(&ifStatement.getTrueBranch());
			builder.create<mlir::rlc::AbortOp>(ifStatement.getLoc());

			// construct the false branch that does nothing
			auto *falseBranch = builder.createBlock(&ifStatement.getElseBranch());
			builder.create<mlir::rlc::Yield>(ifStatement.getLoc());
		}
	};

}	 // namespace mlir::rlc

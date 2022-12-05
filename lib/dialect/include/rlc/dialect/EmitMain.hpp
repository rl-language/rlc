#pragma once
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace rlc
{

	struct EmitMainPass
			: public mlir::
						PassWrapper<EmitMainPass, mlir::OperationPass<mlir::ModuleOp>>
	{
		public:
		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(EmitMainPass)

		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect, mlir::cf::ControlFlowDialect>();
		}
		void runOnOperation() final;

		private:
		mlir::TypeConverter converter;
	};

	inline std::unique_ptr<mlir::Pass> createEmitMainPass()
	{
		return std::make_unique<EmitMainPass>();
	}
}	 // namespace rlc

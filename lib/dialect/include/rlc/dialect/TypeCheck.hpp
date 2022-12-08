#pragma once

#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"

namespace rlc
{
	struct RLCTypeCheck
			: public mlir::
						PassWrapper<RLCTypeCheck, mlir::OperationPass<mlir::ModuleOp>>
	{
		public:
		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(RLCTypeCheck)

		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}
		void runOnOperation() final;
	};

	inline std::unique_ptr<mlir::Pass> createRLCTypeCheck()
	{
		return std::make_unique<RLCTypeCheck>();
	}

}	 // namespace rlc

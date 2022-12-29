#pragma once

#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/python/Dialect.h"

namespace rlc
{

	struct RLCToPython
			: public mlir::
						PassWrapper<RLCToPython, mlir::OperationPass<mlir::ModuleOp>>
	{
		public:
		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(RLCToPython)

		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
			registry.insert<mlir::rlc::python::RLCPython>();
		}
		void runOnOperation() final;
	};

	inline std::unique_ptr<mlir::Pass> createRLCToPython()
	{
		return std::make_unique<RLCToPython>();
	}
}	 // namespace rlc

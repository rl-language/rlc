#pragma once

#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/Dialect.h"

namespace rlc
{
	struct RLCLowerArrayCalls
			: public mlir::
						PassWrapper<RLCLowerArrayCalls, mlir::OperationPass<mlir::ModuleOp>>
	{
		public:
		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(RLCLowerArrayCalls)

		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}
		void runOnOperation() final;
	};

	inline std::unique_ptr<mlir::Pass> createRLCLowerArrayCalls()
	{
		return std::make_unique<RLCLowerArrayCalls>();
	}

	struct RLCToLLVMLoweringPass: public mlir::PassWrapper<
																		RLCToLLVMLoweringPass,
																		mlir::OperationPass<mlir::ModuleOp>>
	{
		public:
		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(RLCToLLVMLoweringPass)

		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}
		void runOnOperation() final;
	};

	inline std::unique_ptr<mlir::Pass> createRLCToLLVMLoweringPass()
	{
		return std::make_unique<RLCToLLVMLoweringPass>();
	}

	struct RLCToCfLoweringPass: public mlir::PassWrapper<
																	RLCToCfLoweringPass,
																	mlir::OperationPass<mlir::ModuleOp>>
	{
		public:
		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(RLCToCfLoweringPass)

		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}
		void runOnOperation() final;
	};

	inline std::unique_ptr<mlir::Pass> createRLCToCfLoweringPass()
	{
		return std::make_unique<RLCToCfLoweringPass>();
	}

	struct RLCLowerActions
			: public mlir::
						PassWrapper<RLCLowerActions, mlir::OperationPass<mlir::ModuleOp>>
	{
		public:
		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(RLCLowerActions)

		void getDependentDialects(mlir::DialectRegistry &registry) const override
		{
			registry.insert<mlir::rlc::RLCDialect>();
		}
		void runOnOperation() final;
	};

	inline std::unique_ptr<mlir::Pass> createRLCLowerActions()
	{
		return std::make_unique<RLCLowerActions>();
	}
}	 // namespace rlc

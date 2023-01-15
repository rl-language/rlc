#pragma once

#include "mlir/IR/Operation.h"
#include "rlc/dialect/Operations.hpp"

namespace mlir::rlc
{

	class IntegerArgumentConstraints
	{
		public:
		using IntType = int64_t;

		void maybeNewMax(IntType newMax) { max = std::min(max, newMax); }
		void maybeNewMin(IntType newMin) { min = std::max(min, newMin); }

		[[nodiscard]] IntType getMin() const { return min; }

		[[nodiscard]] IntType getMax() const { return max; }

		private:
		IntType min = std::numeric_limits<IntType>::min();
		IntType max = std::numeric_limits<IntType>::max();
	};

	class ActionArgumentAnalysis
	{
		public:
		explicit ActionArgumentAnalysis(mlir::Operation* op);

		const IntegerArgumentConstraints& getBoundsOf(mlir::Value arg)
		{
			return contraints[arg];
		}

		private:
		void handle(mlir::rlc::ActionStatement statement);
		void handle(mlir::rlc::ActionFunction statement);
		void handleArgument(mlir::Value argument, mlir::Operation* contraint);
		void handleBinaryOp(
				mlir::Operation* op,
				mlir::Value argument,
				IntegerArgumentConstraints::IntType constant);
		void handleBinaryOp(
				mlir::Operation* op,
				IntegerArgumentConstraints::IntType constant,
				mlir::Value argument);
		mlir::DenseMap<mlir::Value, IntegerArgumentConstraints> contraints;
	};
}	 // namespace mlir::rlc

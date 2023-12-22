#include "rlc/dialect/ActionArgumentAnalysis.hpp"

#include "rlc/dialect/Operations.hpp"

namespace mlir::rlc
{
	void ActionArgumentAnalysis::handleBinaryOp(
			mlir::Operation* op,
			mlir::Value argument,
			IntegerArgumentConstraints::IntType constant)
	{
		if (mlir::isa<mlir::rlc::LessOp>(op))
		{
			contraints[argument].maybeNewMax(constant - 1);
			return;
		}

		if (mlir::isa<mlir::rlc::LessEqualOp>(op))
		{
			contraints[argument].maybeNewMax(constant);
			return;
		}

		if (mlir::isa<mlir::rlc::GreaterOp>(op))
		{
			contraints[argument].maybeNewMin(constant + 1);
			return;
		}

		if (mlir::isa<mlir::rlc::GreaterEqualOp>(op))
		{
			contraints[argument].maybeNewMin(constant);
			return;
		}

		if (mlir::isa<mlir::rlc::EqualOp>(op))
		{
			contraints[argument].maybeNewMin(constant);
			contraints[argument].maybeNewMax(constant);
			return;
		}
	}

	void ActionArgumentAnalysis::handleBinaryOp(
			mlir::Operation* op,
			IntegerArgumentConstraints::IntType constant,
			mlir::Value argument)
	{
		if (mlir::isa<mlir::rlc::LessOp>(op))
		{
			contraints[argument].maybeNewMin(constant + 1);
			return;
		}

		if (mlir::isa<mlir::rlc::LessEqualOp>(op))
		{
			contraints[argument].maybeNewMin(constant);
			return;
		}

		if (mlir::isa<mlir::rlc::GreaterOp>(op))
		{
			contraints[argument].maybeNewMax(constant - 1);
			return;
		}

		if (mlir::isa<mlir::rlc::GreaterEqualOp>(op))
		{
			contraints[argument].maybeNewMax(constant);
			return;
		}

		if (mlir::isa<mlir::rlc::EqualOp>(op))
		{
			contraints[argument].maybeNewMin(constant);
			contraints[argument].maybeNewMax(constant);
			return;
		}
	}

	void ActionArgumentAnalysis::handleArgument(
			mlir::Value argument, mlir::Operation* contraint)
	{
		if (not argument.getType().isa<mlir::rlc::IntegerType>())
			return;

		if (contraint->getOperands().size() != 2)
			return;

		if (contraint->getOperand(0) == argument and
				contraint->getOperand(1).getDefiningOp() and
				mlir::isa<mlir::rlc::Constant>(
						contraint->getOperand(1).getDefiningOp()))
		{
			handleBinaryOp(
					contraint,
					argument,
					contraint->getOperand(1)
							.getDefiningOp<mlir::rlc::Constant>()
							.getValue()
							.cast<mlir::IntegerAttr>()
							.getInt());
			return;
		}

		if (contraint->getOperand(1) == argument and
				contraint->getOperand(0).getDefiningOp() and
				mlir::isa<mlir::rlc::Constant>(
						contraint->getOperand(0).getDefiningOp()))
		{
			handleBinaryOp(
					contraint,
					contraint->getOperand(0)
							.getDefiningOp<mlir::rlc::Constant>()
							.getValue()
							.cast<mlir::IntegerAttr>()
							.getInt(),
					argument);
			return;
		}
	}

	void ActionArgumentAnalysis::handle(mlir::rlc::ActionFunction statement)
	{
		for (auto& arg : statement.getBody().front().getArguments())
		{
			for (auto& use : arg.getUses())
			{
				if (use.getOwner()->getParentRegion() != &statement.getPrecondition())
					continue;
				handleArgument(arg, use.getOwner());
			}
		}
	}

	void ActionArgumentAnalysis::handle(mlir::rlc::ActionStatement statement)
	{
		for (auto arg : statement.getPrecondition().getArguments())
		{
			for (auto& use : arg.getUses())
			{
				if (use.getOwner()->getParentRegion() != &statement.getPrecondition())
					continue;
				handleArgument(arg, use.getOwner());
			}
		}
	}

	ActionArgumentAnalysis::ActionArgumentAnalysis(mlir::Operation* op)
	{
		assert(
				mlir::isa<mlir::rlc::ActionFunction>(op) or
				mlir::isa<mlir::rlc::ActionStatement>(op));

		if (auto casted = mlir::dyn_cast<mlir::rlc::ActionStatement>(op))
		{
			handle(casted);
		}

		if (auto casted = mlir::dyn_cast<mlir::rlc::ActionFunction>(op))
		{
			handle(casted);
		}
	}
}	 // namespace mlir::rlc

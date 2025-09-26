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
		if (not mlir::isa<mlir::rlc::IntegerType>(argument.getType()))
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
					mlir::cast<mlir::IntegerAttr>(
							contraint->getOperand(1)
									.getDefiningOp<mlir::rlc::Constant>()
									.getValue())
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
					mlir::cast<mlir::IntegerAttr>(
							contraint->getOperand(0)
									.getDefiningOp<mlir::rlc::Constant>()
									.getValue())
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
				auto* firstNonAndParent = use.getOwner()->getParentOp();
				while (mlir::dyn_cast<mlir::rlc::ShortCircuitingAnd>(firstNonAndParent))
					firstNonAndParent = firstNonAndParent->getParentOp();

				if (firstNonAndParent != statement)
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
				auto* firstNonAndParent = use.getOwner()->getParentOp();
				while (mlir::dyn_cast<mlir::rlc::ShortCircuitingAnd>(firstNonAndParent))
					firstNonAndParent = firstNonAndParent->getParentOp();

				if (firstNonAndParent != statement)
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

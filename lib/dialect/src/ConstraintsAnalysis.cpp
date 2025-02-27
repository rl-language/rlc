/*
Copyright 2024 Matteo Cenzato

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
#include "rlc/dialect/ConstraintsAnalysis.hpp"

//////////////////////////////////////////////////////////////////////////////
// USEFUL STATIC METHODS
//////////////////////////////////////////////////////////////////////////////

// Useful wrapper method
static bool isPotentialRelationalOp(mlir::Operation* op)
{
	return mlir::isa<mlir::rlc::GreaterOp>(op) or
				 mlir::isa<mlir::rlc::GreaterEqualOp>(op) or
				 mlir::isa<mlir::rlc::LessOp>(op) or
				 mlir::isa<mlir::rlc::LessEqualOp>(op) or
				 mlir::isa<mlir::rlc::EqualOp>(op) or
				 mlir::isa<mlir::rlc::NotEqualOp>(op) or
				 mlir::isa<mlir::rlc::UninitializedConstruct>(op);
}

// Method that peeks the lattice for the ranges and updates the "result" value
// accordingly based on the arithmetic operation
static bool calculateNewRangeAndUpdate(
		mlir::Operation* op,
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::rlc::ConstraintsAnalysis* analysis,
		const mlir::Value& result,
		const mlir::Value& update,
		const mlir::Value& other,
		bool branch)
{
	if (not currentLattice->isValueNotTopOrBottom(result, branch))
		return false;

	auto result_range = currentLattice->getRange(result, branch);

	mlir::rlc::ConstraintsLattice::IntegerRange new_range(
			mlir::rlc::ConstraintsLattice::APInteger(1),
			mlir::rlc::ConstraintsLattice::APInteger(0));

	// If the other is a constant take its constant value
	if (auto other_casted =
					mlir::dyn_cast<mlir::rlc::Constant>(other.getDefiningOp()))
	{
		// Kinda ugly but this is what we have to do with constant ranges
		if (auto other_const =
						other_casted->getAttr("value").dyn_cast<mlir::IntegerAttr>())
		{
			new_range = currentLattice->createRange(other_const.getInt());
		}
		else
			return false;
	}
	// Else check if the value carried by the operation is
	else
	{
		// If I have altready a range then i can modify it
		if (not currentLattice->isValueNotBottom(other, branch))
			return false;
		// NB: this can become TOP
		new_range = currentLattice->getRange(other, branch);
	}
	// Update only if I found the new range
	if (new_range.getLower().sle(new_range.getUpper()))
	{
		llvm::TypeSwitch<mlir::Operation*>(op).Case<mlir::rlc::AddOp>(
				[&](mlir::rlc::AddOp addop) {
					new_range = result_range.subWithNoWrap(
							new_range, 0, llvm::ConstantRange::PreferredRangeType::Signed);
				});
		// Insert the range where is needed
		return currentLattice->insertOrUpdate({ update, branch }, new_range);
	}
	// Else case just do nothing
	return false;
}

// Method that given an arithmetic operation finds the operand to update (lhs or
// rhs) and updates it accordingly
static bool findRelevantOperandAndUpdate(
		mlir::Operation* op,
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::rlc::ConstraintsAnalysis* analysis,
		const mlir::Value& result)
{
	// Keep operand aliases for clarity
	const auto& op1 = op->getOperand(0);
	const auto& op2 = op->getOperand(1);
	const auto& op1_defop = op1.getDefiningOp();
	const auto& op2_defop = op2.getDefiningOp();

	// Understand the operand that I have to update
	mlir::Value update = nullptr;
	mlir::Value other = nullptr;

	// Check which operand should be updated (the other should be a constant)
	if (op1_defop == nullptr or not mlir::isa<mlir::rlc::Constant>(op1_defop))
	{
		update = op1;
		other = op2;
	}
	else if (
			op2_defop == nullptr or not mlir::isa<mlir::rlc::Constant>(op2_defop))
	{
		update = op2;
		other = op1;
	}
	else
		return false;

	bool ret1 = calculateNewRangeAndUpdate(
			op, currentLattice, analysis, result, update, other, true);
	bool ret2 = calculateNewRangeAndUpdate(
			op, currentLattice, analysis, result, update, other, false);

	return ret1 or ret2;
}

// Method for returning the branches values for the rlc.crb operation
static const std::tuple<bool, bool, bool, bool> GetBranchesCondBranch(
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::Operation* statement,
		mlir::rlc::ConstraintsAnalysis* analysis,
		const mlir::Value& value)
{
	auto casted = mlir::dyn_cast<mlir::rlc::CondBranch>(statement);

	// Look at the next lattice in the true branch
	auto trueLattice =
			analysis->getObjLattice(&(casted.getTrueBranch()->front()));

	// Understand if there is a value or a constant (useful for corner cases
	// after)
	auto branch_true_has_value = trueLattice->isValuePresentInLattice(value);
	auto branch_true_has_const = trueLattice->isConstPresentInLattice();

	auto btyt = trueLattice->isValueOfTypePresentInLattice(true);
	auto btyf = trueLattice->isValueOfTypePresentInLattice(false);

	// And do the same in the false
	auto falseLattice =
			analysis->getObjLattice(&(casted.getFalseBranch()->front()));

	auto branch_false_has_value = falseLattice->isValuePresentInLattice(value);
	auto branch_false_has_const = falseLattice->isConstPresentInLattice();

	auto bfyt = falseLattice->isValueOfTypePresentInLattice(true);
	auto bfyf = falseLattice->isValueOfTypePresentInLattice(false);

	// Corner cases
	if (branch_false_has_const.first and branch_true_has_value.first and
			not branch_false_has_value.first)
	{
		currentLattice->insertOrJoin({ value, true }, currentLattice->getTOP());
	}

	if (branch_false_has_const.second and branch_true_has_value.second and
			not branch_false_has_value.second)
	{
		currentLattice->insertOrJoin({ value, false }, currentLattice->getTOP());
	}

	return std::make_tuple(btyt, btyf, bfyt, bfyf);
}

// Method to retrieve the result range from a relational operation and insert it
// in the lattice
static bool getRangeOfRelationalOperation(
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::Operation* statement,
		bool result,
		const mlir::Value& to_update,
		const mlir::Operation* other,
		const bool branch_to_update)
{
	auto other_casted = mlir::dyn_cast<mlir::rlc::Constant>(other);

	auto rhs_const = 0ULL;

	// Operate only if constant is an integer
	if (auto rhs_integer =
					other_casted->getAttr("value").dyn_cast<mlir::IntegerAttr>())
		rhs_const = rhs_integer.getInt();
	else
		return false;

	std::optional<mlir::rlc::ConstraintsLattice::Integer> min;
	std::optional<mlir::rlc::ConstraintsLattice::Integer> max;

	llvm::TypeSwitch<mlir::Operation*>(statement)
			.Case<mlir::rlc::GreaterOp>([&](mlir::rlc::GreaterOp op) {
				if (result)
					min = rhs_const + 1;
				else
					max = rhs_const;
			})
			.Case<mlir::rlc::LessOp>([&](mlir::rlc::LessOp op) {
				if (result)
					max = rhs_const - 1;
				else
					min = rhs_const;
			})
			.Case<mlir::rlc::GreaterEqualOp>([&](mlir::rlc::GreaterEqualOp op) {
				if (result)
					min = rhs_const;
				else
					max = rhs_const - 1;
			})
			.Case<mlir::rlc::LessEqualOp>([&](mlir::rlc::LessEqualOp op) {
				if (result)
					max = rhs_const;
				else
					min = rhs_const + 1;
			})
			.Case<mlir::rlc::EqualOp>([&](mlir::rlc::EqualOp op) {
				if (result)
				{
					max = rhs_const;
					min = rhs_const;
				}
				else
				{
					max = mlir::rlc::ConstraintsLattice::MAX;
					min = mlir::rlc::ConstraintsLattice::MIN;
				}
			})
			.Case<mlir::rlc::NotEqualOp>([&](mlir::rlc::NotEqualOp op) {
				if (result)
				{
					max = mlir::rlc::ConstraintsLattice::MAX;
					min = mlir::rlc::ConstraintsLattice::MIN;
				}
				else
				{
					max = rhs_const;
					min = rhs_const;
				}
			});

	// Insert the range where is needed
	return currentLattice->insertOrUpdate(
			{ to_update, branch_to_update }, min, max);
}

// Method called when visiting the yield operation
static bool visitYield(
		mlir::Operation* statement, mlir::rlc::ConstraintsLattice* currentLattice)
{
	mlir::Operation* conditional_op = nullptr;

	llvm::TypeSwitch<mlir::Operation*>(statement)
			.Case<mlir::rlc::Yield>([&](mlir::rlc::Yield op) {
				conditional_op = op.getOperand(0).getDefiningOp();
			})
			.Case<mlir::rlc::BuiltinAssignOp>([&](mlir::rlc::BuiltinAssignOp op) {
				conditional_op = op.getOperand(1).getDefiningOp();
			});

	// Do the checking only if the operation is a relational
	if (not isPotentialRelationalOp(conditional_op))
		return false;

	// Aliases
	const auto& lhs = conditional_op->getOperand(0);
	const auto& rhs = conditional_op->getOperand(1);
	const auto& lhs_op = lhs.getDefiningOp();
	const auto& rhs_op = rhs.getDefiningOp();

	auto to_return = false;

	// If the rhs is a constant
	if (rhs_op != nullptr)
	{	 // Corner case because if rhs is a blockargument this is null
		if (auto rhs_casted = mlir::dyn_cast<mlir::rlc::Constant>(rhs_op))
		{
			to_return = getRangeOfRelationalOperation(
					currentLattice, conditional_op, true, lhs, rhs_casted, true);
			to_return =
					getRangeOfRelationalOperation(
							currentLattice, conditional_op, false, lhs, rhs_casted, false) or
					to_return;

			return to_return;
		}
	}
	// else do the same but with inverted branches
	else if (lhs_op != nullptr)
	{
		if (auto lhs_casted = mlir::dyn_cast<mlir::rlc::Constant>(lhs_op))
		{
			to_return = getRangeOfRelationalOperation(
					currentLattice, conditional_op, false, rhs, lhs_casted, true);
			to_return =
					getRangeOfRelationalOperation(
							currentLattice, conditional_op, true, rhs, lhs_casted, false) or
					to_return;

			return to_return;
		}
	}
	// else do nothing
	return false;
}

// Operation called when visiting a region terminator
static bool setReturnValueInLattice(
		mlir::Operation* branchOp, mlir::rlc::ConstraintsLattice* currentLattice)
{
	const auto& value = mlir::cast<mlir::rlc::Yield>(branchOp).getArguments()[0];

	// Case when I am directly returning an operation
	if (isPotentialRelationalOp(value.getDefiningOp()))
		return visitYield(branchOp, currentLattice);

	if (not mlir::isa<mlir::rlc::Constant>(value.getDefiningOp()))
		return false;

	// This should work because at the start of the analysis we already check
	// that the function should return bool
	auto bool_branch =
			mlir::dyn_cast<mlir::BoolAttr>(value.getDefiningOp()->getAttr("value"));
	if (not bool_branch)
		return false;

	return currentLattice->insertOrJoin(
			{ value, bool_branch.getValue() }, currentLattice->getTOP());
}
//////////////////////////////////////////////////////////////////////////////
// COMMUTATIVE OPERATION
//////////////////////////////////////////////////////////////////////////////

bool mlir::rlc::AddOp::constraintsAnalyze(
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::rlc::ConstraintsAnalysis* analysis)
{
	// Aliases for more clarity
	const auto& result = this->getOperation()->getResult(0);
	const auto& op1 = this->getOperation()->getOperand(0);
	const auto& op2 = this->getOperation()->getOperand(1);

	// Do nothing if the lhs of the operation is not found in the lattice
	if (not currentLattice->foundInRanges({ result, true }) and
			not currentLattice->foundInRanges({ result, false }))
		return false;

	// Call the interface operation which updates the lattice accordingly
	return findRelevantOperandAndUpdate(
			this->getOperation(), currentLattice, analysis, result);
}

//////////////////////////////////////////////////////////////////////////////
// YIELD OPERATION
//////////////////////////////////////////////////////////////////////////////

bool mlir::rlc::Yield::constraintsAnalyze(
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::rlc::ConstraintsAnalysis* analysis)
{
	// Just return the lattice method, as rlc.builtin_assign will need the same
	// call too
	return visitYield(this->getOperation(), currentLattice);
}

//////////////////////////////////////////////////////////////////////////////
// CONDBRANCH OPERATION
//////////////////////////////////////////////////////////////////////////////

bool mlir::rlc::CondBranch::constraintsAnalyze(
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::rlc::ConstraintsAnalysis* analysis)
{
	auto casted = mlir::dyn_cast<mlir::rlc::CondBranch>(this->getOperation());

	mlir::Operation* conditional_op;

	// Do the checking only if the operation is a relational
	if (not isPotentialRelationalOp(casted.getCond().getDefiningOp()))
		return false;

	// Unfortunately it is ugly but needed
	if (auto uninit_construct = mlir::dyn_cast<mlir::rlc::UninitializedConstruct>(
					casted.getCond().getDefiningOp()))
	{
		auto opIter = casted.getCond().getUses().begin();

		// Increment the iterator to the second operation
		++opIter;

		conditional_op = opIter->getOwner()->getOperand(1).getDefiningOp();
	}
	// else it is a relational operation
	else
		conditional_op = casted.getCond().getDefiningOp();

	// Aliases
	const auto& lhs = conditional_op->getOperand(0);
	const auto& rhs = conditional_op->getOperand(1);
	const auto& lhs_op = lhs.getDefiningOp();
	const auto& rhs_op = rhs.getDefiningOp();

	// Branch T/F yield T/F
	bool btyt = false;
	bool btyf = false;
	bool bfyt = false;
	bool bfyf = false;

	auto to_return = false;

	if (auto rhs_casted = mlir::dyn_cast<mlir::rlc::Constant>(rhs_op))
	{
		// Here we use the assumption that a relational operator has always two
		// successors

		std::tie(btyt, btyf, bfyt, bfyf) = GetBranchesCondBranch(
				currentLattice, this->getOperation(), analysis, lhs);

		// Modify my ranges only when i am sure that i am passing through a
		// initialized (true/false) branch
		if (not btyt and not btyf and not bfyt and not bfyf)
			return false;

		// If both branches yield true then there is no need to update since no more
		// information can be retrieved
		if (not btyt == bfyt)
		{
			if (btyt)
				to_return =
						getRangeOfRelationalOperation(
								currentLattice, conditional_op, true, lhs, rhs_casted, true) or
						to_return;

			if (bfyt)
				to_return =
						getRangeOfRelationalOperation(
								currentLattice, conditional_op, false, lhs, rhs_casted, true) or
						to_return;
		}

		if (not btyf == bfyf)
		{
			if (btyf)
				to_return =
						getRangeOfRelationalOperation(
								currentLattice, conditional_op, true, lhs, rhs_casted, false) or
						to_return;

			if (bfyf)
				to_return = getRangeOfRelationalOperation(
												currentLattice,
												conditional_op,
												false,
												lhs,
												rhs_casted,
												false) or
										to_return;
		}
	}
	// If the lhs is a constant then do the same but with inverted branches
	else if (auto lhs_casted = mlir::dyn_cast<mlir::rlc::Constant>(lhs_op))
	{
		std::tie(btyt, btyf, bfyt, bfyf) = GetBranchesCondBranch(
				currentLattice, this->getOperation(), analysis, rhs);

		if (not btyt and not btyf and not bfyt and not bfyf)
			return false;

		if (not btyt == bfyt)
		{
			if (btyt)
				to_return =
						getRangeOfRelationalOperation(
								currentLattice, conditional_op, false, rhs, lhs_casted, true) or
						to_return;

			if (bfyt)
				to_return = getRangeOfRelationalOperation(
												currentLattice,
												conditional_op,
												false,
												rhs,
												lhs_casted,
												false) or
										to_return;
		}

		if (not btyf == bfyf)
		{
			if (btyf)
				to_return =
						getRangeOfRelationalOperation(
								currentLattice, conditional_op, true, rhs, lhs_casted, true) or
						to_return;

			if (bfyf)
				to_return =
						getRangeOfRelationalOperation(
								currentLattice, conditional_op, true, rhs, lhs_casted, false) or
						to_return;
		}
	}
	// else do nothing

	// Return if any of the operations modified the lattice
	return to_return;
}

//////////////////////////////////////////////////////////////////////////////
// BUILTIN_ASSIGN OPERATION
//////////////////////////////////////////////////////////////////////////////

bool mlir::rlc::BuiltinAssignOp::constraintsAnalyze(
		mlir::rlc::ConstraintsLattice* currentLattice,
		mlir::rlc::ConstraintsAnalysis* analysis)
{
	auto casted =
			mlir::dyn_cast<mlir::rlc::BuiltinAssignOp>(this->getOperation());

	// Interesting case is with the rhs being another variable
	if (casted->getOperand(1).getDefiningOp() == nullptr)
	{
		return false;
	}

	// When i have statements such as 'a=a+1' I can check them with builtin
	// assigns
	if (auto rhs_casted = mlir::dyn_cast<mlir::rlc::AddOp>(
					casted->getOperand(1).getDefiningOp()))
	{
		return findRelevantOperandAndUpdate(
				casted->getOperand(1).getDefiningOp(),
				currentLattice,
				analysis,
				casted->getOperand(0));
	}

	return false;
}

namespace mlir::rlc
{

	//////////////////////////////////////////////////////////////////////////////
	// ConstraintsLattice METHODS
	//////////////////////////////////////////////////////////////////////////////

	// Method for returning the lattice (useful only at the end) <- maybe can do
	// everything in the class
	const llvm::DenseMap<mlir::rlc::CA_Value, ConstraintsLattice::IntegerRange>&
	ConstraintsLattice::getUnderlyingLattice() const
	{
		return this->ranges;
	}

	// This is be the opposite of a join, but for the analysis we can treat it as
	// a join
	mlir::ChangeResult ConstraintsLattice::meet(
			const mlir::dataflow::AbstractDenseLattice& val)
	{
		return this->join(val);
	}

	// Simple join operation
	mlir::ChangeResult ConstraintsLattice::join(
			const mlir::dataflow::AbstractDenseLattice& val)
	{
		// Need to cast it
		const auto& other = *static_cast<const ConstraintsLattice*>(&val);

		// Keep it to check if there is no change
		auto copy = *this;

		// Insert the other in this
		for (const auto& other_range : other.ranges)
		{
			this->insertOrJoin(other_range.first, other_range.second);
		}

		if (*this == copy and other == copy)
		{
			return mlir::ChangeResult::NoChange;
		}
		return mlir::ChangeResult::Change;
	}

	// Operation to deep copy two lattices
	mlir::ChangeResult ConstraintsLattice::copy(const ConstraintsLattice& other)
	{
		if (*this == other)
		{
			return mlir::ChangeResult::NoChange;
		}

		// Insert the other in this
		for (const auto& other_range : other.ranges)
		{
			// We have to insert the other range
			// If we find it then we have to join, else we have to insert it
			this->insertOrUpdate(other_range.first, other_range.second);
		}

		return mlir::ChangeResult::Change;
	}

	// Overload of copy method
	mlir::ChangeResult ConstraintsLattice::copy(
			const mlir::dataflow::AbstractDenseLattice& val)
	{
		return this->copy(*static_cast<const ConstraintsLattice*>(&val));
	}

	// Printing current lattice (only for debugging)
	void ConstraintsLattice::print(raw_ostream& os) const
	{
		os << "Ranges:\n";
		if (not this->ranges.empty())
		{
			for (auto range : this->ranges)
			{
				os << "SSA: " << range.first.value
					 << " Branch: " << range.first.return_type << " RANGE: ("
					 << range.second.getLower().getSExtValue() << ", "
					 << range.second.getUpper().getSExtValue() << ")\n";
			}
		}
		else
		{
			os << "Empty\n";
		}
	}

	// Equality operator
	bool ConstraintsLattice::operator==(const ConstraintsLattice& other) const
	{
		return this->ranges == other.ranges;
	}

	// Useful method
	bool ConstraintsLattice::isValueNotTopOrBottom(
			const mlir::Value& value, bool branch) const
	{
		auto found = this->ranges.find({ value, branch });

		return found != this->ranges.end() and found->second != this->getTOP();
	}

	// Useful method
	bool ConstraintsLattice::isValueNotBottom(
			const mlir::Value& value, bool branch) const
	{
		auto found = this->ranges.find({ value, branch });

		return found != this->ranges.end();
	}

	// Useful method
	const ConstraintsLattice::IntegerRange& ConstraintsLattice::getRange(
			const mlir::Value& value, bool branch) const
	{
		return this->ranges.find({ value, branch })->second;
	}

	// Useful method for checking which branches(T/F) are saved in the current
	// lattice (rlc.constant or even a value)
	const std::pair<bool, bool> ConstraintsLattice::branchesContainedInLattice()
			const
	{
		bool can_true = false;
		bool can_false = false;

		for (auto pair : this->ranges)
		{
			if (pair.first.return_type)
				can_true = true;
			else
				can_false = true;
		}

		return std::make_pair(can_true, can_false);
	}

	// Method for checking if the value in input is contained in both branches in
	// the lattice
	const std::pair<bool, bool> ConstraintsLattice::isValuePresentInLattice(
			const mlir::Value& value) const
	{
		bool can_true = false;
		bool can_false = false;

		for (auto pair : this->ranges)
		{
			if (pair.first.value == value)
			{
				if (pair.first.return_type)
					can_true = true;
				else
					can_false = true;
			}
		}

		return std::make_pair(can_true, can_false);
	}

	// Method for checking if there is a constant in the lattice
	const std::pair<bool, bool> ConstraintsLattice::isConstPresentInLattice()
			const
	{
		bool can_true = false;
		bool can_false = false;

		for (auto pair : this->ranges)
		{
			// Sanity check as the DefiningOp of function arguments is nullptr
			if (not(pair.first.value.getDefiningOp() == nullptr) and
					mlir::isa<mlir::rlc::Constant>(pair.first.value.getDefiningOp()))
			{
				if (pair.first.return_type)
					can_true = true;
				else
					can_false = true;
			}
		}

		return std::make_pair(can_true, can_false);
	}

	// Useful case for branch checking
	bool ConstraintsLattice::isValueOfTypePresentInLattice(bool type) const
	{
		for (auto pair : this->ranges)
		{
			if (pair.first.return_type == type)
				return true;
		}
		return false;
	}

	// Useful method
	bool ConstraintsLattice::rangesEmpty() const { return this->ranges.empty(); }

	// Useful method
	bool ConstraintsLattice::foundInRanges(const mlir::rlc::CA_Value& key) const
	{
		return this->ranges.find(key) != this->ranges.end();
	}

	// Useful method
	ConstraintsLattice::IntegerRange ConstraintsLattice::getTOP() const
	{
		return createRange(MIN, MAX);
	}

	// Method for either inserting a value or joining the existing one
	bool ConstraintsLattice::insertOrJoin(
			const mlir::rlc::CA_Value& key,
			const ConstraintsLattice::IntegerRange& other)
	{
		auto found = this->ranges.find(key);
		if (found != this->ranges.end())
		{
			auto copy = found->second;
			found->second = ConstraintsLattice::joinRange(found->second, other);
			return copy != found->second;
		}
		else
		{
			this->ranges.insert(std::make_pair(key, other));
			return true;
		}
	}

	// Method for either inserting a value or meeting the existing one
	bool ConstraintsLattice::insertOrMeet(
			const mlir::rlc::CA_Value& key,
			const ConstraintsLattice::IntegerRange& other)
	{
		auto found = this->ranges.find(key);
		if (found != this->ranges.end())
		{
			auto copy = found->second;
			found->second = ConstraintsLattice::meetRange(found->second, other);
			return copy != found->second;
		}
		else
		{
			this->ranges.insert(std::make_pair(key, other));
			return true;
		}
	}

	// Method for either inserting a value or updating the existing one
	bool ConstraintsLattice::insertOrUpdate(
			const mlir::rlc::CA_Value& key,
			const ConstraintsLattice::IntegerRange& other)
	{
		auto found = this->ranges.find(key);
		if (found != this->ranges.end())
		{
			auto copy = found->second;
			found->second = other;
			return copy != found->second;
		}
		else
		{
			this->ranges.insert(std::make_pair(key, other));
			return true;
		}
	}

	// Method for either inserting a value or joining the existing one (overload)
	bool ConstraintsLattice::insertOrUpdate(
			const mlir::rlc::CA_Value& keyToInsert,
			const std::optional<Integer>& min,
			const std::optional<Integer>& max)
	{
		auto found = this->ranges.find(keyToInsert);
		if (found != this->ranges.end())
		{
			auto copy = found->second;
			found->second = ConstraintsLattice::updateRange(found->second, min, max);
			return copy != found->second;
		}
		else
		{
			this->ranges.insert(std::make_pair(
					keyToInsert, createRange(min.value_or(MIN), max.value_or(MAX))));
			return true;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// ConstraintsAnalysis METHODS
	//////////////////////////////////////////////////////////////////////////////

	void ConstraintsAnalysis::visitOperation(
			mlir::Operation* op,
			const ConstraintsLattice& after,
			ConstraintsLattice* before)
	{
		if (auto casted = mlir::dyn_cast<mlir::rlc::ConstraintsAnalyzable>(op))
		{
			auto res = mlir::ChangeResult::NoChange;

			if (mlir::isa<mlir::rlc::CondBranch>(casted) or
					mlir::isa<mlir::rlc::Yield>(casted))
				res = before->join(after);
			else
				res = before->copy(after);

			res = casted.constraintsAnalyze(before, this) == true
								? mlir::ChangeResult::Change
								: res;
			propagateIfChanged(before, res);
		}
		else
		{
			propagateIfChanged(before, before->copy(after));
		}
	}

	void ConstraintsAnalysis::visitCallControlFlowTransfer(
			CallOpInterface call,
			mlir::dataflow::CallControlFlowAction action,
			const ConstraintsLattice& before,
			ConstraintsLattice* after)
	{
		propagateIfChanged(after, after->copy(before));
	}

	void ConstraintsAnalysis::visitRegionBranchControlFlowTransfer(
			mlir::RegionBranchOpInterface branch,
			mlir::RegionBranchPoint regionFrom,
			mlir::RegionBranchPoint regionTo,
			const ConstraintsLattice& before,
			ConstraintsLattice* after)
	{
		auto res = setReturnValueInLattice(branch, after)
									 ? mlir::ChangeResult::Change
									 : mlir::ChangeResult::NoChange;

		propagateIfChanged(after, res);
	}

	void ConstraintsAnalysis::setToExitState(ConstraintsLattice* lattice) {}

	// Useful debugging method
	void ConstraintsAnalysis::printRanges(mlir::Operation* op)
	{
		auto* lattice = op != nullptr ? getLattice(mlir::ProgramPoint(op))
																	: getLattice(op->getBlock());
		lattice->print(llvm::outs());
	}

	// Useful method
	const llvm::DenseMap<
			mlir::rlc::CA_Value,
			mlir::rlc::ConstraintsLattice::IntegerRange>&
	ConstraintsAnalysis::getUnderlyingLattice(mlir::Operation* op)
	{
		return this->getLattice(mlir::ProgramPoint(op))->getUnderlyingLattice();
	}

	// Useful method
	const mlir::rlc::ConstraintsLattice* ConstraintsAnalysis::getObjLattice(
			mlir::Operation* op)
	{
		return this->getLattice(mlir::ProgramPoint(op));
	}

	// Class which starts the analysis
	class Constraints
	{
		public:
		explicit Constraints(mlir::rlc::FlatFunctionOp fun): function(fun), config()
		{
			config.setInterprocedural(false);
			solver = std::make_unique<DataFlowSolver>(config);
			solver->load<mlir::dataflow::DeadCodeAnalysis>();
			solver->load<mlir::dataflow::SparseConstantPropagation>();
			analysis = solver->load<ConstraintsAnalysis>(symbolTable);
			auto top = fun;
			auto res = solver->initializeAndRun(top);
		}

		// Useful debugging method
		void printRangesFound(mlir::Operation* op)
		{
			this->analysis->printRanges(op);
		}

		// Useful method
		const llvm::DenseMap<
				mlir::rlc::CA_Value,
				mlir::rlc::ConstraintsLattice::IntegerRange>&
		getOperationLattice(mlir::Operation* op)
		{
			return this->analysis->getUnderlyingLattice(op);
		}

		private:
		mlir::SymbolTableCollection symbolTable;
		mlir::rlc::FlatFunctionOp function;
		DataFlowConfig config;
		std::unique_ptr<DataFlowSolver> solver;
		ConstraintsAnalysis* analysis;
	};

#define GEN_PASS_DEF_CONSTRAINTSANALYSISPASS
#include "rlc/dialect/Passes.inc"

	struct ConstraintsAnalysisPass
			: impl::ConstraintsAnalysisPassBase<ConstraintsAnalysisPass>
	{
		using impl::ConstraintsAnalysisPassBase<
				ConstraintsAnalysisPass>::ConstraintsAnalysisPassBase;

		void runOnOperation() override
		{
			// Perform the analysis only if the function returns a boolean
			for (auto fun : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
			{
				if (fun.getResult().getType().getNumResults() == 0)
					continue;
				// If function does not return a boolean do nothing
				if (not mlir::isa<mlir::rlc::BoolType>(
								fun.getResult().getType().getResult(0)))
					continue;

				// Moreover if the function has no arguments or are all not integer do
				// nothing
				if (fun.getRegion(0).front().getNumArguments() == 0)
					continue;

				if (fun.getNumRegions() == 0 or fun.getRegion(0).getNumArguments() == 0)
					continue;

				bool found = false;
				for (auto arg : fun.getRegion(0).front().getArguments())
				{
					if (not mlir::isa<mlir::rlc::IntegerType>(arg.getType()))
						found = true;
				}

				if (found)
					continue;

				// Corner case
				if (fun.getBlocks().size() <= 2)
					continue;

				// Call the analysis on the function
				Constraints con(fun);

				// Here I append to the function the ranges I found
				const auto& block =
						fun.getRegion(0).front().getNextNode()->getNextNode();

				auto lattice = con.getOperationLattice(&block->front());

				// Iterate through the arguments -> are the arguments of the first block
				for (auto arg : fun.getRegion(0).front().getArguments())
				{
					auto pair_true = lattice.find({ arg, true });
					auto pair_false = lattice.find({ arg, false });

					// Insert the range as attribute of function
					if (pair_true != lattice.end())
					{
						fun->setAttr(
								mlir::StringAttr::get(
										fun->getContext(),

										fun.getInfo().getArguments()[arg.getArgNumber()].getName() +
												"_T"),
								mlir::StringAttr::get(
										fun->getContext(),
										"[" +
												std::to_string(
														pair_true->second.getLower().getSExtValue()) +
												"," +
												std::to_string(
														pair_true->second.getUpper().getSExtValue()) +
												")"));
					}
					if (pair_false != lattice.end())
					{
						fun->setAttr(
								mlir::StringAttr::get(
										fun->getContext(),
										fun.getArgNames()[arg.getArgNumber()] + "_F"),
								mlir::StringAttr::get(
										fun->getContext(),
										"[" +
												std::to_string(
														pair_false->second.getLower().getSExtValue()) +
												"," +
												std::to_string(
														pair_false->second.getUpper().getSExtValue()) +
												")"));
					}
				}
			}
		}
	};
}	 // namespace mlir::rlc

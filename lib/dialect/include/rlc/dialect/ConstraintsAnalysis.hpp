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
#pragma once

#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <utility>

#include "llvm/ADT/TypeSwitch.h"
#include "llvm/IR/ConstantRange.h"
#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/DenseAnalysis.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/IR/Value.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"

namespace mlir::rlc
{
	typedef struct CA_Value
	{
		mlir::Value value;
		bool return_type;
	} CA_Value;
}	 // namespace mlir::rlc

// Add the map key to the llvm scope
// I have to admit that if this works the llms will take over the world someday
// Ok it compiles
// It seems to work. Wow
namespace llvm
{
	template<>
	struct DenseMapInfo<mlir::rlc::CA_Value>
	{
		static mlir::rlc::CA_Value getEmptyKey()
		{
			return { llvm::DenseMapInfo<mlir::Value>::getEmptyKey(), false };
		}
		static mlir::rlc::CA_Value getTombstoneKey()
		{
			return { DenseMapInfo<mlir::Value>::getTombstoneKey(), false };
		}
		static unsigned getHashValue(const mlir::rlc::CA_Value& Val)
		{
			return hash_combine(
					DenseMapInfo<mlir::Value>::getHashValue(Val.value), Val.return_type);
		}
		static bool isEqual(
				const mlir::rlc::CA_Value& lhs, const mlir::rlc::CA_Value& rhs)
		{
			return DenseMapInfo<mlir::Value>::isEqual(lhs.value, rhs.value) &&
						 lhs.return_type == rhs.return_type;
		}
	};
}	 // namespace llvm

namespace mlir::rlc
{

	// Class prototypes
	// LEAVE THEM OR IT WILL BREAK THE COMPILATION DUE TO A CIRCULAR DEPENDENCE
	// WHICH FOR THE MOMENT CANNOT BE SOLVED (IT JUST WORKS cit.)
	class ConstraintsLattice;
	class ConstraintsAnalysis;

	class ConstraintsLattice: public mlir::dataflow::AbstractDenseLattice
	{
		public:
		typedef int64_t Integer;
		constexpr static Integer MAX = INT64_MAX;
		constexpr static Integer MIN = INT64_MIN;

		// https://llvm.org/doxygen/classllvm_1_1ConstantRange.html#a0e6f2069000829208cbac185a07d8082
		using IntegerRange = llvm::ConstantRange;
		// Bit length of the numbers
		constexpr static unsigned int BITWIDTH = sizeof(Integer) * 8;

		// Create a custom method to create an llvm::APInteger of BITWIDTH dimension
		static const inline llvm::APInt APInteger(const Integer& n)
		{
			return llvm::APInt(BITWIDTH, n, true);
		}

		// Create custom wrapper for the creation of a range
		static const IntegerRange createRange(
				const Integer& min, const Integer& max)
		{
			// Corner case if it is a constant
			if (min == max)
				return IntegerRange(APInteger(min));

			// If the maximum is already MAX_INT then adding one will wrap to MIN_INT
			// and for the moment we do not like this
			if (max == MAX)
				return IntegerRange(APInteger(min), APInteger(max));

			return IntegerRange(APInteger(min), APInteger(max + 1));
		}

		// Overload for a constant
		static const IntegerRange createRange(const Integer& n)
		{
			return createRange(n, n);
		}

		// Create a custom static method for joining two ranges
		// TODO: maybe inline
		static const IntegerRange joinRange(
				const IntegerRange& range1, const IntegerRange& range2)
		{
			// TODO: understand this weird PreferredRangeType
			return range1.unionWith(range2, IntegerRange::PreferredRangeType::Signed);
		}

		// Do also for the meet (maybe useful)
		static const IntegerRange meetRange(
				const IntegerRange& range1, const IntegerRange& range2)
		{
			// TODO: understand this weird PreferredRangeType
			return range1.intersectWith(
					range2, IntegerRange::PreferredRangeType::Signed);
		}

		// Create a custom static method for updating a range
		// TODO: maybe inline
		static const IntegerRange updateRange(
				const IntegerRange& range1,
				const std::optional<Integer>& min,
				const std::optional<Integer>& max)
		{
			return createRange(
					min.value_or(range1.getLower().getSExtValue()),
					max.value_or(range1.getUpper().getSExtValue() - 1));
		}

		MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConstraintsLattice);
		using dataflow::AbstractDenseLattice::AbstractDenseLattice;

		// Method for returning the lattice (useful only at the end) <- maybe can do
		// everything in the class
		const llvm::DenseMap<CA_Value, IntegerRange>& getUnderlyingLattice() const;

		// TODO: understand what this meet does
		//  In theory it should be the opposite of a join
		mlir::ChangeResult meet(
				const mlir::dataflow::AbstractDenseLattice& val) override;

		mlir::ChangeResult join(
				const mlir::dataflow::AbstractDenseLattice& val) override;

		// Operation to copy two lattices
		// NB: this is a deep copy
		mlir::ChangeResult copy(const ConstraintsLattice& other);

		mlir::ChangeResult copy(const mlir::dataflow::AbstractDenseLattice& val);

		void print(raw_ostream& os) const override;

		bool operator==(const ConstraintsLattice& other) const;

		bool isValueNotTopOrBottom(const mlir::Value& value, bool branch) const;

		bool isValueNotBottom(const mlir::Value& value, bool branch) const;

		const IntegerRange& getRange(const mlir::Value& value, bool branch) const;

		const std::pair<bool, bool> branchesContainedInLattice() const;

		const std::pair<bool, bool> isValuePresentInLattice(
				const mlir::Value& value) const;

		const std::pair<bool, bool> isConstPresentInLattice() const;

		bool isValueOfTypePresentInLattice(bool type) const;

		bool rangesEmpty() const;

		bool foundInRanges(const mlir::rlc::CA_Value& key) const;

		IntegerRange getTOP() const;

		bool insertOrJoin(
				const mlir::rlc::CA_Value& key, const IntegerRange& other);

		bool insertOrMeet(
				const mlir::rlc::CA_Value& key, const IntegerRange& other);

		bool insertOrUpdate(
				const mlir::rlc::CA_Value& key, const IntegerRange& other);

		bool insertOrUpdate(
				const mlir::rlc::CA_Value& keyToInsert,
				const std::optional<Integer>& min,
				const std::optional<Integer>& max);

		private:
		llvm::DenseMap<CA_Value, IntegerRange> ranges;
	};

	class ConstraintsAnalysis
			: public mlir::dataflow::DenseBackwardDataFlowAnalysis<ConstraintsLattice>
	{
		public:
		using mlir::dataflow::DenseBackwardDataFlowAnalysis<
				ConstraintsLattice>::DenseBackwardDataFlowAnalysis;

		private:
		mlir::LogicalResult visitOperation(
				mlir::Operation* op,
				const ConstraintsLattice& after,
				ConstraintsLattice* before) override;

		void visitCallControlFlowTransfer(
				CallOpInterface call,
				mlir::dataflow::CallControlFlowAction action,
				const ConstraintsLattice& before,
				ConstraintsLattice* after) final;

		void visitRegionBranchControlFlowTransfer(
				mlir::RegionBranchOpInterface branch,
				mlir::RegionBranchPoint regionFrom,
				mlir::RegionBranchPoint regionTo,
				const ConstraintsLattice& before,
				ConstraintsLattice* after) override;

		void setToExitState(ConstraintsLattice* lattice) override;

		public:
		void printRanges(mlir::Operation* op);

		const llvm::DenseMap<
				mlir::rlc::CA_Value,
				mlir::rlc::ConstraintsLattice::IntegerRange>&
		getUnderlyingLattice(mlir::Operation* op);

		const mlir::rlc::ConstraintsLattice* getObjLattice(mlir::Operation* op);
	};

}	 // namespace mlir::rlc

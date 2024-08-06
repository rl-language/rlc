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
#include <utility>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

#include "llvm/ADT/TypeSwitch.h"
#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/DenseAnalysis.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/conversion/TypeConverter.h"
#include "llvm/IR/ConstantRange.h"

// Add the map key to the llvm scope
// I have to admit that if this works the llms will take over the world someday
// Ok it compiles
// It seems to work. Wow
namespace llvm{
	template <>
	struct llvm::DenseMapInfo<std::pair<mlir::Value,bool>>{
		static std::pair<mlir::Value,bool> getEmptyKey(){
			return std::make_pair(llvm::DenseMapInfo<mlir::Value>::getEmptyKey(),false);
		}
		static std::pair<mlir::Value, bool> getTombstoneKey() {
			return std::make_pair(DenseMapInfo<mlir::Value>::getTombstoneKey(), false);
		}
		static unsigned getHashValue(const std::pair<mlir::Value, bool> &Val) {
			return hash_combine(DenseMapInfo<mlir::Value>::getHashValue(Val.first), Val.second);
		}
		static bool isEqual(const std::pair<mlir::Value, bool> &lhs, const std::pair<mlir::Value, bool> &rhs) {
			return DenseMapInfo<mlir::Value>::isEqual(lhs.first, rhs.first) && lhs.second == rhs.second;
		}
	};
}

namespace mlir::rlc
{
	namespace
	{
		// Class prototypes
		// LEAVE THEM OR IT WILL BREAK THE COMPILATION DUE TO A CIRCULAR DEPENDENCE WHICH FOR THE MOMENT CANNOT BE SOLVED (IT JUST WORKS cit.)
		class ConstraintsLattice;
		class ConstraintsAnalysis;

		// I will leave this here

		// AbstractDenseLattice
		// A dense lattice is attached to operations to represent the program state after their execution or to blocks to represent the program state at the beginning of the block. A dense lattice is propagated through the IR by dense data-flow analysis. 

		// AbstractSparseLattice
		// A lattice contains information about an SSA value and is what's propagated across the IR by sparse data-flow analysis.

		// AbstractDenseForwardDataFlowAnalysis
		// Dense data-flow analysis attaches a lattice between the execution of operations and implements a transfer function from the lattice before each operation to the lattice after. The lattice contains information about the state of the program at that point.

		// In this implementation, a lattice attached to an operation represents the state of the program after its execution, and a lattice attached to block represents the state of the program right before it starts executing its body. 

		// AbstractSparseForwardDataFlowAnalysis
		// A sparse analysis implements a transfer function on operations from the lattices of the operands to the lattices of the results. This analysis will propagate lattices across control-flow edges and the callgraph using liveness information. 

		// My initial conclusion is then that we need an AbstractDenseLattice and an AbstractDenseBackwardDataFLowAnalysis

		// So our lattice is attached to operations -> for each operation we need to store information about the operands and results
		// This means that we need a set (more precisely a dictionary) since we do not know how many operands need to be saved

		//TODO: Can i make it generic wrt an integer type?
		class ConstraintsLattice: public mlir::dataflow::AbstractDenseLattice
		{

			public:

			typedef int Integer;
			constexpr static Integer MAX=INT32_MAX;
			constexpr static Integer MIN=INT32_MIN;
			
			// So now let's have some fun
			// Try to see what happens when trying to use this funny class
			//https://llvm.org/doxygen/classllvm_1_1ConstantRange.html#a0e6f2069000829208cbac185a07d8082
			using IntegerRange=llvm::ConstantRange;
			// Bit length of the numbers
			constexpr static unsigned int BITWIDTH=sizeof(Integer)*8;

			// Create a custom method to create an llvm::APInteger of BITWIDTH dimension
			static inline llvm::APInt APInteger(const Integer& n){
				return llvm::APInt(BITWIDTH,n,true);
			}

			// Create custom wrapper for the creation of a range
			static IntegerRange createRange(const Integer& min, const Integer& max){
				// Corner case if it is a constant
				if(min==max)
					return IntegerRange(APInteger(min));
				
				// If the maximum is already MAX_INT then adding one will wrap to MIN_INT and
				// for the moment we do not like this
				if(max==MAX)
					return IntegerRange(APInteger(min),APInteger(max));

				return IntegerRange(APInteger(min),APInteger(max+1));
			}

			// Overload for a constant
			static IntegerRange createRange(const Integer& n){
				return createRange(n,n);
			} 

			// Create a custom static method for joining two ranges
			// TODO: maybe inline
			static IntegerRange joinRange(const IntegerRange& range1, const IntegerRange& range2){
				// TODO: understand this weird PreferredRangeType
				return range1.unionWith(range2,IntegerRange::PreferredRangeType::Signed);
			}

			// Create a custom static method for updating a range
			// TODO: maybe inline
			static IntegerRange updateRange(const IntegerRange& range1, const std::optional<Integer>& min, const std::optional<Integer>& max){
				return createRange(
					min.value_or(range1.getLower().getSExtValue()),
					max.value_or(range1.getUpper().getSExtValue()-1));
			}

			MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConstraintsLattice);
			explicit ConstraintsLattice(mlir::ProgramPoint point)
					: mlir::dataflow::AbstractDenseLattice(point)
			{
			}

			// Method for returning the lattice (useful only at the end) <- maybe can do everything in the class
			const llvm::DenseMap<std::pair<mlir::Value,bool>,IntegerRange>& getUnderlyingLattice() const;

			//TODO: understand what this meet does
			// In theory it should be the opposite of a join
			mlir::ChangeResult meet(const mlir::dataflow::AbstractDenseLattice& val) override;

			mlir::ChangeResult join(const mlir::dataflow::AbstractDenseLattice& val) override;

			// Operation to copy two lattices
			// NB: this is a deep copy
			mlir::ChangeResult copy(const ConstraintsLattice& other);

			mlir::ChangeResult copy(const mlir::dataflow::AbstractDenseLattice& val);

			void print(raw_ostream& os) const override;

			bool operator==(const ConstraintsLattice& other) const;

			mlir::ChangeResult setCurrentBranch(mlir::RegionBranchOpInterface* branchOp);
			
			const std::pair<bool,bool> branchesContainedInLattice() const;

			const std::pair<bool,bool> isValuePresentInLattice(const mlir::Value& value) const;

			const std::pair<bool,bool> isConstPresentInLattice() const;

			bool isOnlyElementInToCheckLattice(const mlir::Value& value) const;

			bool isOnlyToCheckPresentInLattice(const mlir::Value& value) const;

			bool isValueOfTypePresentInLattice(bool type) const;

			bool rangesEmptyAndToCheckOneElement() const;

			void removeConstFromLattice(const bool type);

			// When I visit a builtin assign operation I need to check if it is in the lattice
			mlir::ChangeResult visitBuiltinAssign(mlir::Operation* statement, mlir::rlc::ConstraintsAnalysis* analysis);

			// When I visit an uninitialized construct I need to insert the ssa value 
			// in the set
			mlir::ChangeResult visitUninitializedConstruct(mlir::Operation* statement);

			// TODO: divide this operation in (at least) two parts
			// Here we should write the transfer function based on the operation 
			mlir::ChangeResult visitArithmetic(mlir::Operation* statement, bool can_true, bool can_false);

			mlir::ChangeResult visitYield(mlir::Operation* statement);

			mlir::ChangeResult visitBranch(mlir::Operation* statement, ConstraintsAnalysis* analysis);

			private:

			IntegerRange getTOP() const;
			
			IntegerRange getRangeOrTOP(const std::pair<mlir::Value,bool>& key) const;

			void insertOrJoin(const std::pair<mlir::Value,bool>& key, const IntegerRange& other);

			void insertOrUpdate(const std::pair<mlir::Value,bool>& key, const IntegerRange& other);

			void insertOrUpdate(const std::pair<mlir::Value,bool>& keyToInsert, const std::optional<Integer>& min, const std::optional<Integer>& max);

			// This method takes the operation, the range
			// statement is the original operation
			// other is the other operand so that we can do to_update=result operation other
			void updateArithmeticCommutativeRange(const mlir::Operation* statement, const mlir::Value& result, const mlir::Value& to_update, const mlir::Value& other, const bool branch_to_update);

			// This function is actually quite simpler since we always know 
			// the lhs is not a constant and the rhs is a constant
			// Result is useful to understand which check do to the operation
			void updateRelationalRange(const mlir::Operation* statement, bool result, const mlir::Value& to_update, const mlir::rlc::Constant& other, const bool branch_to_update);

			llvm::DenseMap<std::pair<mlir::Value,bool>,IntegerRange> ranges;

			// Here I store the information about those constructs
			// which cannot be checked immediately but need a specific operation
			// Indeed an rlc.uninitialized_construct cannot be evaluated until
			// a rlc.builtin_assign is called
			llvm::DenseSet<mlir::Value> to_check;
		};

		class ConstraintsAnalysis
				: public mlir::dataflow::DenseBackwardDataFlowAnalysis<
							ConstraintsLattice>
		{
			public:
			using mlir::dataflow::DenseBackwardDataFlowAnalysis<
					ConstraintsLattice>::DenseBackwardDataFlowAnalysis;

			// Utility
			static bool isArithmetic(mlir::Operation* op){
				if (mlir::isa<mlir::rlc::AddOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::MultOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::DivOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::SubOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::MinusOp>(op))
					return true;
				
				return false;
			}
			static bool isConditional(mlir::Operation* op){
				if(mlir::isa<mlir::rlc::CondBranch>(op))
					return true;

				return false;
			}
			static bool isYield(mlir::Operation* op){
				if(mlir::isa<mlir::rlc::Yield>(op))
					return true;
				
				return false;
			}
			static bool isBuiltinAssign(mlir::Operation* op){
				if(mlir::isa<mlir::rlc::BuiltinAssignOp>(op))
					return true;
				
				return false;
			}

			private:

			// Definitions taken from https://mlir.llvm.org/doxygen/DenseAnalysis_8h_source.html#l00423

			/// Transfer function. Visits an operation with the dense lattice after its
   			/// execution. This function is expected to set the dense lattice before its
   			/// execution and trigger propagation in case of change..
			void visitOperation(
				mlir::Operation *op, 
				const ConstraintsLattice &after,
                ConstraintsLattice *before) override;

			/// Hook for customizing the behavior of lattice propagation along the call
   			/// control flow edges. Two types of (back) propagation are possible here:
			///   - `action == CallControlFlowAction::Enter` indicates that:
			///     - `after` is the state at the top of the callee entry block;
			///     - `before` is the state before the call operation;
			///   - `action == CallControlFlowAction::Exit` indicates that:
			///     - `after` is the state after the call operation;
			///     - `before` is the state of exit blocks of the callee.
			/// By default, the `before` state is simply met with the `after` state.
			/// Concrete analyses can override this behavior or delegate to the parent
			/// call for the default behavior. Specifically, if the `call` op may affect
			/// the lattice prior to entering the callee, the custom behavior can be added
			/// for `action == CallControlFlowAction::Enter`. If the `call` op may affect
			/// the lattice post exiting the callee, the custom behavior can be added for
   			/// `action == CallControlFlowAction::Exit`.
			// rlc.call
			void visitCallControlFlowTransfer(
					CallOpInterface call,
					mlir::dataflow::CallControlFlowAction action,
					const ConstraintsLattice& before,
					ConstraintsLattice* after) final;

			/// Hook for customizing the behavior of lattice propagation along the control
			/// flow edges between regions and their parent op. The control flows from
			/// `regionFrom` to `regionTo`, both of which may be `nullopt` to indicate the
			/// parent op. The lattice is propagated back along this edge. The lattices
			/// are as follows:
			///   - `after`:
			///     - if `regionTo` is a region, this is the lattice at the beginning of
			///       the entry block of that region;
			///     - otherwise, this is the lattice after the parent op.
			///   - `before:`
			///     - if `regionFrom` is a region, this is the lattice at the end of the
			///       block that exits the region; note that for multi-exit regions, the
			///       lattices are equal at the end of all exiting blocks, but they are
			///       associated with different program points.
			///     - otherwise, this is the lattice before the parent op.
			/// By default, the `before` state is simply met with the `after` state.
			/// Concrete analyses can override this behavior or delegate to the parent
			/// call for the default behavior. Specifically, if the `branch` op may affect
			/// the lattice before entering any region, the custom behavior can be added
			/// for `regionFrom == nullopt`. If the `branch` op may affect the lattice
			/// after all terminated, the custom behavior can be added for `regionTo ==
			/// nullptr`. The behavior can be further refined for specific pairs of "from"
			/// and "to" regions.
			// rlc.yield
   			void visitRegionBranchControlFlowTransfer(
					mlir::RegionBranchOpInterface branch,
					mlir::RegionBranchPoint regionFrom,
					mlir::RegionBranchPoint regionTo,
					const ConstraintsLattice& before,
					ConstraintsLattice* after) override;

			// At the end we can take the information about the arguments in our lattice and attach it
			// to the function arguments
			// TODO: understand when this is called and where to append the information obtained when finishing
			// For the moment this function is just observing the IR, so we can leave it as it is
			void setToExitState(ConstraintsLattice* lattice) override;

			public:

			void printRanges(mlir::Operation* op);

			const llvm::DenseMap<std::pair<mlir::Value,bool>,mlir::rlc::ConstraintsLattice::IntegerRange>& getUnderlyingLattice(mlir::Operation* op);

			const mlir::rlc::ConstraintsLattice* getObjLattice(mlir::Operation* op);

		};

		//////////////////////////////////////////////////////////////////////////////
		// ConstraintsLattice METHODS
		//////////////////////////////////////////////////////////////////////////////

		// Method for returning the lattice (useful only at the end) <- maybe can do everything in the class
		const llvm::DenseMap<std::pair<mlir::Value,bool>,ConstraintsLattice::IntegerRange>& ConstraintsLattice::getUnderlyingLattice() const{
			return this->ranges;
		}

		//TODO: understand what this meet does
		// In theory it should be the opposite of a join
		mlir::ChangeResult ConstraintsLattice::meet(const mlir::dataflow::AbstractDenseLattice& val)
		{
			llvm::outs()<<"meet\n";
			//return mlir::ChangeResult::NoChange;
			// Do a funny thing and let the meet perform the join
			return this->join(val);
		}

		mlir::ChangeResult ConstraintsLattice::join(const mlir::dataflow::AbstractDenseLattice& val)
		{
			//Need to cast it
			const auto& other = *static_cast<const ConstraintsLattice*>(&val);
			
			//Keep it to check if there is no change
			auto copy = *this;

			llvm::outs()<<"join\n";
			// this->print(llvm::outs());
			// other.print(llvm::outs());

			//Insert the other in this
			for(const auto& other_range : other.ranges){
				
				// We have to insert the other range
				// If we find it then we have to join, else we have to insert it
				this->insertOrJoin(other_range.first, other_range.second);
			}
			
			for(const auto& other_to_check: other.to_check){
				this->to_check.insert(other_to_check);
			}

			//
			if (*this==copy and other==copy){
				//this->print(llvm::outs());
				return mlir::ChangeResult::NoChange;
			}
			//this->print(llvm::outs());
			return mlir::ChangeResult::Change;
		}

		// Operation to copy two lattices
		// NB: this is a deep copy
		mlir::ChangeResult ConstraintsLattice::copy(const ConstraintsLattice& other){
			
			llvm::outs()<<"Did i copy?\n";
			llvm::outs()<<"mine : ";this->print(llvm::outs());
			llvm::outs()<<"other: ";other.print(llvm::outs());

			if (*this==other){
				llvm::outs()<<"no\n";
				return mlir::ChangeResult::NoChange;
			}
			this->ranges=other.ranges;
			this->to_check=other.to_check;
			return mlir::ChangeResult::Change;
		}

		mlir::ChangeResult ConstraintsLattice::copy(const mlir::dataflow::AbstractDenseLattice& val){
			return this->copy(*static_cast<const ConstraintsLattice*>(&val));
		}

		void ConstraintsLattice::print(raw_ostream& os) const{

			os<<"Ranges:\n";
			if(not this->ranges.empty()){
				for(auto range: this->ranges){
					os << "SSA: "<< range.first.first <<" Branch: "<< range.first.second << 
					" RANGE: (" <<range.second.getLower().getSExtValue() << ", " << range.second.getUpper().getSExtValue() << ")\n";
				}
			}
			else {
				os<<"Empty\n";
			}
			os<<"To check:\n";
			if(not this->to_check.empty()){
				for(auto check: this->to_check){
					os << "SSA: "<< check << "\n";
				}
			}
			else {
				os<<"Empty\n";
			}
		}

		bool ConstraintsLattice::operator==(const ConstraintsLattice& other) const{
			return this->ranges == other.ranges
				and this->to_check == other.to_check;
		}

		mlir::ChangeResult ConstraintsLattice::setCurrentBranch(mlir::RegionBranchOpInterface* branchOp){
			// The yield has this format
			// %12 = rlc.constant true !rlc.bool
			// rlc.yield %12 : !rlc.bool
			// So I need to retrieve "true" in this case

			// So here we need to be very careful 
			// We could also directly return something that is not a constant
			// For instant we could do 'return a>10' but in this case we can just call the function that handles it
			// Here we get the rlc.constant true
			const auto& value= mlir::cast<mlir::rlc::Yield>(branchOp->getOperation()).getArguments()[0];

			value.getDefiningOp()->print(llvm::outs());

			// TODO: wrap in some static method or put a trait
			if( mlir::isa<mlir::rlc::GreaterOp>(value.getDefiningOp()) or	
				mlir::isa<mlir::rlc::GreaterEqualOp>(value.getDefiningOp()) or
				mlir::isa<mlir::rlc::LessOp>(value.getDefiningOp()) or
				mlir::isa<mlir::rlc::LessEqualOp>(value.getDefiningOp()) or
				mlir::isa<mlir::rlc::UninitializedConstruct>(value.getDefiningOp())
				)
				return this->visitYield(branchOp->getOperation());

			auto bool_branch= mlir::dyn_cast<BoolAttr>(value.getDefiningOp()->getAttr("value"));
			if (not bool_branch)
				return mlir::ChangeResult::NoChange;

			this->ranges.insert(std::make_pair(
				std::make_pair(
					value,bool_branch.getValue()),
					this->getTOP() ));
				
			return mlir::ChangeResult::Change;
		}
		
		const std::pair<bool,bool> ConstraintsLattice::branchesContainedInLattice() const{
			bool can_true=false;
			bool can_false=false;

			for(auto pair : this->ranges){
				if(pair.first.second)
					can_true=true;
				else can_false=true;
			}

			return std::make_pair(can_true,can_false);
		}

		const std::pair<bool,bool> ConstraintsLattice::isValuePresentInLattice(const mlir::Value& value) const{
			bool can_true=false;
			bool can_false=false;

			for(auto pair : this->ranges){
				if(pair.first.first==value){
					if(pair.first.second)
						can_true=true;
					else can_false=true;
				}
			}

			return std::make_pair(can_true,can_false);
		}

		const std::pair<bool,bool> ConstraintsLattice::isConstPresentInLattice() const{
			bool can_true=false;
			bool can_false=false;

			for(auto pair : this->ranges){
				if(not (pair.first.first.getDefiningOp()==nullptr) and
					mlir::isa<mlir::rlc::Constant>(pair.first.first.getDefiningOp())){
					if(pair.first.second)
						can_true=true;
					else can_false=true;
				}
			}

			return std::make_pair(can_true,can_false);
		}

		bool ConstraintsLattice::isOnlyElementInToCheckLattice(const mlir::Value& value) const{
			//return this->to_check.size()==1 and to_check.contains(value) and this->ranges.empty();
			return to_check.contains(value) and this->ranges.empty();
		}

		bool ConstraintsLattice::isOnlyToCheckPresentInLattice(const mlir::Value& value) const{
			
			for(auto pair : this->ranges){
				if(pair.first.first==value)
					return false;
			}
			return not this->to_check.empty();
		}

		bool ConstraintsLattice::isValueOfTypePresentInLattice(bool type) const{

			for(auto pair : this->ranges){
				if(pair.first.second==type)
					return true;
			}
			return false;
		}

		bool ConstraintsLattice::rangesEmptyAndToCheckOneElement() const{
			return this->ranges.empty() and this->to_check.size()==1;
		}

		void ConstraintsLattice::removeConstFromLattice(const bool type){
			for(auto pair: this->ranges){
				if(not (pair.first.first.getDefiningOp()==nullptr) and
					mlir::isa<mlir::rlc::Constant>(pair.first.first.getDefiningOp()))
					if(pair.first.second==type)
						this->ranges.erase(pair.first);
			}
		}

		// When I visit a builtin assign operation I need to check if it is in the lattice
		mlir::ChangeResult ConstraintsLattice::visitBuiltinAssign(mlir::Operation* statement, mlir::rlc::ConstraintsAnalysis* analysis){
		
			llvm::outs()<<"Visiting builtin assign operation\n";
		
			if(this->to_check.empty())
				return mlir::ChangeResult::NoChange;

			auto casted=mlir::dyn_cast<mlir::rlc::BuiltinAssignOp>(statement);

			// Check lhs
			auto found=this->to_check.find(casted->getOperand(0));
			
			if(found==this->to_check.end())
				return mlir::ChangeResult::NoChange;
				

			// We also need to check the peculiar case when we assign to the construct
			// another uninitialized construct.
			// In this way we need to remove the one we already know and add the other one
			if(mlir::isa<mlir::rlc::UninitializedConstruct>(casted->getOperand(1).getDefiningOp())){
				//this->to_check.erase(casted->getOperand(0));
				this->to_check.insert(casted->getOperand(1));
				return mlir::ChangeResult::Change;
			}
		
			// If there is a valid operation then we have to treat it similarly to a rlc.crb
			// But here we need to consider an important assumption (or else everything will be more complicated):
			// When doing a rlc.builtin_assign of a relational operator we will check it immediately later in a rlc.crb

			// This helps avoiding doing the same check in the visitBranch method

			// What we need to do here is something very important: 
			// check the operation in mostly the same way 
			// but now I do not need to look for the constant but for the presence of the rlc.uninit_construct

			auto copy=*this;
			
			// Before this there could be a case in which I have only one successor, 
			// in that case I just call the visitYield
			if(statement->getBlock()->getNumSuccessors()==1)
				return this->visitYield(statement);


			mlir::Operation* conditional_op=casted->getOperand(1).getDefiningOp();

			// if(auto casted=mlir::dyn_cast<rlc::UninitializedConstruct>(conditional_op)){
			// 	llvm::outs()<<"\nAdded uninitialized construct:\n";
			// 	return this->visitUninitializedConstruct(statement);
			// }

			// Do the checking only if the operation is a conditional (relational)
			if(	not mlir::isa<mlir::rlc::LessOp>(conditional_op) and 
				not mlir::isa<mlir::rlc::GreaterOp>(conditional_op) and
				not mlir::isa<mlir::rlc::LessEqualOp>(conditional_op) and 
				not mlir::isa<mlir::rlc::GreaterEqualOp>(conditional_op))
				return mlir::ChangeResult::NoChange;

			const auto& lhs = conditional_op->getOperand(0);
			const auto& rhs = conditional_op->getOperand(1);
			const auto& lhs_op = lhs.getDefiningOp();
			const auto& rhs_op = rhs.getDefiningOp();

			// The algorithm will work mostly as the one for the arithmetic operations, but it will be simpler (no commutativity problem)
			// NB: note that they are not exclusive so they have to be executed always

			// If the rhs is a constant
			if(auto rhs_casted=mlir::dyn_cast<mlir::rlc::Constant>(rhs_op)){

				// Here we use the assumption that a relational operator has always two successors

				// Look at the next lattice in the true branch
				auto trueLattice=analysis->getObjLattice(&statement->getBlock()->getSuccessor(0)->front());

				// This is equivalent to haveing a return constant (in this case is return true)
				auto branch_true_is_only_tocheck=trueLattice->isOnlyElementInToCheckLattice(casted.getOperand(0));
				auto branch_true_is_value_present=trueLattice->isValuePresentInLattice(lhs);
				auto branch_true_has_true=this->isValueOfTypePresentInLattice(true);

				// After we called the two functions we need to understand if it is a valid update.
				// Then the result is a valid update if one of the two values is true 
				auto branch_true_yield_true=branch_true_is_only_tocheck or branch_true_is_value_present.first or branch_true_has_true;
				auto branch_true_yield_false=not branch_true_is_only_tocheck and branch_true_is_value_present.second;

				// And do the same in the false
				auto falseLattice=analysis->getObjLattice(&statement->getBlock()->getSuccessor(1)->front());

				auto branch_false_is_only_tocheck=falseLattice->isOnlyElementInToCheckLattice(casted.getOperand(0));
				auto branch_false_is_value_present=falseLattice->isValuePresentInLattice(lhs);
				auto branch_false_has_false=this->isValueOfTypePresentInLattice(false);

				auto branch_false_yield_true=not branch_false_is_only_tocheck and branch_false_is_value_present.first;
				auto branch_false_yield_false=branch_false_is_only_tocheck or branch_false_is_value_present.second or branch_false_has_false;

				// We need to perform a similar operation 

				// llvm::outs()<<"\nLUIS "<<
				// branch_true_is_only_tocheck<<
				// branch_true_is_value_present.first<<
				// branch_true_is_value_present.second<<
				// branch_false_is_only_tocheck<<
				// branch_false_is_value_present.first<<
				// branch_false_is_value_present.second<<"\n"<<
				// branch_true_yield_true<<
				// branch_true_yield_false<<
				// branch_false_yield_true<<
				// branch_false_yield_false<<"\n";
				// this->print(llvm::outs());
				// trueLattice->print(llvm::outs());
				// falseLattice->print(llvm::outs());

				// Modify my ranges only when i am sure that i am passing through a initialized (true/false) branch
				if (not branch_true_yield_true and not branch_true_yield_false and
					not branch_false_yield_true and not branch_false_yield_false)
					return mlir::ChangeResult::NoChange;
				

				// If both branches yield true then there is no need to update since no more information can be retrieved
				if(not branch_true_yield_true==branch_false_yield_true){

					if(branch_true_yield_true)
						this->updateRelationalRange(conditional_op, true, lhs, rhs_casted, true);	

					if(branch_false_yield_true)
						this->updateRelationalRange(conditional_op, false, lhs, rhs_casted, true);
				
				}

				if(not branch_true_yield_false==branch_false_yield_false){

					if(branch_true_yield_false)
						this->updateRelationalRange(conditional_op, true, lhs, rhs_casted, false);
					
					if(branch_false_yield_false)
						this->updateRelationalRange(conditional_op, false, lhs, rhs_casted, false);	
				
				}
			}
			// If the lhs is a constant then do the same but with inverted branches
			/*else if(auto lhs_casted=mlir::dyn_cast<mlir::rlc::Constant>(lhs_op)){

				if(not branch_true_yield_true==branch_false_yield_true){

					if(branch_true_yield_true)
						this->updateRelationalRange(conditional_op, false, rhs, lhs_casted, true);	
					
					if(branch_true_yield_false)
						this->updateRelationalRange(conditional_op, false, rhs, lhs_casted, false);	
				
				}
				else{
					this->insertOrJoin(std::make_pair(rhs,true),this->getTOP());
				}

				if(not branch_true_yield_false==branch_false_yield_false){

					if(branch_false_yield_true)
						this->updateRelationalRange(conditional_op, true, rhs, lhs_casted, true);	
					
					if(branch_false_yield_false)
						this->updateRelationalRange(conditional_op, true, rhs, lhs_casted, false);	
					
				}
				else{
					this->insertOrJoin(std::make_pair(rhs,false),this->getTOP());
				}

			}
			*/
			// else do nothing

			if(*this!=copy){
				llvm::outs()<<"something should have changed\n";
				this->print(llvm::outs());
				return mlir::ChangeResult::Change;
			}
			else{
				return mlir::ChangeResult::NoChange;
			}

			return mlir::ChangeResult::NoChange;

		}

		// When I visit an uninitialized construct I need to insert the ssa value 
		// in the set
		mlir::ChangeResult ConstraintsLattice::visitUninitializedConstruct(mlir::Operation* statement){
			
			bool res=false;

			// The operation can be either a crb or a yield
			if(auto casted=mlir::dyn_cast<mlir::rlc::CondBranch>(statement)){
				res = this->to_check.insert(casted.getCond()).second;
			}
			else if(auto casted=mlir::dyn_cast<mlir::rlc::Yield>(statement)){
				res = this->to_check.insert(casted.getOperand(0)).second;
				llvm::outs()<<"BIG "<<res<<"\n";
			}

			// NB: if the ssa value is already present then we can just leave it there

			if(res)
				return mlir::ChangeResult::Change;
			else
				return mlir::ChangeResult::NoChange;

		}

		// TODO: divide this operation in (at least) two parts
		// Here we should write the transfer function based on the operation 
		mlir::ChangeResult ConstraintsLattice::visitArithmetic(mlir::Operation* statement, bool can_true, bool can_false)
		{

			// Modify my ranges only when i am sure that i am passing through a initialized (true/false) branch
			if (not can_true and not can_false)
				return mlir::ChangeResult::NoChange;

			// Here the operation has passed the sanity check so we can switch
			auto copy = *this;

			// So this becomes extremely tricky now, we have tstatehe information about the branch we are passing from
			// The idea is, i am visiting one branch => update one, I am visiting both => update both

			// TODO: this should be a casting to a CommutativeOp <- TODO: add
			// For the moment it should work so let's leave it like this
			if (auto casted=mlir::dyn_cast<mlir::rlc::AddOp>(statement)){
				llvm::outs()<<"addop\n";
				this->print(llvm::outs());
				// Do some name changing so that hopefully is more clear
				// NB : these are values
				const auto& a = casted.getResult();
				const auto& b = casted.getLhs();
				const auto& c = casted.getRhs();
				// This can be nullptr if for example it is a function argument
				// TODO: are there any other cases ?
				const auto& b_op = b.getDefiningOp();
				const auto& c_op = c.getDefiningOp();
				// TODO: understand what happens if both have ranges -> should I update both ? (because if yes, in which order ?)
				// NB: there iConstraintsLattices the case in which both are non constantv-> if we know nothing about
				//     both we do nothing
				// Then first find which variable we need to operate on
				mlir::Value      state=nullptr;
				mlir::Value      stato=nullptr;
				// First get our candidates
				if(b_op == nullptr or not mlir::isa<mlir::rlc::Constant>(b_op)){
					state=b;
					stato=c;
				}
				else if(c_op == nullptr or not mlir::isa<mlir::rlc::Constant>(c_op)){
					state=c;
					stato=b;
				}
				// Then update the corresponding branch
				// NB: they are not exclusive so I have to update both
				if(can_true)
					this->updateArithmeticCommutativeRange(statement, a, state, stato, true);
				if(can_false)
					this->updateArithmeticCommutativeRange(statement, a, state, stato, false);

			}

			if(*this!=copy){
				llvm::outs()<<"something should have changed\n";
				this->print(llvm::outs());
				return mlir::ChangeResult::Change;
			}
			else{
				return mlir::ChangeResult::NoChange;
			}
		}

		mlir::ChangeResult ConstraintsLattice::visitYield(mlir::Operation* statement){

			auto copy=*this;

			// TODO: Think about a typeswitch here too
			mlir::Operation* conditional_op=nullptr;
			if(auto casted=mlir::dyn_cast<mlir::rlc::Yield>(statement)){
				conditional_op=casted.getOperand(0).getDefiningOp();
			}
			else if(auto casted=mlir::dyn_cast<mlir::rlc::BuiltinAssignOp>(statement)){
				conditional_op=casted.getOperand(1).getDefiningOp();
			}

			if(auto casted=mlir::dyn_cast<rlc::UninitializedConstruct>(conditional_op)){
				llvm::outs()<<"\nAdded uninitialized construct:\n";
				return this->visitUninitializedConstruct(statement);
			}

			// Do the checking only if the operation is a conditional (relational)
			if(	not mlir::isa<mlir::rlc::LessOp>(conditional_op) and 
				not mlir::isa<mlir::rlc::GreaterOp>(conditional_op) and
				not mlir::isa<mlir::rlc::LessEqualOp>(conditional_op) and 
				not mlir::isa<mlir::rlc::GreaterEqualOp>(conditional_op))
				return mlir::ChangeResult::NoChange;

				

			const auto& lhs = conditional_op->getOperand(0);
			const auto& rhs = conditional_op->getOperand(1);
			const auto& lhs_op = lhs.getDefiningOp();
			const auto& rhs_op = rhs.getDefiningOp();

			// The algorithm will work mostly as the one for the arithmetic operations, but it will be simpler (no commutativity problem)
			// NB: note that they are not exclusive so they have to be executed always
			
			/*
			// Look at the next lattice in the true branch
			auto trueLattice=analysis->getObjLattice(&casted.getTrueBranch()->front());

			auto branch_true=trueLattice->branchesContainedInLattice();

			// And do the same in the false
			auto falseLattice=getLattice(mlir::ProgramPoint(&casted.getFalseBranch()->front()));

			auto branch_false=falseLattice->branchesContainedInLattice();
			*/

			// If the rhs is a constant
			if(auto rhs_casted=mlir::dyn_cast<mlir::rlc::Constant>(rhs_op)){

				this->updateRelationalRange(conditional_op, true, lhs, rhs_casted, true);
			
				this->updateRelationalRange(conditional_op, false, lhs, rhs_casted, false);

			}
			// If the lhs is a constant then do the same but with inverted branches
			else if(auto lhs_casted=mlir::dyn_cast<mlir::rlc::Constant>(lhs_op)){

				this->updateRelationalRange(conditional_op, false, rhs, lhs_casted, true);	
			
				this->updateRelationalRange(conditional_op, true, rhs, lhs_casted, false);	
					
			}
			// else do nothing

			if(*this!=copy){
				llvm::outs()<<"something should have changed\n";
				this->print(llvm::outs());
				return mlir::ChangeResult::Change;
			}
			else{
				return mlir::ChangeResult::NoChange;
			}
		}

		mlir::ChangeResult ConstraintsLattice::visitBranch(mlir::Operation* statement, mlir::rlc::ConstraintsAnalysis* analysis){

			if (not this->to_check.empty())
				return mlir::ChangeResult::NoChange;

			auto copy=*this;

			auto casted=mlir::dyn_cast<mlir::rlc::CondBranch>(statement);

			mlir::Operation* conditional_op=casted.getCond().getDefiningOp();

			// if(auto casted=mlir::dyn_cast<rlc::UninitializedConstruct>(conditional_op)){
			// 	llvm::outs()<<"\nAdded uninitialized construct:\n";
			// 	return this->visitUninitializedConstruct(statement);
			// }

			// Do the checking only if the operation is a conditional (relational)
			if(	not mlir::isa<mlir::rlc::LessOp>(conditional_op) and 
				not mlir::isa<mlir::rlc::GreaterOp>(conditional_op) and
				not mlir::isa<mlir::rlc::LessEqualOp>(conditional_op) and 
				not mlir::isa<mlir::rlc::GreaterEqualOp>(conditional_op))
				return mlir::ChangeResult::NoChange;

			const auto& lhs = conditional_op->getOperand(0);
			const auto& rhs = conditional_op->getOperand(1);
			const auto& lhs_op = lhs.getDefiningOp();
			const auto& rhs_op = rhs.getDefiningOp();

			// The algorithm will work mostly as the one for the arithmetic operations, but it will be simpler (no commutativity problem)
			// NB: note that they are not exclusive so they have to be executed always

			// If the rhs is a constant
			if(auto rhs_casted=mlir::dyn_cast<mlir::rlc::Constant>(rhs_op)){

				// Look at the next lattice in the true branch
				auto trueLattice=analysis->getObjLattice(&casted.getTrueBranch()->front());

				// Understand if there is a value or a constant
				auto branch_true_has_value=trueLattice->isValuePresentInLattice(lhs);
				auto branch_true_has_const=trueLattice->isConstPresentInLattice();

				// The result is just the logical or
				auto branch_true_yield_true =branch_true_has_value.first or branch_true_has_const.first;
				auto branch_true_yield_false=branch_true_has_value.second or branch_true_has_const.second;

				// And do the same in the false
				auto falseLattice=analysis->getObjLattice(&casted.getFalseBranch()->front());

				auto branch_false_has_value=falseLattice->isValuePresentInLattice(lhs);
				auto branch_false_has_const=falseLattice->isConstPresentInLattice();

				auto branch_false_yield_true =branch_false_has_value.first or branch_false_has_const.first;
				auto branch_false_yield_false=branch_false_has_value.second or branch_false_has_const.second;

				// Here we need to be extremely careful because there is one tricky case:
				// if we have a const in one branch but not in the other it means we are in a case of
				// if ... :
				//   return ...
				// else:
				//   return false
				// 
				// Since we need to be conservative we need to set the false value to TOP

				// We need to be even a bit more careful because this operation is only valid if
				// the else branch lattice does not contain another lattice used in ...

				if(branch_false_has_const.first and branch_true_has_value.first and not branch_false_has_value.first){
					this->insertOrJoin(std::make_pair(lhs,true),this->getTOP());
				}

				if(branch_false_has_const.second and branch_true_has_value.second and not branch_false_has_value.second){
					this->insertOrJoin(std::make_pair(lhs,false),this->getTOP());
				}

				// There is also an interesting case which can be analyzed:
				// When we perform a 
				// return a > 5 and a < 10 and b < 5
				// Here we will need to be sure to evaluate that a < 10.
				// To do it we can check the lattices, and discover that when evaluating
				// the rlc.crb related to a < 10 the false lattice will consist only of an uninitialized construct.
				// So the false branch has to be set to TOP and the true branch will be regularly evaluated

				// if(falseLattice->rangesEmptyAndToCheckOneElement()){
				// 	branch_true_yield_true=true;
				// 	branch_true_yield_false=false;
				// 	branch_false_yield_true=false;
				// 	branch_false_yield_false=false;
				// 	llvm::outs()<<"SOUMM\n";
				// 	this->insertOrJoin(std::make_pair(lhs,false),this->getTOP());
				// }
				// // Then we need to check for uninitialized constructs derived from the flattening of logical operations
				// // And we perform a similar operation as with constants
				// else{
				// 	if(trueLattice->isOnlyToCheckPresentInLattice(lhs)){
				// 		this->insertOrJoin(std::make_pair(lhs,true),this->getTOP());
				// 		// Dirty trick to force to not update
				// 		branch_true_yield_true=branch_false_yield_true;
				// 	}

				// 	if(falseLattice->isOnlyToCheckPresentInLattice(lhs)){
				// 		this->insertOrJoin(std::make_pair(lhs,false),this->getTOP());
				// 		branch_true_yield_false=branch_false_yield_false;
				// 	}
				// }


				llvm::outs()<<"\nMUIS\n\n";
				trueLattice->print(llvm::outs());
				falseLattice->print(llvm::outs());

				// Modify my ranges only when i am sure that i am passing through a initialized (true/false) branch
				if (not branch_true_yield_true and not branch_true_yield_false and
					not branch_false_yield_true and not branch_false_yield_false)
					return mlir::ChangeResult::NoChange;
				

				// If both branches yield true then there is no need to update since no more information can be retrieved
				if(not branch_true_yield_true==branch_false_yield_true){

					if(branch_true_yield_true)
						this->updateRelationalRange(conditional_op, true, lhs, rhs_casted, true);	

					if(branch_false_yield_true)
						this->updateRelationalRange(conditional_op, false, lhs, rhs_casted, true);
				
				}

				if(not branch_true_yield_false==branch_false_yield_false){

					if(branch_true_yield_false)
						this->updateRelationalRange(conditional_op, true, lhs, rhs_casted, false);
					
					if(branch_false_yield_false)
						this->updateRelationalRange(conditional_op, false, lhs, rhs_casted, false);	
				
				}

			}
			// If the lhs is a constant then do the same but with inverted branches
			/*else if(auto lhs_casted=mlir::dyn_cast<mlir::rlc::Constant>(lhs_op)){

				if(not branch_true_yield_true==branch_false_yield_true){

					if(branch_true_yield_true)
						this->updateRelationalRange(conditional_op, false, rhs, lhs_casted, true);	
					
					if(branch_true_yield_false)
						this->updateRelationalRange(conditional_op, false, rhs, lhs_casted, false);	
				
				}
				else{
					this->insertOrJoin(std::make_pair(rhs,true),this->getTOP());
				}

				if(not branch_true_yield_false==branch_false_yield_false){

					if(branch_false_yield_true)
						this->updateRelationalRange(conditional_op, true, rhs, lhs_casted, true);	
					
					if(branch_false_yield_false)
						this->updateRelationalRange(conditional_op, true, rhs, lhs_casted, false);	
					
				}
				else{
					this->insertOrJoin(std::make_pair(rhs,false),this->getTOP());
				}

			}
			*/
			// else do nothing

			if(*this!=copy){
				llvm::outs()<<"something should have changed\n";
				this->print(llvm::outs());
				return mlir::ChangeResult::Change;
			}
			else{
				return mlir::ChangeResult::NoChange;
			}
		}

		ConstraintsLattice::IntegerRange ConstraintsLattice::getTOP() const{
			return createRange(MIN,MAX);
		}
		
		ConstraintsLattice::IntegerRange ConstraintsLattice::getRangeOrTOP(const std::pair<mlir::Value,bool>& key) const{
			auto found = this->ranges.find(key);
			if(found!=this->ranges.end())
				return found->second;
			else
				return this->getTOP();
				
		}

		void ConstraintsLattice::insertOrJoin(const std::pair<mlir::Value,bool>& key, const ConstraintsLattice::IntegerRange& other){
			auto found=this->ranges.find(key);
			if(found!=this->ranges.end()){
				found->second= 
					ConstraintsLattice::joinRange(found->second, other);
			}
			else{
				this->ranges.insert(std::make_pair(key, other));
			}
		}

		void ConstraintsLattice::insertOrUpdate(const std::pair<mlir::Value,bool>& key, const ConstraintsLattice::IntegerRange& other){
			auto found=this->ranges.find(key);
			if(found!=this->ranges.end()){
				found->second=other;
			}
			else{
				this->ranges.insert(
					std::make_pair(key, other));
			}
		}

		void ConstraintsLattice::insertOrUpdate(const std::pair<mlir::Value,bool>& keyToInsert, const std::optional<Integer>& min, const std::optional<Integer>& max){
			auto found=this->ranges.find(keyToInsert);
			if(found!=this->ranges.end()){
				found->second=ConstraintsLattice::updateRange(found->second, min, max);
			}
			else{
				this->ranges.insert(
					std::make_pair(keyToInsert,
						createRange(
							min.value_or(MIN),
							max.value_or(MAX))));
			}
		}

		// This method takes the operation, the range
		// statement is the original operation
		// other is the other operand so that we can do to_update=result operation other
		void ConstraintsLattice::updateArithmeticCommutativeRange(const mlir::Operation* statement, const mlir::Value& result, const mlir::Value& to_update, const mlir::Value& other, const bool branch_to_update){
			ConstraintsLattice::IntegerRange new_range(APInteger(1),APInteger(0));
			// If the other is a constant take its constant value
			if(auto other_casted=mlir::dyn_cast<mlir::rlc::Constant>(other.getDefiningOp())){
				auto other_const=other_casted->getAttr("value").dyn_cast<IntegerAttr>().getInt();
				// Kinda ugly but this is what we have to do with constant ranges
				new_range=createRange(other_const);
			}
			// Else check if the value carried by the operation is 
			else {
				//If I have altready a range then i can modify it
				auto other_range=this->ranges.find(std::make_pair(other,branch_to_update));
				if(other_range!=this->ranges.end()){
					// This should call the copy constructor
					new_range=other_range->second;
				}
			}
			// Update only if I found the new range
			if(new_range.getLower().sle(new_range.getUpper())){

				auto a_range=this->getRangeOrTOP(std::make_pair(result,branch_to_update));
				// This operation has to be done indipendently from the branch
				// TODO: add more cases of course
				// TODO: with the llvm class this by default wraps around, do not do that thx
				if(mlir::isa<mlir::rlc::AddOp>(statement))
					new_range=a_range.sub(new_range);
				llvm::outs()<<"should have changed\n";
				// Insert the range where is needed
				this->insertOrUpdate(std::make_pair(to_update,branch_to_update),new_range);
			}
		}

		// This function is actually quite simpler since we always know 
		// the lhs is not a constant and the rhs is a constant
		// Result is useful to understand which check do to the operation
		void ConstraintsLattice::updateRelationalRange(const mlir::Operation* statement, bool result, const mlir::Value& to_update, const mlir::rlc::Constant& other, const bool branch_to_update){

			auto rhs_const=other->getAttr("value").dyn_cast<IntegerAttr>().getInt();

			std::optional<Integer> min;
			std::optional<Integer> max;

			// TODO: this with a typeswitch would be cute
			if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
					if(result)
					min=rhs_const+1;
					else
					max=rhs_const;
			}
			else if (mlir::isa<mlir::rlc::LessOp>(statement)){
				if(result)
					max=rhs_const-1;
				else
					min=rhs_const;
			}
			else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
				if(result)
					min=rhs_const;
				else
					max=rhs_const-1;
			}
			else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
				if(result)
					max=rhs_const;
				else
					min=rhs_const+1;
			}
			
			llvm::outs()<<"should have changed\n";
			// Insert the range where is needed
			this->insertOrUpdate(std::make_pair(to_update,branch_to_update),min,max);
		}

		//////////////////////////////////////////////////////////////////////////////
		// ConstraintsAnalysis METHODS
		//////////////////////////////////////////////////////////////////////////////

		// Definitions taken from https://mlir.llvm.org/doxygen/DenseAnalysis_8h_source.html#l00423

		void ConstraintsAnalysis::visitOperation(
			mlir::Operation *op, 
			const ConstraintsLattice &after,
			ConstraintsLattice *before){

			llvm::outs()<<"visiting operation: ";
			op->print(llvm::outs());
			llvm::outs()<<"\n";
			// For the moment implement the basic operations
			auto res=before->join(after);
			
			// We have to decide which return values to update (true or false)
			// In order to do so we need to :
			// - peek at the successors of this block 
			// - look at their lattice
			// - find in their lattice if there is some value with key true/false
			// - keep in mind that information

			if (mlir::rlc::ConstraintsAnalysis::isArithmetic(op)){

				//NB: here the operation can be done also implicitly -> actually should be done implicitly maybe
				auto can = before->branchesContainedInLattice();

				res = before->visitArithmetic(op,can.first,can.second) == mlir::ChangeResult::Change ?
						mlir::ChangeResult::Change : res;
				propagateIfChanged(before,res);
			}
			else if(mlir::rlc::ConstraintsAnalysis::isConditional(op)){

				// If it is a conditional then I need to peek in the next block from each branch

				res = before->visitBranch(op,this) == mlir::ChangeResult::Change ?
					mlir::ChangeResult::Change : res;
				propagateIfChanged(before, res);

			}
			else if(mlir::rlc::ConstraintsAnalysis::isYield(op)){

				// If it is a yield then it means that the function already inherently contains
				// the information that the function returns something

				res = before->visitYield(op) == mlir::ChangeResult::Change ?
					mlir::ChangeResult::Change : res;
				propagateIfChanged(before, res);

			}
			else if(mlir::rlc::ConstraintsAnalysis::isBuiltinAssign(op)){
			
				// Call the function which will call the yield
				res = before->visitBuiltinAssign(op,this) == mlir::ChangeResult::Change ?
					mlir::ChangeResult::Change : res;
				propagateIfChanged(before, res);
			}
			// This is for the instructions which do not modify the lattice
			else
				propagateIfChanged(before, res);
		}

		void ConstraintsAnalysis::visitCallControlFlowTransfer(
				CallOpInterface call,
				mlir::dataflow::CallControlFlowAction action,
				const ConstraintsLattice& before,
				ConstraintsLattice* after){
			
			llvm::outs()<<"visited call control flow transfer of operation: ";
			call.print(llvm::outs());
			propagateIfChanged(after, after->copy(before));
		}

		void ConstraintsAnalysis::visitRegionBranchControlFlowTransfer(
				mlir::RegionBranchOpInterface branch,
				mlir::RegionBranchPoint regionFrom,
				mlir::RegionBranchPoint regionTo,
				const ConstraintsLattice& before,
				ConstraintsLattice* after){

			//auto casted = mlir::dyn_cast_or_null<mlir::rlc::CondBranch>(op);
			//getLattice(mlir::ProgramPoint(&casted.getTrueBranch()->front()));

			// TODO: this should be called also in rlc.crb
			// For the moment execute only the join
			llvm::outs()<<"visited region branch control flow transfer of operation: ";
			branch.getOperation()->print(llvm::outs());
			llvm::outs()<<"\nregion from: ";
			if(auto regionf=regionFrom.getRegionOrNull()){
				regionf->front().print(llvm::outs());
			}
			else
				llvm::outs()<<"null\n";
			llvm::outs()<<"region to: ";
			if(auto regiont=regionTo.getRegionOrNull()){
				regiont->front().print(llvm::outs());
			}
			else
				llvm::outs()<<"null\n";
			auto res1=after->setCurrentBranch(&branch);
			//auto res1=mlir::ChangeResult::Change;
			auto res2=after->join(before);
			auto res= res1==mlir::ChangeResult::Change ? res1 : (res2==mlir::ChangeResult::Change ? res2 : mlir::ChangeResult::NoChange);
			
			// Let's try this to have a bit of fun -> ChangeResult is an enum (this should not work
			propagateIfChanged(after,res);
		}

		// At the end we can take the information about the arguments in our lattice and attach it
		// to the function arguments
		// TODO: understand when this is called and where to append the information obtained when finishing
		// For the moment this function is just observing the IR, so we can leave it as it is
		void ConstraintsAnalysis::setToExitState(ConstraintsLattice* lattice){
		
			llvm::outs()<<"something was set to exit state\n";
		}

		void ConstraintsAnalysis::printRanges(mlir::Operation* op){
			auto* lattice = op != nullptr ? getLattice(mlir::ProgramPoint(op))
																		: getLattice(op->getBlock());
			llvm::outs()<<"KEK ";
			lattice->print(llvm::outs());
		}

		const llvm::DenseMap<std::pair<mlir::Value,bool>,mlir::rlc::ConstraintsLattice::IntegerRange>& ConstraintsAnalysis::getUnderlyingLattice(mlir::Operation* op){
			return this->getLattice(mlir::ProgramPoint(op))->getUnderlyingLattice();
		}

		const mlir::rlc::ConstraintsLattice* ConstraintsAnalysis::getObjLattice(mlir::Operation* op){
			return this->getLattice(mlir::ProgramPoint(op));
		}

		// TODO: should I put it in another namespace ? (as in "ActionLiveness.cpp")
		// This is the class that starts the analysis (I guess)
		class Constraints
		{
			public:
			explicit Constraints(mlir::rlc::FlatFunctionOp fun)
					: function(fun), config()
			{
				config.setInterprocedural(false);
				solver = std::make_unique<DataFlowSolver>(config);
				//TODO: understand what are these -> other analysis? so should I keep them ? I think I should
				solver->load<mlir::dataflow::DeadCodeAnalysis>();
				solver->load<mlir::dataflow::SparseConstantPropagation>();
				analysis = solver->load<ConstraintsAnalysis>(symbolTable);
				auto top = fun;
				llvm::outs()<<"Initialize analysis from operation: ";
				top->print(llvm::outs());
				auto res = solver->initializeAndRun(top);
			}
			
			void printRangesFound(mlir::Operation* op){
				llvm::outs()<<"Printing operation: ";
				op->print(llvm::outs());
				llvm::outs()<<"\n";
				this->analysis->printRanges(op);
			}

			const llvm::DenseMap<std::pair<mlir::Value,bool>,mlir::rlc::ConstraintsLattice::IntegerRange>& getOperationLattice(mlir::Operation* op){
				return this->analysis->getUnderlyingLattice(op);
			}

			private:
			mlir::SymbolTableCollection symbolTable;
			mlir::rlc::FlatFunctionOp function;
			DataFlowConfig config;
			std::unique_ptr<DataFlowSolver> solver;
			ConstraintsAnalysis* analysis;
		};

	}	 // namespace


#define GEN_PASS_DEF_CONSTRAINTSANALYSISPASS
#include "rlc/dialect/Passes.inc"

	struct ConstraintsAnalysisPass
			: impl::ConstraintsAnalysisPassBase<ConstraintsAnalysisPass>
	{
		using impl::ConstraintsAnalysisPassBase<
				ConstraintsAnalysisPass>::ConstraintsAnalysisPassBase;

		void runOnOperation() override
		{
			//Perform the analysis only if the function returns a boolean
			for (auto fun : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
			{
				
				if(not mlir::isa<mlir::rlc::BoolType>(fun.getResult().getType().getResult(0)) )
					continue;

				if(fun.getBlocks().size()<=2)
					continue;

				Constraints con(fun);
				std::cout<<"In theory I should have run the analysis"<<std::endl;
				// Here I append to the function the ranges I found
				// TODO: understand why the analysis is done backwards but the blocks are traversed forwards
				// https://mlir.llvm.org/doxygen/DenseAnalysis_8cpp_source.html#l00387 <- I am just stupid, here it goes through the predecessors
				// Blocks have a getPredecessors() method
				auto block=fun.getRegion(0).front().getNextNode()->getNextNode();
				con.printRangesFound(&block->front());

				auto lattice = con.getOperationLattice(&block->front());

				// Iterate through the arguments -> are the arguments of the first block
				for(auto arg: fun.getRegion(0).front().getArguments()){

					auto pair_true=lattice.find(std::make_pair(arg,true));
					auto pair_false=lattice.find(std::make_pair(arg,false));

					// Better but still very ugly, think about another way to insert the attribute
					// Add range as a function argument
					if(pair_true!=lattice.end()){
						fun->setAttr(mlir::StringAttr::get(fun->getContext(),mlir::dyn_cast<mlir::StringAttr>(fun.getArgNames()[arg.getArgNumber()]).getValue().str()+"_T"), 
							mlir::StringAttr::get(fun->getContext(),
							"[" + std::to_string(pair_true->second.getLower().getSExtValue()) + 
							"," + std::to_string(pair_true->second.getUpper().getSExtValue()) + ")"));
					}
					if(pair_false!=lattice.end()){
						fun->setAttr(mlir::StringAttr::get(fun->getContext(),mlir::dyn_cast<mlir::StringAttr>(fun.getArgNames()[arg.getArgNumber()]).getValue().str()+"_F"), 
							mlir::StringAttr::get(fun->getContext(),
							"[" + std::to_string(pair_false->second.getLower().getSExtValue()) + 
							"," + std::to_string(pair_false->second.getUpper().getSExtValue()) + ")"));
					}
				}

				// TODO : add normalization pass -> YieldBecomeNotConditionalBranches
				// TODO : wrap everything in an interface -> look at Interfaces.td and .cpp
				// UNDERSTAND HOW TO BE PRETTY
				// https://mlir.llvm.org/docs/Interfaces/
			}
		}
	};
}	 // namespace mlir::rlc

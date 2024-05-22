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

namespace mlir::rlc
{
	namespace
	{
		// Represents the pair (min, max) of values that can be assigned to a variable 
		// in a function

		// We need an abstraction to represent the range, because we need to 
		// define our particular bounds. 
		// Indeed we require an uninitialized state (we call it BOTTOM) and a TOP
		// NB: TOP is the maximum practical value the variable can assume -> for the lhs is INT_MIN and rhs is INT_MAX
		// TODO: what to do when we finish in an invalid range? E.G. (1,-1)
		class IntegerRange{
			public:
			
			// Useful constants
			typedef int Integer;
			constexpr static Integer MAX=INT32_MAX;
			constexpr static Integer MIN=INT32_MIN;

			// TODO: maybe "default" can work too
			IntegerRange() : range(std::nullopt,std::nullopt) {};
			// Constructors for partial ranges
			IntegerRange(std::nullopt_t nulltype, const Integer max) : range(std::nullopt,max) {};
			IntegerRange(const Integer min, std::nullopt_t nulltype) : range(min,std::nullopt) {};
			// Constructor for full range
			IntegerRange(const Integer min, const Integer max) : range(min,max) {};
			// Casting operator from std::pair
			IntegerRange(const std::pair<Integer,Integer>& other) : range(other.first,other.second) {};
			// Copy operator
			// TODO: maybe can be removed because we pass from the casting
			IntegerRange& operator=(const std::pair<Integer,Integer>& other){
				
				this->range=other;
				return *this;
			}

			//IntegerRange& operator=(IntegerRange&  other)=default;
			//IntegerRange& operator=(IntegerRange&& other)=default;
			
			// Possibly useful setters (maybe avoidable)
			void setMin(const Integer min){
				this->range.first = std::optional<Integer>(min);
			}

			void setMax(const Integer max){
				this->range.second = std::optional<Integer>(max);
			}
			// Overload with optionals
			void setMin(const std::optional<Integer>& min){
				// NB: this should call the copy constructor
				this->range.first=min;
			}
			void setMax(const std::optional<Integer>& max){
				this->range.second=max;
			}

			void setMinUnbounded(){
				this->range.first = std::nullopt;
			}

			void setMaxUnbounded(){
				this->range.first = std::nullopt;
			}

			void setConstant(const Integer con){
				this->range.first =con;
				this->range.second=con;
			}

			//Useful getters (mainly for debug reasons)

			Integer getMin() const {
				return this->range.first.has_value() ? this->range.first.value() : MIN;
			}

			Integer getMax() const {
				return this->range.second.has_value() ? this->range.second.value() : MAX;
			}

			// Operators useful for range operations 
			// TODO: this needs to do a bound checking
			
			// When performing the addition of two integer ranges I have to be careful
			IntegerRange& operator+=(const IntegerRange& rhs){

				// Operate only if we know the bound on our range and the lhs
				if(this->range.first.has_value() and rhs.range.first.has_value()){
					this->range.first.value() += rhs.range.first.value();
				}
				if(this->range.second.has_value() and rhs.range.second.has_value()){
					this->range.second.value() += rhs.range.second.value();
				}
				
				return *this;
			}
			// For a std::pair just cast it to an IntegerRange (it is ugly but it does the job)
			IntegerRange& operator+=(const std::pair<Integer,Integer>& rhs){
				return this->operator+=(IntegerRange(rhs));
			}
			IntegerRange& operator-=(const IntegerRange& rhs){

				// Operate only if we know the bound on our range and the lhs
				if(this->range.first.has_value() and rhs.range.first.has_value()){
					this->range.first.value() -= rhs.range.first.value();
				}
				if(this->range.second.has_value() and rhs.range.second.has_value()){
					this->range.second.value() -= rhs.range.second.value();
				}
				
				return *this;
			}
			IntegerRange& operator-=(const std::pair<Integer,Integer>& rhs){
				return this->operator-=(IntegerRange(rhs));
			}

			friend IntegerRange operator+(IntegerRange& lhs, const std::pair<Integer,Integer>& rhs){
				lhs+=rhs;
				return lhs; // <- this uses the implicit move constructor (do not try to redefine it)
			}
			friend IntegerRange operator+(IntegerRange& lhs, const IntegerRange& rhs){
				lhs+=rhs;
				return lhs; // <- this uses the implicit move constructor (do not try to redefine it)
			}
			friend IntegerRange operator-(IntegerRange& lhs, const std::pair<Integer,Integer>& rhs){
				lhs-=rhs;
				return lhs; // <- this uses the implicit move constructor (do not try to redefine it)
			}
			friend IntegerRange operator-(IntegerRange& lhs, const IntegerRange& rhs){
				lhs-=rhs;
				return lhs; // <- this uses the implicit move constructor (do not try to redefine it)
			}

			// Useful operator for the insertion in a map (maybe not needed but better to leave it)
			bool operator==(const IntegerRange& other) const{
				return this->range.first==other.range.first and this->range.second==other.range.second;
			}

			// Join operation
			// Defined as: J((a,b),(c,d)):(min(a,c),max(b,d)) for each equal SSA value
			// NB: we have to ignore the BOTTOM
			static IntegerRange joinRange(const IntegerRange& r1, const IntegerRange& r2){
					const auto& r1_m=r1.range.first;
					const auto& r1_M=r1.range.second;
					const auto& r2_m=r2.range.first;
					const auto& r2_M=r2.range.second;

					// Range initialized to std::nullopt
					IntegerRange toReturn;

					// TODO: possibly refactor
					// MIN
					if(r1_m.has_value())
						toReturn.setMin(std::min(r1_m.value(),r2_m.value_or(MAX)));
					else if (r2_m.has_value())
						toReturn.setMin(r2_m.value());
					
					// MAX
					if(r1_M.has_value())
						toReturn.setMax(std::max(r1_M.value(),r2_M.value_or(MIN)));
					else if (r2_M.has_value())
						toReturn.setMax(r2_M.value());


					return toReturn;
					
				}

			private:

			std::pair<std::optional<Integer>,std::optional<Integer>> range;

		};

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

			using Integer=mlir::rlc::IntegerRange::Integer;
			constexpr static Integer MAX=INT32_MAX;
			constexpr static Integer MIN=INT32_MIN;

			MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConstraintsLattice);
			explicit ConstraintsLattice(mlir::ProgramPoint point)
					: mlir::dataflow::AbstractDenseLattice(point)
			{
			}

			// Method for returning the lattice (useful only at the end) <- maybe can do everything in the class
			const llvm::DenseMap<mlir::Value,IntegerRange>& getUnderlyingLattice() const{
				return this->ranges;
			}

			//TODO: understand what this meet does
			// In theory it should be the opposite of a join
			mlir::ChangeResult meet(
					const mlir::dataflow::AbstractDenseLattice& val) override
			{
				llvm::outs()<<"meet\n";
				//return mlir::ChangeResult::NoChange;
				// Do a funny thing and let the meet perform the join
				return this->join(val);
			}

			mlir::ChangeResult join(
					const mlir::dataflow::AbstractDenseLattice& val) override
			{
				//Need to cast it
				const auto& other = *static_cast<const ConstraintsLattice*>(&val);
				
				//Keep it to check if there is no change
				auto copy = this->ranges;

				llvm::outs()<<"Did i join?\n";
				this->print(llvm::outs());
				other.print(llvm::outs());

				//Insert the other in this
				for(auto other_range : other.ranges){
					
					// We have to insert the other range
					// If we find it then we have to join, else we have to insert it
					this->insertOrJoin(other_range.first, other_range.second);
				}

				//
				if (this->ranges == copy){
					llvm::outs()<<"no\n";
					this->print(llvm::outs());
					return mlir::ChangeResult::NoChange;
				}
				this->print(llvm::outs());
				return mlir::ChangeResult::Change;
			}

			// Operation to copy two lattices
			// NB: this is a deep copy
			mlir::ChangeResult copy(const ConstraintsLattice& other)
			{
				llvm::outs()<<"Did i copy?\n";
				llvm::outs()<<"mine : ";this->print(llvm::outs());
				llvm::outs()<<"other: ";other.print(llvm::outs());
				if (this->ranges == other.ranges){
					llvm::outs()<<"no\n";
					return mlir::ChangeResult::NoChange;
				}
				this->ranges = other.ranges;
				return mlir::ChangeResult::Change;
			}

			mlir::ChangeResult copy(const mlir::dataflow::AbstractDenseLattice& val){
				return this->copy(*static_cast<const ConstraintsLattice*>(&val));
			}

			void print(raw_ostream& os) const override
			{
				if(not this->ranges.empty()){
					for(auto range: this->ranges){
						os << "SSA: "<< range.first <<" RANGE: (" << range.second.getMin() << ", " << range.second.getMax() << ")\n";
					}
				}
				else {
					os<<"Empty ranges XD \n";
				}
			}

			bool operator==(const ConstraintsLattice& other) const
			{
				return this->ranges == other.ranges;
			}

			// Here we should write the transfer function based on the operation 
			mlir::ChangeResult visitOperation(mlir::Operation* statement)
			{
				// Here the operation has passed the sanity check so we can switch
				bool changed=false;

				// a = b + c
				// TODO: refactor everything as this
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
					// We can perform the operation if we know the bounds on a
					// TODO: when there is something already it should be a JOIN
					// Here our range can assume a found value in the map or not (use BOTTOM)
					auto a_range=this->getRangeOrBottom(a);
					// TODO: understand what happens if both have ranges -> should I update both ? (because if yes, in which order ?)
					// NB: there iConstraintsLattices the case in which both are non constantv-> if we know nothing about
					//     both we do nothing
					// Then first find which variable we need to operate on
					mlir::Value      state=nullptr;
					mlir::Value      stato=nullptr;
					//mlir::Operation* opera=nullptr;
					mlir::Operation* other=nullptr; // This is the one we know the bounds of
					IntegerRange new_range(1,0);
					// First get our candidates
					// NB: this can be done only if the operation is associative
					if(b_op == nullptr or not mlir::isa<mlir::rlc::Constant>(b_op)){
						state=b;
						stato=c;
						//opera=b_op;
						other=c_op;
					}
					else if(c_op == nullptr or not mlir::isa<mlir::rlc::Constant>(c_op)){
						state=c;
						stato=b;
						//opera=c_op;
						other=b_op;
					}
					// Then extract infromation about the ranges
					if(mlir::isa<mlir::rlc::Constant>(other)){
						auto other_const=other->getAttr("value").dyn_cast<IntegerAttr>().getInt();
						new_range.setConstant(other_const);
					}
					else {
						//TODO: think about this
						auto other_range=this->ranges.find(stato);
						if(other_range!=this->ranges.end()){
							// This should call the copy constructor
							new_range=other_range->second;
						}
					}
					// If i have a valid range (i.e. if i modified the initial range which is not valid)
					// then modify or add the new range 
					if(new_range.getMin()<=new_range.getMax()){
						//Calculate the new range
						new_range=a_range-new_range;
						llvm::outs()<<"should have changed\n";
						this->insertOrJoin(state,new_range);
						changed=true;
					}
				} // if (not mlir::isa<mlir::rlc::AddOp>(statement))
				// These should be pretty much the same  -> TODO: refactor
				// I do not understand why but in Operations.td the syntax is (lhs,rhs) but in the actual  print IR is reversed (or i am crazy, that is possible too)
				// else it is a greater/less (and equal)
				else {
					// TODO: fix lhs and rhs in Operations.td 
					const auto& lhs = statement->getOperand(0);
					const auto& rhs = statement->getOperand(1);
					const auto& lhs_op = lhs.getDefiningOp();
					const auto& rhs_op = rhs.getDefiningOp();
					IntegerRange new_range;
					// First we check if our rhs is a constant (easy case)
					// TODO: watch TypeSwitch
					if(mlir::isa<mlir::rlc::Constant>(rhs_op)){
						//And the other one is not a constant
						//TODO: appartently the arguments must be treated in a careful way -> they do not have a operation defining them
						if(lhs_op==nullptr or not mlir::isa<mlir::rlc::Constant>(lhs_op)){
							auto rhs_const=rhs_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
							llvm::outs()<<"Constant value: "<<rhs_const<<"\n";
							// TODO: remember to check if i am passing through a true or false branch, for the moment leave it as true
							if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
								new_range.setMin(rhs_const+1);
							}
							else if (mlir::isa<mlir::rlc::LessOp>(statement)){
								new_range.setMax(rhs_const-1);
							}
							else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
								new_range.setMin(rhs_const);
							}
							else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
								new_range.setMax(rhs_const);
							}
							//If it is present then update it, else insert it
							this->insertOrJoin(lhs,new_range);
							changed=true;
						} // if(lhs_op!=nullptr and mlir::isa<mlir::rlc::Constant>(lhs_op))
					} // if(not mlir::isa<mlir::rlc::Constant>(rhs_op))
					// Else we basically have to do the same checkings but carefully 
					// because we have to reverse the operation
					else if(mlir::isa<mlir::rlc::Constant>(lhs_op)){
						if(rhs_op==nullptr or not mlir::isa<mlir::rlc::Constant>(rhs_op)){
							auto lhs_const=lhs_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
							llvm::outs()<<"Constant value: "<<lhs_const<<"\n";
							// TODO: remember to check if i am passing through a true or false branch, for the moment leave it as true
							if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
								new_range.setMax(lhs_const-1);
							}
							else if (mlir::isa<mlir::rlc::LessOp>(statement)){
								new_range.setMin(lhs_const+1);
							}
							else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
								new_range.setMax(lhs_const);
							}
							else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
								new_range.setMin(lhs_const);
							}
							//If it is present then update it, else insert it
							this->insertOrJoin(rhs,new_range);
							changed=true;
						} // if(rhs_op!=nullptr and mlir::isa<mlir::rlc::Constant>(rhs_op))
					} //if(not mlir::isa<mlir::rlc::Constant>(lhs_op))
				}

				if(changed){
					llvm::outs()<<"something should have changed\n";
					this->print(llvm::outs());
					return mlir::ChangeResult::Change;
				}
				else{
					return mlir::ChangeResult::NoChange;
				}
			}

			private:

			IntegerRange getBottom(){
				return IntegerRange();
			}
			
			IntegerRange getRangeOrBottom(mlir::Value key){
				auto found = this->ranges.find(key);
				if(found!=this->ranges.end())
					return found->second;
				else
					return this->getBottom();
				
			}

			bool isBottom(mlir::Value key){
				return this->ranges.find(key)==this->ranges.end();
			}

			void insertOrJoin(mlir::Value keyToInsert, IntegerRange rangeToInsert){
				auto found=this->ranges.find(keyToInsert);
				if(found!=this->ranges.end()){
					found->second=
						mlir::rlc::IntegerRange::joinRange(found->second, rangeToInsert);
				}
				else{
					this->ranges.insert(std::make_pair(keyToInsert,rangeToInsert));
				}
			}

			// TODO: make our life harder, distinguish betweeen true and false execution paths
			// The lattice is attached to an operation, so we save a map containing all the values calculated
			// by the operation
			llvm::DenseMap<mlir::Value,IntegerRange> ranges;
		};

		class ConstraintsAnalysis
				: public mlir::dataflow::DenseBackwardDataFlowAnalysis<
							ConstraintsLattice>
		{
			public:
			using mlir::dataflow::DenseBackwardDataFlowAnalysis<
					ConstraintsLattice>::DenseBackwardDataFlowAnalysis;

			// Utility
			// TODO: understand if i can collapse
			static bool isOfInterest(mlir::Operation* op){
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
				else if (mlir::isa<mlir::rlc::GreaterOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::LessOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::GreaterEqualOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::LessEqualOp>(op))
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
                ConstraintsLattice *before) override
			{
				llvm::outs()<<"visiting operation: ";
				op->print(llvm::outs());
				llvm::outs()<<"\n";
				// For the moment implement the basic operations
				auto res=before->copy(after);
				if (mlir::rlc::ConstraintsAnalysis::isOfInterest(op))
					propagateIfChanged(before,before->visitOperation(op));
				
				// This is for the instructions which do not modify the lattice
				else
					propagateIfChanged(before, res);
			}

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
					ConstraintsLattice* after) final
			{
				llvm::outs()<<"visited call control flow transfer\n";
				propagateIfChanged(after, after->copy(before));
			}

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
					ConstraintsLattice* after) override
			{
				// TODO: this should be called also in rlc.crb
				// For the moment execute only the join
				llvm::outs()<<"visited region branch control flow transfer\n";
				llvm::outs()<<"region from: ";
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
				propagateIfChanged(after, after->join(before));
			}

			// At the end we can take the information about the arguments in our lattice and attach it
			// to the function arguments
			// TODO: understand when this is called and where to append the information obtained when finishing
			// For the moment this function is just observing the IR, so we can leave it as it is
			void setToExitState(ConstraintsLattice* lattice) override
			{
				
				llvm::outs()<<"something was set to exit state\n";
				
				// This retrieves a pair key value
				for(auto pair: lattice->getUnderlyingLattice()){
					// Check if it is indeed a block argument
					const auto& firstBlock=(*pair.first.getParentBlock()->getPredecessors().begin());
					bool isValid=false;
					for(auto arg: firstBlock->getArguments()){
						//Then it is a block argument and insert the range found
						if(arg==pair.first){
							isValid=true;
						}
					}

					if(not isValid)
						continue;

					// Traverse the parent operations until i arrived to a FlatFunctionOp
					auto parentOp=pair.first.getDefiningOp()->getParentOp();
					while(not mlir::isa<mlir::rlc::FlatFunctionOp>(parentOp)){
						parentOp=parentOp->getParentOp();
					}
					// Cast it
					mlir::rlc::FlatFunctionOp parentFunc=mlir::dyn_cast<mlir::rlc::FlatFunctionOp>(parentOp);
					// Add the arguments
					std::string name;
					llvm::raw_string_ostream ostring(name);
					pair.first.print(ostring);
					//parentFunc.setAttr("rlc.attr"+name+"min",mlir::IntegerAttr::get(mlir::IntegerType::get(parentFunc->getContext(),32),INT_MIN));
					//parentFunc.setAttr("rlc.attr"+name+"MAX",mlir::IntegerAttr::get(mlir::IntegerType::get(parentFunc->getContext(),32),INT_MAX));
				}
				
			}

			public:

			void printRanges(mlir::Operation* op){
				auto* lattice = op != nullptr ? getLattice(mlir::ProgramPoint(op))
																			: getLattice(op->getBlock());
				llvm::outs()<<"KEK ";
				lattice->print(llvm::outs());
			}
		};

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
			for (auto fun : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
			{

				Constraints con(fun);
				std::cout<<"In theory I should have run the analysis"<<std::endl;
				// Here I append to the function the ranges I found
				// TODO: understand why the analysis is done backwards but the blocks are traversed forwards
				// https://mlir.llvm.org/doxygen/DenseAnalysis_8cpp_source.html#l00256 <- well this explains a lot
				// Blocks have a getPredecessors() method
				auto& block=fun.getRegion(0).back();
				con.printRangesFound(&block.front());
			}
		}
	};
}	 // namespace mlir::rlc
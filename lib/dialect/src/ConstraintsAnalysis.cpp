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
		
		// TODO: I could think about implementing disjointed ranges but the algorithms starts to become 
		// very complicated (think about a multiplication, definitely not trivial)
		
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
		
		// TODO: if I feel crazy I could do a state pattern

		// This enum represents a sort of lattice for checking which branch I am passing through
		// It is useful to understand which data structures have to be updated
		// Why is it a lattice ? Because it can be ordered:
		// UNKNOWN < TRUE/FALSE < BOTH
		// NB: TRUE/FALSE states do not intersect (if they do they automatically finish in BOTH)
		enum class CURRENT_BRANCH{
			B_UNKNOWN,
			B_TRUE,
			B_FALSE,
			B_BOTH
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
			const llvm::DenseMap<mlir::Value,IntegerRange>& getUnderlyingLatticeTrue() const{
				return this->ranges_true;
			}
			const llvm::DenseMap<mlir::Value,IntegerRange>& getUnderlyingLatticeFalse() const{
				return this->ranges_false;
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
				auto copy = *this;

				llvm::outs()<<"Did i join?\n";
				this->print(llvm::outs());
				other.print(llvm::outs());

				// I have to join the information about the branch
				if(this->curr_branch==mlir::rlc::CURRENT_BRANCH::B_UNKNOWN)
					this->curr_branch=other.curr_branch;
				else if((this->curr_branch==mlir::rlc::CURRENT_BRANCH::B_TRUE and other.curr_branch==mlir::rlc::CURRENT_BRANCH::B_FALSE) or
					    (this->curr_branch==mlir::rlc::CURRENT_BRANCH::B_FALSE and other.curr_branch==mlir::rlc::CURRENT_BRANCH::B_TRUE) or
						other.curr_branch==mlir::rlc::CURRENT_BRANCH::B_BOTH)
					this->curr_branch=mlir::rlc::CURRENT_BRANCH::B_BOTH;
				//If it does not have the value do not do anything

				//Insert the other in this
				for(auto other_range : other.ranges_true){
					
					// We have to insert the other range
					// If we find it then we have to join, else we have to insert it
					this->insertOrJoinTrue(other_range.first, other_range.second);
				}
				for(auto other_range : other.ranges_false){
					
					// We have to insert the other range
					// If we find it then we have to join, else we have to insert it
					this->insertOrJoinFalse(other_range.first, other_range.second);
				}

				//
				if (*this==copy){
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

				if (*this==other){
					llvm::outs()<<"no\n";
					return mlir::ChangeResult::NoChange;
				}
				this->ranges_true = other.ranges_true;
				this->ranges_false= other.ranges_false;
				// TODO: maybe this has to be a join ?
				this->curr_branch = other.curr_branch;
				return mlir::ChangeResult::Change;
			}

			mlir::ChangeResult copy(const mlir::dataflow::AbstractDenseLattice& val){
				return this->copy(*static_cast<const ConstraintsLattice*>(&val));
			}

			void print(raw_ostream& os) const override
			{
				os<<"Active branch: ";
				switch(this->curr_branch){
					case mlir::rlc::CURRENT_BRANCH::B_UNKNOWN:
						os<<"unknokwn\n";
						break;
					case mlir::rlc::CURRENT_BRANCH::B_TRUE:
						os<<"true\n";
						break;
					case mlir::rlc::CURRENT_BRANCH::B_FALSE:
						os<<"false\n";
						break;
					case mlir::rlc::CURRENT_BRANCH::B_BOTH:
						os<<"both\n";
						break;
					default: break;
				}
				os<<"True ranges:\n";
				if(not this->ranges_true.empty()){
					for(auto range: this->ranges_true){
						os << "SSA: "<< range.first <<" RANGE: (" << range.second.getMin() << ", " << range.second.getMax() << ")\n";
					}
				}
				else {
					os<<"Empty\n";
				}
				os<<"False ranges:\n";
				if(not this->ranges_false.empty()){
					for(auto range: this->ranges_false){
						os << "SSA: "<< range.first <<" RANGE: (" << range.second.getMin() << ", " << range.second.getMax() << ")\n";
					}
				}
				else {
					os<<"Empty\n";
				}
			}

			bool operator==(const ConstraintsLattice& other) const
			{
				return this->ranges_true == other.ranges_true 
					and this->ranges_false == other.ranges_false 
					and this->curr_branch == other.curr_branch;
			}

			mlir::ChangeResult setCurrentBranch(mlir::RegionBranchOpInterface* branchOp){
				// The yield has this format
				// %12 = rlc.constant true !rlc.bool
    			// rlc.yield %12 : !rlc.bool
				// So I need to retrieve "true" in this case

				auto copy = this->curr_branch;

				auto branch=
					branchOp->getOperation() // gets the rlc.yield
					->getOperand(0) // get the first operand of the yield (%12:!rlc.bool)
					.getDefiningOp() // get its operation
					->getAttr("value"); //get the value of the constant

				// We can only do this analysis if the function return true or false
				if(auto bool_branch=mlir::dyn_cast<BoolAttr>(branch)){
					// Force to work on one branch
					//if(!bool_branch.getValue())
					// TODO: this should not need a check because if I am already in a state i should not pass from here (hopefully)
					this->curr_branch=bool_branch.getValue() ? mlir::rlc::CURRENT_BRANCH::B_TRUE : mlir::rlc::CURRENT_BRANCH::B_FALSE;
				}

				if(this->curr_branch==copy)
					return mlir::ChangeResult::NoChange;
				else
					return mlir::ChangeResult::Change;
			}

			// Recursive function to get if next blocks yield true or false	
			mlir::rlc::CURRENT_BRANCH visitNextBlocks(mlir::Block* block, mlir::rlc::CURRENT_BRANCH known) const{
				// Look recursively at each successor
				auto my_ret=known;
				if(block->getNumSuccessors()>0){
					for(auto succ : block->getSuccessors()){
						my_ret=visitNextBlocks(succ, my_ret);
					}
				}
				// When finished the successors look at this block if it has a yield operation at the end
				if(auto casted=mlir::dyn_cast<mlir::rlc::Yield>(block->getTerminator())){
					
					const auto ret=mlir::dyn_cast<BoolAttr>(
						casted.getOperand(0).getDefiningOp()->getAttr("value")).getValue();
					
					// Sort of join operation
					if(my_ret==mlir::rlc::CURRENT_BRANCH::B_UNKNOWN)
						my_ret=ret ? mlir::rlc::CURRENT_BRANCH::B_TRUE : mlir::rlc::CURRENT_BRANCH::B_FALSE;
					else if((my_ret==mlir::rlc::CURRENT_BRANCH::B_TRUE and not ret) or
						(my_ret==mlir::rlc::CURRENT_BRANCH::B_FALSE and ret))
						my_ret=mlir::rlc::CURRENT_BRANCH::B_BOTH;
				}
				// If i did not find a yield then this value is not updated
				return my_ret;
			}

			// Here we should write the transfer function based on the operation 
			mlir::ChangeResult visitOperation(mlir::Operation* statement)
			{

				// Modify my ranges only when i am sure that i am passing through a initialized (true/false) branch
				if (this->curr_branch==mlir::rlc::CURRENT_BRANCH::B_UNKNOWN)
					return mlir::ChangeResult::NoChange;

				// Here the operation has passed the sanity check so we can switch
				bool changed=false;

				// So this becomes extremely tricky now, we have the information about the branch we are passing from
				// The idea is, i am visiting one branch => update one, I am visiting both => update both

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
						auto other_range=this->ranges_true.find(stato);
						if(other_range!=this->ranges_true.end()){
							// This should call the copy constructor
							new_range=other_range->second;
						}
					}
					// TODO: think what happens when i am passing through the false range
					// If i have a valid range (i.e. if i modified the initial range which is not valid)
					// then modify or add the new range 
					if(new_range.getMin()<=new_range.getMax()){

						// Calculate the new range
						auto new_range_true=new_range;
						auto new_range_false=new_range;
						auto a_range_true=this->getRangeOrBottomTrue(a);
						auto a_range_false=this->getRangeOrBottomFalse(a);
						// This operation has to be done indipendently from the branch
						new_range_true=a_range_true-new_range;
						new_range_false=a_range_false-new_range;
						llvm::outs()<<"should have changed\n";
						// Insert the range where is needed
						switch(this->curr_branch){
							case mlir::rlc::CURRENT_BRANCH::B_BOTH :
								this->insertOrJoinTrue(state,new_range_true);
								this->insertOrJoinFalse(state,new_range_false);
							break;
							case mlir::rlc::CURRENT_BRANCH::B_TRUE :
								this->insertOrJoinTrue(state,new_range_true);
							break;
							case mlir::rlc::CURRENT_BRANCH::B_FALSE :
								this->insertOrJoinFalse(state,new_range_false);
							break;
							default : break;
						}
						changed=true;
					}
				} // if (not mlir::isa<mlir::rlc::AddOp>(statement))
				// These should be pretty much the same  -> TODO: refactor
				// I do not understand why but in Operations.td the syntax is (lhs,rhs) but in the actual  print IR is reversed (or i am crazy, that is possible too)
				// else it is a greater/less (and equal)
				else{
					
					// TODO: We have another huge problem now.
					// We know if we return true or false from the function.
					// But now with that information we really cannot do anything, the important
					// information that we have to keep track of is the current branch from which we 
					// are moving in the analysis
					// E.G if(a>0) //branch_true else //branch_false 
					// We have to keep track of the branch we are in
					// rlc.crb %7 !rlc.bool ^bb4 ^bb3
					// We are interested here from which basic block we came from (3 or 4 == false or true)

					// The only feasible way that I see is looking at all the successors of this block
					// And see if at some point I arrive to a return true or return false
					// If i see both then no problem update both
					// Well... this solution is a recursive call in the subgraph of the problem
					// It can explode in complexity... maybe think about implementing before a forward analysis ? Who knows...

					// We can assume that a control operation is always the last in its basic block
					// NB: always true but it can be either a conditional or a simple jump

					// NB: the documentation says that a mlir::Block at the end has a "Terminator Operation" -> specifies the successors

					// TODO: after doing this another idea came to my mind 
					// This information needs to be computed only once per block, so we can just do that at the start
					// Idea: before running the analyisis compute this information and store it in a 
					// llvm::DenseMap<mlir::Block,std::pair<CURRENT_BRANCH,CURRENT_BRANCH>>
					// Then the analysis can peek into this map to see the information we need
					// TODO: maybe this can avoid us the problem of keeping track of our current branch <- i am not sure how to prove this, maybe testing is enough
					// Then when looking at an operation: 
					// 		update the yield true/false depending on the result of each branch (stored in the map above)

					// Visit the true block and the false block of the operation
					auto res0=this->visitNextBlocks(statement->getNextNode()->getSuccessor(0),mlir::rlc::CURRENT_BRANCH::B_UNKNOWN);
					auto res1=this->visitNextBlocks(statement->getNextNode()->getSuccessor(1),mlir::rlc::CURRENT_BRANCH::B_UNKNOWN);

					llvm::outs()<<"Visited both branches of the conditional and this are the results\n";
					statement->getNextNode()->print(llvm::outs());
					llvm::outs()<<"\nTrue: ";
					switch(res0){
						case mlir::rlc::CURRENT_BRANCH::B_BOTH :
							llvm::outs()<<"both\n";
						break;
						case mlir::rlc::CURRENT_BRANCH::B_TRUE :
							llvm::outs()<<"true\n";
						break;
						case mlir::rlc::CURRENT_BRANCH::B_FALSE :
							llvm::outs()<<"false\n";
						break;
						default : break;
					}
					llvm::outs()<<"False: ";
					switch(res1){
						case mlir::rlc::CURRENT_BRANCH::B_BOTH :
							llvm::outs()<<"both\n";
						break;
						case mlir::rlc::CURRENT_BRANCH::B_TRUE :
							llvm::outs()<<"true\n";
						break;
						case mlir::rlc::CURRENT_BRANCH::B_FALSE :
							llvm::outs()<<"false\n";
						break;
						default : break;
					}

					// What do I do with this info? Now I know which regions I will be visiting
					// So i need to keep two ranges (for the true and false) and then update my info
					// in the following manner:
					// I know my function will return true (for ex.) then i have to see if from the 
					// true branch i can see it -> update and do same for false branch

					// TODO: fix lhs and rhs in Operations.td 
					const auto& lhs = statement->getOperand(0);
					const auto& rhs = statement->getOperand(1);
					const auto& lhs_op = lhs.getDefiningOp();
					const auto& rhs_op = rhs.getDefiningOp();

					IntegerRange new_range_true;
					IntegerRange new_range_false;
					// First we check if our rhs is a constant (easy case)
					// TODO: watch TypeSwitch
					if(mlir::isa<mlir::rlc::Constant>(rhs_op)){
						// And the other one is not a constant
						// TODO: appartently the arguments must be treated in a careful way -> they do not have a operation defining them
						if(lhs_op==nullptr or not mlir::isa<mlir::rlc::Constant>(lhs_op)){
							auto rhs_const=rhs_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
							llvm::outs()<<"Constant value: "<<rhs_const<<"\n";
							// TODO: now that I remember where I am passing from, think about refactoring
							if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMin(rhs_const+1);
								// else
								 	new_range_false.setMax(rhs_const);
							}
							else if (mlir::isa<mlir::rlc::LessOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMax(rhs_const-1);
								// else
								 	new_range_false.setMin(rhs_const);
							}
							else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMin(rhs_const);
								// else
								 	new_range_false.setMax(rhs_const-1);
							}
							else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMax(rhs_const);
								// else
								 	new_range_false.setMin(rhs_const+1);
							}
							//If it is present then update it, else insert it in the right data structure
							switch(this->curr_branch){
								case mlir::rlc::CURRENT_BRANCH::B_BOTH :
									switch (res0){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											this->insertOrJoinTrue(lhs,new_range_true);
											this->insertOrJoinFalse(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_true);
											break;
										default : break;
									}
									switch (res1){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											this->insertOrJoinTrue(lhs,new_range_false);
											this->insertOrJoinFalse(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_false);
											break;
										default : break;
									}
								break;
								case mlir::rlc::CURRENT_BRANCH::B_TRUE :
									switch (res0){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :

										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :

											break;
										default : break;
									}
									switch (res1){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											break;
										default : break;
									}
								break;
								case mlir::rlc::CURRENT_BRANCH::B_FALSE :
									switch (res0){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :

										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :

											break;
										default : break;
									}
									switch (res1){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											break;
										default : break;
									}
								break;
								default : break;
							}
							changed=true;
						} // if(lhs_op!=nullptr and mlir::isa<mlir::rlc::Constant>(lhs_op))
					} // if(not mlir::isa<mlir::rlc::Constant>(rhs_op))
					// Else we basically have to do the same checkings but carefully 
					// because we have to reverse the operation
					else if(mlir::isa<mlir::rlc::Constant>(lhs_op)){
						if(rhs_op==nullptr or not mlir::isa<mlir::rlc::Constant>(rhs_op)){
							auto lhs_const=lhs_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
							llvm::outs()<<"Constant value: "<<lhs_const<<"\n";
							// TODO: now that I remember where I am passing from, think about refactoring
							if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMax(lhs_const-1);
								// else
								 	new_range_false.setMin(lhs_const);
							}
							else if (mlir::isa<mlir::rlc::LessOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMin(lhs_const+1);
								// else
								 	new_range_false.setMax(lhs_const);
							}
							else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMax(lhs_const);
								// else
								 	new_range_false.setMin(lhs_const+1);
							}
							else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
								// if(this->last_branch)
									new_range_true.setMin(lhs_const);
								// else
								 	new_range_false.setMax(lhs_const-1);
							}
							//If it is present then update it, else insert it in the right data structure
							switch(this->curr_branch){
								case mlir::rlc::CURRENT_BRANCH::B_BOTH :
									switch (res0){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											this->insertOrJoinTrue(lhs,new_range_true);
											this->insertOrJoinFalse(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_true);
											break;
										default : break;
									}
									switch (res1){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											this->insertOrJoinTrue(lhs,new_range_false);
											this->insertOrJoinFalse(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_false);
											break;
										default : break;
									}
								break;
								case mlir::rlc::CURRENT_BRANCH::B_TRUE :
									switch (res0){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :

										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :

											break;
										default : break;
									}
									switch (res1){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											this->insertOrJoinTrue(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											break;
										default : break;
									}
								break;
								case mlir::rlc::CURRENT_BRANCH::B_FALSE :
									switch (res0){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :

										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_true);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :

											break;
										default : break;
									}
									switch (res1){
										case mlir::rlc::CURRENT_BRANCH::B_BOTH :
											break;
										case mlir::rlc::CURRENT_BRANCH::B_FALSE :
											this->insertOrJoinFalse(lhs,new_range_false);
											break;
										case mlir::rlc::CURRENT_BRANCH::B_TRUE :
											break;
										default : break;
									}
								break;
								default : break;
							}
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
			
			IntegerRange getRangeOrBottomTrue(mlir::Value key){
				auto found = this->ranges_true.find(key);
				if(found!=this->ranges_true.end())
					return found->second;
				else
					return this->getBottom();
				 
			}
			IntegerRange getRangeOrBottomFalse(mlir::Value key){
				auto found = this->ranges_false.find(key);
				if(found!=this->ranges_false.end())
					return found->second;
				else
					return this->getBottom();
				 
			}

			// bool isBottom(mlir::Value key){
			// 	auto& ranges=this->branch_true.value() ? this->ranges_true : this->ranges_false;
			// 	return ranges.find(key)==ranges.end();
			// }

			void insertOrJoinTrue(mlir::Value keyToInsert, IntegerRange rangeToInsert){
				auto found=this->ranges_true.find(keyToInsert);
				if(found!=this->ranges_true.end()){
					found->second=
						mlir::rlc::IntegerRange::joinRange(found->second, rangeToInsert);
				}
				else{
					this->ranges_true.insert(std::make_pair(keyToInsert,rangeToInsert));
				}
			}

			void insertOrJoinFalse(mlir::Value keyToInsert, IntegerRange rangeToInsert){
				auto found=this->ranges_false.find(keyToInsert);
				if(found!=this->ranges_false.end()){
					found->second=
						mlir::rlc::IntegerRange::joinRange(found->second, rangeToInsert);
				}
				else{
					this->ranges_false.insert(std::make_pair(keyToInsert,rangeToInsert));
				}
			}

			// TODO: make our life harder, distinguish betweeen true and false execution paths
			// The lattice is attached to an operation, so we save a map containing all the values calculated
			// by the operation

			// In order to do this we can try the option of keeping two ranges: 
			// one for true branch and one false (identical).
			// Then we can keep a bool that defines in which you save the branch 
			// and use that to decide which range to modify.
			// The bool needs to be an std::optional because at the start we do not know
			// which branch we are traversing so we do nothing.

			// TODO: I could wrap this in a pair (or maybe better in a 2 element std::array) 
			// but it would be extremely weird (maybe I kinda have to <- then maybe wrapping in a class)

			llvm::DenseMap<mlir::Value,IntegerRange> ranges_true;
			llvm::DenseMap<mlir::Value,IntegerRange> ranges_false;
			mlir::rlc::CURRENT_BRANCH curr_branch=mlir::rlc::CURRENT_BRANCH::B_UNKNOWN;
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
				llvm::outs()<<"visited call control flow transfer of operation: ";
				call.print(llvm::outs());
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
			void setToExitState(ConstraintsLattice* lattice) override
			{
				
				llvm::outs()<<"something was set to exit state\n";
				
				// This retrieves a pair key value
				for(auto pair: lattice->getUnderlyingLatticeTrue()){
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
			//Perform the analysis only if the function returns a boolean
			for (auto fun : getOperation().getOps<mlir::rlc::FlatFunctionOp>())
			{
				
				if(not mlir::isa<mlir::rlc::BoolType>(fun.getResult().getType().getResult(0)) )
					continue;

				Constraints con(fun);
				std::cout<<"In theory I should have run the analysis"<<std::endl;
				// Here I append to the function the ranges I found
				// TODO: understand why the analysis is done backwards but the blocks are traversed forwards
				// https://mlir.llvm.org/doxygen/DenseAnalysis_8cpp_source.html#l00387 <- I am just stupid, here it goes through the predecessors
				// Blocks have a getPredecessors() method
				auto& block=fun.getRegion(0).back();
				con.printRangesFound(&block.front());
			}
		}
	};
}	 // namespace mlir::rlc
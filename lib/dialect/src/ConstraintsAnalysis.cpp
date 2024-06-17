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

			void setMinTOP(){
				this->range.first = MIN;
			}

			void setMaxTOP(){
				this->range.second = MAX;
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

			bool hasMin(){
				return this->range.first.has_value();
			}

			bool hasMax(){
				return this->range.second.has_value();
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


				// Funny case: the ranges are disjointed -> take the min of one and the max of the other
				// This is useful in the case (?,10) and (20, ?) -> result should be (TOP,TOP)
				
				if((not r1_m.has_value() and not r2_M.has_value()) or
					(not r2_m.has_value() and not r1_M.has_value()))
					return std::make_pair(MIN,MAX);

				// Range initialized to std::nullopt
				IntegerRange toReturn;

				// TODO: possibly refactor
				// MIN
				
				if(r1_m.has_value())
					toReturn.setMin(std::min(r1_m.value(),r2_m.value_or(MIN)));
				else if (r2_m.has_value())
					toReturn.setMin(r2_m.value());
				else 
					toReturn.setMinTOP();
				

				// MAX
				
				if(r1_M.has_value())
					toReturn.setMax(std::max(r1_M.value(),r2_M.value_or(MAX)));
				else if (r2_M.has_value())
					toReturn.setMax(r2_M.value());
				else
					toReturn.setMaxTOP();
				

				return toReturn;
				
			}

			// Method to update the value of a range, it is similar to a join but it works only on transfer functions
			void update(const IntegerRange& other){
				
				// TODO: think about checking if inserting the min or the max

				// MIN
				if(other.range.first.has_value())
					this->range.first=other.range.first;

				// MAX
				if(other.range.second.has_value())
					this->range.second=other.range.second;

			}

			// Method to force an optional to the TOP value (use carefully)
			void forceTop(){
				if(not this->range.first.has_value())
					this->range.first=MIN;
				if(not this->range.second.has_value())
					this->range.second=MAX;
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
			const llvm::DenseMap<std::pair<mlir::Value,bool>,IntegerRange>& getUnderlyingLattice() const{
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
				auto copy = *this;

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
				this->ranges=other.ranges;
				return mlir::ChangeResult::Change;
			}

			mlir::ChangeResult copy(const mlir::dataflow::AbstractDenseLattice& val){
				return this->copy(*static_cast<const ConstraintsLattice*>(&val));
			}

			void print(raw_ostream& os) const override
			{
				os<<"Ranges:\n";
				if(not this->ranges.empty()){
					for(auto range: this->ranges){
						os << "SSA: "<< range.first.first <<" Branch: "<< range.first.second << 
						" RANGE: (" << (range.second.hasMin() ? std::to_string(range.second.getMin()):"?") 
						<< ", " << (range.second.hasMax() ? std::to_string(range.second.getMax()):"?") << ")\n";
					}
				}
				else {
					os<<"Empty\n";
				}
			}

			bool operator==(const ConstraintsLattice& other) const
			{
				return this->ranges == other.ranges;
			}

			mlir::ChangeResult setCurrentBranch(mlir::RegionBranchOpInterface* branchOp){
				// The yield has this format
				// %12 = rlc.constant true !rlc.bool
    			// rlc.yield %12 : !rlc.bool
				// So I need to retrieve "true" in this case

				// Here we get the rlc.constant true
				auto value=
					branchOp->getOperation() // gets the rlc.yield
					->getOperand(0); // get the first operand of the yield (%12:!rlc.bool)

				// We can only do this analysis if the function return true or false
				if(auto bool_branch=mlir::dyn_cast<BoolAttr>(value.getDefiningOp()->getAttr("value"))){
					// Insert in the lattice the constant I have found
					// We can insert an empty range since this is used just for check
					this->ranges.insert(std::make_pair(
						std::make_pair(value,bool_branch.getValue()),
						this->getBottom() ));
					
					return mlir::ChangeResult::Change;
				}

				return mlir::ChangeResult::NoChange;
			}

			// TODO: divide this operation in (at least) two parts
			// Here we should write the transfer function based on the operation 
			mlir::ChangeResult visitArithmeticOperation(mlir::Operation* statement, bool can_true, bool can_false)
			{

				// Modify my ranges only when i am sure that i am passing through a initialized (true/false) branch
				if (not can_true and not can_false)
					return mlir::ChangeResult::NoChange;

				// Here the operation has passed the sanity check so we can switch
				auto copy = *this;

				// So this becomes extremely tricky now, we have the information about the branch we are passing from
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

			mlir::ChangeResult visitBranchOperation(mlir::Operation* statement, bool branch_true_yield_true, bool branch_true_yield_false,
													bool branch_false_yield_true, bool branch_false_yield_false ){
				
				// Modify my ranges only when i am sure that i am passing through a initialized (true/false) branch
				if (not branch_true_yield_true and not branch_true_yield_false and
					not branch_false_yield_true and not branch_false_yield_false)
					return mlir::ChangeResult::NoChange;

				auto copy=*this;

				// This can always be performed
				auto casted=mlir::dyn_cast<mlir::rlc::CondBranch>(statement);

				// We can assume that a control operation is always the last in its basic block
				// NB: always true but it can be either a conditional or a simple jump
				// NB: the documentation says that a mlir::Block at the end has a "Terminator Operation" -> specifies the successors
				
				// Now we have to be careful, we have a conditional 
				// Whic%5 !rlc.bool ^bb2 ^bb1h can be either value operation const or const operation value
				// So what we can do actually is undersand where is the constant and if it is on the left just flip the condition ;)

				// As I thought it is not possible, so let's do an easier way ;)

				// Now it gives a range, but a wrong one. So I will debug it later

				auto conditional_op = casted.getCond().getDefiningOp();

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
				else if(auto lhs_casted=mlir::dyn_cast<mlir::rlc::Constant>(lhs_op)){

					if(not branch_true_yield_true==branch_true_yield_false){

						if(branch_true_yield_true)
							this->updateRelationalRange(conditional_op, false, rhs, lhs_casted, true);	
						
						if(branch_true_yield_false)
							this->updateRelationalRange(conditional_op, false, rhs, lhs_casted, false);	
					
					}

					if(not branch_false_yield_true==branch_false_yield_false){

						if(branch_false_yield_true)
							this->updateRelationalRange(conditional_op, true, rhs, lhs_casted, true);	
						
						if(branch_false_yield_false)
							this->updateRelationalRange(conditional_op, true, rhs, lhs_casted, false);	
						
					}

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

			private:

			IntegerRange getBottom(){
				return IntegerRange();
			}
			
			IntegerRange getRangeOrBottom(std::pair<mlir::Value,bool> key){
				auto found = this->ranges.find(key);
				if(found!=this->ranges.end())
					return found->second;
				else
					return this->getBottom();
				 
			}

			// bool isBottom(mlir::Value key){
			// 	auto& ranges=this->branch_true.value() ? this->ranges_true : this->ranges_false;
			// 	return ranges.find(key)==ranges.end();
			// }

			void insertOrJoin(std::pair<mlir::Value,bool> keyToInsert, IntegerRange rangeToInsert){
				auto found=this->ranges.find(keyToInsert);
				if(found!=this->ranges.end()){
					found->second=
						mlir::rlc::IntegerRange::joinRange(found->second, rangeToInsert);
				}
				else{
					this->ranges.insert(std::make_pair(keyToInsert,rangeToInsert));
				}
			}

			void insertOrUpdate(std::pair<mlir::Value,bool> keyToInsert, IntegerRange rangeToInsert){
				auto found=this->ranges.find(keyToInsert);
				if(found!=this->ranges.end()){
					found->second.update(rangeToInsert);
				}
				else{
					this->ranges.insert(std::make_pair(keyToInsert,rangeToInsert));
				}
			}

			// This method takes the operation, the range
			// statement is the original operation
			// other is the other operand so that we can do to_update=result operation other
			void updateArithmeticCommutativeRange(const mlir::Operation* statement, const mlir::Value& result, const mlir::Value& to_update, const mlir::Value& other, const bool branch_to_update){
				IntegerRange new_range(1,0);
				// If the other is a constant take its constant value
				if(auto other_casted=mlir::dyn_cast<mlir::rlc::Constant>(other.getDefiningOp())){
					auto other_const=other_casted->getAttr("value").dyn_cast<IntegerAttr>().getInt();
					new_range.setConstant(other_const);
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
				if(new_range.getMin()<=new_range.getMax()){

					auto a_range=this->getRangeOrBottom(std::make_pair(result,branch_to_update));
					// This operation has to be done indipendently from the branch
					// TODO: add more cases of course
					if(mlir::isa<mlir::rlc::AddOp>(statement))
						new_range=a_range-new_range;
					llvm::outs()<<"should have changed\n";
					// Insert the range where is needed
					this->insertOrUpdate(std::make_pair(to_update,branch_to_update),new_range);
				}
			}

			// This function is actually quite simpler since we always know 
			// the lhs is not a constant and the rhs is a constant
			// Result is useful to understand which check do to the operation
			void updateRelationalRange(const mlir::Operation* statement, bool result, const mlir::Value& to_update, const mlir::rlc::Constant& other, const bool branch_to_update){

				auto rhs_const=other->getAttr("value").dyn_cast<IntegerAttr>().getInt();

				IntegerRange new_range;

				// TODO: this with a typeswitch would be cute
				if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
					 if(result)
						new_range.setMin(rhs_const+1);
					 else
						new_range.setMax(rhs_const);
				}
				else if (mlir::isa<mlir::rlc::LessOp>(statement)){
					if(result)
						new_range.setMax(rhs_const-1);
					else
						new_range.setMin(rhs_const);
				}
				else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
					if(result)
						new_range.setMin(rhs_const);
					else
						new_range.setMax(rhs_const-1);
				}
				else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
					if(result)
						new_range.setMax(rhs_const);
					else
						new_range.setMin(rhs_const+1);
				}
				
				llvm::outs()<<"should have changed\n";
				// Insert the range where is needed
				this->insertOrUpdate(std::make_pair(to_update,branch_to_update),new_range);
			}

			llvm::DenseMap<std::pair<mlir::Value,bool>,IntegerRange> ranges;
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
				/*
				if (mlir::isa<mlir::rlc::GreaterOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::LessOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::GreaterEqualOp>(op))
					return true;
				else if (mlir::isa<mlir::rlc::LessEqualOp>(op))
					return true;
				*/
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
				auto res=before->join(after);
				
				// We have to decide which return values to update (true or false)
				// In order to do so we need to :
				// - peek at the successors of this block 
				// - look at their lattice
				// - find in their lattice if there is some value with key true/false
				// - keep in mind that information

				if (mlir::rlc::ConstraintsAnalysis::isArithmetic(op)){
					bool can_true=false;
					bool can_false=false;

					// Since here we have a lattice that is copied we can just look at it
					for(auto pair : before->getUnderlyingLattice()){
						if(pair.first.second==true)
							can_true=true;
						else can_false=true;
					}

					propagateIfChanged(before,before->visitArithmeticOperation(op,can_true,can_false));
				}
				else if(mlir::rlc::ConstraintsAnalysis::isConditional(op)){

					// If it is a conditional then I need to peek in the next block from each branch
					bool branch_true_yield_true=false;
					bool branch_true_yield_false=false;
					bool branch_false_yield_true=false;
					bool branch_false_yield_false=false;

					// Look at the next lattice in the true branch
					auto casted = mlir::dyn_cast_or_null<mlir::rlc::CondBranch>(op);
                	auto leftLattice=getLattice(mlir::ProgramPoint(&casted.getTrueBranch()->front()));

					for(auto pair : leftLattice->getUnderlyingLattice()){
						if(pair.first.second==true)
								branch_true_yield_true=true;
							else branch_true_yield_false=true;
					}

					// And do the same in the false
					auto rightLattice=getLattice(mlir::ProgramPoint(&casted.getFalseBranch()->front()));

					for(auto pair : rightLattice->getUnderlyingLattice()){
						if(pair.first.second==true)
								branch_false_yield_true=true;
							else branch_false_yield_false=true;
					}
					propagateIfChanged(before,before->visitBranchOperation(op,branch_true_yield_true, branch_true_yield_false, branch_false_yield_true, branch_false_yield_false));

				}
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
			void setToExitState(ConstraintsLattice* lattice) override
			{
				
				llvm::outs()<<"something was set to exit state\n";
				
			}

			public:

			void printRanges(mlir::Operation* op){
				auto* lattice = op != nullptr ? getLattice(mlir::ProgramPoint(op))
																			: getLattice(op->getBlock());
				llvm::outs()<<"KEK ";
				lattice->print(llvm::outs());
			}

			const llvm::DenseMap<std::pair<mlir::Value,bool>,IntegerRange>& getUnderlyingLattice(mlir::Operation* op){
				return this->getLattice(mlir::ProgramPoint(op))->getUnderlyingLattice();
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

			const llvm::DenseMap<std::pair<mlir::Value,bool>,IntegerRange>& getOperationLattice(mlir::Operation* op){
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
				con.printRangesFound(&block->back());

				auto lattice = con.getOperationLattice(&block->back());

				// Iterate through the arguments -> are the arguments of the first block
				for(auto arg: fun.getRegion(0).front().getArguments()){
					auto casted=mlir::dyn_cast<mlir::Value>(arg);
					for(auto pair : lattice){
						if(pair.first.first==casted){
							std::string to_write;
							auto stream=llvm::raw_string_ostream(to_write);
							arg.printAsOperand(stream,mlir::OpPrintingFlags());
							stream.flush();
							// Add range as a function argument
							fun->setAttr("rlc.attr."+to_write+"_branch_"+std::to_string(pair.first.second)+"_MIN",mlir::IntegerAttr::get(mlir::IntegerType::get(fun->getContext(),32),pair.second.getMin()));
							fun->setAttr("rlc.attr."+to_write+"_branch_"+std::to_string(pair.first.second)+"_MAX",mlir::IntegerAttr::get(mlir::IntegerType::get(fun->getContext(),32),pair.second.getMax()));
						}
					}
				}
			}
		}
	};
}	 // namespace mlir::rlc

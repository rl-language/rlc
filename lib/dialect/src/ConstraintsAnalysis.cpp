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

			using Integer=int;
			constexpr static Integer MAX=INT32_MAX;
			constexpr static Integer MIN=INT32_MIN;

			MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConstraintsLattice);
			explicit ConstraintsLattice(mlir::ProgramPoint point)
					: mlir::dataflow::AbstractDenseLattice(point)
			{
			}

			//TODO: understand what this meet does
			// In theory it should be the opposite of a join
			mlir::ChangeResult meet(
					const mlir::dataflow::AbstractDenseLattice& val) override
			{
				return mlir::ChangeResult::NoChange;
			}

			// Join operation
			// Defined as: J((a,b),(c,d)):(min(a,c),max(b,d)) for each equal SSA value
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

				for(auto range : this->ranges){
					
					// Check if there is an SSA value in the ranges from the other operation
					auto other_range=other.ranges.find(range.first);
					if(other_range!=other.ranges.end()){
						// Perform the join
						range.second.first =std::min(range.second.first ,other_range->second.first );
						range.second.second=std::max(range.second.second,other_range->second.second);
					}
					// else we have to insert it manually
					else{
						this->ranges.insert(std::make_pair(other_range->first, other_range->second));
					}
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
			mlir::ChangeResult copy(const ConstraintsLattice& before)
			{
				llvm::outs()<<"Did i copy?\n";
				if (this->ranges == before.ranges){
					llvm::outs()<<"no\n";
					return mlir::ChangeResult::NoChange;
				}
				this->ranges = before.ranges;
				return mlir::ChangeResult::Change;
			}

			void print(raw_ostream& os) const override
			{
				if(not this->ranges.empty()){
					for(auto range: this->ranges){
						os << "SSA: "<< range.first <<" RANGE: (" << range.second.first << ", " << range.second.second << ")\n";
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
				if (mlir::isa<mlir::rlc::AddOp>(statement)){
					llvm::outs()<<"addop\n";
					this->print(llvm::outs());
					// Do some name changing so that hopefully is more clear
					// NB : these are values
					const auto& a = statement->getResult(0);
					const auto& b = statement->getOperand(0);
					const auto& c = statement->getOperand(1);
					// This can be nullptr if for example it is a function argument
					// TODO: are there any other cases ?
					const auto& b_op = b.getDefiningOp();
					const auto& c_op = c.getDefiningOp();
					// We can perform the operation if we know the bounds on a 
					auto a_range=this->ranges.find(a);
					if(a_range!=this->ranges.end()){
						// TODO: understand what happens if both have ranges -> should I update both ? (because if yes, in which order ?)
						// NB: there is the case in which both are non constant-> if we know nothing about
						//     both we do nothing
						// Then first find which variable we need to operate on
						mlir::Value      state=nullptr;
						mlir::Value      stato=nullptr;
						//mlir::Operation* opera=nullptr;
						mlir::Operation* other=nullptr; // This is the one we know the bounds of
						std::pair<Integer,Integer> new_range=std::make_pair(1,0);
						// First get our candidates
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
							new_range.first = other_const;
							new_range.second= other_const;
						}
						else {
							auto other_range=this->ranges.find(stato);
							if(other_range!=this->ranges.end()){
								new_range.first = other_range->second.first;
								new_range.second= other_range->second.second;
							}
						}
						// If i have a valid range (i.e. if i modified the initial range which is not valid)
						// then modify or add the new range 
						if(new_range.first<=new_range.second){
							//Calculate the new range
							new_range.first =a_range->second.first -new_range.first;
							new_range.second=a_range->second.second-new_range.second;
							llvm::outs()<<"should have changed\n";
							auto opera_range=this->ranges.find(state);
							if(opera_range!=this->ranges.end()){
								opera_range->second=new_range;
							}
							else{
								this->ranges.insert(std::make_pair(state,new_range));
							}
							changed=true;
						}
					} // a_range == this->ranges.end()
				} // if (not mlir::isa<mlir::rlc::AddOp>(statement))
				// These should be pretty much the same  -> TODO: refactor
				// I do not understand why but in Operations.td the syntax is (lhs,rhs) but in the actual  print IR is reversed (or i am crazy, that is possible too)
				// else it is a greater/less (and equal)
				else {
					llvm::outs()<<"condop\n";
					const auto& lhs = statement->getOperand(0);
					const auto& rhs = statement->getOperand(1);
					const auto& lhs_op = lhs.getDefiningOp();
					const auto& rhs_op = rhs.getDefiningOp();
					std::pair<Integer,Integer> new_range=std::make_pair(1,0);
					// First we check if our rhs is a constant (easy case)
					if(mlir::isa<mlir::rlc::Constant>(rhs_op)){
						//And the other one is not a constant
						//TODO: appartently the arguments must be treated in a careful way -> they do not have a operation defining them
						if(lhs_op==nullptr or not mlir::isa<mlir::rlc::Constant>(lhs_op)){
							auto lhs_range=this->ranges.find(lhs);
							auto rhs_const=rhs_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
							llvm::outs()<<"Constant value: "<<rhs_const<<"\n";
							// TODO: remember to check if i am passing through a true or false branch, for the moment leave it as true
							if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
								new_range.first = rhs_const+1;
								new_range.second= MAX;
							}
							else if (mlir::isa<mlir::rlc::LessOp>(statement)){
								new_range.first = MIN;
								new_range.second= rhs_const-1;
							}
							else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
								new_range.first = rhs_const;
								new_range.second= MAX;
							}
							else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
								new_range.first = MIN;
								new_range.second= rhs_const;
							}
							//If it is present then update it, else insert it
							if(lhs_range!=this->ranges.end()){
								lhs_range->second=new_range;
							}
							else{
								this->ranges.insert(std::make_pair(lhs,new_range));
							}
							changed=true;
						} // if(lhs_op!=nullptr and mlir::isa<mlir::rlc::Constant>(lhs_op))
					} // if(not mlir::isa<mlir::rlc::Constant>(rhs_op))
					// Else we basically have to do the same checkings but carefully 
					// because we have to reverse the operation
					else if(mlir::isa<mlir::rlc::Constant>(lhs_op)){
						if(rhs_op==nullptr or not mlir::isa<mlir::rlc::Constant>(rhs_op)){
							auto rhs_range=this->ranges.find(lhs);
							auto lhs_const=lhs_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
							llvm::outs()<<"Constant value: "<<lhs_const<<"\n";
							// TODO: remember to check if i am passing through a true or false branch, for the moment leave it as true
							if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
								new_range.first = MIN;
								new_range.second= lhs_const-1;
							}
							else if (mlir::isa<mlir::rlc::LessOp>(statement)){
								new_range.first = lhs_const+1;
								new_range.second= MAX;
							}
							else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
								new_range.first = MIN;
								new_range.second= lhs_const;
							}
							else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
								new_range.first = lhs_const;
								new_range.second= MAX;
							}
							//If it is present then update it, else insert it
							if(rhs_range!=this->ranges.end()){
								rhs_range->second=new_range;
							}
							else{
								this->ranges.insert(std::make_pair(rhs,new_range));
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

			// TODO: make our life harder, distinguish betweeen true and false execution paths
			// The lattice is attached to an operation, so we save a map containing all the values calculated
			// by the operation
			llvm::DenseMap<mlir::Value,std::pair<Integer,Integer>> ranges;
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
   			/// execution and trigger propagation in case of change.
			void visitOperation(
				mlir::Operation *op, 
				const ConstraintsLattice &after,
                ConstraintsLattice *before) override
			{
				op->print(llvm::outs());
				llvm::outs()<<"\n";
				// For the moment implement the basic operations
				// TODO: the transfer function has to copy the content of the lattice before execution, is this good?
				auto res=before->copy(after);
				if (mlir::rlc::ConstraintsAnalysis::isOfInterest(op))
					propagateIfChanged(before,before->visitOperation(op));
				
				// This is for the instructions which do not modify the lattice
				else
					propagateIfChanged(before, mlir::ChangeResult::NoChange);
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
				// For the moment execute only the join
				llvm::outs()<<"visited region branch control flow transfer\n";
				propagateIfChanged(after, after->join(before));
			}

			// no action is executed at the exit 
			void setToExitState(ConstraintsLattice* lattice) override
			{
				// propagateIfChanged(lattice, lattice->visitAction(nullptr));
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
				//TODO: understand what are these -> other analysis? so should I keep them ? I think I should not
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
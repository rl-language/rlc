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
					// Do some name changing so that hopefully is more clear
					// NB : these are values
					const auto& a = statement->getResult(0);
					const auto& b = statement->getOperand(0);
					const auto& c = statement->getOperand(1);
					// TODO: assert because it can return nullptr
					const auto& b_op = b.getDefiningOp();
					const auto& c_op = c.getDefiningOp();
					// We can perform the operation if we know the bounds on a 
					auto a_range=this->ranges.find(a);
					if(a_range!=this->ranges.end()){
					// b is not a constant -> b = a - c
						if(b_op==nullptr or not mlir::isa<mlir::rlc::Constant>(b_op)){
							// If c is a constant
							if (mlir::isa<mlir::rlc::Constant>(c_op)){
								auto b_range=this->ranges.find(b);
								// TODO: i am assuming this should work
								auto c_const=c_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
								Integer first =a_range->second.first  - c_const;
								Integer second=a_range->second.second - c_const;
								// If b is present then update it, else insert it
								if(b_range!=this->ranges.end()){
									b_range->second.first = first;
									b_range->second.second=second;
								}
								else{
									this->ranges.insert(std::make_pair(b,std::make_pair(first,second)));
								}
								changed=true;
							}
						}
					}

				}
				// These should be pretty much the same  -> TODO: refactor
				// I do not understand why but in Operations.td the syntax is (lhs,rhs) but in the actual IR is reversed (or i am crazy, that is possible too)
				//else it is a greater/less (and equal)
				else {
					llvm::outs()<<"condop\n";
					const auto& lhs = statement->getOperand(0);
					const auto& rhs = statement->getOperand(1);
					const auto& lhs_op = lhs.getDefiningOp();
					const auto& rhs_op = rhs.getDefiningOp();
					// First we check if our rhs is a constant (easy case)
					if(mlir::isa<mlir::rlc::Constant>(rhs_op)){
						//And the other one is not a constant
						//TODO: appartently the arguments must be treated in a careful way -> they do not have a operation defining them
						if(lhs_op==nullptr or not mlir::isa<mlir::rlc::Constant>(lhs_op)){
							auto lhs_range=this->ranges.find(lhs);
							auto rhs_const=rhs_op->getAttr("value").dyn_cast<IntegerAttr>().getInt();
							llvm::outs()<<"Constant value: "<<rhs_const<<"\n";
							//If it is present then update it, else insert it
							if(lhs_range!=this->ranges.end()){
								// TODO: remember to check if i am passing through a true or false branch, for the moment leave it as true
								if (mlir::isa<mlir::rlc::GreaterOp>(statement)){
									lhs_range->second.first = rhs_const+1;
									lhs_range->second.second= MAX;
								}
								else if (mlir::isa<mlir::rlc::LessOp>(statement)){
									lhs_range->second.first = MIN;
									lhs_range->second.second= rhs_const-1;
								}
								else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
									lhs_range->second.first = rhs_const;
									lhs_range->second.second= MAX;
								}
								else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
									lhs_range->second.first = MIN;
									lhs_range->second.second= rhs_const;
								}
							}
							else{
								if (mlir::isa<mlir::rlc::GreaterOp>(statement))
									this->ranges.insert(std::make_pair(lhs,std::make_pair(rhs_const+1,MAX)));
								else if (mlir::isa<mlir::rlc::LessOp>(statement)){
									this->ranges.insert(std::make_pair(lhs,std::make_pair(MIN,rhs_const-1)));
								}
								else if (mlir::isa<mlir::rlc::GreaterEqualOp>(statement)){
									this->ranges.insert(std::make_pair(lhs,std::make_pair(rhs_const,MAX)));
								}
								else if (mlir::isa<mlir::rlc::LessEqualOp>(statement)){
									this->ranges.insert(std::make_pair(lhs,std::make_pair(MIN,rhs_const)));
								}
							}
							changed=true;
						}
					}
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

			/*
			[[nodiscard]] llvm::SmallVector<mlir::Operation*, 4> getPredecessors()
					const
			{
				return llvm::SmallVector<mlir::Operation*, 4>(
						content.begin(), content.end());
			}
			*/

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
			// This is the transfer function
			void visitOperation(
					mlir::Operation* op,
					const ConstraintsLattice& before,
					ConstraintsLattice* after) override
			{
				assert(after != nullptr);
				op->print(llvm::outs());
				llvm::outs()<<"\n";
				auto res=after->copy(before);
				// For the moment implement the basic operations
				if (mlir::rlc::ConstraintsAnalysis::isOfInterest(op))
					auto res2=after->visitOperation(op);
				
				// This is for the instructions which do not modify the lattice
				else
					propagateIfChanged(after, mlir::ChangeResult::Change);
			}

			// Propagation of lattice in a control flow edge
			void visitCallControlFlowTransfer(
					CallOpInterface call,
					mlir::dataflow::CallControlFlowAction action,
					const ConstraintsLattice& before,
					ConstraintsLattice* after) final
			{
				llvm::outs()<<"visited call control flow transfer\n";
				propagateIfChanged(after, after->copy(before));
			}

			// Propagation of lattice between regions
			// TODO: understand if I can have different regions in a function (and if so think about what I have to do)
			void visitRegionBranchControlFlowTransfer(
					mlir::RegionBranchOpInterface branch,
					mlir::RegionBranchPoint regionFrom,
					mlir::RegionBranchPoint regionTo,
					const ConstraintsLattice& before,
					ConstraintsLattice* after) override
			{
				/*
				if (auto action = mlir::dyn_cast<mlir::rlc::ActionStatement>(
								branch.getOperation()))
					propagateIfChanged(after, after->visitOperation(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionFunction>(
										 branch.getOperation());
								 action && not regionFrom.has_value())
					propagateIfChanged(after, after->visitOperation(action));
				else if (auto action = mlir::dyn_cast<mlir::rlc::ActionsStatement>(
										 branch.getOperation());
								 action && not regionFrom.has_value())
					propagateIfChanged(after, after->visitOperation(action));
				else if (
						auto action = mlir::dyn_cast<mlir::rlc::SubActionStatement>(
								branch.getOperation()))
					propagateIfChanged(after, after->visitOperation(action));
				else
				*/
				// For the moment execute only the join
				llvm::outs()<<"visited region branch control flow transfer\n";
				propagateIfChanged(after, after->join(before));
			}

			// no action is executed at the starting entry point
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
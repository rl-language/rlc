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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/MLIRContext.h"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/Types.hpp"
#include <iostream>
#include <limits>

namespace mlir::rlc
{

#define GEN_PASS_DEF_CONSTRAINTSTESTPASS
#include "rlc/dialect/Passes.inc"

	void calculateConstraintsOp(mlir::Operation* op){
		
		//For the moment leave it like this
		using MyFunction=mlir::rlc::FlatFunctionOp;

		//Get all the functions
		llvm::SmallVector<MyFunction> all_functions;
		op->walk([&](MyFunction fun){all_functions.push_back(fun);});
		//Now try to understand which are the arguments passed to the functions
		for(auto fun : all_functions){
			//If I uncomment the /*rlc:::*/ it does not work -> I guess it is because there is no automatic conversion from a dialect type to a builtin type
			//fun->setAttr("rlc.TEST",mlir::IntegerAttr::get(mlir::/*rlc::*/IntegerType::get(fun->getContext(),32),i++));
			//NB: arg is a mlir::Type 
			//TODO: maybe with a constant I can simply save min=max ? -> understand if that can help or not
			//Traverse the regions
			//If i use auto here the compiler cries :'(
			for(mlir::Region& region: fun->getRegions()){
				//Weird interaction passing from pointers to references -> C++ style as we like it
				for(mlir::Block& block: region.getBlocks()){
					size_t op_in_block=0;

					if(block.getNumArguments()>0) {
						//Check equality of two arguments
						if(block.getArgument(0)==block.getArgument(1))
							std::cout<<"TRUEEEEE"<<std::endl;
						else
							//IT RETURNS THIS -> VERY VERY IMPORTANT
							std::cout<<"FALSEEEE"<<std::endl;
						/*
						for(auto arg: block.getArguments()){
							std::string buffer;
							llvm::raw_string_ostream stringStream(buffer);
							arg.print(stringStream);
							std::cout << buffer << std::endl;
						}
						*/
					}

					for(mlir::Operation& oper: block.getOperations()){
						//Check the operation
						//TODO: add a wrapper function that checks which operations are valid
						/*
						if(not mlir::isa<mlir::rlc::Constant>(oper)){
							//Add labels to it
							oper.setAttr("rlc.attr"+std::to_string(op_in_block)+"min",mlir::IntegerAttr::get(mlir::IntegerType::get(fun->getContext(),32),INT_MIN));
							oper.setAttr("rlc.attr"+std::to_string(op_in_block)+"MAX",mlir::IntegerAttr::get(mlir::IntegerType::get(fun->getContext(),32),INT_MAX));
							attr_num++;
						}
						*/
					}
				}
			}
		}
		}

	//TODO: understand if I should merge this in the "ConstraintsAnalysis.cpp" file
	struct ConstraintsTestPass
			: impl::ConstraintsTestPassBase<ConstraintsTestPass>
	{
		using impl::ConstraintsTestPassBase<
				ConstraintsTestPass>::ConstraintsTestPassBase;

		void runOnOperation() override
		{
			//mlir::rlc::ModuleBuilder builder(getOperation());
			//lowerConstructOps(builder, getOperation());
			calculateConstraintsOp(getOperation());
			std::cout<<"FUNZIA :)"<<std::endl;
		}
	};
}	 // namespace mlir::rlc

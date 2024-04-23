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

#define GEN_PASS_DEF_CONSTRAINTSPASS
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
			size_t attr_num=0;
			//For each argument of the function I Iattach some attributes to it -> should i use an array ?
			//NB: arg is a mlir::Type 
			for(auto arg: fun.getFunctionType().getInputs()){
				fun->setAttr("rlc.attr"+std::to_string(attr_num)+"min",mlir::IntegerAttr::get(mlir::/*rlc::*/IntegerType::get(fun->getContext(),32),INT_MIN));
				fun->setAttr("rlc.attr"+std::to_string(attr_num)+"MAX",mlir::IntegerAttr::get(mlir::/*rlc::*/IntegerType::get(fun->getContext(),32),INT_MAX));
				attr_num++;
			}
		}
		}

	struct ConstraintsPass
			: impl::ConstraintsPassBase<ConstraintsPass>
	{
		using impl::ConstraintsPassBase<
				ConstraintsPass>::ConstraintsPassBase;

		void runOnOperation() override
		{
			//mlir::rlc::ModuleBuilder builder(getOperation());
			//lowerConstructOps(builder, getOperation());
			calculateConstraintsOp(getOperation());
			std::cout<<"FUNZIA :)"<<std::endl;
		}
	};
}	 // namespace mlir::rlc

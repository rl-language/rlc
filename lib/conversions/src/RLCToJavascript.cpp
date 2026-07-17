/*
Copyright 2024 Massimo Fioravanti

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

#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "rlc/dialect/ActionArgumentAnalysis.hpp"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/MemberFunctionsTable.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"
#include "rlc/dialect/Types.hpp"
#include "rlc/dialect/Visits.hpp"
#include "rlc/utils/PatternMatcher.hpp"
namespace mlir::rlc
{

#define GEN_PASS_DEF_PRINTJAVASCRIPTPASS
#include "rlc/dialect/Passes.inc"
	/***
	 *This pass emits a python module that describes the content of the rlc
	 *module being compiled. It does the following:
	 * 1 For each ClassType and AlternativeType in the RLC program it emits a
	 *   CType structure or union that with same members, and makes sure that
	 *   constructors, clone and destructor invoke the right method in the RLC
	 *   shared library.
	 *
	 *
	 * 2 For each alias it emits the same alias
	 *
	 * 3 For each free function it emits a ctypes function declaration that wraps
	 *   that RLC function, annotated with the real signature of the function. The
	 *   emitted function has the same mangled name as the function in the
	 *   library.
	 *
	 *   Then, it emits a dispatcher function that accepts a argument list and
	 *   invokes the correct overload according to the types of the variables the
	 *   python user has provided.
	 *
	 *   Finally it emits typehinting annotations so that the user can see the
	 *   proper overloads available for a free function
	 *
	 * 4 For each member function it does the same thing that it did in step 3,
	 *   but instead of putting it into the global name space, the generated stuff
	 *   is placed inside the class that own the free function
	 *
	 * 5 For each ActionFunction it emits the ActionFunction as a ctypes class,
	 *   and inside that class it emits the ctypes function wrappers to invoke
	 *   the is_done function and actions statements that the ActionFunction
	 *   declares.
	 ***/
	struct PrintJavascriptPass: impl::PrintJavascriptPassBase<PrintJavascriptPass>
	{
		using impl::PrintJavascriptPassBase<PrintJavascriptPass>::PrintJavascriptPassBase;

		void runOnOperation() override
		{
			(*OS) << "Javascript wrapper";
		}
	};

}	 // namespace mlir::rlc

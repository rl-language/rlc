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
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/IR/BuiltinDialect.h"
#include "rlc/conversions/CSharpConversions.hpp"
#include "rlc/conversions/RLCToC.hpp"
#include "rlc/dialect/Operations.hpp"
#include "rlc/dialect/Passes.hpp"

namespace mlir::rlc
{
#define GEN_PASS_DEF_PRINTCSHARPPASS
#include "rlc/dialect/Passes.inc"

	struct PrintCSharpPass: impl::PrintCSharpPassBase<PrintCSharpPass>
	{
		using impl::PrintCSharpPassBase<PrintCSharpPass>::PrintCSharpPassBase;

		void runOnOperation() override
		{
			PatternMatcher matcher(*OS);
			matcher.add<CSharpFunctionDeclarationMatcher>();
			matcher.apply(getOperation());
		}
	};
}	 // namespace mlir::rlc

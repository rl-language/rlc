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
#include "mlir/Pass/Pass.h"
#include "rlc/python/Interfaces.hpp"
#include "rlc/python/Operations.hpp"
#include "rlc/python/Passes.hpp"

namespace mlir::python
{
#define GEN_PASS_DEF_PRINTPYTHONPASS
#include "rlc/python/Passes.inc"

	struct PrintPythonPass: impl::PrintPythonPassBase<PrintPythonPass>
	{
		using impl::PrintPythonPassBase<PrintPythonPass>::PrintPythonPassBase;

		void runOnOperation() override
		{
			if (mlir::rlc::python::serializePythonModule(
							*OS, *getOperation().getOperation())
							.failed())
				signalPassFailure();
		}
	};
}	 // namespace mlir::python

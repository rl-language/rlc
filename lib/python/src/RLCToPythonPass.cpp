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

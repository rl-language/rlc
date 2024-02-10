#include "rlc/driver/Driver.hpp"

#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/LLVMIR/Transforms/Passes.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "rlc/dialect/Dialect.h"
#include "rlc/dialect/Passes.hpp"
#include "rlc/python/Passes.hpp"

namespace mlir::rlc
{

	void Driver::configurePassManager(mlir::PassManager &manager) const
	{
		if (not skipParsing)
			manager.addPass(mlir::rlc::createParseFilePass(
					{ &includeDirs, inputFile, srcManager }));
		if (request == Request::dumpUncheckedAST)
		{
			manager.addPass(mlir::rlc::createPrintIRPass({ OS, hidePosition }));
			return;
		}

		if (request == Request::dumpDot)
		{
			manager.addPass(mlir::rlc::createUncheckedAstToDotPass({ OS }));
			return;
		}

		manager.addPass(mlir::rlc::createEmitEnumEntitiesPass());
		manager.addPass(mlir::rlc::createMemberFunctionsToRegularFunctionsPass());
		manager.addPass(mlir::rlc::createTypeCheckEntitiesPass());
		manager.addPass(mlir::rlc::createTypeCheckPass());
		manager.addPass(mlir::rlc::createValidateStorageQualifiersPass());

		if (emitFuzzer) {
			manager.addPass(mlir::rlc::createEmitFuzzTargetPass());
		}

		if (request == Request::dumpCheckedAST)
		{
			manager.addPass(mlir::rlc::createPrintIRPass({ OS, hidePosition }));
			return;
		}
		manager.addPass(mlir::createCanonicalizerPass());


		manager.addPass(mlir::rlc::createEmitImplicitDestructorInvocationsPass());
		manager.addPass(mlir::rlc::createEmitImplicitDestructorsPass());
		manager.addPass(mlir::rlc::createLowerForFieldOpPass());
		manager.addPass(mlir::rlc::createLowerIsOperationsPass());
		manager.addPass(mlir::rlc::createLowerAssignPass());
		manager.addPass(mlir::rlc::createLowerConstructOpPass());
		manager.addPass(mlir::rlc::createLowerDestructorsPass());

		manager.addPass(mlir::rlc::createInstantiateTemplatesPass());

		manager.addPass(mlir::rlc::createLowerConstructOpPass());
		manager.addPass(mlir::rlc::createLowerAssignPass());
		manager.addPass(mlir::rlc::createEmitImplicitDestructorsPass());
		manager.addPass(mlir::rlc::createLowerDestructorsPass());

		manager.addPass(mlir::rlc::createEmitImplicitAssignPass());
		manager.addPass(mlir::rlc::createEmitImplicitInitPass());
		manager.addPass(mlir::rlc::createLowerArrayCallsPass());
		if (emitBoundChecks)
			manager.addPass(mlir::rlc::createAddOutOfBoundsCheckPass());

		if (request == Request::dumpCWrapper)
		{
			manager.addPass(mlir::rlc::createPrintCHeaderPass({ OS }));
			return;
		}

		if (request == Request::dumpGodotWrapper)
		{
			manager.addPass(mlir::rlc::createPrintGodotPass({ OS }));
			return;
		}

		if (request == Request::dumpPythonWrapper or
				request == Request::dumpPythonAST)
		{
			manager.addPass(mlir::rlc::createSortTypeDeclarationsPass());
			manager.addPass(mlir::python::createRLCTypesToPythonTypesPass());
			manager.addPass(mlir::python::createRLCToPythonPass());
			if (request == Request::dumpPythonAST)
				manager.addPass(mlir::rlc::createPrintIRPass({ OS, hidePosition }));
			else
				manager.addPass(mlir::python::createPrintPythonPass({ OS }));
			return;
		}

		if (request == Request::dumpRLC)
		{
			manager.addPass(mlir::rlc::createPrintIRPass({ OS, hidePosition }));
			return;
		}

		manager.addPass(mlir::rlc::createLowerActionPass());
		manager.addPass(mlir::rlc::createLowerAssignPass());

		if (request == Request::dumpAfterImplicit)
		{
			manager.addPass(mlir::rlc::createPrintIRPass({ OS, hidePosition }));
			return;
		}

		manager.addPass(mlir::rlc::createExtractPreconditionPass());

		if (emitPreconditionChecks)
		{
			manager.addPass(mlir::rlc::createAddPreconditionsCheckPass());
		}

		manager.addPass(mlir::rlc::createLowerAssertsPass());

		manager.addPass(mlir::rlc::createLowerInitializerListsPass());

		manager.addPass(mlir::rlc::createLowerToCfPass());
		manager.addPass(mlir::rlc::createActionStatementsToCoroPass());
		manager.addPass(mlir::rlc::createStripFunctionMetadataPass());
		manager.addPass(mlir::rlc::createRewriteCallSignaturesPass());
		if (request == Request::dumpFlatIR)
		{
			manager.addPass(mlir::rlc::createPrintIRPass({ OS, hidePosition }));
			return;
		}
		manager.addPass(mlir::rlc::createLowerToLLVMPass());
		manager.addPass(mlir::rlc::createRemoveUselessAllocaPass());
		if (request == Request::executable and not emitFuzzer)
			manager.addPass(mlir::rlc::createEmitMainPass());
		manager.addPass(mlir::createCanonicalizerPass());

		if (request == Request::dumpMLIR)
		{
			manager.addPass(mlir::rlc::createPrintIRPass({ OS, hidePosition }));
			return;
		}
		if (debug)
			manager.addNestedPass<mlir::LLVM::LLVMFuncOp>(
					mlir::LLVM::createDIScopeForLLVMFuncOpPass());
		manager.addPass(mlir::rlc::createRLCBackEndPass(
				mlir::rlc::RLCBackEndPassOptions{ OS,
																					clangPath,
																					outputFile,
																					&extraObjectFiles,
																					dumpIR,
																					Request::compile == request,
																					emitFuzzer,
																					&rPath,
																					targetInfo }));
	}

}	 // namespace mlir::rlc

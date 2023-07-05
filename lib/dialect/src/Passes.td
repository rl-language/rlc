include "mlir/Pass/PassBase.td"
include "mlir/Rewrite/PassUtil.td"

def LowerActionPass : Pass<"rlc-lower-actions", "mlir::ModuleOp"> {
  let summary = "lower actions";
  let dependentDialects = ["mlir::rlc::RLCDialect"];
}

def LowerArrayCallsPass : Pass<"rlc-lower-array-calls", "mlir::ModuleOp"> {
  let summary = "lower array calls";
  let dependentDialects = ["mlir::rlc::RLCDialect"];
}

def LowerToCfPass : Pass<"rlc-to-cf", "mlir::ModuleOp"> {
  let summary = "lower to cf";
  let dependentDialects = ["rlc::RLCDialect"];
}

def ActionStatementsToCoroPass : Pass<"rlc-action-statements-to-coro", "mlir::ModuleOp"> {
  let summary = "lower to cf";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerToLLVMPass : Pass<"rlc-lower-to-llvm", "mlir::ModuleOp"> {
  let summary = "lower to cf";
  let dependentDialects = ["rlc::RLCDialect", "LLVM::LLVMDialect"];
}

def TypeCheckPass : Pass<"rlc-type-check", "mlir::ModuleOp"> {
  let summary = "type check rlc";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitMainPass: Pass<"rlc-emit-main", "mlir::ModuleOp"> {
  let summary = "emit rlc main pass";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerIsOperationsPass : Pass<"rlc-lower-is-operations", "mlir::ModuleOp"> {
  let summary = "lower is op operations";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerAssignPass : Pass<"rlc-assign-pass", "mlir::ModuleOp"> {
  let summary = "lower assign operations";
  let dependentDialects = ["rlc::RLCDialect"];
}

def InstantiateTemplatesPass : Pass<"rlc-instantiate-templates", "mlir::ModuleOp"> {
  let summary = "instantiate templates";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitAssignPass : Pass<"rlc-emit-implicit-assign-pass", "mlir::ModuleOp"> {
  let summary = "emit implicit assign pass";
  let dependentDialects = ["rlc::RLCDialect"];
}

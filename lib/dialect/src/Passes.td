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

def TypeCheckEntitiesPass : Pass<"rlc-type-check-entities", "mlir::ModuleOp"> {
  let summary = "type check rlc entities";
  let dependentDialects = ["rlc::RLCDialect"];
}

def TypeCheckPass : Pass<"rlc-type-check", "mlir::ModuleOp"> {
  let summary = "type check rlc functions";
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

def LowerForFieldOpPass : Pass<"rlc-for-field-op", "mlir::ModuleOp"> {
  let summary = "lower for field operations";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerConstructOpPass : Pass<"rlc-lower-construct-op", "mlir::ModuleOp"> {
  let summary = "lower construct operations";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitInitPass : Pass<"rlc-emit-implicit-init-pass", "mlir::ModuleOp"> {
  let summary = "emits default builtin init functions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitDestructorInvocationsPass : Pass<"rlc-emit-implicit-destructor-invocation-pass", "mlir::ModuleOp"> {
  let summary = "emits default builtin init functions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitDestructorsPass : Pass<"rlc-emit-implicit-destructors-pass", "mlir::ModuleOp"> {
  let summary = "emits default builtin init functions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def RespectCReturnTypeCallingConventions: Pass<"rlc-respect-c-return-type-calling-conventions", "mlir::ModuleOp"> {
  let summary = "make sure that function with a return type larger than 64 are passed as pointers";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerDestructorsPass : Pass<"rlc-lower-destructors-pass", "mlir::ModuleOp"> {
  let summary = "maps destructors invocations to a call";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitEnumEntitiesPass : Pass<"rlc-emit-enum-entities-pass", "mlir::ModuleOp"> {
  let summary = "for each enum declare a entity";
  let dependentDialects = ["rlc::RLCDialect"];
}

def SortTypeDeclarationsPass : Pass<"rlc-sort-type-declarations", "mlir::ModuleOp"> {
  let summary = "for each enum declare a entity";
  let dependentDialects = ["rlc::RLCDialect"];
}

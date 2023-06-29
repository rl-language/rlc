include "mlir/Pass/PassBase.td"
include "mlir/Rewrite/PassUtil.td"

def RLCToPythonPass: Pass<"rlc-to-python", "mlir::ModuleOp"> {
  let summary = "convert rlc to python";
}

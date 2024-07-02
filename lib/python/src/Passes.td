include "mlir/Pass/PassBase.td"
include "mlir/Rewrite/PassUtil.td"

def RLCToPythonPass: Pass<"rlc-to-python", "mlir::ModuleOp"> {
  let summary = "convert rlc to python";
  let options = [
    Option<"target_is_windows", "target is windows", "bool", /*default=*/"false",
           "true if the target is windows">
  ];
}

def RLCTypesToPythonTypesPass: Pass<"rlc-types-to-python-types", "mlir::ModuleOp"> {
  let summary = "convert rlc types to python types";
}

def PrintPythonPass : Pass<"rlc-print-python-pass", "mlir::ModuleOp"> {
  let summary = "prints the python wrapper file on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">
  ];
}

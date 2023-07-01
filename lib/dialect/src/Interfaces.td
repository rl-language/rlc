include "mlir/IR/OpBase.td"

class RLC_Interface<string name> : OpInterface<name> {
    let cppNamespace = "::mlir::rlc";
}


def TypeCheckable : RLC_Interface<"TypeCheckable"> {
    let description = [{
        Interface for a Modelica equation.
    }];

    let methods = [
        InterfaceMethod<
            "executes type check.",
            "mlir::LogicalResult", "typeCheck",
            (ins "mlir::IRRewriter&":$rewriter, "rlc::ValueTable&":$table, "mlir::rlc::RLCTypeConverter&":$converter)
        >,

];
}


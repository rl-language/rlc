include "mlir/IR/OpBase.td"

class RLC_Interface<string name> : OpInterface<name> {
    let cppNamespace = "::mlir::rlc::python";
}


def EmitPython : RLC_Interface<"EmitPython"> {
    let description = [{
        Interface for a Modelica equation.
    }];

    let methods = [
        InterfaceMethod<
            "executes type check.",
            "mlir::LogicalResult", "emit",
            (ins "llvm::raw_ostream&":$OS, "SerializationContext&":$context)
        >,

];

}



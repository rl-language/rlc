include "mlir/IR/OpBase.td"

class RLC_Interface<string name, list<OpInterface> baseinterface = []> : OpInterface<name, baseinterface> {
    let cppNamespace = "::mlir::rlc";
}

def TypeCheckable : RLC_Interface<"TypeCheckable"> {
    let description = [{
		//todo
    }];

    let methods = [
        InterfaceMethod<
            "executes type check.",
            "mlir::LogicalResult", "typeCheck",
            (ins "mlir::rlc::ModuleBuilder&":$builder)
        >,

];
}

def ConstraintsAnalyzable : RLC_Interface<"ConstraintsAnalyzable"> {
    let description = [{
        Operations analyzable by the ConstraintsAnalysis pass
    }];

    let methods = [
        InterfaceMethod<
            "method called when visiting an operation",
            "bool", "constraintsAnalyze",
            (ins "mlir::rlc::ConstraintsLattice*":$currentLattice, 
            "mlir::rlc::ConstraintsAnalysis*":$analysis)
        >,
    ];
}


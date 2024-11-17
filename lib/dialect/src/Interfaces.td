#ifndef __RLC_INTERFACES__
#define __RLC_INTERFACES__
include "mlir/IR/OpBase.td"

def RLC_RLCSerializable
    : TypeInterface<"RLCSerializable"> {
  let cppNamespace = "::mlir::rlc";

  let description = [{
    An interface for types that are allowed as elements of LLVM pointer type.
    Such types must have a size.
  }];

  let methods = [
    InterfaceMethod<
      /*description=*/"Serializes a type to string as it would be written in a rlc file.",
      /*retTy=*/"void",
      /*methodName=*/"rlc_serialize",
      /*args=*/(ins "llvm::raw_ostream&":$OS, "const mlir::rlc::SerializationContext &":$context),
      /*methodBody=*/""
    >
  ];
}

class RLC_Interface<string name, list<OpInterface> baseinterface = []> : OpInterface<name, baseinterface> {
    let cppNamespace = "::mlir::rlc";
}

def TypeUser : RLC_Interface<"TypeUser"> {
    let description = [{
        A element of the AST where the user has written a type.
        example
        let i : Int
    }];

    let methods = [
        InterfaceMethod<
            "range in the source file where the type was used. Null if there was no range",
            "mlir::rlc::SourceRangeAttr", "getTypeSourceRange",
            (ins)
        >,
        InterfaceMethod<
            "type written by the user in the source code",
            "mlir::Type", "getExplicitType",
            (ins)
        >,
];
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

#endif

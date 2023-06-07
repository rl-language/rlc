// Include the definition of the necessary tablegen constructs for defining
// our types.
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/SubElementInterfaces.td"
include "Dialect.td"

// It's common to define a base classes for types in the same dialect. This
// removes the need to pass in the dialect for each type, and can also be used
// to define a few fields ahead of time.
class RLC_Type<string name, string typeMnemonic, list<Trait> traits = []>
    : TypeDef<RLC_Dialect, name, traits> {
  let mnemonic = typeMnemonic;
}

// Here is a simple definition of an "integer" type, with a width parameter.
def RLC_IntegerType : RLC_Type<"Integer", "int"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

  /// Here we define the textual format of the type declaratively, which will
  /// automatically generate parser and printer logic. This will allow for
  /// instances of the type to be output as, for example:
  ///
  ///    !my.int<10> // a 10-bit integer.
  ///

  /// Indicate that our type will add additional verification to the parameters.
  let genVerifyDecl = 0;
}

def RLC_VoidType : RLC_Type<"Void", "void"> {
  let summary = "RLC builtin float type";
  let description = [{
    Implementation agnostic float type.
  }];

}

def RLC_FloatType : RLC_Type<"Float", "float"> {
  let summary = "RLC builtin float type";
  let description = [{
    Implementation agnostic float type.
  }];

}

def RLC_BoolType : RLC_Type<"Bool", "bool"> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
}


def RLC_ArrayType : RLC_Type<"Array", "array", [DeclareTypeInterfaceMethods<SubElementTypeInterface>]> {
  let summary = "RLC array type";
  let description = [{
    Entity type.
  }];

  let parameters = (ins "Type":$underlying, "int64_t":$size);
  let assemblyFormat = "`<` $underlying `:` $size`>` ";
}

def RLC_TypeUse : RLC_Type<"ScalarUse", "type_use"> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
	let parameters = (ins OptionalParameter<"Type">:$underlying, StringRefParameter<>:$read_type, "int64_t":$size);
  let assemblyFormat = "$underlying $read_type $size ";
}

def RLC_Unknown : RLC_Type<"Unknown", "unknown"> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
}

def RLC_FunctionTypeUse : RLC_Type<"FunctionUse", "type_use", [DeclareTypeInterfaceMethods<SubElementTypeInterface>]> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
	let parameters = (ins ArrayRefParameter<"Type">:$subTypes);
  let assemblyFormat = "`(` $subTypes `)`";
}




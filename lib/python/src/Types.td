// Include the definition of the necessary tablegen constructs for defining
// our types.
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/SubElementInterfaces.td"
include "Dialect.td"

class RLC_PythonType<string name, string typeMnemonic, list<Trait> traits = []>
    : TypeDef<RLC_Python, name, traits> {
  let mnemonic = typeMnemonic;
}

def RLC_CDLLType : RLC_PythonType<"CDLL", "cdll"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];
}

def RLC_PythonNone : RLC_PythonType<"None", "none"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

}

def RLC_CTypesBool : RLC_PythonType<"CTypesBool", "ctypes_bool"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

}

def RLC_CTypesFloat : RLC_PythonType<"CTypesFloat", "ctypes_float"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

}

def RLC_CTypeArrayType : RLC_PythonType<"CArray", "ctypes_array"> {
  let parameters = (ins "Type":$sub_type, "int64_t":$size);

  let assemblyFormat = " $sub_type `:` $size ";
}

def RLC_CTypePointerType : RLC_PythonType<"CTypesPointer", "ctypes_pointer"> {
  let parameters = (ins "Type":$underlying);

  let assemblyFormat = "`<` $underlying`>` ";
}

def RLC_CTypeStructType : RLC_PythonType<"CTypeStruct", "ctypes_struct"> {
  let parameters = (ins StringRefParameter<>:$name, ArrayRefParameter<"Type">:$sub_types);

  let assemblyFormat = "$name `<` $sub_types`>` ";
}

def RLC_CTypesInt : RLC_PythonType<"CTypesInt", "ctypes_int"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];
  let parameters = (ins "int64_t":$size);
  let assemblyFormat = "`<` $size`>`";
}


def RLC_PythonBool : RLC_PythonType<"Bool", "bool"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

}

def RLC_PythonFloat : RLC_PythonType<"Float", "float"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

}

def RLC_PythonInt : RLC_PythonType<"Int", "int"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];
  let parameters = (ins "int64_t":$size);
  let assemblyFormat = "`<` $size`>`";
}

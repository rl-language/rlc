#ifndef __RLC_ATTRS__
#define __RLC_ATTRS__
include "mlir/IR/AttrTypeBase.td"
include "Dialect.td"
include "mlir/IR/OpBase.td"
include "Interfaces.td"


class RLC_Attr<string name, string typeMnemonic, list<Trait> traits = []>
    : AttrDef<RLC_Dialect, name, traits> {
  let mnemonic = typeMnemonic;
}

def RLC_ClassFieldAttr : RLC_Attr<"ClassField", "class_field"> {
  let summary = "class field attribute ";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins StringRefParameter<>:$name, "mlir::Type":$type);


  let assemblyFormat = "`<` $name `:` $type `>`";
}

def ClassFieldsArrayAttr : TypedArrayAttrBase<RLC_ClassFieldAttr, "array of attribudes describing array attr">;
#endif

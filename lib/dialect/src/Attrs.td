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

def RLC_SourceRangeAttr : RLC_Attr<"SourceRange", "source_range_attr"> {
  let summary = "source range";
  let description = [{
    used to keep track of where certain elements of the ast came from
    for example which caracters contribuited to the type of a argument
    of a function
  }];

  let parameters = (ins "mlir::FileLineColLoc":$start, "mlir::FileLineColLoc":$end);

  let assemblyFormat = "`<` $start `,` $end`>`";

  let builders = [
	  AttrBuilderWithInferredContext<(ins "mlir::FileLineColLoc":$start, "mlir::FileLineColLoc":$end),[{
		return mlir::rlc::SourceRangeAttr::get(start.getContext(), start, end);
	  }]>,
  ];

}


def RLC_ClassFieldAttr : RLC_Attr<"ClassField", "class_field"> {
  let summary = "class field attribute ";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins StringRefParameter<>:$name, "mlir::Type":$type, OptionalParameter<"mlir::rlc::SourceRangeAttr">:$type_location);


  let assemblyFormat = "`<` $name `:` $type (`location` `=` $type_location^ )? `>`";

  let builders = [
	  AttrBuilderWithInferredContext<(ins "llvm::StringRef":$name, "mlir::Type":$type),[{
		return mlir::rlc::ClassFieldAttr::get(type.getContext(), name, type, nullptr);
	  }]>,
      AttrBuilderWithInferredContext<(ins "llvm::StringRef":$name, "mlir::Type":$type, "mlir::rlc::SourceRangeAttr":$type_location),[{
		return mlir::rlc::ClassFieldAttr::get(type.getContext(), name, type, type_location);
	  }]>,
  ];
}


def ClassFieldsArrayAttr : TypedArrayAttrBase<RLC_ClassFieldAttr, "array of attribudes describing array attr">;
#endif

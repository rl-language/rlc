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

def RLC_FunctionArgumentAttr : RLC_Attr<"FunctionArgument", "function_argument"> {
  let summary = "AST infos about function arguments, such as the name of the field, the source range of the name, the shugarized type argument and the source range of the shugarized type.";
  let description = [{
        info about function argument
  }];

  let parameters = (ins StringRefParameter<>:$name, OptionalParameter<"mlir::rlc::SourceRangeAttr">:$name_location, OptionalParameter<"mlir::Type">:$shugarized_type, OptionalParameter<"mlir::rlc::SourceRangeAttr">:$type_location);

  let assemblyFormat = "`<`  `name` `=` $name  (`shugarized_type` `=` $shugarized_type^ )? (`name_location` `=` $name_location^ )? (`type_location` `=` $type_location^ )? `>`";

  let builders = [
	  AttrBuilder<(ins "llvm::StringRef":$name),[{
        return mlir::rlc::FunctionArgumentAttr::get($_ctxt, name, nullptr, nullptr, nullptr);
	  }]>,
  ];
}

def RLC_FunctionInfoAttr : RLC_Attr<"FunctionInfo", "function_info"> {
  let summary = "AST infos about a function";


  let parameters = (ins ArrayRefParameter<"mlir::rlc::FunctionArgumentAttr">:$args, OptionalParameter<"mlir::rlc::SourceRangeAttr">:$return_type_location, OptionalParameter<"mlir::Type">:$shugarized_return_type);

  let assemblyFormat = "`<`  `args` `=` $args (`return_type_location` `=` $return_type_location^ )? (`shugarized_return_type` `=` $shugarized_return_type^ )? `>`";

  let builders = [
	  AttrBuilder<(ins "llvm::ArrayRef<llvm::StringRef>":$arg_names),[{
        llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 4> functionArgs;
        for (auto name : arg_names) {
            functionArgs.push_back(mlir::rlc::FunctionArgumentAttr::get($_ctxt, name));
        }
		return mlir::rlc::FunctionInfoAttr::get($_ctxt, functionArgs, nullptr, nullptr);
	  }]>,
  ];

  let extraClassDeclaration = [{
      mlir::rlc::FunctionInfoAttr addSelfArgument() {
		llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 4> args;
        args.push_back(mlir::rlc::FunctionArgumentAttr::get(getContext(), "self", nullptr, nullptr, nullptr));
		for (const auto& arg : getArgs())
        {
			args.push_back(arg);
        }
        return mlir::rlc::FunctionInfoAttr::get(getContext(), args, getReturnTypeLocation(), getShugarizedReturnType());
      }
  }];
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

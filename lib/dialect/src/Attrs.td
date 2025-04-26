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

def RLC_ShugarizedTypeAttr : RLC_Attr<"ShugarizedType", "shugarized_type"> {
  let summary = "AST info about a place in the source program where there was a hand written type";
  let description = [{
        info about a type in the source
  }];

  let parameters = (ins "mlir::rlc::SourceRangeAttr":$location, "mlir::Type":$type);

  let assemblyFormat = "`<` $location $type`>`";

  let builders = [
	  AttrBuilderWithInferredContext<(ins "mlir::rlc::SourceRangeAttr":$location, "mlir::Type":$type),[{
        assert(location != nullptr);
        assert(type != nullptr);
        return mlir::rlc::ShugarizedTypeAttr::get(type.getContext(), location, type);
	  }]>,
  ];

  let extraClassDeclaration = [{
      mlir::rlc::ShugarizedTypeAttr replaceType(mlir::Type type) const {
        return mlir::rlc::ShugarizedTypeAttr::get(getContext(), getLocation(), type);
      }
  }];
}

def RLC_FunctionArgumentAttr : RLC_Attr<"FunctionArgument", "function_argument"> {
  let summary = "AST infos about function arguments, such as the name of the field, the source range of the name, the shugarized type argument and the source range of the shugarized type.";
  let description = [{
        info about function argument
  }];

  let parameters = (ins StringRefParameter<>:$name, OptionalParameter<"mlir::rlc::SourceRangeAttr">:$name_location, OptionalParameter<"mlir::rlc::ShugarizedTypeAttr">:$shugarized_type);

  let assemblyFormat = "`<`  `name` `=` $name  (`shugarized_type` `=` $shugarized_type^ )? (`name_location` `=` $name_location^ )? `>`";

  let builders = [
	  AttrBuilder<(ins "llvm::StringRef":$name),[{
        return mlir::rlc::FunctionArgumentAttr::get($_ctxt, name, nullptr, nullptr);
	  }]>,
  ];

  let extraClassDeclaration = [{
      mlir::rlc::FunctionArgumentAttr replaceType(mlir::Type type) const {
        if (getShugarizedType() == nullptr)
            return *this;
        return mlir::rlc::FunctionArgumentAttr::get(getContext(), getName(), getNameLocation(), getShugarizedType().replaceType(type));
      }
  }];
}

def RLC_FunctionInfoAttr : RLC_Attr<"FunctionInfo", "function_info"> {
  let summary = "AST infos about a function";


  let parameters = (ins OptionalArrayRefParameter<"mlir::rlc::FunctionArgumentAttr">:$arguments, OptionalParameter<"mlir::rlc::ShugarizedTypeAttr">:$shugarized_return_type);

  let assemblyFormat = "`<` ( `[` $arguments^ `]` )?  (`shugarized_return_type` `=` $shugarized_return_type^ )? `>`";

  let builders = [
	  AttrBuilder<(ins "llvm::ArrayRef<llvm::StringRef>":$arg_names),[{
        llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 4> functionArgs;
        for (auto name : arg_names) {
            functionArgs.push_back(mlir::rlc::FunctionArgumentAttr::get($_ctxt, name));
        }
		return mlir::rlc::FunctionInfoAttr::get($_ctxt, functionArgs, nullptr);
	  }]>,
	  AttrBuilder<(ins "llvm::ArrayRef<mlir::rlc::FunctionArgumentAttr>":$args),[{
		return mlir::rlc::FunctionInfoAttr::get($_ctxt, args, nullptr);
	  }]>,
	  AttrBuilder<(ins),[{
		return mlir::rlc::FunctionInfoAttr::get($_ctxt, llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 4>(), nullptr);
	  }]>,
  ];

  let extraClassDeclaration = [{
      mlir::rlc::FunctionInfoAttr addSelfArgument() {
		llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 4> args;
        args.push_back(mlir::rlc::FunctionArgumentAttr::get(getContext(), "self", nullptr, nullptr));
		for (const auto& arg : getArguments())
        {
			args.push_back(arg);
        }
        return mlir::rlc::FunctionInfoAttr::get(getContext(), args, getShugarizedReturnType());
      }
      llvm::SmallVector<llvm::StringRef, 2> getArgNames() const {
        llvm::SmallVector<llvm::StringRef, 2> toReturn;
        for (auto arg : getArguments())
           toReturn.push_back(arg.getName());
        return toReturn;
      }

      mlir::rlc::FunctionInfoAttr replaceTypes(mlir::FunctionType shugarizedNewType) const {
            llvm::SmallVector<mlir::rlc::FunctionArgumentAttr, 2> newArgs; 
          for (auto [arg, new_type] : llvm::zip(getArguments(), shugarizedNewType.getInputs())) 
            newArgs.push_back(arg.replaceType(new_type));

          mlir::rlc::ShugarizedTypeAttr attr = nullptr;
          if (getShugarizedReturnType() != nullptr and shugarizedNewType.getNumResults() != 0) {
               attr = getShugarizedReturnType().replaceType(shugarizedNewType.getResults()[0]); 
          }
          return mlir::rlc::FunctionInfoAttr::get(getContext(), newArgs, attr);

      }
  }];
}

def RLC_ClassFieldAttr : RLC_Attr<"ClassField", "class_field"> {
  let summary = "class field attribute ";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins StringRefParameter<>:$name, "mlir::Type":$type);


  let assemblyFormat = "`<` $name `:` $type `>`";

  let builders = [
	  AttrBuilderWithInferredContext<(ins "llvm::StringRef":$name, "mlir::Type":$type),[{
		return mlir::rlc::ClassFieldAttr::get(type.getContext(), name, type);
	  }]>,
  ];

}

def RLC_ClassFieldDeclarationAttr : RLC_Attr<"ClassFieldDeclaration", "class_field_declaration"> {
  let summary = "class field attribute ";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins "mlir::rlc::ClassFieldAttr":$field, OptionalParameter<"mlir::rlc::ShugarizedTypeAttr">:$shugarized_type);


  let assemblyFormat = "`<` $field (`shugarized` `=` $shugarized_type^ )? `>`";

  let builders = [
	  AttrBuilderWithInferredContext<(ins "llvm::StringRef":$name, "mlir::Type":$type),[{
		return mlir::rlc::ClassFieldDeclarationAttr::get(type.getContext(), mlir::rlc::ClassFieldAttr::get(name, type), nullptr);
	  }]>,
	  AttrBuilderWithInferredContext<(ins "mlir::rlc::ClassFieldAttr":$field),[{
		return mlir::rlc::ClassFieldDeclarationAttr::get(field.getContext(), field, nullptr);
	  }]>,
	  AttrBuilderWithInferredContext<(ins "mlir::rlc::ClassFieldAttr":$field, "mlir::rlc::ShugarizedTypeAttr":$shugarized_type),[{
		return mlir::rlc::ClassFieldDeclarationAttr::get(field.getContext(), field, shugarized_type);
	  }]>,
      AttrBuilderWithInferredContext<(ins "llvm::StringRef":$name, "mlir::Type":$type, "mlir::rlc::ShugarizedTypeAttr":$shugarized_type),[{
        assert(shugarized_type != nullptr);
		return mlir::rlc::ClassFieldDeclarationAttr::get(shugarized_type.getContext(), mlir::rlc::ClassFieldAttr::get(name, type), shugarized_type);
	  }]>,
  ];

  let extraClassDeclaration = [{
      mlir::Type getDeshugarizedType() {
        return getField().getType();
      }
      llvm::StringRef getName() {
        return getField().getName();
      }
  }];
}


def ClassFieldsArrayAttr : TypedArrayAttrBase<RLC_ClassFieldAttr, "array of attribudes describing array attr">;
def ClassDeclarationFieldsArrayAttr : TypedArrayAttrBase<RLC_ClassFieldDeclarationAttr, "array of attribudes describing array attr">;

#endif

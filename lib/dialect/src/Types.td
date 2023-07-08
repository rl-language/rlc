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

def RLC_OwningPtrType : RLC_Type<"OwningPtr", "owning_ptr", [DeclareTypeInterfaceMethods<SubElementTypeInterface>]> {
  let summary = "RLC array type";
  let description = [{
    Entity type.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying `>` ";
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
    type that rappresents a not yet type checked type use.
  }];
	let parameters = (ins OptionalParameter<"Type">:$underlying, StringRefParameter<>:$read_type, "int64_t":$size, OptionalArrayRefParameter<"Type">:$explicit_template_parameters);
  let assemblyFormat = "`<`  $read_type (`[` $explicit_template_parameters^ `]`)? $size (`:` $underlying^ )?`>`";
}

def RLC_Unknown : RLC_Type<"Unknown", "unknown"> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
}

def RLC_FunctionTypeUse : RLC_Type<"FunctionUse", "function", [DeclareTypeInterfaceMethods<SubElementTypeInterface>]> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
	let parameters = (ins ArrayRefParameter<"Type">:$subTypes);
  let assemblyFormat = "`(` $subTypes `)`";
}


def RLC_TypeType : RLC_Type<"Type", "type"> {
  let summary = "RLC type of type";
  let description = [{
	rapresents the informations kmown at run time of a compile time type.
  }];
}

def RLC_TraitMetaType : RLC_Type<"TraitMeta", "trait", [DeclareTypeInterfaceMethods<SubElementTypeInterface>]> {
  let summary = "RLC type rappresenting a trait type";
  let description = [{
	 rapresents a trait meta type
  }];
  let parameters = (ins StringRefParameter<>:$name, StringRefParameter<>:$template_parameter, ArrayRefParameter<"FunctionType">:$requested_function_types, ArrayRefParameter<"mlir::StringAttr">:$requested_function_names);

  let assemblyFormat = "`<`  $name  `<` $template_parameter `>` `[`$requested_function_types `]` `[` $requested_function_names `]` `>`";

  let extraClassDeclaration = [{
	  mlir::rlc::TemplateParameterType getTemplateParameterType() {
		return mlir::rlc::TemplateParameterType::get(getContext(), getTemplateParameter(), nullptr);
	  }

	mlir::LogicalResult typeRespectsTrait(mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table);
	private:
	mlir::LogicalResult typeRespectsTraitFunctionDeclaration(mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table, size_t FuctionDeclarationIndex);
	public:
  }];
}

def RLC_UncheckedTemplateParameterType : RLC_Type<"UncheckedTemplateParameter", "unchecked_template_parameter", [DeclareTypeInterfaceMethods<SubElementTypeInterface>]> {
  let summary = "RLC type rappresenting a template paremeter before typechecking";
  let description = [{
	 rapresents template parameter before typecheckng
  }];
  let parameters = (ins StringRefParameter<>:$name, OptionalParameter<"mlir::StringAttr", "\"\"">:$trait);
  let assemblyFormat = "`<`  $name (`:` $trait^ )?`>`";
}


def RLC_TemplateParameterType : RLC_Type<"TemplateParameter", "template_parameter", [DeclareTypeInterfaceMethods<SubElementTypeInterface>]> {
  let summary = "RLC type rappresenting a template paremeter";
  let description = [{
	 rapresents template parameter
  }];
  let parameters = (ins StringRefParameter<>:$name, OptionalParameter<"mlir::rlc::TraitMetaType">:$trait);
  let assemblyFormat = "`<`  $name (`:` $trait^ )?`>`";
}


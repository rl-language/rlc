// Include the definition of the necessary tablegen constructs for defining
// our types.
include "mlir/IR/AttrTypeBase.td"
include "Dialect.td"
include "mlir/IR/OpBase.td"
include "Interfaces.td"



// It's common to define a base classes for types in the same dialect. This
// removes the need to pass in the dialect for each type, and can also be used
// to define a few fields ahead of time.
class RLC_Type<string name, string typeMnemonic, list<Trait> traits = []>
    : TypeDef<RLC_Dialect, name, traits> {
  let mnemonic = typeMnemonic;
}

// Here is a simple definition of an "integer" type, with a width parameter.
def RLC_IntegerType : RLC_Type<"Integer", "int", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins "int64_t":$size);

  /// Here we define the textual format of the type declaratively, which will
  /// automatically generate parser and printer logic. This will allow for
  /// instances of the type to be output as, for example:
  ///
  ///    !my.int<10> // a 10-bit integer.
  ///

  /// Indicate that our type will add additional verification to the parameters.
  let genVerifyDecl = 0;

  let extraClassDeclaration = [{
	  static mlir::rlc::IntegerType getInt64(mlir::MLIRContext* ctx) {
		return mlir::rlc::IntegerType::get(ctx, 64);
	  }
	  static mlir::rlc::IntegerType getInt8(mlir::MLIRContext* ctx) {
		return mlir::rlc::IntegerType::get(ctx, 8);
	  }

  }];

  let assemblyFormat = "`<` $size`>`";
}

def RLC_StringLiteralType : RLC_Type<"StringLiteral", "str_literal", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC string literal type";
  let description = [{
    Implementation agnostic float type.
  }];
}

def RLC_VoidType : RLC_Type<"Void", "void"> {
  let summary = "RLC builtin float type";
  let description = [{
    Implementation agnostic float type.
  }];

}

def RLC_FloatType : RLC_Type<"Float", "float", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC builtin float type";
  let description = [{
    Implementation agnostic float type.
  }];

}

def RLC_BoolType : RLC_Type<"Bool", "bool", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
}

def RLC_ReferenceType : RLC_Type<"Reference", "ref", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC reference type";
  let description = [{
    reference type.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying `>`";
  let builders = [
	  TypeBuilderWithInferredContext<(ins "mlir::Type":$underlying),[{
		return 	mlir::rlc::ReferenceType::get(underlying.getContext(), underlying);
	  }]>,
  ];
}

def RLC_OwningPtrType : RLC_Type<"OwningPtr", "owning_ptr", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC array type";
  let description = [{
    Class type.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying `>` ";

  let builders = [
	  TypeBuilderWithInferredContext<(ins "mlir::Type":$underlying),[{
		return 	mlir::rlc::OwningPtrType::get(underlying.getContext(), underlying);
	  }]>,
  ];
}

def RLC_AlternativeType : RLC_Type<"Alternative", "alternative", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC alternative type";
  let description = [{
    Alternative type.
  }];

  let parameters = (ins ArrayRefParameter<"Type">:$underlying, StringRefParameter<>:$name);
  let assemblyFormat = "`<`$name $underlying `>` ";
  let extraClassDeclaration = [{
	  std::string getMangledName();
  }];

}


def RLC_ArrayType : RLC_Type<"Array", "array", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC array type";
  let description = [{
    Class type.
  }];

  let extraClassDeclaration = [{
	  int64_t getArraySize();
  }];

  let parameters = (ins "Type":$underlying, "mlir::Type":$size);
  let assemblyFormat = "`<` $underlying `:` $size`>` ";
  let builders = [
	  TypeBuilder<(ins "mlir::Type":$underlying ,"int64_t":$size),[{
		return 	ArrayType::get($_ctxt, underlying, mlir::rlc::IntegerLiteralType::get($_ctxt, size));
	  }]>,
  ];
}

def RLC_TypeUse : RLC_Type<"ScalarUse", "type_use"> {
  let summary = "RLC builtin bool type";
  let description = [{
    type that rappresents a not yet type checked type use.
  }];
	let parameters = (ins OptionalParameter<"Type">:$underlying, StringRefParameter<>:$read_type, "mlir::Type":$size, OptionalArrayRefParameter<"Type">:$explicit_template_parameters);
  let assemblyFormat = "`<`  $read_type (`[` $explicit_template_parameters^ `]`)? `array_size` `=` $size (`:` $underlying^ )?`>`";

  let builders = [
	  TypeBuilder<(ins "mlir::Type":$underlying),[{
		return 	ScalarUseType::get($_ctxt, underlying, "", mlir::rlc::IntegerLiteralType::get($_ctxt, 0), {});
	  }]>,
	  TypeBuilder<(ins "llvm::StringRef":$name, CArg<"int64_t", "0">:$size, CArg<"mlir::TypeRange", "{}">:$template_parameters),[{
		return ScalarUseType::get($_ctxt, name, mlir::rlc::IntegerLiteralType::get($_ctxt, size), template_parameters);
	  }]>,
	  TypeBuilder<(ins "llvm::StringRef":$name, "mlir::Type":$size, CArg<"mlir::TypeRange", "{}">:$template_parameters),[{
assert(size != nullptr);
		llvm::SmallVector<mlir::Type, 3> templates(template_parameters.begin(), template_parameters.end());
		return ScalarUseType::get($_ctxt, nullptr, name, size, templates);
	  }]>,
  ];
}

def RLC_Unknown : RLC_Type<"Unknown", "unknown"> {
  let summary = "RLC builtin bool type";
  let description = [{
    Implementation agnostic bool type.
  }];
}


def RLC_FunctionTypeUse : RLC_Type<"FunctionUse", "function"> {
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

def RLC_UncheckedTemplateParameterType : RLC_Type<"UncheckedTemplateParameter", "unchecked_template_parameter"> {
  let summary = "RLC type rappresenting a template paremeter before typechecking";
  let description = [{
	 rapresents template parameter before typecheckng
  }];
  let parameters = (ins StringRefParameter<>:$name, OptionalParameter<"mlir::StringAttr", "\"\"">:$trait);
  let assemblyFormat = "`<`  $name (`:` $trait^ )?`>`";
}


def RLC_TemplateParameterType : RLC_Type<"TemplateParameter", "template_parameter", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC type rappresenting a template paremeter";
  let description = [{
	 rapresents template parameter
  }];
  let parameters = (ins StringRefParameter<>:$name, OptionalParameter<"mlir::rlc::TraitMetaType">:$trait, "bool":$is_int_literal);
  let assemblyFormat = "`<`  $name (`:` $trait^ )? struct($is_int_literal) `>`";
}

def RLC_TraitMetaType : RLC_Type<"TraitMeta", "trait", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC type rappresenting a trait type";
  let description = [{
	 rapresents a trait meta type
  }];
  let parameters = (ins StringRefParameter<>:$name, ArrayRefParameter<"mlir::Type">:$template_parameters, ArrayRefParameter<"FunctionType">:$requested_function_types, ArrayRefParameter<"mlir::StringAttr">:$requested_function_names, ArrayRefParameter<"mlir::ArrayAttr">:$requested_function_arg_names);

  let assemblyFormat = "`<`  $name  `[` $template_parameters `]` `[`$requested_function_types `]` `[` $requested_function_names `]` `[`$requested_function_arg_names `]` `>`";

  let extraClassDeclaration = [{

	mlir::LogicalResult typeRespectsTrait(mlir::Location callPoint, mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table);
	private:
	mlir::LogicalResult typeRespectsTraitFunctionDeclaration(mlir::Location callPoint, mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table, size_t FuctionDeclarationIndex);
	public:
  }];
}

def RLC_ContextType : RLC_Type<"Context", "ctx", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "Marks a type as being allocated externally to the action and needs to passed to every action.";
  let description = [{
        Marks a type as being allocated in the frame of a action and needs to be passed to every action.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying`>`";
  let builders = [
	  TypeBuilderWithInferredContext<(ins "mlir::Type":$underlying),[{
		return mlir::rlc::ContextType::get(underlying.getContext(), underlying);
	  }]>,
  ];
}

def RLC_AliasType : RLC_Type<"Alias", "alias", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>, DeclareTypeInterfaceMethods<RLC_Renemable>]> {
  let summary = "a type created by the using X = Y statements";
  let description = [{
Syntactical shugar type that is kept around to allow for LSP renames, better errors and so on. It does nothing for semantics of the program and never appears on values.
  }];

  let parameters = (ins  StringRefParameter<>:$name, "Type":$underlying, OptionalArrayRefParameter<"Type">:$explicit_template_parameters);
  let assemblyFormat = "`<`$name `=` $underlying`>`";
  let builders = [
	  TypeBuilderWithInferredContext<(ins  "llvm::StringRef":$name, "mlir::Type":$underlying, CArg<"llvm::ArrayRef<mlir::Type>", "{}">:$explicit_template_parameters),[{
		return mlir::rlc::AliasType::get(underlying.getContext(), name, underlying, explicit_template_parameters);
	  }]>,
  ];
}

def RLC_FrameType : RLC_Type<"Frame", "frame", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "Marks a type as being allocated in the frame of a action.";
  let description = [{
        Marks a type as being allocated in the frame of a action.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying`>`";
  let builders = [
	  TypeBuilderWithInferredContext<(ins "mlir::Type":$underlying),[{
		return mlir::rlc::FrameType::get(underlying.getContext(), underlying);
	  }]>,
  ];
}

// this types is here to abuse the type converter when lowering llvm ir
def RLC_ProxyType : RLC_Type<"Proxy", "proxy"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying`>`";
  let builders = [
	  TypeBuilderWithInferredContext<(ins "mlir::Type":$underlying),[{
		return mlir::rlc::ProxyType::get(underlying.getContext(), underlying);
	  }]>,
  ];
}


// Here is a simple definition of an "integer" type, with a width parameter.
def RLC_IntegerLiteralType : RLC_Type<"IntegerLiteral", "int_literal", [DeclareTypeInterfaceMethods<RLC_RLCSerializable>]> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins "int64_t":$value);
  let assemblyFormat = "`<` $value`>`";
}

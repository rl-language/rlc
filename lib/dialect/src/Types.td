// Include the definition of the necessary tablegen constructs for defining
// our types.
include "mlir/IR/AttrTypeBase.td"
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

def RLC_ReferenceType : RLC_Type<"Reference", "ref"> {
  let summary = "RLC reference type";
  let description = [{
    reference type.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying `>` ";
}

def RLC_OwningPtrType : RLC_Type<"OwningPtr", "owning_ptr"> {
  let summary = "RLC array type";
  let description = [{
    Entity type.
  }];

  let parameters = (ins "Type":$underlying);
  let assemblyFormat = "`<` $underlying `>` ";
}

def RLC_AlternativeType : RLC_Type<"Alternative", "alternative"> {
  let summary = "RLC alternative type";
  let description = [{
    Alternative type.
  }];

  let parameters = (ins ArrayRefParameter<"Type">:$underlying);
  let assemblyFormat = "`<` $underlying `>` ";
  let extraClassDeclaration = [{
	  std::string getMangledName();
  }];
}


def RLC_ArrayType : RLC_Type<"Array", "array"> {
  let summary = "RLC array type";
  let description = [{
    Entity type.
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

def RLC_TraitMetaType : RLC_Type<"TraitMeta", "trait"> {
  let summary = "RLC type rappresenting a trait type";
  let description = [{
	 rapresents a trait meta type
  }];
  let parameters = (ins StringRefParameter<>:$name, StringRefParameter<>:$template_parameter, ArrayRefParameter<"FunctionType">:$requested_function_types, ArrayRefParameter<"mlir::StringAttr">:$requested_function_names);

  let assemblyFormat = "`<`  $name  `<` $template_parameter `>` `[`$requested_function_types `]` `[` $requested_function_names `]` `>`";

  let extraClassDeclaration = [{
	  mlir::rlc::TemplateParameterType getTemplateParameterType() {
		return mlir::rlc::TemplateParameterType::get(getContext(), getTemplateParameter(), nullptr, false);
	  }

	mlir::LogicalResult typeRespectsTrait(mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table);
	private:
	mlir::LogicalResult typeRespectsTraitFunctionDeclaration(mlir::Type type, mlir::rlc::SymbolTable<mlir::Value> &table, size_t FuctionDeclarationIndex);
	public:
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


def RLC_TemplateParameterType : RLC_Type<"TemplateParameter", "template_parameter"> {
  let summary = "RLC type rappresenting a template paremeter";
  let description = [{
	 rapresents template parameter
  }];
  let parameters = (ins StringRefParameter<>:$name, OptionalParameter<"mlir::rlc::TraitMetaType">:$trait, "bool":$is_int_literal);
  let assemblyFormat = "`<`  $name (`:` $trait^ )? struct($is_int_literal) `>`";
}


// Here is a simple definition of an "integer" type, with a width parameter.
def RLC_IntegerLiteralType : RLC_Type<"IntegerLiteral", "int_literal"> {
  let summary = "RLC builtin integer type";
  let description = [{
    Implementation agnostic integer type.
  }];

  let parameters = (ins "int64_t":$value);
  let assemblyFormat = "`<` $value`>`";
}

include "Types.td"
include "Attrs.td"
include "Enums.td"
include "mlir/IR/RegionKindInterface.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/CallInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/ViewLikeInterface.td"

class RLC_Dialect<string mnemonic, list<Trait> traits = []> :
    Op<RLC_Dialect, mnemonic, traits>;

def AnyRLCScalarType : AnyTypeOf<[RLC_BoolType, RLC_IntegerType, RLC_FloatType, RLC_OwningPtrType, RLC_ReferenceType, RLC_Unknown]>;

def RLCIntegerOrUnkownType : AnyTypeOf<[RLC_IntegerType, RLC_Unknown]>;
def RLCArrayOrUnkownType : AnyTypeOf<[RLC_ArrayType, RLC_OwningPtrType, RLC_StringLiteralType, RLC_Unknown]>;
def RLCBoolOrUnkownType : AnyTypeOf<[RLC_BoolType, RLC_Unknown]>;
def RLCPtrOrUnkownType : AnyTypeOf<[RLC_OwningPtrType, RLC_Unknown]>;

def AnyFunctionType : AnyTypeOf<[RLC_Unknown, FunctionType]>;

def RLC_ExpressionStatement : RLC_Dialect<"exp", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "expression.";

  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	regions	 attr-dict
  }];
}

def RLC_UncheckedTraitDefinition : RLC_Dialect<"unckecked_trait", [DeclareOpInterfaceMethods<TypeCheckable>, NoRegionArguments, NoTerminator, HasOnlyGraphRegion]> {
  let summary = "trait declaration";

  let description = [{
	rappresentation of a trait before the typechecking	
  }];

  let arguments = (ins StrAttr:$name, StrArrayAttr:$template_parameters);

  let regions = (region
	SizedRegion<1>:$body
  );

  let assemblyFormat = [{
	$name `<`$template_parameters `>` $body attr-dict
  }];

  let extraClassDeclaration = [{
	llvm::SmallVector<mlir::rlc::TemplateParameterType, 4> getTemplateParameterTypes(); 
  }];
}

def RLC_TraitDefinition : RLC_Dialect<"trait"> {
  let arguments = (ins TypeAttrOf<RLC_TraitMetaType>:$meta_type);

  let results = (outs Variadic<FunctionType>:$result);

  let assemblyFormat = [{
	$meta_type `:` type($result) attr-dict
  }];

  let builders = [
        OpBuilder<(ins "mlir::rlc::TraitMetaType":$metaType), [{
			llvm::SmallVector<mlir::Type, 3> types;
			for (auto& type : metaType.getRequestedFunctionTypes())
				types.push_back(type);
			build($_builder, $_state, types, metaType);
  }]>];
}

def RLC_ShortCircuitingOr : RLC_Dialect<"short_circuiting_or", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "short circuiting and that makes sure right and is not executed if not necessary";

  let results = (outs RLC_BoolType:$result);

  let description = [{
	expression.
  }];

  let regions = (region
      AnyRegion:$lhs,
      AnyRegion:$rhs
  );

  let assemblyFormat = [{
	$lhs $rhs `->` type($result) attr-dict
  }];

   let builders = [
        OpBuilder<(ins ), [{
            mlir::ValueRange val;
            build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()));
    }]>];
}

def RLC_ShortCircuitingAnd : RLC_Dialect<"short_circuiting_and", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "short circuiting and that makes sure right and is not executed if not necessary";

  let results = (outs RLC_BoolType:$result);

  let description = [{
	expression.
  }];

  let regions = (region
      AnyRegion:$lhs,
      AnyRegion:$rhs
  );

  let assemblyFormat = [{
	$lhs $rhs `->` type($result) attr-dict
  }];

   let builders = [
        OpBuilder<(ins ), [{
            mlir::ValueRange val;
            build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()));
    }]>];
}


def RLC_IfStatement : RLC_Dialect<"if", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "if statement.";

  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$condition,
  AnyRegion:$trueBranch,
  AnyRegion:$elseBranch
  );

  let assemblyFormat = [{
	`cond` $condition `true_branch` $trueBranch `false_branch` $elseBranch attr-dict
  }];
}

def RLC_ForFieldStatement : RLC_Dialect<"for_field", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "while.";

  let arguments = (ins StrArrayAttr:$names);

  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$condition,
  AnyRegion:$body
  );

  code extraClassDeclaration = [{
      bool hasFieldNameVariable(); 

      auto getNonFieldNameNames() {
        auto begin = getNames().begin();
        if (hasFieldNameVariable()) {
            begin++; 
        }
        return llvm::make_range(begin, getNames().end());
      }

      auto getNonFieldNameArgs() {
        auto begin = getBody().getArguments().begin();
        if (hasFieldNameVariable()) {
            begin++; 
        }
        return llvm::make_range(begin, getBody().getArguments().end());
      }
  }];

  let assemblyFormat = [{
	`cond` $condition `body` $body attr-dict
  }];
}

def RLC_ForLoopStatement : RLC_Dialect<"for", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "for loop statement.";

  let arguments = (ins AnyType:$expression);
  let description = [{
	for loop statement.
  }];

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	$expression `:` type($expression) `body` $body attr-dict
  }];
}

def RLC_ForLoopVarDeclOp : RLC_Dialect<"for_loop_variable", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "the variable generated by a for loop";

  let arguments = (ins StrAttr:$name);
  let description = [{
        the variable generated by a for loop
  }];

  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	 $name `:` type($result) attr-dict
  }];
}

def RLC_WhileStatement : RLC_Dialect<"while", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "while.";

  let description = [{
	while statement.
  }];

  let regions = (region
  AnyRegion:$condition,
  AnyRegion:$body
  );

  let assemblyFormat = [{
	`cond` $condition `body` $body attr-dict
  }];
}

def RLC_ContinueStatement : RLC_Dialect<"continue", [Terminator, DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]> ]> {
  let summary = "continue.";

  let description = [{
	expression.
  }];

  let assemblyFormat = [{
	$on_end attr-dict
  }];

  let regions = (region
  AnyRegion:$on_end
  );

}

def RLC_BreakStatement : RLC_Dialect<"break", [Terminator, DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "break.";

  let description = [{
	expression.
  }];

  let assemblyFormat = [{
	$on_end attr-dict
  }];

  let regions = (region
  AnyRegion:$on_end
  );


}

def RLC_ReturnStatement : RLC_Dialect<"return", [Terminator, DeclareOpInterfaceMethods<TypeCheckable>,  DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "return.";

  let description = [{
	expression.
  }];

  let arguments = (ins TypeAttrOf<AnyType>:$result);

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	$result $body attr-dict
  }];

}

def RLC_YieldReference : RLC_Dialect<"yield_reference", [ReturnLike, Terminator, DeclareOpInterfaceMethods<RegionBranchTerminatorOpInterface>]> {
  let summary = "ends statements";

  let arguments = (ins AnyType:$argument);

  let description = [{
	expression.
  }];


  let assemblyFormat = [{
	$argument `:` type($argument) attr-dict
  }];
}

def RLC_Yield : RLC_Dialect<"yield", [Terminator, DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchTerminatorOpInterface, ["getMutableSuccessorOperands", "getSuccessorRegions"]>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>, DeclareOpInterfaceMethods<ConstraintsAnalyzable>]> {
  let summary = "ends statements";

  let arguments = (ins Variadic<AnyType>:$arguments);

   let builders = [
        OpBuilder<(ins ), [{
            mlir::ValueRange val;
            build($_builder, $_state, val);
    }]>];

  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$on_end
  );


  let assemblyFormat = [{
	($arguments^ `:` type($arguments))? ( $on_end^ )? attr-dict
  }];
}

def RLC_SetActiveEntryOp : RLC_Dialect<"set_active_entry"> {
  let summary = "sets which is the current option currently active in a alternative type.";

  let description = [{
	sets which is the current option currently active in a alternative type. This should not be exposed to the front end but rather is something needed to implement assign and init of alternative types
  }];

  let arguments = (ins RLC_AlternativeType:$to_set, I64Attr:$new_active);

  let assemblyFormat = [{
	$to_set `:` type($to_set) `,` $new_active attr-dict 
  }];
}

def RLC_UsingTypeOp : RLC_Dialect<"using_type", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "statement list.";

  let description = [{
	expression.
  }];

  let arguments = (ins StrAttr:$name);

  let regions = (region
    AnyRegion:$body
  );

  let assemblyFormat = [{
	$name $body attr-dict 
  }];
}

def RLC_ConstantGlobalOp : RLC_Dialect<"global", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<TypeUser>]> {
  let summary = "constant global array";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyAttr:$values, StrAttr:$name, OptionalAttr<RLC_ShugarizedTypeAttr>:$shugarized_type);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$name $values `->` type($result) (`shugarized_type` `=` $shugarized_type^ )? attr-dict 
  }];

  let builders = [
        OpBuilder<(ins "mlir::Type":$type, "mlir::Attribute":$value, "mlir::StringRef":$name), [{
			build($_builder, $_state, type, value, name, nullptr);
  }]>];
}

def RLC_FlatConstantGlobalOp : RLC_Dialect<"flat_global"> {
  let summary = "constant global array";

  let description = [{
	expression.
  }];

  let arguments = (ins TypeAttrOf<AnyType>:$type, AnyAttr:$values, StrAttr:$name);

}

def RLC_StringLiteralOp : RLC_Dialect<"string_literal", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant global array";

  let description = [{
	expression.
  }];

  let arguments = (ins StrAttr:$value);
  let results = (outs RLC_StringLiteralType:$result);

  let assemblyFormat = [{
	$value `:` type($result) attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::StringRef":$value), [{
			build($_builder, $_state, mlir::rlc::StringLiteralType::get($_builder.getContext()), value);
    }]>];
}

def RLC_InitializerListOp : RLC_Dialect<"initializer_list", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "statement list.";

  let description = [{
	expression.
  }];

  let results = (outs AnyType:$result);

  let regions = (region
    AnyRegion:$body
  );

  let assemblyFormat = [{
	type($result) $body attr-dict 
  }];
  let hasFolder = 1;
}

def RLC_MakeRefOp : RLC_Dialect<"make_ref"> {
  let summary = "dereference a reference";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$ref);
  let results = (outs RLC_ReferenceType:$result);

  let assemblyFormat = [{
	$ref `:` type($ref) type($result) attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::Value":$ref), [{
			build($_builder, $_state, mlir::rlc::ReferenceType::get(ref.getContext(), ref.getType()), ref);
    }]>];
}

def RLC_DerefOp : RLC_Dialect<"deref"> {
  let summary = "dereference a reference";

  let description = [{
	expression.
  }];

  let arguments = (ins RLC_ReferenceType:$ref);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$ref `:` type($ref) type($result) attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::Value":$ref), [{
			build($_builder, $_state, ref.getType().cast<mlir::rlc::ReferenceType>().getUnderlying(), ref);
    }]>];
}

def RLC_CopyOp : RLC_Dialect<"copy"> {
  let summary = "statement list.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$to_copy);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$to_copy `:` type($to_copy) type($result) attr-dict 
  }];
}


def RLC_StatementList : RLC_Dialect<"list", [SymbolTable, DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "statement list.";

  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	$body attr-dict
  }];
}

def RLC_TypeAliasOp : RLC_Dialect<"alias", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<TypeUser>, DeclareOpInterfaceMethods<TypeDeclarer>]> {
  let summary = "statement list.";

  let arguments = (ins StrAttr:$name, TypeAttrOf<AnyType>:$aliased, OptionalAttr<RLC_SourceRangeAttr>:$declared_name_location, OptionalAttr<RLC_ShugarizedTypeAttr>:$shugarized_type);
  let description = [{
	expression.
  }];

  let assemblyFormat = [{
	$name `=` $aliased ( `shugarized` `=` $shugarized_type^ )? (`declared_name_location` `=` $declared_name_location^ )? attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "llvm::StringRef":$name, "mlir::Type":$aliased), [{
			build($_builder, $_state, name, aliased, nullptr, nullptr);
    }]>];
}

def RLC_Reference : RLC_Dialect<"ref"> {
  let summary = "statement list.";

  let arguments = (ins FlatSymbolRefAttr:$referred);
  let description = [{
	expression.
  }];

  let results = (outs AnyType:$result);


  let assemblyFormat = [{
	$referred `:` type($result) attr-dict 
  }];
}

def RLC_MallocOp : RLC_Dialect<"malloc_op", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<TypeUser>]> {
  let summary = "implement malloc";

  let description = [{
	implements malloc.
  }];

  let arguments = (ins RLCIntegerOrUnkownType:$size, RLC_ShugarizedTypeAttr:$shugarized_type);

  let results = (outs RLCPtrOrUnkownType:$result);

  let assemblyFormat = [{
	$size type($size) `->` type($result) $shugarized_type attr-dict 
  }];
}

def RLC_EnumDeclarationOp : RLC_Dialect<"enum_declaration_op", [NoTerminator, NoRegionArguments, HasOnlyGraphRegion]> {
  let summary = "implement malloc";

  let description = [{
	implements malloc.
  }];

  let arguments = (ins StrAttr:$name, OptionalAttr<RLC_SourceRangeAttr>:$type_location);

  let regions = (region
    AnyRegion:$body
  );

  code extraClassDeclaration = [{
	size_t countFields();
  }];

  let assemblyFormat = [{
	$name $body attr-dict ( `type_location` `=` $type_location^ )?
  }];
}

def RLC_EnumFieldExpressionOp : RLC_Dialect<"enum_field_expression", [NoRegionArguments, HasOnlyGraphRegion]> {
  let summary = "implement malloc";

  let description = [{
	 a entry of a enum.
  }];

  let arguments = (ins StrAttr:$name);

  let regions = (region
    AnyRegion:$body
  );

  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$name $body `:` type($result) attr-dict 
  }];
}

def RLC_EnumFieldDeclarationOp : RLC_Dialect<"enum_field_declaration", [NoTerminator, NoRegionArguments, HasOnlyGraphRegion]> {
  let summary = "implement malloc";

  let description = [{
	 a entry of a enum.
  }];

  let arguments = (ins StrAttr:$name);

  let regions = (region
  AnyRegion:$body
  );


  let assemblyFormat = [{
	$name $body attr-dict 
  }];
}

def RLC_EnumUse : RLC_Dialect<"enum_use", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "implement malloc";

  let description = [{
	implements malloc.
  }];


  let arguments = (ins I64Attr:$enum_value);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$enum_value `->` type($result) attr-dict 
  }];
}


def RLC_IsAlternativeTypeOp : RLC_Dialect<"is_alternative_type", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "is alternative type";

  let description = [{
	returns true if the underlying type is a alternative type.
  }];


  let arguments = (ins AnyType:$input);
  let results = (outs RLC_BoolType:$result);

  let assemblyFormat = [{
	$input  `:` type($input) `->` type($result) attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::Value":$expression), [{
			build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()), expression);
    }]>];
}

def RLC_UncheckedEnumUse : RLC_Dialect<"unchecked_enum_use"> {
  let summary = "implement malloc";

  let description = [{
	implements malloc.
  }];


  let arguments = (ins StrAttr:$enum_name, StrAttr:$enum_value);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$enum_name $enum_value `->` type($result) attr-dict 
  }];
}

def RLC_FreeOp : RLC_Dialect<"free_op", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "implement malloc";

  let description = [{
	implements malloc.
  }];


  let arguments = (ins RLCPtrOrUnkownType:$argument);

  let assemblyFormat = [{
	$argument `:` type($argument) attr-dict 
  }];
}

def RLC_UncheckedIsOp : RLC_Dialect<"UncheckedIsOp", [DeclareOpInterfaceMethods<TypeCheckable>,  DeclareOpInterfaceMethods<TypeUser>]> {
  let summary = "implement is operator";

  let description = [{
	implements is operator.
  }];

  let arguments = (ins AnyType:$expression, TypeAttrOf<AnyType>:$type_or_trait, RLC_ShugarizedTypeAttr:$shugarized_type);

  let results = (outs RLC_BoolType:$result);

  let assemblyFormat = [{
	$expression $type_or_trait type($expression) `->` type($result) $shugarized_type attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::Value":$expression, "mlir::Type":$type, "mlir::rlc::ShugarizedTypeAttr":$shugarized_type), [{
			build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()), expression,type, shugarized_type);
    }]>];
}

def RLC_IsOp : RLC_Dialect<"IsOp", [DeclareOpInterfaceMethods<TypeUser>]> {
  let summary = "implement is operator";

  let description = [{
	implements is operator.
  }];

  let arguments = (ins AnyType:$expression, TypeAttrOf<AnyType>:$type_or_trait, OptionalAttr<RLC_ShugarizedTypeAttr>:$shugarized_type);

  let results = (outs RLC_BoolType:$result);

  let assemblyFormat = [{
	$expression  $type_or_trait  `:` type($expression) `->` type($result) (`shugarized_type` `=` $shugarized_type^ )? attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::Value":$expression, "mlir::Type":$type, "mlir::rlc::ShugarizedTypeAttr":$shugarized_type), [{
			build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()), expression,type, shugarized_type);
    }]>, 
        OpBuilder<(ins "mlir::Value":$expression, "mlir::Type":$type), [{
			build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()), expression, type, nullptr);
    }]>

];
}

def RLC_UninitializedConstruct  : RLC_Dialect<"uninit_construct"> {
  let summary = "statement list.";

  let description = [{
	expression.
  }];

  let results = (outs AnyType:$result);


  let assemblyFormat = [{
	type($result) attr-dict 
  }];
}

def RLC_SubActionStatement : RLC_Dialect<"subaction", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "subaction.";

  let arguments = (ins DefaultValuedAttr<StrAttr, "\"\"">:$name, BoolAttr:$run_once);
  let description = [{
	subaction.
  }];

  let regions = (region
    AnyRegion:$body,
    AnyRegion:$forwarded_args
  );

  let assemblyFormat = [{
	( $name^ `=` )? $forwarded_args $body attr-dict 
  }];

    let extraClassDeclaration = [{
	
	size_t forwardedArgsCount();
	
	}];
}

def RLC_VarNameOp : RLC_Dialect<"var_name"> {
  let summary = "contains the name of a value so that it can be used for debugging purposed.";

  let arguments = (ins AnyType:$value, StrAttr:$name);
  let description = [{
        debug info for a variable
  }];


  let assemblyFormat = [{
	 $name $value `:` type($value) attr-dict
  }];
}

def RLC_DeclarationStatement : RLC_Dialect<"decl", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "statement list.";

  let arguments = (ins StrAttr:$sym_name);
  let description = [{
	expression.
  }];

  let results = (outs AnyType:$result);

  let regions = (region
  AnyRegion:$body
  );

  code extraClassDeclaration = [{
	bool isReference();
  }];

  let assemblyFormat = [{
	$sym_name `:` type($result) $body attr-dict
  }];
}


class RLC_FunctionBase<string mnemonic> :
RLC_Dialect<mnemonic, [CallableOpInterface, AutomaticAllocationScope, TypeCheckable, DeclareOpInterfaceMethods<TypeUser>]> {

  let arguments = (ins StrAttr:$unmangled_name, RLC_FunctionInfoAttr:$info);
  let description = [{
	expression.
  }];

  let results = (outs FunctionType:$result);


  let regions = (region
  AnyRegion:$body, AnyRegion:$precondition
  );

   let builders = [
        OpBuilder<(ins "StringRef":$name, "FunctionType":$type, "mlir::rlc::FunctionInfoAttr":$info), [{
			build($_builder, $_state, type, name, info);
    }]>];

  let assemblyFormat = [{
	$unmangled_name type($result) $info $precondition $body attr-dict 
  }];
    code extraBaseClassDeclaration = [{

        Region *getCallableRegion() {
            return isDeclaration() ? nullptr : &getBody();
        }

		std::string getMangledName() {
			return rlc::mangledName(getUnmangledName(), getIsMemberFunction(), getType());
		}

		std::string getCanFunctionMangledName() {
			return rlc::mangledName("can_" + getUnmangledName().str(), getIsMemberFunction(), mlir::FunctionType::get(getContext(), getFunctionType().getInputs(), {mlir::rlc::BoolType::get(getContext())}));
		}

		mlir::FunctionType getType() {
			return getResult().getType().cast<mlir::FunctionType>();
		}

		mlir::FunctionType getFunctionType() {
			return getResult().getType().cast<mlir::FunctionType>();
		}
	
		mlir::Region::BlockListType& getBlocks() {
			return getBody().getBlocks();
		}	

        ArrayRef<Type> getCallableResults() {
            return getType().getResults();
		}

        ArrayRef<Type> getArgumentTypes() {
            return getType().getInputs();
        }

        /// Returns the result types of this function.
        ArrayRef<Type> getResultTypes() {
            return getType().getResults();
        }

		/// Returns the argument attributes for all callable region arguments or
		/// null if there are none.
		ArrayAttr getCallableArgAttrs() {
		  return nullptr;
		}

		/// Returns the result attributes for all callable region results or
		/// null if there are none.
		ArrayAttr getCallableResAttrs() {
		  return nullptr;
		}


		bool isInternal() {
			return getUnmangledName().starts_with("_");
		}

        llvm::SmallVector<llvm::StringRef, 4> getArgNames() {
            llvm::SmallVector<llvm::StringRef, 4> names;
            for (auto arg : getInfo().getArguments()) 
                names.push_back(arg.getName());
            return names;
        }

        bool isDeclaration() {
            return getBody().empty();
        }
	
		mlir::LogicalResult addToSymbolTable(mlir::rlc::SymbolTable<mlir::Value>& table) {
			for (const auto& [field, argument] : llvm::zip(getInfo().getArguments(), llvm::make_range(this->args_begin(), this->args_end()))) {
				table.add(field.getName().str(), argument);
			}
			return mlir::success();
		}

		mlir::LogicalResult typeCheck(
				mlir::rlc::ModuleBuilder& builder)
		{

			for (auto* region : {&getBody(), &getPrecondition()}) {
				auto _  = builder.addSymbolTable();
				if (region->empty())
					continue;

				llvm::SmallVector<mlir::Operation*, 4>  ops;
				for (auto& operation : region->getOps())
					ops.push_back(&operation);

				for (const auto& [operand, field]: llvm::zip(region->front().getArguments(), getInfo().getArguments()))
				{
					auto converted = builder.getConverter().convertType(operand.getType());
                     
					if (not converted ){
						getOperation()->emitRemark("in of argument of function");
					}
					operand.setType(converted);
					builder.getSymbolTable().add(field.getName().str(), operand);
				}
				for (auto* operation : ops)
					if (mlir::rlc::typeCheck(*operation, builder)
							.failed())
						return mlir::failure();
			}

			return mlir::success();
		}

		Block::args_iterator args_begin() {
			return getBody().front().args_begin();
		}

		Block::args_iterator args_end() {
			return getBody().front().args_end();
		}


    }];
}

def RLC_FlatFunctionOp : RLC_FunctionBase<"flat_fun"> {
  let summary = "function.";
  let arguments = (ins StrAttr:$unmangled_name, RLC_FunctionInfoAttr:$info, BoolAttr:$is_member_function);
  let extraClassDeclaration = extraBaseClassDeclaration;

   let builders = [
        OpBuilder<(ins "StringRef":$name, "FunctionType":$type, "mlir::rlc::FunctionInfoAttr":$info, "bool":$isMemberFunction), [{
			build($_builder, $_state, type, name, info, isMemberFunction);
    }]>];
}


def RLC_FunctionOp : RLC_FunctionBase<"fun"> {
  let summary = "function.";
  let arguments = (ins StrAttr:$unmangled_name, RLC_FunctionInfoAttr:$info, BoolAttr:$is_member_function , DefaultValuedAttr<TypeArrayAttr, "{}">:$template_parameters);
  let extraClassDeclaration = extraBaseClassDeclaration # [{
	mlir::LogicalResult typeCheckFunctionDeclaration(
		mlir::IRRewriter& rewriter,
		mlir::rlc::RLCTypeConverter& converter);
    mlir::LogicalResult isTemplate(); 
  }];

   let builders = [
        OpBuilder<(ins "StringRef":$name, "FunctionType":$type, "rlc::FunctionInfoAttr":$info, "bool":$isMemberFunction, CArg<"mlir::TypeRange", "{}">:$templateParameters), [{
			build($_builder, $_state, type, name, info, isMemberFunction ,$_builder.getTypeArrayAttr(templateParameters));
    }]>];

  let assemblyFormat = [{
	$unmangled_name( $template_parameters^ )? type($result) $info $precondition $body attr-dict 
  }];
}

def RLC_ActionFunction : RLC_FunctionBase<"action_fun"> {
  let summary = "function.";
  let results = (outs FunctionType:$result, AnyFunctionType:$is_done_function, Variadic<FunctionType>:$actions);
  let assemblyFormat = [{
	$unmangled_name type($result) type($actions) type($is_done_function) $info $precondition $body attr-dict 
  }];
    let extraClassDeclaration = extraBaseClassDeclaration # [{
        std::pair<ActionFrameContent, ActionFrameContent>  getFrameLists();

		size_t getSubActionsSize() {
			return getActions().size();
		}

		mlir::FunctionType getSubActionType(size_t index) {
			return getActions().getTypes()[index].cast<mlir::FunctionType>();
		}

		mlir::FunctionType getIsDoneFunctionType() {
			return getIsDoneFunction().getType().cast<mlir::FunctionType>();
		}

        bool getIsMemberFunction() {
          return false;
        }

		mlir::FunctionType getMainActionType() {
			return getResult().getType().cast<mlir::FunctionType>();
		}
		mlir::rlc::ClassType getClassType() {
			return getMainActionType().getResult(0).cast<mlir::rlc::ClassType>();
		}
	}];

}

def RLC_ArrayCallOp : RLC_Dialect<"array_call",
    [CallOpInterface,
    DeclareOpInterfaceMethods<SymbolUserOpInterface>,
	DeclareOpInterfaceMethods<TypeCheckable>
    ]>
{
    let summary = "Invoke operation on each element of array .";

    let arguments = (ins AnyFunctionType:$callee, Variadic<RLC_ArrayType>:$args);
    let results = (outs Variadic<RLC_ArrayType>);

    let assemblyFormat = [{
        $callee `:` type($callee) `(` $args `)` attr-dict `:` functional-type($args, results)
    }];

    let builders = [
        OpBuilder<(ins "Value":$callee, CArg<"mlir::ValueRange", "{}">:$args), [{
			auto calleeType = callee.getType();
			auto functionType = calleeType.cast<FunctionType>();
			auto argumentSize = args.front().getType().cast<ArrayType>().getSize();
			if (functionType.getResults().size() == 0 or functionType.getResults().front() == VoidType::get($_builder.getContext()))
				build($_builder, $_state, mlir::TypeRange(), callee, args);
			else
			{
				mlir::Type resType = functionType.getResults().front();
				if (auto casted = resType.dyn_cast<mlir::rlc::ReferenceType>())
					resType = casted.getUnderlying();
				resType = mlir::rlc::ArrayType::get($_builder.getContext(), resType, argumentSize);
				build($_builder, $_state, resType, callee, args);
			}
        }]>    
	];

    let extraClassDeclaration = [{
        //===------------------------------------------------------------------===//
        // CallOpInterface
        //===------------------------------------------------------------------===//


		mlir::FunctionType getCalleeType() {
			return getCallee().getType().cast<mlir::FunctionType>();
		}

        MutableOperandRange getArgOperandsMutable() {
            return MutableOperandRange(*this, 1, getArgs().size() );
        }

        /// Get the argument operands to the called function.
        operand_range getArgOperands() {
            return { arg_operand_begin(), arg_operand_end() };
        }

        operand_iterator arg_operand_begin() {
            return operand_begin() + 1;
        }

        operand_iterator arg_operand_end() {
            return operand_end();
        }

        /// Return the callee of this operation.
        mlir::CallInterfaceCallable getCallableForCallee() {
            return getCallee();
        }
		/// Set the callee for this operation.
		void setCalleeFromCallable(CallInterfaceCallable callee) {
		  (*this)->setOperand(0, callee.get<mlir::Value>());
		}
    }];
}

def RLC_CallOp : RLC_Dialect<"call",
    [CallOpInterface,
    DeclareOpInterfaceMethods<SymbolUserOpInterface>,
	DeclareOpInterfaceMethods<TypeCheckable>
    ]>
{
    let summary = "Call operation.";

    let arguments = (ins AnyFunctionType:$callee, BoolAttr:$is_member_call, Variadic<AnyType>:$args);
    let results = (outs Variadic<AnyType>);

    let assemblyFormat = [{
        $callee `:` type($callee) `(` $args `)` attr-dict  `:` functional-type($args, results)
    }];

    let builders = [
        OpBuilder<(ins "Value":$callee, "bool":$is_member_call, CArg<"mlir::ValueRange", "{}">:$args), [{
			auto calleeType = callee.getType();
			auto functionType = calleeType.cast<FunctionType>();
			if (functionType.getResults().size() == 0 or functionType.getResults().front() == VoidType::get($_builder.getContext()))
				build($_builder, $_state, mlir::TypeRange(), callee, is_member_call ,args);
			else
			{

				mlir::Type resType = functionType.getResults().front();
				if (auto casted = resType.dyn_cast<mlir::rlc::ReferenceType>())
					resType = casted.getUnderlying();
				build($_builder, $_state, resType, callee, is_member_call, args);
			}
        }]>    
	];

    let extraClassDeclaration = [{
        //===------------------------------------------------------------------===//
        // CallOpInterface
        //===------------------------------------------------------------------===//

		mlir::FunctionType getCalleeType() {
			return getCallee().getType().cast<mlir::FunctionType>();
		}

        /// Get the argument operands to the called function.
        operand_range getArgOperands() {
            return { arg_operand_begin(), arg_operand_end() };
        }

        MutableOperandRange getArgOperandsMutable() {
            return MutableOperandRange(*this, 1, getArgs().size() );
        }

        operand_iterator arg_operand_begin() {
            return operand_begin() + 1;
        }

        operand_iterator arg_operand_end() {
            return operand_end();
        }

        /// Return the callee of this operation.
        mlir::CallInterfaceCallable getCallableForCallee() {
            return getCallee();
        }
		/// Set the callee for this operation.
		void setCalleeFromCallable(CallInterfaceCallable callee) {
		  (*this)->setOperand(0, callee.get<mlir::Value>());
		}
    }];
}


def RLC_ActionStatement : RLC_Dialect<"action", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>, DeclareOpInterfaceMethods<TypeUser>]> {
  let summary = "statement list.";

  let arguments = (ins StrAttr:$name, RLC_FunctionInfoAttr:$info, I64Attr:$id, I64Attr:$resumption_point);
  let description = [{
	expression.
  }];
  let results = (outs Variadic<AnyType>:$results);


  let regions = (region AnyRegion:$precondition);

  let assemblyFormat = [{
	$name $info $precondition attr-dict `:` type($results)
  }];

   let builders = [
        OpBuilder<(ins "TypeRange":$results, "StringRef":$name, "mlir::rlc::FunctionInfoAttr":$info), [{
            build($_builder, $_state, results, name, info, -1, -1);
    }]>];

  let extraClassDeclaration = [{
      llvm::SmallVector<llvm::StringRef, 4> getDeclaredNames() {
        llvm::SmallVector<llvm::StringRef, 4> names;
        for (auto args : getInfo().getArguments()) {
            names.push_back(args.getName());
        }

        return names;
      }
  }];
}

def RLC_IntegerLiteralUse : RLC_Dialect<"integer_literal_use", [DeclareOpInterfaceMethods<TypeCheckable>]> {

  let arguments = (ins TypeAttrOf<AnyType>:$input_type);
  let results = (outs RLC_IntegerType:$result);

  let assemblyFormat = [{
	$input_type `:` type($result) attr-dict
  }];
}

def RLC_Constant : RLC_Dialect<"constant", [DeclareOpInterfaceMethods<TypeCheckable>, ConstantLike, Pure]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyAttrOf<[I8Attr, I64Attr, F64Attr, BoolAttr, ArrayAttr]>:$value);
  let results = (outs AnyType:$result);

   let builders = [
        OpBuilder<(ins "double":$value), [{
            build($_builder, $_state, FloatType::get($_builder.getContext()), $_builder.getF64FloatAttr(value));
    }]>,OpBuilder<(ins "bool":$value), [{
            build($_builder, $_state, BoolType::get($_builder.getContext()),  $_builder.getBoolAttr(value));
    }]>,OpBuilder<(ins "std::int64_t":$value), [{
            build($_builder, $_state, IntegerType::getInt64($_builder.getContext()), $_builder.getI64IntegerAttr(value));
    }]>,OpBuilder<(ins "std::int8_t":$value), [{
            build($_builder, $_state, IntegerType::getInt8($_builder.getContext()), $_builder.getI8IntegerAttr(value));
    }]>

	];

  let assemblyFormat = [{
	$value type($result) attr-dict
  }];

  let hasFolder = 1;
}



def RLC_MemberAccess : RLC_Dialect<"member_access", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$value, I64Attr:$member_index);
  let results = (outs AnyType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$value, "std::int64_t":$fieldIndex), [{
			auto index = $_builder.getI64IntegerAttr(fieldIndex);
			auto real_type = value.getType().cast<ClassType>();
			auto resType = real_type.getMembers()[fieldIndex].getType();
            build($_builder, $_state, resType, value, index);
    }]>];

  let assemblyFormat = [{
	$value `:` type($value) `,` $member_index `:` type($result) attr-dict 
  }];
}

def RLC_ArrayAccess : RLC_Dialect<"array_access", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$value, AnyType:$member_index);
  let results = (outs AnyType:$result);

  let builders = [
		OpBuilder<(ins "mlir::Value":$value, "mlir::Value":$fieldIndex), [{
			if (auto real_type = value.getType().dyn_cast<ArrayType>()) {
				auto resType = real_type.getUnderlying();
				build($_builder, $_state, resType, value, fieldIndex);
			} else if (auto real_type = value.getType().dyn_cast<OwningPtrType>()) {
				auto resType = real_type.getUnderlying();
				build($_builder, $_state, resType, value, fieldIndex);
			} else if (auto real_type = value.getType().dyn_cast<StringLiteralType>()) {
				auto resType = mlir::rlc::IntegerType::getInt8(real_type.getContext()); 
				build($_builder, $_state, resType, value, fieldIndex);
			} else 
			abort();
    }]>];

  let assemblyFormat = [{
	`(` $value `:` type($value) `,` $member_index  `:` type($member_index) `)` `:` type($result) attr-dict 
  }];
}

def RLC_ExplicitConstructOp : RLC_Dialect<"explicit_construct", [DeclareOpInterfaceMethods<CallOpInterface>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins FunctionType:$initializer);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
		$initializer `:` type($initializer) `->` type($result) attr-dict
  }];

  let builders = [
		OpBuilder<(ins "mlir::Value":$initializer), [{
            build($_builder, $_state, initializer.getType().cast<mlir::FunctionType>().getInput(0), initializer);
    }]>];

}

def RLC_DestroyOp : RLC_Dialect<"destroy", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$value);

  let assemblyFormat = [{
		$value `:` type($value) attr-dict
  }];
}

def RLC_InplaceInitializeOp : RLC_Dialect<"inplace_initialize", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$value);

  let assemblyFormat = [{
		$value `:` type($value) attr-dict
  }];
}

def RLC_ConstructOp : RLC_Dialect<"construct", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<TypeUser>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins OptionalAttr<RLC_ShugarizedTypeAttr>:$shugarized_type);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
		type($result) ( `shugarized_type` `:` $shugarized_type^ )? attr-dict
  }];

   let builders = [
        OpBuilder<(ins "mlir::Type":$type), [{
			build($_builder, $_state, type, std::nullopt);
    }]>];

}

class RLC_BinaryBitwiseOp<string mnemonic, list<OpInterface> CA_Interface=[]> : RLC_Dialect<mnemonic, [TypeCheckable]#CA_Interface> {

    let extraClassDeclaration = [{
	
	  mlir::LogicalResult typeCheck(
              mlir::rlc::ModuleBuilder& builder) {
	      auto deducedTypeLHS = builder.getConverter().convertType(getLhs().getType());
	      auto deducedTypeRHS = builder.getConverter().convertType(getRhs().getType());
          if (not deducedTypeLHS or not deducedTypeLHS.isa<mlir::rlc::IntegerType>()) {
            
		    return logError(
				getOperation(),
				"Bitwise operation requires integer left operand");
          }

          if (not deducedTypeRHS or not deducedTypeRHS.isa<mlir::rlc::IntegerType>()) {
            
		    return logError(
				getOperation(),
				"Bitwis operation requires integer right operand");
          }

        getResult().setType(deducedTypeLHS);
        return mlir::success();
      }
	}];

    let arguments = (ins RLCIntegerOrUnkownType:$lhs, RLCIntegerOrUnkownType:$rhs);
    let results = (outs RLCIntegerOrUnkownType:$result);

    let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
      }]>];

    let assemblyFormat = [{
	  $rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
    }];
}

def RLC_BitOrOp : RLC_BinaryBitwiseOp<"bit_or"> {}
def RLC_BitAndOp : RLC_BinaryBitwiseOp<"bit_and"> {}
def RLC_BitXorOp : RLC_BinaryBitwiseOp<"bit_xor"> {}
def RLC_LeftShiftOp : RLC_BinaryBitwiseOp<"left_shift"> {}
def RLC_RightShiftOp : RLC_BinaryBitwiseOp<"right_shift"> {}

def RLC_BitNotOp : RLC_Dialect<"bit_not", [TypeCheckable]> {

    let extraClassDeclaration = [{
	
	  mlir::LogicalResult typeCheck(
              mlir::rlc::ModuleBuilder& builder) {
	      auto deducedType = builder.getConverter().convertType(getUnderlying().getType());
          if (not deducedType.isa<mlir::rlc::IntegerType>()) {
            
		    return logError(
				getOperation(),
				"Bitwise not requires integer operand");
          }

        getResult().setType(deducedType);
        return mlir::success();
      }
	}];

    let arguments = (ins RLCIntegerOrUnkownType:$underlying);
    let results = (outs RLCIntegerOrUnkownType:$result);

    let builders = [
        OpBuilder<(ins "mlir::Value":$underlying), [{
            build($_builder, $_state, underlying.getType(), underlying);
      }]>];

    let assemblyFormat = [{
	  $underlying`:` type($underlying) type($result) attr-dict
    }];
}

class RLC_InternalOp<string mnemonic, list<OpInterface> CA_Interface=[]> : RLC_Dialect<mnemonic, [TypeCheckable]#CA_Interface> {

    let extraClassDeclaration = [{
	
	mlir::LogicalResult typeCheck(
			mlir::rlc::ModuleBuilder& builder)
	{
		llvm::SmallVector<mlir::Type> acceptable;
		acceptable.push_back(mlir::rlc::IntegerType::getInt64(this->getContext()));
		acceptable.push_back(mlir::rlc::IntegerType::getInt8(this->getContext()));
		acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
		acceptable.push_back(mlir::rlc::FloatType::get(this->getContext()));
		return mlir::rlc::detail::typeCheckInteralOp(*this, builder, acceptable);
	}

	
	}];
}

class RLC_RelationalOp<string mnemonic> : RLC_Dialect<mnemonic, [TypeCheckable]> {

    let extraClassDeclaration = [{
	
	mlir::LogicalResult typeCheck(
			mlir::rlc::ModuleBuilder& builder)
	{
		llvm::SmallVector<mlir::Type> acceptable;
		acceptable.push_back(mlir::rlc::IntegerType::getInt64(this->getContext()));
		acceptable.push_back(mlir::rlc::IntegerType::getInt8(this->getContext()));
		acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
		acceptable.push_back(mlir::rlc::FloatType::get(this->getContext()));
		return mlir::rlc::detail::typeCheckInteralOp(*this, builder, acceptable, mlir::rlc::BoolType::get(this->getContext()));
	}


	
	}];
}

class RLC_BoolExp<string mnemonic> : RLC_Dialect<mnemonic, [TypeCheckable]> {

    let extraClassDeclaration = [{
	
	mlir::LogicalResult typeCheck(
			mlir::rlc::ModuleBuilder& builder)
	{
		llvm::SmallVector<mlir::Type> acceptable;
		acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
		return mlir::rlc::detail::typeCheckInteralOp(*this, builder, acceptable, mlir::rlc::BoolType::get(this->getContext()));
	}

	
	}];
}

def RLC_AddOp : RLC_InternalOp<"add", [DeclareOpInterfaceMethods<ConstraintsAnalyzable>]> {
  let summary = "add expression.";

  let description = [{
	add expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs AnyRLCScalarType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_MultOp : RLC_InternalOp<"mul"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs AnyRLCScalarType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_DivOp : RLC_InternalOp<"div"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs AnyRLCScalarType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_FromByteArrayOp : RLC_Dialect<"from_byte_array", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<TypeUser>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLCArrayOrUnkownType:$lhs, RLC_ShugarizedTypeAttr:$shugarized_type);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$lhs `:` type($lhs) `->` type($result) $shugarized_type attr-dict
  }];
}

def RLC_AsByteArrayOp : RLC_Dialect<"as_byte_array", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins  AnyRLCScalarType:$lhs);
  let results = (outs RLCArrayOrUnkownType:$result);

  let assemblyFormat = [{
	$lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_CastOp : RLC_Dialect<"cast", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins  AnyRLCScalarType:$lhs);
  let results = (outs AnyRLCScalarType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Type":$target), [{
            build($_builder, $_state, target, lhs);
    }]>];

  let assemblyFormat = [{
	$lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_MinusOp : RLC_InternalOp<"minus"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs);
  let results = (outs AnyRLCScalarType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs), [{
            build($_builder, $_state, lhs.getType(), lhs);
    }]>];

  let assemblyFormat = [{
	$lhs `:` type($lhs) `->` type($result) attr-dict
  }];
  let hasFolder = 1;
}

def RLC_ReminderOp : RLC_InternalOp<"reminder"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs AnyRLCScalarType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}


def RLC_InitOp : RLC_Dialect<"init", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs);

  let assemblyFormat = [{
	$lhs `:` type($lhs) attr-dict
  }];

    let extraClassDeclaration = [{
	
	
	}];

}

def RLC_ImplicitAssignOp : RLC_Dialect<"implicit_assign", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$lhs, AnyType:$rhs);

  let assemblyFormat = [{
	 $lhs `:` type($lhs) `,` $rhs `:` type($rhs)  attr-dict
  }];
}

def RLC_BuiltinAsPtr : RLC_Dialect<"builtin_as_ptr", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$value);

  let results = (outs RLC_IntegerType:$result);

  let assemblyFormat = [{
	 $value`:` type($value) `->` type($result) attr-dict
  }];

  let builders = [
        OpBuilder<(ins "mlir::Value":$value), [{
            build($_builder, $_state, mlir::rlc::IntegerType::getInt64($_builder.getContext()), value);
    }]>];
}

def RLC_BuiltinMangledNameOp : RLC_Dialect<"builtin_mangled_name", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$value);

  let results = (outs RLC_StringLiteralType:$result);

  let assemblyFormat = [{
	 $value`:` type($value) `->` type($result) attr-dict
  }];

  let builders = [
        OpBuilder<(ins "mlir::Value":$value), [{
            build($_builder, $_state, mlir::rlc::StringLiteralType::get($_builder.getContext()), value);
    }]>];
}

def RLC_BuiltinAssignOp : RLC_Dialect<"builtin_assign", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<ConstraintsAnalyzable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);

  let assemblyFormat = [{
	 $lhs `:` type($lhs) `,` $rhs `:` type($rhs)  attr-dict
  }];
}

def RLC_AssignOp : RLC_Dialect<"assign", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$lhs, AnyType:$rhs);

  let assemblyFormat = [{
	 $lhs `:` type($lhs) `,` $rhs `:` type($rhs) attr-dict
  }];
}

def RLC_NotOp : RLC_BoolExp<"not"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLCBoolOrUnkownType:$lhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs), [{
            build($_builder, $_state, lhs.getType(), lhs);
    }]>];

  let assemblyFormat = [{
	$lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}


def RLC_SubOp : RLC_InternalOp<"sub"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs AnyRLCScalarType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$lhs `:` type($lhs) `,` $rhs `:` type($rhs) `->` type($result) attr-dict
  }];
}

def RLC_OrOp : RLC_BoolExp<"or"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLCBoolOrUnkownType:$lhs, RLCBoolOrUnkownType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_AndOp : RLC_BoolExp<"and"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLCBoolOrUnkownType:$lhs, RLCBoolOrUnkownType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_GreaterOp : RLC_RelationalOp<"greater"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_LessOp : RLC_RelationalOp<"less"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_GreaterEqualOp : RLC_RelationalOp<"greater_equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_LessEqualOp : RLC_RelationalOp<"less_equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_EqualOp : RLC_RelationalOp<"equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$lhs, AnyType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_NotEqualOp : RLC_RelationalOp<"not_equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$lhs, AnyType:$rhs);
  let results = (outs RLCBoolOrUnkownType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}


def RLC_CurrentActionResumeIndex : RLC_Dialect<"current_action_resume_index"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins I64Attr:$index);


  let assemblyFormat = [{
	$index attr-dict
  }];
}

def RLC_Branch : RLC_Dialect<"br", [Terminator, DeclareOpInterfaceMethods<BranchOpInterface>] > {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let successors = (successor AnySuccessor:$destination);


  let assemblyFormat = [{
	$destination attr-dict
  }];
}

def RLC_CondBranch : RLC_Dialect<"crb", [Terminator, DeclareOpInterfaceMethods<BranchOpInterface>, DeclareOpInterfaceMethods<ConstraintsAnalyzable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLCBoolOrUnkownType:$cond);

  let successors = (successor AnySuccessor:$true_branch, AnySuccessor:$false_branch);


  let assemblyFormat = [{
	$cond type($cond) $true_branch $false_branch attr-dict
  }];
}

def RLC_ActionsStatement : RLC_Dialect<"actions_statement", [NoRegionArguments, DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let regions = (region
  VariadicRegion<AnyRegion>:$actions
  );


  let assemblyFormat = [{
	$actions attr-dict
  }];
}

def RLC_FlatActionStatement : RLC_Dialect<"flat_actions_statement", [Terminator, DeclareOpInterfaceMethods<BranchOpInterface>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let successors = (successor VariadicSuccessor<AnySuccessor>:$nexts);


  let assemblyFormat = [{
	$nexts attr-dict
  }];
}

def RLC_SelectBranch : RLC_Dialect<"switch", [Terminator, DeclareOpInterfaceMethods<BranchOpInterface>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLC_IntegerType:$cond);

  let successors = (successor VariadicSuccessor<AnySuccessor>:$nexts);


  let assemblyFormat = [{
	$cond type($cond) $nexts attr-dict
  }];
}

def RLC_StorageCast : RLC_Dialect<"storage_cast", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "Turns context and frame types into a regular type";

  let description = [{
    A storage cast is a cast introduced in actions to erase the type informations that refers to the location where a variable will be allocated. For example, if the argument of a action is `ctx Int a`, then before each use of `a` a storage cast must be introduced to remove the ctx qualifier. This operation should not appear in a regular function.
  }];

  let arguments = (ins AnyType:$operand);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$operand `:` type($operand) `->` type($result) attr-dict
  }];
}

def RLC_UnresolvedReference : RLC_Dialect<"unresolved_ref", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "statement list.";

  let arguments = (ins StrAttr:$name);
  let description = [{
	expression.
  }];
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$name `:` type($result) attr-dict
  }];

  let builders = [
        OpBuilder<(ins "llvm::StringRef":$name), [{
            build($_builder, $_state, UnknownType::get($_builder.getContext()), name);
    }]>];
}

def RLC_UnresolvedMemberAccess : RLC_Dialect<"unresolved_access", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$value, StrAttr:$member_name);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$value `:` type($value)  $member_name `:` type($result) attr-dict 
  }];
}

def RLC_ClassDeclaration : RLC_Dialect<"class_decl", [DeclareOpInterfaceMethods<TypeUser>, NoTerminator, NoRegionArguments, HasOnlyGraphRegion, DeclareOpInterfaceMethods<TypeDeclarer>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins StrAttr:$name,  ClassDeclarationFieldsArrayAttr:$members, DefaultValuedAttr<TypeArrayAttr, "{}">:$template_parameters, OptionalAttr<RLC_SourceRangeAttr>:$type_location);
  let results = (outs AnyType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Type":$class_type, "llvm::StringRef":$name, "llvm::ArrayRef<mlir::rlc::ClassFieldDeclarationAttr>":$members, "mlir::ArrayRef<mlir::Type>":$template_parameters, "mlir::rlc::SourceRangeAttr":$type_location), [{
            llvm::SmallVector<mlir::Attribute, 4> fieldAttrs;
            for (auto attr : members) {
                fieldAttrs.push_back(attr);
            }
            build($_builder, $_state, class_type, $_builder.getStringAttr(name), $_builder.getArrayAttr(fieldAttrs), $_builder.getTypeArrayAttr(template_parameters), type_location);
    }]>,
        OpBuilder<(ins "llvm::StringRef":$name, "llvm::ArrayRef<mlir::rlc::ClassFieldDeclarationAttr>":$members, "mlir::ArrayRef<mlir::Type>":$template_parameters, "mlir::rlc::SourceRangeAttr":$type_location), [{
            llvm::SmallVector<mlir::Attribute, 4> fieldAttrs;
            for (auto attr : members) {
                fieldAttrs.push_back(attr);
            }
            build($_builder, $_state, mlir::rlc::ClassType::getIdentified($_builder.getContext(), name, template_parameters), $_builder.getStringAttr(name), $_builder.getArrayAttr(fieldAttrs), $_builder.getTypeArrayAttr(template_parameters), type_location);
    }]>,
        OpBuilder<(ins "llvm::StringRef":$name, "llvm::ArrayRef<mlir::rlc::ClassFieldDeclarationAttr>":$members, "mlir::ArrayRef<mlir::Type>":$template_parameters), [{
            llvm::SmallVector<mlir::Attribute, 4> fieldAttrs;
            for (auto attr : members) {
                fieldAttrs.push_back(attr);
            }
            build($_builder, $_state, mlir::rlc::ClassType::getIdentified($_builder.getContext(), name, template_parameters), $_builder.getStringAttr(name), $_builder.getArrayAttr(fieldAttrs), $_builder.getTypeArrayAttr(template_parameters), nullptr);
    }]>,
];

  let regions = (region
  AnyRegion:$body
  );

  let extraClassDeclaration = [{
      mlir::rlc::ClassFieldDeclarationAttr getMemberField(size_t i) {
        return getMembers()[i].cast<mlir::rlc::ClassFieldDeclarationAttr>();
      }

      llvm::SmallVector <mlir::rlc::ClassFieldDeclarationAttr, 4> getMemberFields() {
        llvm::SmallVector <mlir::rlc::ClassFieldDeclarationAttr, 4> outs;
        for (size_t i = 0; i != getMembers().size(); i++ )
            outs.push_back(getMemberField(i));

        return outs;
      }

  }];

  let assemblyFormat = [{
	$name ( `template_parameters` $template_parameters^ )? $members type($result) $body (`location` `=` $type_location^ )? attr-dict 
  }];
}

def RLC_BuiltinRuntineTypeGlobal : RLC_Dialect<"builtin_runtime_type_global"> {

  let summary = "Rapresents a builtin type runtime information";

  let arguments = (ins StrAttr:$name);

  let results = (outs RLC_TypeType:$result);

  let assemblyFormat = [{
	$name type($result) attr-dict 
  }];

  let builders = [
        OpBuilder<(ins "llvm::StringRef":$name), [{
            build($_builder, $_state, TypeType::get($_builder.getContext()), name);
    }]>];
}

def RLC_ArrayRuntimeTypeGlobal : RLC_Dialect<"array_runtime_type_global"> {

  let summary = "Rapresents a array type runtime information";

  let arguments = (ins RLC_TypeType:$underlying, I64Attr:$size);

  let results = (outs RLC_TypeType:$result);

  let assemblyFormat = [{
	$underlying type($underlying) `,` $size type($result) attr-dict 
  }];

  let builders = [
        OpBuilder<(ins "mlir::Value":$underlying, "size_t":$size), [{
            build($_builder, $_state, TypeType::get($_builder.getContext()), underlying, size);
    }]>];
}


def RLC_FunctionRuntimeTypeGlobal : RLC_Dialect<"function_runtime_type_global"> {

  let summary = "Rapresents a function type runtime information";

  let arguments = (ins RLC_TypeType:$output, Variadic<RLC_TypeType>:$inputs);

  let results = (outs RLC_TypeType:$result);

  let assemblyFormat = [{
	`(` $inputs type($inputs) `->` $output type($output) `)` type($result) attr-dict 
  }];

  let builders = [
        OpBuilder<(ins "mlir::Value":$output, CArg<"mlir::ValueRange", "{}">:$inputs), [{
            build($_builder, $_state, TypeType::get($_builder.getContext()), output, inputs);
    }]>];
}

def RLC_ClassRuntimeTypeGlobal : RLC_Dialect<"class_runtime_type_global"> {

  let summary = "Rapresents a class type runtime information";

  let arguments = (ins StrAttr:$name, Variadic<RLC_TypeType>:$fields, StrArrayAttr:$field_names);

  let results = (outs RLC_TypeType:$result);

  let assemblyFormat = [{
	$name `(` $fields type($fields) `,` $field_names `)` type($result) attr-dict 
  }];

  let builders = [
        OpBuilder<(ins "llvm::StringRef":$name, CArg<"mlir::ValueRange", "{}">:$fields, CArg<"mlir::ArrayAttr", "{}">:$fieldNames), [{
            build($_builder, $_state, TypeType::get($_builder.getContext()), name, fields, fieldNames);
    }]>];
}


def RLC_MemMove : RLC_Dialect<"mem_move"> {

  let arguments = (ins AnyType:$dest, AnyType:$source);

  let assemblyFormat = [{
	$dest type($dest) `=` $source type($source) attr-dict 
  }];
}

def RLC_MemSetZero : RLC_Dialect<"mem_set_zero"> {

  let arguments = (ins AnyType:$dest);

  let assemblyFormat = [{
	$dest type($dest) attr-dict 
  }];
}

def RLC_TemplateInstantiationOp : RLC_Dialect<"template_instantiation", [DeclareOpInterfaceMethods<TypeCheckable>]> {

  let arguments = (ins FunctionType:$inputTemplate);

  let results = (outs FunctionType:$result);

  let assemblyFormat = [{
	$inputTemplate type($inputTemplate) `->` type($result) attr-dict 
  }];
}

def RLC_NullOp : RLC_Dialect<"null"> {

  let results = (outs RLC_OwningPtrType:$result);

  let assemblyFormat = [{
	type($result) attr-dict 
  }];
}

def RLC_ValueUpcastOp : RLC_Dialect<"value_upcast", [DeclareOpInterfaceMethods<TypeCheckable>]> {

  let arguments = (ins AnyType:$input);

  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$input type($input) `->` type($result) attr-dict 
  }];
}

def RLC_FunctionMetadataOp : RLC_Dialect<"function_metadata"> {
  let arguments = (ins FunctionType:$source_function, FunctionType:$precondition_function);

  let assemblyFormat = [{
	$source_function type($source_function) $precondition_function type($precondition_function) attr-dict 
  }];
}

def RLC_UnderTypeCheckMarker : RLC_Dialect<"under_type_check", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary="used then them removed in type check to mark who si being currently typechecked";
  let assemblyFormat = "attr-dict";
}

def RLC_AbortOp : RLC_Dialect<"abort", [Terminator]> {
  let summary="terminates the execution.";

  let arguments = (ins StrAttr:$message);
  
  let assemblyFormat = "$message attr-dict";
}

def RLC_AssertOp : RLC_Dialect<"assert", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary="terminates the execution if the assertion does not evaluate to true.";

  let arguments = (ins RLCBoolOrUnkownType:$assertion, StrAttr:$message);

  let assemblyFormat = "$assertion type($assertion) attr-dict";
}

def RLC_UncheckedCanOp : RLC_Dialect <"unchecked_can", [DeclareOpInterfaceMethods<TypeCheckable>, NoTerminator, NoRegionArguments]> {
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
        $body attr-dict `:` type($result)
    }];

  let regions = (region
    AnyRegion:$body
  );

  let builders = [
        OpBuilder<(ins), [{
			auto resultType = mlir::rlc::UnknownType::get(
					$_builder.getContext());
			build($_builder, $_state, resultType);
  }]>];
}

def RLC_CanOp : RLC_Dialect <"can", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let arguments = (ins AnyFunctionType:$callee);

  let results = (outs FunctionType:$result);

  let assemblyFormat = [{
        $callee `:` type($callee) attr-dict `:` type($result)
    }];

  let builders = [
        OpBuilder<(ins "mlir::Value":$value), [{
			auto resultType = mlir::FunctionType::get(
					value.getContext(),
					value.getType().cast<FunctionType>().getInputs(),
					mlir::rlc::BoolType::get(value.getContext()));
			build($_builder, $_state, resultType, value);
  }]>];
}

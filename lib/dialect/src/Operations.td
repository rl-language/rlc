include "Types.td"
include "Interfaces.td"
include "mlir/IR/FunctionInterfaces.td"
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
def RLCArrayOrUnkownType : AnyTypeOf<[RLC_ArrayType, RLC_OwningPtrType, RLC_Unknown]>;
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

  let arguments = (ins StrAttr:$name, StrAttr:$template_parameter);

  let regions = (region
	SizedRegion<1>:$body
  );

  let assemblyFormat = [{
	$name `<` $template_parameter `>` $body attr-dict
  }];

  let extraClassDeclaration = [{
	  mlir::rlc::TemplateParameterType getTemplateParameterType() {
		return mlir::rlc::TemplateParameterType::get(getContext(), getTemplateParameter(), nullptr, false);
	  }
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

def RLC_IfStatement : RLC_Dialect<"if", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "expression.";

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

  let assemblyFormat = [{
	`cond` $condition `body` $body attr-dict
  }];
}

def RLC_WhileStatement : RLC_Dialect<"while", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "while.";

  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$condition,
  AnyRegion:$body
  );

  let assemblyFormat = [{
	`cond` $condition `body` $body attr-dict
  }];
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

def RLC_Yield : RLC_Dialect<"yield", [ReturnLike, Terminator, DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchTerminatorOpInterface>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
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

def RLC_InitializerListOp : RLC_Dialect<"initializer_list", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "statement list.";

  let description = [{
	expression.
  }];

  let arguments = (ins Variadic<AnyType>:$args);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$args `:` type($args) type($result) attr-dict 
  }];
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

def RLC_MallocOp : RLC_Dialect<"malloc_op", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "implement malloc";

  let description = [{
	implements malloc.
  }];

  let arguments = (ins RLCIntegerOrUnkownType:$size);

  let results = (outs RLCPtrOrUnkownType:$result);

  let assemblyFormat = [{
	$size type($size) `->` type($result) attr-dict 
  }];
}

def RLC_EnumDeclarationOp : RLC_Dialect<"enum_declaration_op"> {
  let summary = "implement malloc";

  let description = [{
	implements malloc.
  }];

  let arguments = (ins StrAttr:$name, StrArrayAttr:$enum_names);

  let assemblyFormat = [{
	$name $enum_names attr-dict 
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

def RLC_UncheckedIsOp : RLC_Dialect<"UncheckedIsOp", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "implement is operator";

  let description = [{
	implements is operator.
  }];

  let arguments = (ins AnyType:$expression, TypeAttrOf<AnyType>:$type_or_trait);

  let results = (outs RLC_BoolType:$result);

  let assemblyFormat = [{
	$expression $type_or_trait type($expression) `->` type($result) attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::Value":$expression, "mlir::Type":$type), [{
			build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()), expression,type);
    }]>];
}

def RLC_IsOp : RLC_Dialect<"IsOp"> {
  let summary = "implement is operator";

  let description = [{
	implements is operator.
  }];

  let arguments = (ins AnyType:$expression, TypeAttrOf<AnyType>:$type_or_trait);

  let results = (outs RLC_BoolType:$result);

  let assemblyFormat = [{
	$expression  $type_or_trait  `:` type($expression) `->` type($result) attr-dict 
  }];

   let builders = [
        OpBuilder<(ins "mlir::Value":$expression, "mlir::Type":$type), [{
			build($_builder, $_state, mlir::rlc::BoolType::get($_builder.getContext()), expression,type);
    }]>];
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

def RLC_SubActionStatement : RLC_Dialect<"subaction", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "subaction.";

  let arguments = (ins DefaultValuedAttr<StrAttr, "\"\"">:$name, BoolAttr:$run_once, Variadic<AnyType>:$forwarded_args);
  let description = [{
	subaction.
  }];

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	( $name^ `=` )? $body ( $forwarded_args^ `:` type($forwarded_args))? attr-dict 
  }];
}

def RLC_DeclarationStatement : RLC_Dialect<"decl", [Symbol, DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "statement list.";

  let arguments = (ins StrAttr:$sym_name);
  let description = [{
	expression.
  }];

  let results = (outs AnyType:$result);

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	$sym_name `:` type($result) $body  attr-dict 
  }];
}


class RLC_FunctionBase<string mnemonic> :
RLC_Dialect<mnemonic, [CallableOpInterface, AutomaticAllocationScope, TypeCheckable]> {

  let arguments = (ins StrAttr:$unmangled_name, StrArrayAttr:$argNames);
  let description = [{
	expression.
  }];

  let results = (outs FunctionType:$result);


  let regions = (region
  AnyRegion:$body, AnyRegion:$precondition
  );

   let builders = [
        OpBuilder<(ins "StringRef":$name, "FunctionType":$type, "ArrayAttr":$argNames), [{
			build($_builder, $_state, type, name ,argNames);
    }]>];

  let assemblyFormat = [{
	$unmangled_name type($result) $argNames $precondition $body attr-dict 
  }];
    code extraBaseClassDeclaration = [{

        Region *getCallableRegion() {
            return isDeclaration() ? nullptr : &getBody();
        }

		std::string getMangledName() {
			return rlc::mangledName(getUnmangledName(), getType());
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
			return getUnmangledName().startswith("_");
		}

        bool isDeclaration() {
            return getBody().empty();
        }
	
		mlir::LogicalResult addToSymbolTable(mlir::rlc::SymbolTable<mlir::Value>& table) {
			for (const auto& [name, argument] : llvm::zip(getArgNames(), llvm::make_range(this->args_begin(), this->args_end()))) {
				table.add(name.cast<mlir::StringAttr>().str(), argument);
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

				for (const auto& [operand, name]: llvm::zip(region->front().getArguments(), getArgNames()))
				{
					auto converted = builder.getConverter().convertType(operand.getType());
					if (not converted){
						getOperation()->emitRemark("in of argument of function");
					}
					operand.setType(converted);
					builder.getSymbolTable().add(name.cast<mlir::StringAttr>(), operand);
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
    let extraClassDeclaration = extraBaseClassDeclaration;
}


def RLC_FunctionOp : RLC_FunctionBase<"fun"> {
  let summary = "function.";
  let arguments = (ins StrAttr:$unmangled_name, StrArrayAttr:$argNames, DefaultValuedAttr<TypeArrayAttr, "{}">:$template_parameters);
  let extraClassDeclaration = extraBaseClassDeclaration # [{
	mlir::LogicalResult typeCheckFunctionDeclaration(
		mlir::IRRewriter& rewriter,
		mlir::rlc::RLCTypeConverter& converter);
  }];

   let builders = [
        OpBuilder<(ins "StringRef":$name, "FunctionType":$type, "ArrayAttr":$argNames, CArg<"mlir::TypeRange", "{}">:$templateParameters), [{
			build($_builder, $_state, type, name ,argNames, $_builder.getTypeArrayAttr(templateParameters));
    }]>];

  let assemblyFormat = [{
	$unmangled_name( $template_parameters^ )? type($result) $argNames $precondition $body attr-dict 
  }];
}

def RLC_ActionFunction : RLC_FunctionBase<"action_fun"> {
  let summary = "function.";
  let results = (outs FunctionType:$result, AnyFunctionType:$is_done_function, Variadic<FunctionType>:$actions);
  let assemblyFormat = [{
	$unmangled_name type($result) type($actions) type($is_done_function) $argNames $precondition $body attr-dict 
  }];
    let extraClassDeclaration = extraBaseClassDeclaration # [{

		size_t getSubActionsSize() {
			return getActions().size();
		}

		mlir::FunctionType getSubActionType(size_t index) {
			return getActions().getTypes()[index].cast<mlir::FunctionType>();
		}

		mlir::FunctionType getIsDoneFunctionType() {
			return getIsDoneFunction().getType().cast<mlir::FunctionType>();
		}

		mlir::FunctionType getMainActionType() {
			return getResult().getType().cast<mlir::FunctionType>();
		}
		mlir::rlc::EntityType getEntityType() {
			return getMainActionType().getResult(0).cast<mlir::rlc::EntityType>();
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

    let arguments = (ins AnyFunctionType:$callee, Variadic<AnyType>:$args);
    let results = (outs Variadic<AnyType>);

    let assemblyFormat = [{
        $callee `:` type($callee) `(` $args `)` attr-dict `:` functional-type($args, results)
    }];

    let builders = [
        OpBuilder<(ins "Value":$callee, CArg<"mlir::ValueRange", "{}">:$args), [{
			auto calleeType = callee.getType();
			auto functionType = calleeType.cast<FunctionType>();
			if (functionType.getResults().size() == 0 or functionType.getResults().front() == VoidType::get($_builder.getContext()))
				build($_builder, $_state, mlir::TypeRange(), callee, args);
			else
			{

				mlir::Type resType = functionType.getResults().front();
				if (auto casted = resType.dyn_cast<mlir::rlc::ReferenceType>())
					resType = casted.getUnderlying();
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


def RLC_ActionStatement : RLC_Dialect<"action", [DeclareOpInterfaceMethods<TypeCheckable>, DeclareOpInterfaceMethods<RegionBranchOpInterface, ["getRegionInvocationBounds"]>]> {
  let summary = "statement list.";

  let arguments = (ins StrAttr:$name, StrArrayAttr:$declared_names, I64Attr:$id, I64Attr:$resumption_point);
  let description = [{
	expression.
  }];
  let results = (outs Variadic<AnyType>:$results);

   let builders = [
        OpBuilder<(ins "TypeRange":$results, "StringRef":$name, "ArrayRef<std::string>":$declared_names, CArg<"int64_t", "-1">:$id, CArg<"int64_t", "-1">:$resumption_point), [{
            auto nameAttr = $_builder.getStringAttr(name);
            llvm::SmallVector<Attribute> attrs;
            for (auto& name : declared_names)
              attrs.push_back($_builder.getStringAttr(name));
            build($_builder, $_state, results, nameAttr, $_builder.getArrayAttr(attrs), id, resumption_point);
    }]>];

  let regions = (region AnyRegion:$precondition);

  let assemblyFormat = [{
	$name `{` $declared_names`}` attr-dict `->` type($results) $precondition
  }];
}

def RLC_IntegerLiteralUse : RLC_Dialect<"integer_literal_use", [DeclareOpInterfaceMethods<TypeCheckable>]> {

  let arguments = (ins TypeAttrOf<AnyType>:$input_type);
  let results = (outs RLC_IntegerType:$result);

  let assemblyFormat = [{
	$input_type `:` type($result) attr-dict
  }];
}

def RLC_Constant : RLC_Dialect<"constant", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyAttrOf<[I8Attr, I64Attr, F64Attr, BoolAttr]>:$value);
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
			auto real_type = value.getType().cast<EntityType>();
			auto resType = real_type.getBody()[fieldIndex];
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

  let arguments = (ins RLCArrayOrUnkownType:$value, RLCIntegerOrUnkownType:$member_index);
  let results = (outs AnyType:$result);

  let builders = [
		OpBuilder<(ins "mlir::Value":$value, "mlir::Value":$fieldIndex), [{
			if (auto real_type = value.getType().dyn_cast<ArrayType>()) {
				auto resType = real_type.getUnderlying();
				build($_builder, $_state, resType, value, fieldIndex);
			} else if (auto real_type = value.getType().dyn_cast<OwningPtrType>()) {
				auto resType = real_type.getUnderlying();
				build($_builder, $_state, resType, value, fieldIndex);
			} else 
			abort();
    }]>];

  let assemblyFormat = [{
	`(` $value `:` type($value) `,` $member_index  `:` type($member_index) `)` `:` type($result) attr-dict 
  }];
}

def RLC_ExplicitConstructOp : RLC_Dialect<"explicit_construct"> {
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

def RLC_ConstructOp : RLC_Dialect<"construct", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let results = (outs AnyType:$result);

  let assemblyFormat = [{
		type($result) attr-dict
  }];

}

class RLC_InternalOp<string mnemonic> : RLC_Dialect<mnemonic, [TypeCheckable]> {

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

def RLC_AddOp : RLC_InternalOp<"_add"> {
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

def RLC_MultOp : RLC_InternalOp<"_mul"> {
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

def RLC_DivOp : RLC_InternalOp<"_div"> {
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

def RLC_FromByteArrayOp : RLC_Dialect<"from_byte_array", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLCArrayOrUnkownType:$lhs);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$lhs `:` type($lhs) `->` type($result) attr-dict
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

def RLC_MinusOp : RLC_InternalOp<"_minus"> {
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
}

def RLC_ReminderOp : RLC_InternalOp<"_reminder"> {
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
  let results = (outs AnyType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	 $lhs `:` type($lhs) `,` $rhs `:` type($rhs) `->` type($result) attr-dict
  }];
}

def RLC_BuiltinAssignOp : RLC_Dialect<"builtin_assign", [DeclareOpInterfaceMethods<TypeCheckable>]> {
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

def RLC_AssignOp : RLC_Dialect<"assign", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyType:$lhs, AnyType:$rhs);
  let results = (outs AnyType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	 $lhs `:` type($lhs) `,` $rhs `:` type($rhs) `->` type($result) attr-dict
  }];
}

def RLC_NotOp : RLC_BoolExp<"_not"> {
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


def RLC_SubOp : RLC_InternalOp<"_sub"> {
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

def RLC_OrOp : RLC_BoolExp<"_or"> {
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

def RLC_AndOp : RLC_BoolExp<"_and"> {
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

def RLC_GreaterOp : RLC_RelationalOp<"_greater"> {
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

def RLC_LessOp : RLC_RelationalOp<"_less"> {
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

def RLC_GreaterEqualOp : RLC_RelationalOp<"_greater_equal"> {
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

def RLC_LessEqualOp : RLC_RelationalOp<"_less_equal"> {
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

def RLC_EqualOp : RLC_RelationalOp<"_equal"> {
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

def RLC_NotEqualOp : RLC_RelationalOp<"_not_equal"> {
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

def RLC_CondBranch : RLC_Dialect<"crb", [Terminator, DeclareOpInterfaceMethods<BranchOpInterface>]> {
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

  let assemblyFormat = [{
	$name attr-dict `:` type($result)
  }];
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

def RLC_EntityDeclaration : RLC_Dialect<"entity_decl"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins StrAttr:$name, TypeArrayAttr:$memberTypes, StrArrayAttr:$memberNames, DefaultValuedAttr<TypeArrayAttr, "{}">:$template_parameters);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$name ( `template_parameters` $template_parameters^ )? $memberTypes $memberNames type($result) attr-dict 
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

def RLC_EntityRuntimeTypeGlobal : RLC_Dialect<"entity_runtime_type_global"> {

  let summary = "Rapresents a entity type runtime information";

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

def RLC_TemplateInstantiationOp : RLC_Dialect<"template_instantiation", [DeclareOpInterfaceMethods<TypeCheckable>]> {

  let arguments = (ins FunctionType:$inputTemplate);

  let results = (outs FunctionType:$result);

  let assemblyFormat = [{
	$inputTemplate type($inputTemplate) `->` type($result) attr-dict 
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


def RLC_AbortOp : RLC_Dialect<"abort", [Terminator]> {
  let summary="terminates the execution.";
  
  let assemblyFormat = "attr-dict";
}

def RLC_AssertOp : RLC_Dialect<"assert"> {
  let summary="terminates the execution if the assertion does not evaluate to true.";

  let arguments = (ins RLCBoolOrUnkownType:$assertion);

  let assemblyFormat = "$assertion type($assertion) attr-dict";
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

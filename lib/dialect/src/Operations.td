include "Types.td"
include "mlir/IR/FunctionInterfaces.td"
include "mlir/IR/RegionKindInterface.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/CallInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/ViewLikeInterface.td"

class RLC_Dialect<string mnemonic, list<Trait> traits = []> :
    Op<RLC_Dialect, mnemonic, traits>;

def AnyRLCScalarType : AnyTypeOf<[RLC_BoolType, RLC_IntegerType, RLC_FloatType]>;

def RLC_ExpressionStatement : RLC_Dialect<"exp"> {
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

def RLC_IfStatement : RLC_Dialect<"if"> {
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

def RLC_WhileStatement : RLC_Dialect<"while"> {
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

def RLC_ReturnStatement : RLC_Dialect<"return", [Terminator]> {
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

def RLC_Yield : RLC_Dialect<"yield", [Terminator]> {
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


  let assemblyFormat = [{
	($arguments^ `:` type($arguments))? attr-dict
  }];
}


def RLC_StatementList : RLC_Dialect<"list", [SymbolTable]> {
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

def RLC_DeclarationStatement : RLC_Dialect<"decl", [Symbol]> {
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
	$sym_name `=` $body attr-dict `:` type($result)
  }];
}

def RLC_FlatFunctionOp : RLC_Dialect<"flat_fun", [IsolatedFromAbove, Symbol, FunctionOpInterface, CallableOpInterface, AutomaticAllocationScope]> {
  let summary = "function.";

  let arguments = (ins StrAttr:$unmangledName, SymbolNameAttr:$sym_name, TypeAttrOf<FunctionType>:$function_type);
  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	$unmangledName $function_type $body attr-dict $sym_name
  }];

    let extraClassDeclaration = [{
        Region *getCallableRegion() {
            return isDeclaration() ? nullptr : &getBody();
        }

        ArrayRef<Type> getCallableResults() {
            return getFunctionType().getResults();
		}

        ArrayRef<Type> getArgumentTypes() {
            return getFunctionType().getInputs();
        }

        /// Returns the result types of this function.
        ArrayRef<Type> getResultTypes() {
            return getFunctionType().getResults();
        }

		bool isInternal() {
			return getName().startswith("_");
		}

        bool isDeclaration() {
            return getBody().empty();
        }

    }];
}

def RLC_FunctionOp : RLC_Dialect<"fun", [IsolatedFromAbove, Symbol, FunctionOpInterface, CallableOpInterface, AutomaticAllocationScope, SymbolTable]> {
  let summary = "function.";

  let arguments = (ins StrAttr:$unmangledName, SymbolNameAttr:$sym_name, TypeAttrOf<FunctionType>:$function_type);
  let description = [{
	expression.
  }];

  let regions = (region
  AnyRegion:$body
  );

  let assemblyFormat = [{
	$unmangledName $function_type $body attr-dict  $sym_name
  }];
    let extraClassDeclaration = [{
        Region *getCallableRegion() {
            return isDeclaration() ? nullptr : &getBody();
        }

        ArrayRef<Type> getCallableResults() {
            return getFunctionType().getResults();
		}

        ArrayRef<Type> getArgumentTypes() {
            return getFunctionType().getInputs();
        }

        /// Returns the result types of this function.
        ArrayRef<Type> getResultTypes() {
            return getFunctionType().getResults();
        }

		bool isInternal() {
			return getName().startswith("_");
		}

        bool isDeclaration() {
            return getBody().empty();
        }

    }];
}

def RLC_CallOp : RLC_Dialect<"call",
    [CallOpInterface,
    DeclareOpInterfaceMethods<SymbolUserOpInterface>
    ]>
{
    let summary = "Call operation.";

    let arguments = (ins AnyType:$callee, Variadic<AnyType>:$args);
    let results = (outs Variadic<AnyType>);

    let assemblyFormat = [{
        $callee `:` type($callee) `(` $args `)` attr-dict `:` functional-type($args, results)
    }];

    let builders = [
        OpBuilder<(ins "Value":$callee, CArg<"mlir::ValueRange", "{}">:$args), [{
			auto calleeType = callee.getType();
			auto functionType = calleeType.cast<FunctionType>();
			if (functionType.getResults().size() != 0 and functionType.getResults().front() == VoidType::get($_builder.getContext()))
				build($_builder, $_state, mlir::TypeRange(), callee, args);
			else
				build($_builder, $_state, functionType.getResults().front(), callee, args);
        }]>    
	];

    let extraClassDeclaration = [{
        //===------------------------------------------------------------------===//
        // CallOpInterface
        //===------------------------------------------------------------------===//

        mlir::FunctionType getCalleeType();

        /// Get the argument operands to the called function.
        operand_range getArgOperands() {
            return { arg_operand_begin(), arg_operand_end() };
        }

        operand_iterator arg_operand_begin() {
            return operand_begin();
        }

        operand_iterator arg_operand_end() {
            return operand_end();
        }

        /// Return the callee of this operation.
        mlir::CallInterfaceCallable getCallableForCallee() {
            return (*this)->getAttrOfType<SymbolRefAttr>("callee");
        }
    }];
}


def RLC_Reference : RLC_Dialect<"Reference"> {
  let summary = "statement list.";

  let arguments = (ins FlatSymbolRefAttr:$name);
  let description = [{
	expression.
  }];
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$name attr-dict `->` type($result)
  }];

   let builders = [
        OpBuilder<(ins "FunctionOp":$function), [{
            build($_builder, $_state, function.getFunctionType(), mlir::FlatSymbolRefAttr::get(function.getSymNameAttr()));

    }]>];

  let assemblyFormat = [{
	$name attr-dict `:` type($result)
  }];
}

def RLC_ActionStatement : RLC_Dialect<"action"> {
  let summary = "statement list.";

  let arguments = (ins StrAttr:$name, StrArrayAttr:$declared_names);
  let description = [{
	expression.
  }];
  let results = (outs Variadic<AnyType>:$results);

   let builders = [
        OpBuilder<(ins "TypeRange":$results, "StringRef":$name, "ArrayRef<std::string>":$declared_names), [{
            auto nameAttr = $_builder.getStringAttr(name);
            llvm::SmallVector<Attribute> attrs;
            for (auto& name : declared_names)
              attrs.push_back($_builder.getStringAttr(name));
            build($_builder, $_state, results, nameAttr, $_builder.getArrayAttr(attrs));
    }]>];


  let assemblyFormat = [{
	$name `{` $declared_names`}` attr-dict `->` type($results)
  }];
}


def RLC_Constant : RLC_Dialect<"constant"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyAttrOf<[I64Attr, F64Attr, BoolAttr]>:$value);
  let results = (outs AnyType:$result);

   let builders = [
        OpBuilder<(ins "double":$value), [{
            build($_builder, $_state, FloatType::get($_builder.getContext()), $_builder.getF64FloatAttr(value));
    }]>,OpBuilder<(ins "bool":$value), [{
            build($_builder, $_state, BoolType::get($_builder.getContext()),  $_builder.getBoolAttr(value));
    }]>,OpBuilder<(ins "std::int64_t":$value), [{
            build($_builder, $_state, IntegerType::get($_builder.getContext()), $_builder.getI64IntegerAttr(value));
    }]>
	];

  let assemblyFormat = [{
	$value type($result) attr-dict
  }];
}


def RLC_MemberAccess : RLC_Dialect<"member_access"> {
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

def RLC_ArrayAccess : RLC_Dialect<"array_access"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLC_ArrayType:$value, RLC_IntegerType:$member_index);
  let results = (outs AnyType:$result);

  let builders = [
		OpBuilder<(ins "mlir::Value":$value, "mlir::Value":$fieldIndex), [{
			auto real_type = value.getType().cast<ArrayType>();
			auto resType = real_type.getUnderlying();
            build($_builder, $_state, resType, value, fieldIndex);
    }]>];

  let assemblyFormat = [{
	`(` $value `:` type($value) `,` $member_index  `:` type($member_index) `)` `:` type($result) attr-dict 
  }];
}

def RLC_ConstructOp : RLC_Dialect<"construct"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins FlatSymbolRefAttr:$initializer);
  let results = (outs AnyType:$result);

  let builders = [
        OpBuilder<(ins "FunctionOp":$init), [{
            build($_builder, $_state, init.getArgumentTypes().front(), init.getSymName());
    }]>];

  let assemblyFormat = [{
		$initializer type($result) attr-dict
  }];
}

def RLC_AddOp : RLC_Dialect<"add"> {
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

def RLC_MultOp : RLC_Dialect<"mul"> {
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

def RLC_DivOp : RLC_Dialect<"div"> {
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

def RLC_CastOp : RLC_Dialect<"cast"> {
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

def RLC_MinusOp : RLC_Dialect<"minus"> {
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

def RLC_ReminderOp : RLC_Dialect<"reminder"> {
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


def RLC_InitOp : RLC_Dialect<"init"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs);

  let assemblyFormat = [{
	$lhs `:` type($lhs) attr-dict
  }];

}

def RLC_AssignOp : RLC_Dialect<"assign"> {
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

def RLC_NotOp : RLC_Dialect<"not"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLC_BoolType:$lhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs), [{
            build($_builder, $_state, lhs.getType(), lhs);
    }]>];

  let assemblyFormat = [{
	$lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}


def RLC_SubOp : RLC_Dialect<"sub"> {
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

def RLC_OrOp : RLC_Dialect<"or"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLC_BoolType:$lhs, RLC_BoolType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_AndOp : RLC_Dialect<"and"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLC_BoolType:$lhs, RLC_BoolType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_GreaterOp : RLC_Dialect<"greater"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_LessOp : RLC_Dialect<"less"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_GreaterEqualOp : RLC_Dialect<"greater_equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_LessEqualOp : RLC_Dialect<"less_equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_EqualOp : RLC_Dialect<"equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_NotEqualOp : RLC_Dialect<"not_equal"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins AnyRLCScalarType:$lhs, AnyRLCScalarType:$rhs);
  let results = (outs RLC_BoolType:$result);

  let builders = [
        OpBuilder<(ins "mlir::Value":$lhs, "mlir::Value":$rhs), [{
            build($_builder, $_state, BoolType::get(lhs.getContext()), lhs, rhs);
    }]>];

  let assemblyFormat = [{
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
  }];
}

def RLC_Branch : RLC_Dialect<"br", [Terminator]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let successors = (successor AnySuccessor:$destination);


  let assemblyFormat = [{
	$destination attr-dict
  }];
}

def RLC_CondBranch : RLC_Dialect<"crb", [Terminator]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins RLC_BoolType:$cond);

  let successors = (successor AnySuccessor:$true_branch, AnySuccessor:$false_branch);


  let assemblyFormat = [{
	$cond type($cond) $true_branch $false_branch attr-dict
  }];
}

def RLC_SelectBranch : RLC_Dialect<"switch", [Terminator]> {
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

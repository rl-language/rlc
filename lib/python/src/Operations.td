include "Types.td"
include "Interfaces.td"
include "mlir/IR/RegionKindInterface.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/CallInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/ViewLikeInterface.td"

class RLC_PythonOp<string mnemonic, list<Trait> traits = []> :
    Op<RLC_Python, mnemonic, traits>;


def RLC_CTypesLoad : RLC_PythonOp<"load_lib", [DeclareOpInterfaceMethods<EmitPython>]> {
  let arguments = (ins StrAttr:$lib_name);

  let results = (outs RLC_CDLLType:$result);

  let assemblyFormat = [{
	$lib_name type($result) attr-dict 
  }];
}


def RLC_CTypeStructDecl : RLC_PythonOp<"CTypeStruct", [DeclareOpInterfaceMethods<EmitPython>]> {
  let arguments = (ins TypeAttr:$type, StrArrayAttr:$field_names);

  let assemblyFormat = [{
	$type $field_names attr-dict 
  }];
}

def RLC_AssignResultType : RLC_PythonOp<"assign_result", [DeclareOpInterfaceMethods<EmitPython>]> {
  let arguments = (ins AnyType:$lhs, TypeAttr:$returnType);

  let assemblyFormat = [{
	$lhs type($lhs) $returnType attr-dict 
  }];
}

def RLC_PythonAccess : RLC_PythonOp<"access", [DeclareOpInterfaceMethods<EmitPython>]> {
  let arguments = (ins AnyType:$lhs, StrAttr:$field_name);

  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$lhs type($lhs) $field_name type($result) attr-dict 
  }];
}

def RLC_PythonReturn : RLC_PythonOp<"return", [DeclareOpInterfaceMethods<EmitPython>, Terminator]> {
  let arguments = (ins Variadic<AnyType>:$lhs);

  let assemblyFormat = [{
	$lhs type($lhs) attr-dict 
  }];
}

def RLC_PythonArgumentConstraint : RLC_PythonOp<"argument_constraint", [DeclareOpInterfaceMethods<EmitPython>]> {
  let arguments = (ins AnyType:$argument, SI64Attr:$min, SI64Attr:$max);

  let assemblyFormat = [{
	$argument type($argument) $min $max attr-dict 
  }];
}

def RLC_PythonActionInfo : RLC_PythonOp<"action_info", [DeclareOpInterfaceMethods<EmitPython>, NoTerminator]> {
  let arguments = (ins AnyType:$action);

  let regions = (region
  AnyRegion:$body 
  );


  let assemblyFormat = [{
	$action type($action) $body attr-dict 
  }];
}

def RLC_PythonCast : RLC_PythonOp<"cast", [DeclareOpInterfaceMethods<EmitPython>]> {
  let arguments = (ins AnyType:$lhs);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$lhs type($lhs) type($result) attr-dict 
  }];
}

def RLC_PythonFun : RLC_PythonOp<"fun", [Symbol, CallableOpInterface, AutomaticAllocationScope, DeclareOpInterfaceMethods<EmitPython>]> {
  let arguments = (ins SymbolNameAttr:$sym_name, StrAttr:$overload_name, StrArrayAttr:$args);
  let results = (outs FunctionType:$result);
  let regions = (region
  AnyRegion:$body 
  );

  let assemblyFormat = [{
	$sym_name $overload_name $args type($result) $body attr-dict 
  }];

    let extraClassDeclaration = [{
        Region *getCallableRegion() {
            return isDeclaration() ? nullptr : &getBody();
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

        ArrayRef<Type> getArgumentTypes() {
            return getType().getInputs();
        }

        /// Returns the result types of this function.
        ArrayRef<Type> getResultTypes() {
            return getType().getResults();
        }

		bool isInternal() {
			return getName().starts_with("_");
		}

        bool isDeclaration() {
            return getBody().empty();
        }
	

		Block::args_iterator args_begin() {
			return getBody().front().args_begin();
		}

		Block::args_iterator args_end() {
			return getBody().front().args_end();
		}

    }];
}


def RLC_PythonCall : RLC_PythonOp<"call", [CallOpInterface, DeclareOpInterfaceMethods<EmitPython>]> {
    let arguments = (ins AnyType:$callee, Variadic<AnyType>:$args);
    let results = (outs Variadic<AnyType>);

    let assemblyFormat = [{
        $callee `:` type($callee) `(` $args `)` attr-dict `:` functional-type($args, results)
    }];

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

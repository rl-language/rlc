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

def AnyRLCScalarType : AnyTypeOf<[RLC_BoolType, RLC_IntegerType, RLC_FloatType, RLC_Unknown]>;

def RLCIntegerOrUnkownType : AnyTypeOf<[RLC_IntegerType, RLC_Unknown]>;
def RLCArrayOrUnkownType : AnyTypeOf<[RLC_ArrayType, RLC_Unknown]>;
def RLCBoolOrUnkownType : AnyTypeOf<[RLC_BoolType, RLC_Unknown]>;

def AnyFunctionType : AnyTypeOf<[RLC_Unknown, FunctionType]>;

def RLC_ExpressionStatement : RLC_Dialect<"exp", [DeclareOpInterfaceMethods<TypeCheckable>]> {
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

def RLC_IfStatement : RLC_Dialect<"if", [DeclareOpInterfaceMethods<TypeCheckable>]> {
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

def RLC_WhileStatement : RLC_Dialect<"while", [DeclareOpInterfaceMethods<TypeCheckable>]> {
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

def RLC_ReturnStatement : RLC_Dialect<"return", [Terminator, DeclareOpInterfaceMethods<TypeCheckable>]> {
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

def RLC_Yield : RLC_Dialect<"yield", [Terminator, DeclareOpInterfaceMethods<TypeCheckable>]> {
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


def RLC_StatementList : RLC_Dialect<"list", [SymbolTable, DeclareOpInterfaceMethods<TypeCheckable>]> {
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

def RLC_DeclarationStatement : RLC_Dialect<"decl", [Symbol, DeclareOpInterfaceMethods<TypeCheckable>]> {
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

  let arguments = (ins StrAttr:$unmangledName, StrArrayAttr:$argNames);
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
	$unmangledName $argNames type($result) $precondition $body attr-dict 
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
				mlir::IRRewriter& rew,
				mlir::rlc::SymbolTable<mlir::Value>& table,
				mlir::TypeConverter& conv)
		{

			for (auto* region : {&getBody(), &getPrecondition()}) {

				mlir::rlc::SymbolTable<mlir::Value> inner(&table);
				if (region->empty())
					continue;

				llvm::SmallVector<mlir::Operation*, 4>  ops;
				for (auto& operation : region->getOps())
					ops.push_back(&operation);

				for (const auto& [operand, name]: llvm::zip(region->front().getArguments(), getArgNames()))
				{
					auto converted = conv.convertType(operand.getType());
					if (not converted){
						getOperation()->emitRemark("in of argument of function");
					}
					operand.setType(converted);
					inner.add(name.cast<mlir::StringAttr>(), operand);
				}
				for (auto* operation : ops)
					if (mlir::rlc::typeCheck(*operation, rew, inner, conv)
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
    let extraClassDeclaration = extraBaseClassDeclaration;
}

def RLC_ActionFunction : RLC_FunctionBase<"action_fun"> {
  let summary = "function.";
  let results = (outs FunctionType:$result, AnyFunctionType:$is_done_function, Variadic<FunctionType>:$actions);
  let assemblyFormat = [{
	$unmangledName $argNames type($result) type($actions) type($is_done_function) $precondition $body attr-dict 
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
				auto resType = mlir::rlc::ArrayType::get($_builder.getContext(), functionType.getResults().front(), argumentSize);
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
				build($_builder, $_state, functionType.getResults().front(), callee, args);
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


def RLC_ActionStatement : RLC_Dialect<"action", [DeclareOpInterfaceMethods<TypeCheckable>]> {
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

  let regions = (region AnyRegion:$precondition);

  let assemblyFormat = [{
	$name `{` $declared_names`}` attr-dict `->` type($results) $precondition
  }];
}


def RLC_Constant : RLC_Dialect<"constant", [DeclareOpInterfaceMethods<TypeCheckable>]> {
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
			auto real_type = value.getType().cast<ArrayType>();
			auto resType = real_type.getUnderlying();
            build($_builder, $_state, resType, value, fieldIndex);
    }]>];

  let assemblyFormat = [{
	`(` $value `:` type($value) `,` $member_index  `:` type($member_index) `)` `:` type($result) attr-dict 
  }];
}

def RLC_ArrayConstructOp : RLC_Dialect<"array_construct"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins FunctionType:$initializer);
  let results = (outs RLC_ArrayType:$result);


  let assemblyFormat = [{
		$initializer `:` type($initializer) type($result) attr-dict
  }];
}

def RLC_ConstructOp : RLC_Dialect<"construct"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins FunctionType:$initializer);
  let results = (outs AnyType:$result);

  let builders = [
        OpBuilder<(ins "FunctionOp":$init), [{
            build($_builder, $_state, init.getArgumentTypes().front(), init);
    }]>];

  let assemblyFormat = [{
		$initializer `:` type($initializer) type($result) attr-dict
  }];
}

class RLC_InternalOp<string mnemonic> : RLC_Dialect<mnemonic, [TypeCheckable]> {

    let extraClassDeclaration = [{
	
	mlir::LogicalResult typeCheck(
			mlir::IRRewriter &rewriter,
			mlir::rlc::SymbolTable<mlir::Value> &table,
			mlir::TypeConverter &conv)
	{
		llvm::SmallVector<mlir::Type> acceptable;
		acceptable.push_back(mlir::rlc::IntegerType::get(this->getContext()));
		acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
		acceptable.push_back(mlir::rlc::FloatType::get(this->getContext()));
		return mlir::rlc::detail::typeCheckInteralOp(*this, rewriter, table, conv, acceptable);
	}

	
	}];
}

class RLC_RelationalOp<string mnemonic> : RLC_Dialect<mnemonic, [TypeCheckable]> {

    let extraClassDeclaration = [{
	
	mlir::LogicalResult typeCheck(
			mlir::IRRewriter &rewriter,
			mlir::rlc::SymbolTable<mlir::Value> &table,
			mlir::TypeConverter &conv)
	{
		llvm::SmallVector<mlir::Type> acceptable;
		acceptable.push_back(mlir::rlc::IntegerType::get(this->getContext()));
		acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
		acceptable.push_back(mlir::rlc::FloatType::get(this->getContext()));
		return mlir::rlc::detail::typeCheckInteralOp(*this, rewriter, table, conv, acceptable, mlir::rlc::BoolType::get(this->getContext()));
	}


	
	}];
}

class RLC_BoolExp<string mnemonic> : RLC_Dialect<mnemonic, [TypeCheckable]> {

    let extraClassDeclaration = [{
	
	mlir::LogicalResult typeCheck(
			mlir::IRRewriter &rewriter,
			mlir::rlc::SymbolTable<mlir::Value> &table,
			mlir::TypeConverter &conv)
	{
		llvm::SmallVector<mlir::Type> acceptable;
		acceptable.push_back(mlir::rlc::BoolType::get(this->getContext()));
		return mlir::rlc::detail::typeCheckInteralOp(*this, rewriter, table, conv, acceptable, mlir::rlc::BoolType::get(this->getContext()));
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

def RLC_AssignOp : RLC_InternalOp<"assign"> {
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
	$rhs `:` type($rhs) `,` $lhs `:` type($lhs) `->` type($result) attr-dict
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

def RLC_NotEqualOp : RLC_RelationalOp<"_not_equal"> {
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

def RLC_Branch : RLC_Dialect<"br", [Terminator] > {
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

  let arguments = (ins RLCBoolOrUnkownType:$cond);

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
	$value `:` type($value) `,` $member_name`:` type($result) attr-dict 
  }];
}

def RLC_EntityDeclaration : RLC_Dialect<"entity_decl"> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let arguments = (ins StrAttr:$name, TypeArrayAttr:$memberTypes, StrArrayAttr:$memberNames);
  let results = (outs AnyType:$result);

  let assemblyFormat = [{
	$name `(` $memberTypes `:` $memberNames `)` type($result) attr-dict 
  }];
}

def RLC_UnresConstructOp : RLC_Dialect<"unresolved_construct", [DeclareOpInterfaceMethods<TypeCheckable>]> {
  let summary = "constant.";

  let description = [{
	expression.
  }];

  let results = (outs AnyType:$result);

  let assemblyFormat = [{
		type($result) attr-dict
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

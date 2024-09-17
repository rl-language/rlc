# CONSTRAINT ANALYSIS

Here I will discuss briefly how the compiler pass `--rlc-test-function-constraints-analysis` works (I will try to avoid details as they are boring and not trivial to understand).

## INTRODUCTION:

We want to extract some information about how the input variables of a function will influence the output result, which for simplicity it is a boolean value.

So if we have a simple function:

```
fun foo(Int a):
	if a>0:
        return true
    else
        return false
```

We expect as output of the analysis: 

```
    a_TRUE : [1,MAX_INT]
    a_FALSE: [MIN_INT,0]
```

The output ranges are objects from the [llvm::ConstantRange](https://llvm.org/doxygen/classllvm_1_1ConstantRange.html) class.

The pass is based on the standard MLIR [Dense Backward DataFlow Analysis](https://mlir.llvm.org/doxygen/classmlir_1_1dataflow_1_1DenseBackwardDataFlowAnalysis.html).

So conceptually we start from the "return" and we traverse the call graph in reverse, extracting some information in the meantime.

## LATTICE AND JOIN

In the lattice we can keep track of the ranges of the variables by storing a map, with the **key** a pair `<mlir::Value, bool>` (which represents the variable and the relative output it produces) and **value** a `<llvm::ConstantRange>`.

The join operation of the analysis is the union of two ranges already defined in the **llvm::ConstantRange::unionWith** method.

*NOTE:* the TOP value of the range is `[MIN_INT,MAX_INT]` and the BOTTOM value is simply an empty entry.

# TRANSFER FUNCTION

Each important operation which has a transfer function in the analysis has an interface **mlir::rlc::ConstraintsAnalyzable**.

We will briefly discuss here what happens in all the interesting operations (at a very high level and omitting implementation magic):

### ARITHMETIC OPERATION

Since the analysis is backward, we need to start from the result and calculate the range of the operands. 

For the moment the only valid operation allowed is `result=operand1 + constant` (where '+' can be generalized to any binary operator).

So we extract the ranges of the result and the value of the constant and we perform the inverse operation `operand1 = result - constant`.

If for example the range of 'result' is TOP, then also the 'result' will be set to TOP.

### YIELD OPERATION

When encountering a function return, we just take the constant and put it in the lattice (as in `return true`) or we evaluate the ranges retrieved from the expression (as in `return a<10`).

### CONDITIONAL BRANCH OPERATION

In a branch we just take the expression in it and retrieve its ranges, depending on the relational operation (less,equal,greater equal,...) and update the lattice accordingly.

Moreover when in this particular operation we also need to peek at the lattice of the next branches, in order to understand which of the two values (a_TRUE or a_FALSE) to update.

### BUILTIN ASSIGN OPERATION

With this operation we need to be extremely careful, as it does not represent an SSA value.

So in this operation we just evaluate the cases like `a=a+1`, but something more could be explored in the future.

## WHAT TO DO NEXT:

The analysis works and most of the operations are detached from their IR representation up to a certain point, of course if you do `a>5` or `5<a` you need to correctly retrieve the information "5" from the IR.

In the future we could expand this idea a bit by:

* finish the relevant operations (arithmetic, remainder,...).
* understand what to do when there are no constants (i.e `if a<b`).
* understand if some mlir interface inheritance is possible to make the analysis prettier to read.
* review the builtin_assign operations (`a=temp+1` types of statement are not supported).
* understand why when attaching values to terminator operations everything works fine but when attaching it to relationals (rlc.lessop,...) sometimes does not work.
* see what else can be done with llvm::ConstantRange class and the possibility of overflows-underflows.
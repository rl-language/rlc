// Include the definition of the necessary tablegen constructs for defining
// our dialect. 
include "mlir/IR/DialectBase.td"

// Here is a simple definition of a dialect.
def RLC_Dialect : Dialect {
  let summary = "Rlc dialect.";
  let description = [{
	very important
  }];
  let hasConstantMaterializer = 1;

  /// This is the namespace of the dialect. It is used to encapsulate the sub-components
  /// of the dialect, such as operations ("my_dialect.foo").
  let name = "rlc";

  /// The C++ namespace that the dialect, and its sub-components, get placed in.
  let cppNamespace = "::mlir::rlc";
  let useDefaultTypePrinterParser = 0;

  let dependentDialects = [
	"LLVM::LLVMDialect"
  ];
  let extraClassDeclaration = [{
	void registerTypes();
	void registerOperations();

	mlir::Type parseType(mlir::DialectAsmParser &parser) const override;
	void printType(
		mlir::Type type, mlir::DialectAsmPrinter &printer) const override;
  }];
}

// Include the definition of the necessary tablegen constructs for defining
// our dialect. 
include "mlir/IR/DialectBase.td"

// Here is a simple definition of a dialect.
def RLC_Python : Dialect {
  let summary = "Rlc dialect.";
  let description = [{
	very important
  }];

  /// This is the namespace of the dialect. It is used to encapsulate the sub-components
  /// of the dialect, such as operations ("my_dialect.foo").
  let name = "python";

  /// The C++ namespace that the dialect, and its sub-components, get placed in.
  let cppNamespace = "::mlir::rlc::python";
  let useDefaultTypePrinterParser = 1;
  let useFoldAPI = kEmitFoldAdaptorFolder;

  let dependentDialects = [
  ];
  let extraClassDeclaration = [{
	void registerTypes();
	void registerOperations();

  }];
}

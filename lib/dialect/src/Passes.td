include "mlir/Pass/PassBase.td"
include "mlir/Rewrite/PassUtil.td"

def ConstraintsAnalysisPass : Pass<"rlc-test-function-constraints-analysis", "mlir::ModuleOp"> {
  let summary = "get minimum and maximum of function arguments";
  let dependentDialects = ["mlir::rlc::RLCDialect"];
}

def LowerActionPass : Pass<"rlc-lower-actions", "mlir::ModuleOp"> {
  let summary = "lower actions";
  let dependentDialects = ["mlir::rlc::RLCDialect"];
}

def LowerArrayCallsPass : Pass<"rlc-lower-array-calls", "mlir::ModuleOp"> {
  let summary = "lower array calls";
  let dependentDialects = ["mlir::rlc::RLCDialect"];
}

def LowerSubActionStatements : Pass<"rlc-lower-sub-action-statements", "mlir::ModuleOp"> {
  let summary = "lower subaction statments";
  let dependentDialects = ["mlir::rlc::RLCDialect"];
}

def LowerToCfPass : Pass<"rlc-to-cf", "mlir::ModuleOp"> {
  let summary = "lower to cf";
  let dependentDialects = ["rlc::RLCDialect"];
}

def MemberFunctionsToRegularFunctionsPass : Pass<"rlc-member-functions-to-regular-functions", "mlir::ModuleOp"> {
  let summary = "extract member functions and adds the implicit argument";
  let dependentDialects = ["rlc::RLCDialect"];
}

def ActionStatementsToCoroPass : Pass<"rlc-action-statements-to-coro", "mlir::ModuleOp"> {
  let summary = "lower to cf";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerToLLVMPass : Pass<"rlc-lower-to-llvm", "mlir::ModuleOp"> {
  let summary = "lower to cf";
  let dependentDialects = ["rlc::RLCDialect", "LLVM::LLVMDialect"];
  let options = [
    Option<"debug_info", "add debug info", "bool", /*default=*/"false",
           "add the debug info to the output module">,
    Option<"abort_symbol", "abort will call the provided symbol instead of trap", "std::string", /*default=*/"\"\"",
           "change the default behaviour of abort">,
  ];
}

def RemoveUselessAllocaPass : Pass<"rlc-remove-useless-alloca", "mlir::ModuleOp"> {
  let summary = "remove useless alloca";
  let dependentDialects = ["LLVM::LLVMDialect"];
}

def TypeCheckEntitiesPass : Pass<"rlc-type-check-entities", "mlir::ModuleOp"> {
  let summary = "type check rlc entities";
  let dependentDialects = ["rlc::RLCDialect"];
}

def TypeCheckPass : Pass<"rlc-type-check", "mlir::ModuleOp"> {
  let summary = "type check rlc functions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def RemoveUninitConstructsPass : Pass<"rlc-remove-uninit-constructs", "mlir::ModuleOp"> {
  let summary = "Remove unitialized constructs from functions that returned the results of shortcircuiting ands and ors";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitMainPass: Pass<"rlc-emit-main", "mlir::ModuleOp"> {
  let summary = "emit rlc main pass";
  let dependentDialects = ["rlc::RLCDialect"];
  let options = [
    Option<"debug_info", "add debug info", "bool", /*default=*/"false",
           "add the debug info to the output module">,
  ];
}

def LowerIsOperationsPass : Pass<"rlc-lower-is-operations", "mlir::ModuleOp"> {
  let summary = "lower is op operations";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerAssignPass : Pass<"rlc-assign-pass", "mlir::ModuleOp"> {
  let summary = "lower assign operations";
  let dependentDialects = ["rlc::RLCDialect"];
}

def InstantiateTemplatesPass : Pass<"rlc-instantiate-templates", "mlir::ModuleOp"> {
  let summary = "instantiate templates";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitAssignPass : Pass<"rlc-emit-implicit-assign-pass", "mlir::ModuleOp"> {
  let summary = "emit implicit assign pass";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerForFieldOpPass : Pass<"rlc-for-field-op", "mlir::ModuleOp"> {
  let summary = "lower for field operations";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerConstructOpPass : Pass<"rlc-lower-construct-op", "mlir::ModuleOp"> {
  let summary = "lower construct operations";
  let dependentDialects = ["rlc::RLCDialect"];
}


def TestReachingDefinitionsPass : Pass<"rlc-test-reaching-definitions-pass", "mlir::ModuleOp"> {
  let summary = "test reaching definitions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitInitPass : Pass<"rlc-emit-implicit-init-pass", "mlir::ModuleOp"> {
  let summary = "emits default builtin init functions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitDestructorInvocationsPass : Pass<"rlc-emit-implicit-destructor-invocation-pass", "mlir::ModuleOp"> {
  let summary = "emits default builtin init functions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitImplicitDestructorsPass : Pass<"rlc-emit-implicit-destructors-pass", "mlir::ModuleOp"> {
  let summary = "emits default builtin init functions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerDestructorsPass : Pass<"rlc-lower-destructors-pass", "mlir::ModuleOp"> {
  let summary = "maps destructors invocations to a call";
  let dependentDialects = ["rlc::RLCDialect"];
}

def EmitEnumEntitiesPass : Pass<"rlc-emit-enum-entities-pass", "mlir::ModuleOp"> {
  let summary = "for each enum declare a entity";
  let dependentDialects = ["rlc::RLCDialect"];
}

def SortTypeDeclarationsPass : Pass<"rlc-sort-type-declarations", "mlir::ModuleOp"> {
  let summary = "for each enum declare a entity";
  let dependentDialects = ["rlc::RLCDialect"];
}

def ExtractPreconditionPass : Pass<"rlc-extract-preconditions", "mlir::ModuleOp"> {
  let summary = "extract precondition pass";
  let dependentDialects = ["rlc::RLCDialect"];
}

def StripFunctionMetadataPass : Pass<"rlc-strip-function-metadata", "mlir::ModuleOp"> {
  let summary = "strip function metadata";
  let dependentDialects = ["rlc::RLCDialect"];
}

def AddOutOfBoundsCheckPass : Pass<"rlc-add-out-of-bounds-check", "mlir::ModuleOp"> {
  let summary = "add bound checks for array accesses";
  let dependentDialects = ["rlc::RLCDialect"];
}

def UncheckedAstToDotPass : Pass<"rlc-unchecked-ast-to-dot-pass", "mlir::ModuleOp"> {
  let summary = "makes a dot graph of the various actions";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">,
    Option<"print_parsable", "print in a machine parsable format that includes informations", "bool", /*default=*/"false",
           "print in a machine parsable format">,
    Option<"inline_calls", "inline all callees into their callsites", "bool", /*default=*/"false",
           "inline calls">,
    Option<"drop_non_actions", "drop non actions and only show actions", "bool", /*default=*/"false",
           "drop non actions">,
    Option<"regex_filter", "filter out actions functions not matching", "llvm::StringRef", /*default=*/"\".*\"",
           "filter">
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def PrintIRPass : Pass<"rlc-print-ir-pass", "mlir::ModuleOp"> {
  let summary = "prints the ir on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">,
    Option<"hide_position", "hide positions", "bool", /*default=*/"true",
           "avoid printing the ir locations">,
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def SerializeRLPass : Pass<"rlc-serialize-ir-pass", "mlir::ModuleOp"> {
  let summary = "prints the ir on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"&llvm::outs()",
           "stream on which to print the module">,
    Option<"file_to_serialize", "file to serialize", "llvm::StringRef", /*default=*/"\"\"",
           "particular file that must be serialized">
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def PrintPythonPass : Pass<"rlc-python-pass", "mlir::ModuleOp"> {
  let summary = "prints the python header on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">,
    Option<"isMac", "true if target platform is mac", "bool", /*default=*/"false",
           "true if target platform is mac">,
    Option<"isWindows", "true if target platform is windows", "bool", /*default=*/"false",
           "true if target platform is windows">,
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def PrintCSharpPass : Pass<"rlc-print-c-sharp-pass", "mlir::ModuleOp"> {
  let summary = "prints the c sharp header on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">,
    Option<"isMac", "true if target platform is mac", "bool", /*default=*/"false",
           "true if target platform is mac">,
    Option<"isWindows", "true if target platform is windows", "bool", /*default=*/"false",
           "true if target platform is windows">,
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def PrintCHeaderPass : Pass<"rlc-print-c-header-pass", "mlir::ModuleOp"> {
  let summary = "prints the c header on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def PrintNewCHeaderPass : Pass<"rlc-print-new-c-header-pass", "mlir::ModuleOp"> {
  let summary = "prints the new c header on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def PrintGodotPass : Pass<"rlc-print-godot-pass", "mlir::ModuleOp"> {
  let summary = "prints the godot file on the provided stream";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "stream on which to print the module">
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}


def ParseFilePass : Pass<"rlc-parse-file-pass", "mlir::ModuleOp"> {
  let summary = "parse file";
  let options = [
    Option<"includeDirs", "include dirs", "const llvm::SmallVector<std::string, 4>*", /*default=*/"nullptr",
           "include dirs">,
    Option<"inputs", "input files", "llvm::ArrayRef<std::string>", /*default=*/"llvm::ArrayRef<std::string>{}",
           "input files">,
    Option<"srcManager", "source manager", "llvm::SourceMgr*", /*default=*/"nullptr",
           "source manager">,
    Option<"keepComments", "keep comments", "bool", /*default=*/"false",
           "keep comments">,
    Option<"dependencyFile", "dependency file", "std::string", /*default=*/"\"\"",
           "dependency file">
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def PrintIncludedFilesPass : Pass<"rlc-print-included-files-pass", "mlir::ModuleOp"> {
  let summary = "print the file that got recursivelly included by the current one, including the current one";
  let options = [
    Option<"includeDirs", "include dirs", "const llvm::SmallVector<std::string, 4>*", /*default=*/"nullptr",
           "include dirs">,
    Option<"inputs", "input files", "llvm::ArrayRef<std::string>", /*default=*/"llvm::ArrayRef<std::string>{}",
           "input files">,
    Option<"srcManager", "source manager", "llvm::SourceMgr*", /*default=*/"nullptr",
           "source manager">,
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "output stream">,
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def RLCBackEndPass : Pass<"rlc-backend-pass", "mlir::ModuleOp"> {
  let summary = "parse file";
  let options = [
    Option<"OS", "output stream", "llvm::raw_ostream*", /*default=*/"nullptr",
           "output stream">,
    Option<"clangPath", "clang path", "std::string", /*default=*/"\"\"",
           "clang to use to link">,
    Option<"outputFile", "output file", "std::string", /*default=*/"\"\"",
           "output file">,
    Option<"extraObjectFiles", "extra object files", "const std::vector<std::string>*", /*default=*/"nullptr",
           "extra object files">,
    Option<"dumpIR", "dump ir and stop", "bool", /*default=*/"false",
           "dump ir and stop">,
    Option<"compileOnly", "compile and stop", "bool", /*default=*/"false",
           "compile and stop">,
    Option<"emitSanitizer", "must emit the sanitizer instrumentation", "bool", /*default=*/"false",
           "emit sanitizer instrumentation">,
    Option<"emitFuzzer", "link against libfuzzer", "bool", /*default=*/"false",
           "link against libfuzzer">,
     Option<"rpaths", "rpath entries", "const std::vector<std::string>*", /*default=*/"nullptr",
          "rpath entries">,
    Option<"targetInfo", "info about the target", "const mlir::rlc::TargetInfo*", /*default=*/"nullptr",
           "info about the target">,
    Option<"verbose", "print sub commands invoked", "bool", /*default=*/"false",
           "prints sub commands invoked">,
  ];
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerAssertsPass : Pass<"rlc-lower-asserts", "mlir::ModuleOp"> {
  let summary = "lower AssertOp's into if statements and AbortOp's";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerForLoopsPass : Pass<"rlc-lower-for-loops", "mlir::ModuleOp"> {
  let summary = "Lower for loops";
  let dependentDialects = ["rlc::RLCDialect"];
}

def AddPreconditionsCheckPass : Pass<"rlc-add-preconditions-check", "mlir::ModuleOp"> {
  let summary = "add bound checks for action preconditions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def RewriteCallSignaturesPass: Pass<"rlc-rewrite-call-signatures", "mlir::ModuleOp"> {
  let summary = "rewrite call signatures";
  let dependentDialects = ["rlc::RLCDialect"];
}

def ValidateStorageQualifiersPass : Pass<"rlc-validate-storage-qualifiers", "mlir::ModuleOp"> {
  let summary = "returns a error if a storage qualifiers is incompatible with the control flow of the actions";
  let dependentDialects = ["rlc::RLCDialect"];
}

def ConstantArrayToGlobalPass: Pass<"rlc-constant-array-to-global", "mlir::ModuleOp"> {
  let summary = "lower array constants to global objects";
  let dependentDialects = ["rlc::RLCDialect"];
}

def LowerInitializerListsPass : Pass<"rlc-lower-initializer-lists", "mlir::ModuleOp"> {
  let summary = "lower initializer list either to regular array";
  let dependentDialects = ["rlc::RLCDialect"];
}

def HoistAllocaPass : Pass<"rlc-hoist-allocasa", "mlir::ModuleOp"> {
  let summary = "lower initializer list either to regular array";
  let dependentDialects = ["LLVM::LLVMDialect"];
}

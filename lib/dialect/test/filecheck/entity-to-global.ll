// RUN: rlc-opt -rlc-lower-to-llvm %s | FileCheck %s

!Asd_0_ = !rlc.entity<Asd {rsd: !rlc.int<64>, tasd: !rlc.int<64>, }>

// CHECK-LABEL: module @unknown 
// CHECK:  llvm.mlir.global linkonce_odr constant @Asd() {addr_space = 0 : i32} : !llvm.struct<"globalVariableType", (ptr, i64, array<2 x ptr>)> {
module @unknown {
  %0 = rlc.entity_decl "Asd" [!rlc.int<64>, !rlc.int<64>] ["rsd", "tasd"] !Asd_0_ {}
}


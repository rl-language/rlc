// COM: -rlc-lower-to-llvm

!Asd_0_ = !rlc.entity<Asd {rsd: !rlc.int<64>, tasd: !rlc.int<64>, }>

// CHECK-LABEL: module @unknown 
module @unknown {
  // CHECK: llvm.mlir.global linkonce_odr constant @Asd() {addr_space = 0 : i32} : !llvm.struct<"globalVariableType", (ptr<i8>, i64, array<2 x ptr<i8>>)>
  %0 = rlc.entity_decl "Asd" [!rlc.int<64>, !rlc.int<64>] ["rsd", "tasd"] !Asd_0_
}


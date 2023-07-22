// COM: -rlc-lower-to-llvm

!Asd_0_ = !rlc.entity<Asd {rsd: !rlc.int<64>, tasd: !rlc.int<64>, }>

// CHECK-LABEL: module @unknown 
module @unknown {
  %0 = rlc.entity_decl "Asd" [!rlc.int<64>, !rlc.int<64>] ["rsd", "tasd"] !Asd_0_
}


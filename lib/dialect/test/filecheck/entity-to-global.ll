// COM: -rlc-lower-to-llvm

!Asd_0_ = !rlc.entity<Asd {rsd: !rlc.int, tasd: !rlc.int, }>

// CHECK-LABEL: module @unknown 
module @unknown {
  %0 = rlc.entity_decl "Asd" [!rlc.int, !rlc.int] ["rsd", "tasd"] !Asd_0_
}


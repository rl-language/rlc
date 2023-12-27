// RUN: rlc-opt -rlc-type-check-entities %s | FileCheck %s
// CHECK-LABEL: !Asd_0_ = !rlc.entity<Asd {rsd: !rlc.int<64>, tasd: !rlc.int<64>, }>

// CHECK-LABEL: module @unknown 
module @unknown {
  // CHECK-NEXT: %0 = rlc.entity_decl "Asd" [!rlc.int<64>, !rlc.int<64>] ["rsd", "tasd"] !Asd_0_
  %0 = rlc.entity_decl "Asd" [!rlc.type_use<"Int" array_size = !rlc.int_literal<0>>, !rlc.type_use<"Int" array_size = !rlc.int_literal<0>>] ["rsd", "tasd"] !rlc.unknown
}

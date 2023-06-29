// COM: -rlc-type-check 
// CHECK-LABEL: !Asd = !rlc<Asd{rsd: !rlc.int, tasd: !rlc.int, }>

// CHECK-LABEL: module @unknown 
module @unknown {
  // CHECK-NEXT: %0 = rlc.entity_decl "Asd" [!rlc.int, !rlc.int] ["rsd", "tasd"] !Asd
  %0 = rlc.entity_decl "Asd" [!rlc.type_use<"Int" 0>, !rlc.type_use<"Int" 0>] ["rsd", "tasd"] !rlc.unknown
}

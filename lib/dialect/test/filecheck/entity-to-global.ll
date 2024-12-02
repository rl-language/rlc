// RUN: rlc-opt -rlc-lower-to-llvm %s | FileCheck %s

// CHECK-LABEL: module @unknown 
// CHECK:  llvm.mlir.global linkonce_odr constant @Asd() {addr_space = 0 : i32} : !llvm.struct<"globalVariableType", (ptr, i64, array<2 x ptr>)> {

#field_info = #rlc.class_field<"asd" : !rlc.int<64>>
#field_info1 = #rlc.class_field<"tasd" : !rlc.int<64>>
#loc = loc("/tmp/asd.rl":2:5)
#loc1 = loc("/tmp/asd.rl":2:8)
#loc2 = loc("/tmp/asd.rl":3:5)
#loc3 = loc("/tmp/asd.rl":3:8)
#loc4 = loc("/tmp/asd.rl":1:5)
#loc5 = loc("/tmp/asd.rl":1:6)
!C_0_ = !rlc.class<Asd {#rlc.class_field<"asd" : !rlc.int<64>>, #rlc.class_field<"tasd" : !rlc.int<64>>, }>
#shugar_type = #rlc.shugarized_type<<#loc, #loc1> !rlc.int<64>>
#shugar_type1 = #rlc.shugarized_type<<#loc2, #loc3> !rlc.int<64>>
module @unknown {
  %0 = rlc.class_decl "Asd" [#rlc.class_field_declaration<#field_info shugarized = #shugar_type>, #rlc.class_field_declaration<#field_info1 shugarized = #shugar_type1>] !C_0_ {
  } location = <#loc4, #loc5>
}

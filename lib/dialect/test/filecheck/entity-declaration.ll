// RUN: rlc-opt -rlc-type-check-entities %s | FileCheck %s

#field_info = #rlc.class_field<"asd" : !rlc.type_use<"Int" array_size = !rlc.int_literal<0>>>
#field_info1 = #rlc.class_field<"tasd" : !rlc.type_use<"Int" array_size = !rlc.int_literal<0>>>
#loc = loc("/tmp/asd.rl":2:5)
#loc1 = loc("/tmp/asd.rl":2:8)
#loc2 = loc("/tmp/asd.rl":3:5)
#loc3 = loc("/tmp/asd.rl":3:8)
#loc4 = loc("/tmp/asd.rl":1:5)
#loc5 = loc("/tmp/asd.rl":1:6)
#shugar_type = #rlc.shugarized_type<<#loc, #loc1> !rlc.type_use<"Int" array_size = !rlc.int_literal<0>>>
#shugar_type1 = #rlc.shugarized_type<<#loc2, #loc3> !rlc.type_use<"Int" array_size = !rlc.int_literal<0>>>
// CHECK-LABEL: module @unknown 
module @unknown {
// CHECK-NEXT %0 = rlc.class_decl "C" [#rlc.class_field_declaration<#field_info shugarized = #shugar_type>, #rlc.class_field_declaration<#field_info1 shugarized = #shugar_type1>] !C_0_
  %0 = rlc.class_decl "C" [#rlc.class_field_declaration<#field_info shugarized = #shugar_type>, #rlc.class_field_declaration<#field_info1 shugarized = #shugar_type1>] !rlc.unknown {
  } location = <#loc4, #loc5>
}

# RUN: rlc %s -o - --ir -O2 | FileCheck %s

# CHECK-LABEL: define void @rl_m_init__Asd(ptr writeonly captures(none) initializes((0, 8)) %0) local_unnamed_addr 
# CHECK: store ptr null, ptr %0, align 8
cls Asd:
  OwningPtr<Int> a

fun main() -> Int:
  return 0


# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 9:1: error: Trait Trait already declared
# CHECK: 6:1: remark: Previous declaration here

trait<T> Trait:
	fun wasd(T x, Int y) -> Int

trait<T> Trait:
	fun rasd(T K)


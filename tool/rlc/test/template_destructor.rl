# RUN: rlc %s -o - -i %stdlib --flattened | FileCheck %s

ent<T> CustomDestructor:
    Int a

    fun drop():
        self.a = 2

# CHECK: rlc.flat_fun "main"
fun main() -> Int:
    # CHECK: rlc.ref @rl_m_drop__CustomDestructorTint64_tT_r_void
    # CHECK-NEXT: rlc.call
    let var : CustomDestructor<Int>
    return 0
# CHECK-NOT: rlc.ref @rl_m_drop__CustomDestructorTint64_tT_r_void

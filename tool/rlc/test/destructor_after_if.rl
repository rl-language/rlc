# RUN: rlc %s -o - -i %stdlib --flattened | FileCheck %s

import collections.vector

# CHECK: rlc.flat_fun "main"
fun main() -> Int:
    if (0 == 1):
        return 1
    let v : Vector<Int>
    if (0 == 1):
        # CHECK: rlc.ref @rl_m_drop__VectorTint64_tT_r_void
        # CHECK-NEXT: rlc.call
        return 1
    # CHECK: rlc.ref @rl_m_drop__VectorTint64_tT_r_void
    # CHECK-NEXT: rlc.call
    return 0
# CHECK-NOT: rlc.ref @rl_m_drop__VectorTint64_tT_r_void

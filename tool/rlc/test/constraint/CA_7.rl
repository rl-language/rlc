# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[-9223372036854775808,9223372036854775807)", a_T = "[11,20)", is_member_function = false}

fun foo(Int a) -> Bool {true}:
	if a > 10:
        if a < 20:
            return true
        if a > 15:
            return false
	return false


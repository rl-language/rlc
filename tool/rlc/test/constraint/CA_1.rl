# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[-9223372036854775808,11)", a_T = "[11,9223372036854775807)", is_member_function = false}

# THE POLICY WE USE NOW IS [min,max)

fun foo(Int a) -> Bool {true}:
	if a > 10:
		return true
	return false

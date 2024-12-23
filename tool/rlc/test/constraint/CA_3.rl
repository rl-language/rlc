# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[10,9223372036854775807)", a_T = "[-9223372036854775808,10)", b_F = "[-9223372036854775808,11)", b_T = "[11,9223372036854775807)", is_member_function = false}

fun foo(Int a, Int b) -> Bool {true}:
	if b > 10:
		if a < 10:
			return true
	return false


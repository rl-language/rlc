# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[10,11)", a_T = "[-9223372036854775808,9223372036854775807)", is_member_function = false}

fun foo(Int a) -> Bool:
	if a > 10:
		return true
	else if a < 10:
		return true
	return false


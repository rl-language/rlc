# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[-9223372036854775808,9223372036854775807)", a_T = "[5,10)", is_member_function = false}

fun foo(Int a) -> Bool {true}:
	a=a+1
	return a<11 and a>5

# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[-9223372036854775808,9223372036854775807)", a_T = "[1,9)", is_member_function = false}

fun foo(Int a) -> Bool:
	if a>0:
		let temp=a+1
		return temp<10
	return false

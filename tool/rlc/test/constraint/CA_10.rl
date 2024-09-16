# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[-9223372036854775808,9223372036854775807)", a_T = "[6,11)", is_member_function = false}

# We could note that doing this it would work fine
	#if a < 11:
	#	if a > 5:
	#		return true
	#return false

fun foo(Int a) -> Bool:
	return a<11 and a>5

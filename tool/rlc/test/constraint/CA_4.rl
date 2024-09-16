# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[-9223372036854775808,9223372036854775807)", a_T = "[11,10)", is_member_function = false}

# a_true  : invalid range (MIN > MAX)

fun foo(Int a) -> Bool:
	if a > 10:
        if a < 10:
            return true
	return false


# RUN: rlc %s --flattened --hide-position %t -i %stdlib -o - | rlc-opt --rlc-test-function-constraints-analysis | FileCheck %s

# CHECK: {a_F = "[-9223372036854775808,9223372036854775807)", a_T = "[6,11)", b_F = "[-9223372036854775808,9223372036854775807)", b_T = "[7,10)", c_F = "[-9223372036854775808,9223372036854775807)", c_T = "[8,9)", is_member_function = false}

# We could note that doing this it would work fine
	#if a < 11:
	#	if a > 5:
	#		if b < 10: 
	#			if b > 6:
	#				if c < 9:
	#					if c > 7:
	#						return true
	#return false

fun foo(Int a, Int b, Int c) -> Bool {true}:
	return a<11 and a>5 and b<10 and b>6 and c<9 and c>7

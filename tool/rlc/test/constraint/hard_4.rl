# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7,4,3)
	if exit_code == true:
		return 0
	else:
		return -1

# This is a WIP, still fixing some minor bugs

# We could note that doing this it would work fine
	#if a < 11:
	#	if a > 5:
	#		if b < 10: 
	#			if b > 6:
	#				if c < 9:
	#					if c > 7:
	#						return true
	#return false

fun foo(Int a, Int b, Int c) -> Bool:
	
	return (a<11 and a>5) and (b<10 and b>6) and (c<9 and c>7)

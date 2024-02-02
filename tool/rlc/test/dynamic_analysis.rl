# RUN: rlc %s -o %t -i %stdlib --fuzzer
# RUN: %t
import fuzzer.cpp_functions
import fuzzer.utils

fun crash_on_five(Int input) -> Int {input != 5}:
	return 0

fun plus_three(Int input) -> Int:
	return input + 3

act play() -> Play:
	frm current = 0
	act subact(Int x) {x > -5, x < 10}
	    current = x

	act subact(Int x) {x > (plus_three(current) + 8) / 2, x < 10}
	    current = x
	
	crash_on_five(5)

	while current != 7:
	    act subact(Int x) {x > current, x < 10}
	    current = x
	    act that(Int a) {a >= 0, a < 100}
	    crash_on_five(a)





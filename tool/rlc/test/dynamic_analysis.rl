# RUN: rlc %s -o %t -i %stdlib --fuzzer
# RUN: %t
import fuzzer.cpp_functions
import fuzzer.utils

fun crash_on_five(Int input) -> Int {input != 5}:
	return 0

fun plus_three(Int input) -> Int:
	return input + 3

fun factorial(Int input) -> Int:
	if input <= 0:
		return 1
	else:
		return input * factorial(input - 1)

act play() -> Play:
	frm current = 0
	while current != 7:
	    act subact(Int x) {x > (plus_three(current) + 8) / 2, x < 10 + factorial(current)}
	    current = x
	    act that(Int a) {a >= 0, a < 100}
	    crash_on_five(a)





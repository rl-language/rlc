# RUN: rlc %s -o %t -i %stdlib --fuzzer --fuzzer-lib=%fuzzer_lib
# RUN: %t -runs=1000

fun crash_on_five(Int input) -> Int {input != 5}:
	return 0

act play() -> Play:
	frm current = 0
	while current != 7:
	    act subact(Int x) {x > 5, x < 15}
	    current = x

	    actions:
	        act this(Int y, Int z) {z < 3, z > 0, y < 14, y >= 0 }
	        current = y
	        act that(Int a) {a >= 0, a < 100}
	        crash_on_five(a)
	    
	    act subact(Int x) {x > -12, x < 3}

fun main() -> Int:
	return 0

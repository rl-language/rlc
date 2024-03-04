# RUN: rlc %s -o %t -i %stdlib --fuzzer --fuzzer-lib=%fuzzer_lib
# RUN: %t -runs=100000
# XFAIL: *

import action

fun crash_on_five(Byte input) -> Int {input != byte(5)}:
	return 0

act play() -> Play:
	frm current = 0
	while current != 7:
        act that(Byte a) {a >= byte(0), a < byte(100)}
        crash_on_five(a)

fun fuzz(Vector<Byte> input):
    let frame = play()
    let action : AnyPlayAction
    parse_and_execute(frame, action, input)

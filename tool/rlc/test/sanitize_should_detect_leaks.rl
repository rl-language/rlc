# RUN: rlc %s -o %t -i %stdlib --sanitize -g
# RUN: %t%exeext
# XFAIL: *
# UNSUPPORTED: system-windows, system-darwin

fun has_loop():
    let i = 0
    while i < 10:
	    let new_data = __builtin_malloc_do_not_use<Int>(1000)
        i = i + 1

fun main() -> Int:
    has_loop()
    return 0

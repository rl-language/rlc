# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
	if "asd"[3] != '\0':
		return -1
	else:
		return 0

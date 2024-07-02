# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
	if (float(1) == float(true)):
		return int(bool(int(true) - int(float(1))))
	return 1

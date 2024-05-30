# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

enum Asd:
	first	
	second	

fun main() -> Int:
	let asd : Asd
	asd = Asd::second
    if max(asd) != 1:
        return -2
	return asd.value - 1

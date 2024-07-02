# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls<T, Y> Pair:
	T first
	Y second

cls<K> SamePair:
	Pair<K, K> content

	fun init():
		if self.content.first is Int:
			self.content.first = 5
			self.content.second = 5

fun main() -> Int:
	let var : SamePair<Int>
	return var.content.first + var.content.second - 10

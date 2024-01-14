# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent<T, Y> Pair:
	T first
	Y second

ent<K> SamePair:
	Pair<K, K> content

	fun init():
		if self.content.first is Int:
			self.content.first = 5
			self.content.second = 5

fun main() -> Int:
	let var : SamePair<Int>
	return var.content.first + var.content.second - 10

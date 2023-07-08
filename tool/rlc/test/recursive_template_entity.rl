ent<T, Y> Pair:
	T first
	Y second

ent<K> SamePair:
	Pair<K, K> content

fun<K> init(SamePair<K> pair):
	if pair.content.first is Int:
		pair.content.first = 5
		pair.content.second = 5

fun<K> main() -> Int:
	let var : SamePair<Int>
	return var.content.first + var.content.second - 10

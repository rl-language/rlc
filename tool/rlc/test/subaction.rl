act outer():
	let frame = inner()
	act run(Int x)
	frame.asd(x)

act inner():
	let to_return : Int
	act asd(Int x)
	to_return = x

fun main() -> Int:
	let frame = outer()
	frame.run(8)
	return frame.frame.to_return - 8

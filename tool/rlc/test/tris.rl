ent Board:
	Int[9] slots
	Bool playerTurn

fun get(Board b, Int x, Int y) -> Int:
	return b.slots[x + (y*3)]

fun set(Board b, Int x, Int y, Int val): 
	b.slots[x + (y * 3)] = val

fun full(Board b) -> Bool:
	let x = 0
	let y = 0

	while x < 3:
		while y < 3:
			if b.get(x, y) == 0:
				return false
			y = y + 1
		x = x + 1

	return true

fun three_in_a_line_player_row(Board b, Int player_id, Int row) -> Bool:
	return b.get(0, row) == b.get(1, row) and b.get(0, row) == b.get(2, row) and b.get(0, row) == player_id

fun three_in_a_line_player(Board b, Int player_id) -> Bool:
	let x = 0
	while x < 3:
		if b.get(x, 0) == b.get(x, 1) and b.get(x, 0) == b.get(x, 2) and b.get(x, 0) == player_id:
			return true

		if three_in_a_line_player_row(b, player_id, x):
			return true
		x = x + 1

	if b.get(0, 0) == b.get(1, 1) and b.get(0, 0) == b.get(2, 2) and b.get(0, 0) == player_id:
		return true

	if b.get(0, 2) == b.get(1, 1) and b.get(0, 2) == b.get(2, 0) and b.get(0, 2) == player_id:
		return true

	return false

act play():
	let b : Board
	while !full(b):
		act mark(Int x, Int y)
		req x < 3
		req x >= 0
		req y < 3
		req y >= 0
		req b.get(x, y) == 0
		b.set(x, y, int(b.playerTurn) + 1)

		if b.three_in_a_line_player(int(b.playerTurn) + 1):
			return

		b.playerTurn = !b.playerTurn

fun main() -> Int:
	let game = play()
	game.mark(0, 0)
	game.mark(1, 0)
	game.mark(1, 1)
	game.mark(2, 0)
	game.mark(2, 2)
	return int(game.b.three_in_a_line_player(1)) - 1

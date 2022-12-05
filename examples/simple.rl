ent Board:
	Int[3][3] slots
	Bool playerTurn


fun full(Board b) -> Bool:
	int x = 0
	int y = 0

	while x < 3:
		while y < 3:
			if b.slots[x][y] == 0:
				return false
			y = y + 1
		x = x + 1

	return true

fun three_in_a_line_player(Board b, int player_id) -> Bool:
	int x = 0;
	while x < 3:
		if b.slots[x][0] == b.slots[x][1] and b.slots[x][0] == b.slots[x][2] and b.slots[x][0] == player_id:
			return true

		if b.slots[0][x] == b.slots[0][x] and b.slots[0][x] == b.slots[0][x] and b.slots[0][x] == player_id:
			return true
		x = x + 1

	if b.slots[0][0] == b.slots[1][1] and b.slots[0][0] == b.slots[2][2] and b.slots[0][0] == player_id:
		return true

	if b.slots[0][2] == b.slots[1][1] and b.slots[0][2] == b.slots[2][0] and b.slots[0][2] == player_id:
		return true

	return false

act play():
	let b : Board
	while !full(b) and !three_in_a_line_player(b, int(b.playerTurn) + 1) == 0:
		await b.mark(int x, int y)
		b.slots[x][y] = int(b.playerTurn) + 1

		b.playerTurn = !b.playerTurn

# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import serialization.to_byte_vector
import string
import action

ent Board:
    Int[9] slots
    Bool playerTurn


    fun get(Int x, Int y) -> Int:
        return self.slots[x + (y*3)]

    fun set(Int x, Int y, Int val): 
        self.slots[x + (y * 3)] = val

    fun full() -> Bool:
        let x = 0

        while x < 3:
            let y = 0
            while y < 3:
                if self.get(x, y) == 0:
                    return false
                y = y + 1
            x = x + 1

        return true

    fun three_in_a_line_player_row(Int player_id, Int row) -> Bool:
        return self.get(0, row) == self.get(1, row) and self.get(0, row) == self.get(2, row) and self.get(0, row) == player_id

    fun three_in_a_line_player(Int player_id) -> Bool:
        let x = 0
        while x < 3:
            if self.get(x, 0) == self.get(x, 1) and self.get(x, 0) == self.get(x, 2) and self.get(x, 0) == player_id:
                return true

            if self.three_in_a_line_player_row(player_id, x):
                return true
            x = x + 1

        if self.get(0, 0) == self.get(1, 1) and self.get(0, 0) == self.get(2, 2) and self.get(0, 0) == player_id:
            return true

        if self.get(0, 2) == self.get(1, 1) and self.get(0, 2) == self.get(2, 0) and self.get(0, 2) == player_id:
            return true

        return false

    fun current_player() -> Int:
        return int(self.playerTurn) + 1

    fun next_turn():
        self.playerTurn = !self.playerTurn

act play() -> TicTacToe:
    frm board : Board
    while !board.full():
        act mark(Int x, Int y) {
            x < 3,
            x >= 0,
            y < 3,
            y >= 0,
            board.get(x, y) == 0
        }

        board.set(x, y, board.current_player())

        if board.three_in_a_line_player(board.current_player()):
            return

        board.next_turn()

fun gen_printer_parser():
    let state = play()
    let serialized = as_byte_vector(state)
    from_byte_vector(state, serialized)
    let x : AnyTicTacToeAction
    apply(x, state)
    to_string(state)
    to_string(x)
    from_string(x, ""s)

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyTicTacToeAction 
    parse_and_execute(state, action, input) 

fun main() -> Int:
    let game = play()
    game.mark(0, 0)
    if game.board.full():
        return 1
    game.mark(1, 0)
    if game.board.full():
        return 2
    game.mark(1, 1)
    if game.board.full():
        return 3
    game.mark(2, 0)
    if game.board.full():
        return 4
    game.mark(2, 2)
    if game.board.full():
        return 5
    if game.board.three_in_a_line_player(1):
        return 0
    else:
        return 1

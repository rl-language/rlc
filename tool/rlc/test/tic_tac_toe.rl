# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import serialization.to_byte_vector
import string
import action

cls Board:
    BInt<0, 3>[9] slots
    Bool playerTurn


    fun get(Int x, Int y) -> Int:
        return self.slots[x + (y*3)].value

    fun set(Int x, Int y, Int val): 
        self.slots[x + (y * 3)].value = val

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

act play() -> Game:
    frm board : Board
    frm score = 10
    while !board.full():
        act mark(BInt<0, 3> x, BInt<0, 3> y) {
            board.get(x.value, y.value) == 0
        }

        score = score - 1
        board.set(x.value, y.value, board.current_player())

        if board.three_in_a_line_player(board.current_player()):
            return

        board.next_turn()

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    #return g.board.current_player() - 1
    return 0 

fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0 
    if g.board.three_in_a_line_player(1) and player_id == 0:
        return -1.0
    if g.board.three_in_a_line_player(2) and player_id == 0:
        return 1.0
    return -1.0

fun get_num_players() -> Int:
    return 1

fun max_game_lenght() -> Int:
    return 100

fun gen_printer_parser():
    let state : Game
    let any_action :  AnyGameAction
    gen_python_methods(state, any_action)

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction 
    parse_and_execute(state, action, input) 

fun main() -> Int:
    let game = play()
    let x : BInt<0, 3>
    let y : BInt<0, 3>
    x.value = 0
    y.value = 0
    game.mark(x, y)
    if game.board.full():
        return 1
    x.value = 1
    y.value = 0
    game.mark(x, y)
    if game.board.full():
        return 2
    x.value = 1
    y.value = 1
    game.mark(x, y)
    if game.board.full():
        return 3
    x.value = 2
    y.value = 0
    game.mark(x, y)
    if game.board.full():
        return 4
    x.value = 2
    y.value = 2
    game.mark(x, y)
    if game.board.full():
        return 5
    if game.board.three_in_a_line_player(1):
        return 0
    else:
        return 1

fun pretty_print(Game g):
    let i = 0
    while i != 3:
        let to_print : String
        let y = 0
        while y != 3:
            to_print.append(to_string(g.board.get(i, y)))
            y = y + 1 
        print(to_print)
        i = i + 1

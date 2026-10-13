# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.vector
import range
import serialization.print
import action
import algorithms.diff
import python

cls SudokuCallbacks:
    PyObject on_changed   

cls RedBoard:
    BInt<0, 3>[3][3] slots

cls Board:
    Vector<Vector<Int>> slots
    SudokuCallbacks callback

    fun place(Int row, Int col, Int value):
        self.slots[row][col] = value
        let place_args : PyObject[3]
        place_args[0] = to_pyobject(self)
        place_args[1] = to_pyobject(row)
        place_args[2] = to_pyobject(col)
        self.callback.on_changed.call("on_changed", place_args)

fun is_full(Board board) -> Bool:
    for r in range(9):
        for c in range(9):
            if board.slots[r][c] == 0:
                return false
    return true

fun can_place(Board board, Int row, Int col, Int num) -> Bool:
    # Check row
    for c in range(9):
        if board.slots[row][c] == num:
            return false
    # Check column
    for r in range(9):
        if board.slots[r][col] == num:
            return false
    # Check 3x3 box
    let box_row_start = (row / 3) * 3
    let box_col_start = (col / 3) * 3
    for i in range(3):
        for j in range(3):
            if board.slots[box_row_start + i][box_col_start + j] == num:
                return false
    return true

@classes
act play() -> Game:
    frm board : Board
    # Initialize empty board
    board.slots.resize(9)
    for i in range(9):
        board.slots[i].resize(9)
        for j in range(9):
            board.slots[i][j] = 0

    # Set up an easy Sudoku puzzle (hardcoded)
    board.slots[0][3] = 2
    board.slots[0][4] = 6
    board.slots[0][6] = 7
    board.slots[0][8] = 1
    board.slots[1][0] = 6
    board.slots[1][2] = 8
    board.slots[1][4] = 7
    board.slots[1][7] = 9
    board.slots[2][0] = 1
    board.slots[2][1] = 9
    board.slots[2][5] = 4
    board.slots[2][6] = 5
    board.slots[3][0] = 8
    board.slots[3][1] = 2
    board.slots[3][3] = 1
    board.slots[3][7] = 4
    board.slots[4][2] = 4
    board.slots[4][3] = 6
    board.slots[4][5] = 2
    board.slots[4][6] = 9
    board.slots[5][1] = 5
    board.slots[5][5] = 3
    board.slots[5][7] = 2
    board.slots[5][8] = 8
    board.slots[6][2] = 9
    board.slots[6][3] = 3
    board.slots[6][7] = 7
    board.slots[6][8] = 4
    board.slots[7][1] = 4
    board.slots[7][4] = 5
    board.slots[7][7] = 3
    board.slots[7][8] = 6
    board.slots[8][0] = 7
    board.slots[8][2] = 3
    board.slots[8][4] = 1
    board.slots[8][5] = 8

    # Game loop: place numbers until the board is full
    while !is_full(board):
        act place(BInt<1,10> num, BInt<0,9> row, BInt<0,9> col) {
            board.slots[row.value][col.value] == 0 and can_place(board, row.value, col.value, num.value)
        }
        # board.slots[row.value][col.value] = num.value
        board.place(row.value, col.value, num.value)

        if is_full(board):
            return

    print("Sudoku solved!")

fun make_num(Int x) -> BInt<1, 10>:
    let num : BInt<1, 10> 
    num = x
    return num

fun make_pos(Int x) -> BInt<0, 9>:
    let num : BInt<0, 9> 
    num = x
    return num

fun pretty_print(Board board):
    let result = ""s
    for r in range(9):
        for c in range(9):
            let val = board.slots[r][c]
            if val == 0:
                result = result + "."s
            else:
                result = result + to_string(val)
            result = result + " "s
        result = result + "\n"s
    print(result)

fun main() -> Int:
    let game = play()
    pretty_print(game.board)

    if is_full(game.board):
        return 0
    else:
        return 1

fun game_diff(Game before, Game after, Vector<String> out):
    diff(before, after, out)
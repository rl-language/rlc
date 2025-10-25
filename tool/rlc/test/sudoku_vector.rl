# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.vector
import range
import serialization.print
import action

cls Board:
    Vector<Vector<Int>> slots

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
    board.slots[0][2] = 5
    board.slots[0][4] = 4
    board.slots[0][8] = 8
    board.slots[1][1] = 1
    board.slots[1][3] = 5
    board.slots[1][5] = 2
    board.slots[1][7] = 4
    board.slots[2][0] = 8
    board.slots[2][4] = 1
    board.slots[2][6] = 5
    board.slots[3][0] = 1
    board.slots[3][2] = 8
    board.slots[3][7] = 3
    board.slots[3][8] = 6
    board.slots[4][1] = 6
    board.slots[4][3] = 8
    board.slots[4][5] = 4
    board.slots[4][7] = 2
    board.slots[5][0] = 3
    board.slots[5][1] = 2
    board.slots[5][6] = 8
    board.slots[5][8] = 4
    board.slots[6][2] = 3
    board.slots[6][4] = 2
    board.slots[6][8] = 9
    board.slots[7][1] = 5
    board.slots[7][3] = 4
    board.slots[7][5] = 9
    board.slots[7][7] = 6
    board.slots[8][0] = 9
    board.slots[8][4] = 3
    board.slots[8][6] = 4

    # Game loop: place numbers until the board is full
    while !is_full(board):
        act place(frm BInt<1,10> num, BInt<0,9> row, BInt<0,9> col) {
            board.slots[row.value][col.value] == 0 and can_place(board, row.value, col.value, num.value)
        }
        board.slots[row.value][col.value] = num.value

    print("Sudoku solved!")

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
    # Example moves (assuming valid; in practice, use can to check)
    # game.place(6, BInt<0,9>(0), BInt<0,9>(0))  # etc.
    return 0
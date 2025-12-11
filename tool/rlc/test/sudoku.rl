# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import range
import serialization.print
import action

cls Board:
    Int[3][3][3][3] slots  # 3x3 blocks of 3x3 cells (total 9x9 Sudoku)

# Helper to get or set a cell using global coordinates (0..8)
fun get(Board board, Int row, Int col) -> Int:
    return board.slots[row / 3][col / 3][row % 3][col % 3]

fun set(Board board, Int row, Int col, Int val):
    board.slots[row / 3][col / 3][row % 3][col % 3] = val


fun is_full(Board board) -> Bool:
    for r in range(9):
        for c in range(9):
            if get(board, r, c) == 0:
                return false
    return true


fun can_place(Board board, Int row, Int col, Int num) -> Bool:
    # Check row
    for c in range(9):
        if get(board, row, c) == num:
            return false

    # Check column
    for r in range(9):
        if get(board, r, col) == num:
            return false

    # Check 3x3 box
    let box_row_start = (row / 3) * 3
    let box_col_start = (col / 3) * 3
    for i in range(3):
        for j in range(3):
            if get(board, box_row_start + i, box_col_start + j) == num:
                return false

    return true


@classes
act play() -> Game:
    frm board : Board

    # Initialize empty 3x3x3x3 board
    for br in range(3):
        for bc in range(3):
            for r in range(3):
                for c in range(3):
                    board.slots[br][bc][r][c] = 0

    # Sample Sudoku puzzle
    set(board, 0, 2, 5)
    set(board, 0, 4, 4)
    set(board, 0, 8, 8)
    set(board, 1, 1, 1)
    set(board, 1, 3, 5)
    set(board, 1, 5, 2)
    set(board, 1, 7, 4)
    set(board, 2, 0, 8)
    set(board, 2, 4, 1)
    set(board, 2, 6, 5)
    set(board, 3, 0, 1)
    set(board, 3, 2, 8)
    set(board, 3, 7, 3)
    set(board, 3, 8, 6)
    set(board, 4, 1, 6)
    set(board, 4, 3, 8)
    set(board, 4, 5, 4)
    set(board, 4, 7, 2)
    set(board, 5, 0, 3)
    set(board, 5, 1, 2)
    set(board, 5, 6, 8)
    set(board, 5, 8, 4)
    set(board, 6, 2, 3)
    set(board, 6, 4, 2)
    set(board, 6, 8, 9)
    set(board, 7, 1, 5)
    set(board, 7, 3, 4)
    set(board, 7, 5, 9)
    set(board, 7, 7, 6)
    set(board, 8, 0, 9)
    set(board, 8, 4, 3)
    set(board, 8, 6, 4)

    # Game loop: fill until full
    while !is_full(board):
        act place(frm Int num, Int row, Int col){
            get(board, row, col) == 0 and can_place(board, row, col, num)
        }

        set(board, row, col, num)

    print("Sudoku solved!")


fun pretty_print(Board board):
    for r in range(9):
        let line = ""s
        for c in range(9):
            let val = get(board, r, c)
            if val == 0:
                line = line + "."s
            else:
                line = line + to_string(val)
            line = line + " "s
            if c % 3 == 2 and c != 8:
                line = line + "| "s
        print(line)
        if r % 3 == 2 and r != 8:
            print("------+-------+------"s)


fun main() -> Int:
    let game = play()
    pretty_print(game.board)
    return 0

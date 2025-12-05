# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.vector
import range
import serialization.print
import action

cls RedBoard:
    BInt<0, 3>[3][3] slots

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
    frm redboard : RedBoard
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
        board.slots[row.value][col.value] = num.value

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
    # Example moves (assuming valid; in practice, use can to check)
    let row : BInt<0, 9>
    let col : BInt<0, 9>
    let num : BInt<1, 10>
    row.value = 0
    col.value = 0
    num.value = 4
    game.place(num, row, col)

    row.value = 0
    col.value = 1
    num.value = 3
    game.place(num, row, col)

    row.value = 0
    col.value = 2
    num.value = 5
    game.place(num, row, col)

    row.value = 0
    col.value = 5
    num.value = 9
    game.place(num, row, col)

    row.value = 0
    col.value = 7
    num.value = 8
    game.place(num, row, col)

    row.value = 1
    col.value = 2
    num.value = 2
    game.place(num, row, col)

    row.value = 1
    col.value = 3
    num.value = 5
    game.place(num, row, col)

    row.value = 1
    col.value = 5
    num.value = 1
    game.place(num, row, col)

    row.value = 1
    col.value = 6
    num.value = 4
    game.place(num, row, col)

    row.value = 1
    col.value = 8
    num.value = 3
    game.place(num, row, col)

    row.value = 2
    col.value = 2
    num.value = 7
    game.place(num, row, col)

    row.value = 2
    col.value = 3
    num.value = 8
    game.place(num, row, col)

    row.value = 2
    col.value = 4
    num.value = 3
    game.place(num, row, col)

    row.value = 2
    col.value = 7
    num.value = 6
    game.place(num, row, col)

    row.value = 2
    col.value = 8
    num.value = 2
    game.place(num, row, col)

    row.value = 3
    col.value = 2
    num.value = 6
    game.place(num, row, col)

    row.value = 3
    col.value = 4
    num.value = 9
    game.place(num, row, col)

    row.value = 3
    col.value = 5
    num.value = 5
    game.place(num, row, col)
    row.value = 3
    col.value = 6
    num.value = 3
    game.place(num, row, col)
    row.value = 3
    col.value = 8
    num.value = 7
    game.place(num, row, col)
    row.value = 4
    col.value = 0
    num.value = 3
    game.place(num, row, col)
    row.value = 4
    col.value = 1
    num.value = 7
    game.place(num, row, col)
    row.value = 4
    col.value = 4
    num.value = 8
    game.place(num, row, col)
    row.value = 4
    col.value = 7
    num.value = 1
    game.place(num, row, col)
    row.value = 4
    col.value = 8
    num.value = 5
    game.place(num, row, col)
    row.value = 5
    col.value = 0
    num.value = 9
    game.place(num, row, col)

    row.value = 5
    col.value = 2
    num.value = 1
    game.place(num, row, col)
    row.value = 5
    col.value = 3
    num.value = 7
    game.place(num, row, col)
    row.value = 5
    col.value = 4
    num.value = 4
    game.place(num, row, col)
    row.value = 5
    col.value = 6
    num.value = 6
    game.place(num, row, col)
    row.value = 6
    col.value = 0
    num.value = 5
    game.place(num, row, col)
    row.value = 6
    col.value = 1
    num.value = 1
    game.place(num, row, col)
    row.value = 6
    col.value = 4
    num.value = 2
    game.place(num, row, col)
    row.value = 6
    col.value = 5
    num.value = 6
    game.place(num, row, col)
    row.value = 6
    col.value = 6
    num.value = 8
    game.place(num, row, col)
    row.value = 7
    col.value = 0
    num.value = 2
    game.place(num, row, col)
    row.value = 7
    col.value = 2
    num.value = 8
    game.place(num, row, col)
    row.value = 7
    col.value = 3
    num.value = 9
    game.place(num, row, col)
    row.value = 7
    col.value = 5
    num.value = 7
    game.place(num, row, col)
    row.value = 7
    col.value = 6
    num.value = 1
    game.place(num, row, col)
    row.value = 8
    col.value = 1
    num.value = 6
    game.place(num, row, col)
    row.value = 8
    col.value = 3
    num.value = 4
    game.place(num, row, col)
    row.value = 8
    col.value = 6
    num.value = 2
    game.place(num, row, col)
    row.value = 8
    col.value = 7
    num.value = 5
    game.place(num, row, col)
    row.value = 8
    col.value = 8
    num.value = 9
    game.place(num, row, col)

    if is_full(game.board):
        return 0
    else:
        return 1
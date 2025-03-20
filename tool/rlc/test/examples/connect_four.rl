// Copyright 2023 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Implementation of Connect Four in RL language

// Constants for the game
const Int kRows = 6
const Int kCols = 7
const Int kNumCells = kRows * kCols
const Int kNumPlayers = 2

// Cell states
enum CellState:
    Empty = 0
    Cross = 1  // Player 0
    Nought = 2 // Player 1

// Outcome of the game
enum Outcome:
    Unknown = 0
    Player1Win = 1
    Player2Win = 2
    Draw = 3

// Board representation
struct Board:
    Array<CellState> cells = Array<CellState>(kNumCells, CellState.Empty)
    Int current_player = 0
    Outcome outcome = Outcome.Unknown

    // Get cell state at specific row and column
    fun cell_at(Int row, Int col) -> CellState:
        return cells[row * kCols + col]

    // Set cell state at specific row and column
    fun set_cell_at(Int row, Int col, CellState state):
        cells[row * kCols + col] = state

    // Check if a column is full
    fun is_column_full(Int col) -> Bool:
        return cell_at(kRows - 1, col) != CellState.Empty

    // Check if the entire board is full
    fun is_full() -> Bool:
        for Int col = 0; col < kCols; col = col + 1:
            if !is_column_full(col):
                return false
        return true

    // Get the lowest empty row in a column
    fun get_empty_row(Int col) -> Int:
        for Int row = 0; row < kRows; row = row + 1:
            if cell_at(row, col) == CellState.Empty:
                return row
        return -1  // Column is full

    // Convert player number to cell state
    fun player_to_state(Int player) -> CellState:
        if player == 0:
            return CellState.Cross
        return CellState.Nought

    // Check if there's a line from a specific position in a specific direction
    fun has_line_from_in_direction(Int player, Int row, Int col, Int drow, Int dcol) -> Bool:
        if row + 3 * drow >= kRows || col + 3 * dcol >= kCols || 
           row + 3 * drow < 0 || col + 3 * dcol < 0:
            return false
        
        let cell_state = player_to_state(player)
        for Int i = 0; i < 4; i = i + 1:
            if cell_at(row + i * drow, col + i * dcol) != cell_state:
                return false
        return true

    // Check if player has a line from a specific position
    fun has_line_from(Int player, Int row, Int col) -> Bool:
        return has_line_from_in_direction(player, row, col, 0, 1) ||  // horizontal
               has_line_from_in_direction(player, row, col, 1, 0) ||  // vertical
               has_line_from_in_direction(player, row, col, 1, 1) ||  // diagonal up-right
               has_line_from_in_direction(player, row, col, 1, -1)    // diagonal up-left

    // Check if player has a line anywhere on the board
    fun has_line(Int player) -> Bool:
        let cell_state = player_to_state(player)
        for Int col = 0; col < kCols; col = col + 1:
            for Int row = 0; row < kRows; row = row + 1:
                if cell_at(row, col) == cell_state && has_line_from(player, row, col):
                    return true
        return false

    // Print the board
    fun to_string() -> String:
        var result = ""
        for Int row = kRows - 1; row >= 0; row = row - 1:
            for Int col = 0; col < kCols; col = col + 1:
                if cell_at(row, col) == CellState.Empty:
                    result = result + "."
                elif cell_at(row, col) == CellState.Cross:
                    result = result + "x"
                else:
                    result = result + "o"
            result = result + "\n"
        return result

    // Switch to the next player
    fun next_player():
        current_player = 1 - current_player

// The main Connect Four game
act play() -> ConnectFour:
    let board = Board()
    
    while board.outcome == Outcome.Unknown:
        act drop(Int col) {
            col >= 0,
            col < kCols,
            !board.is_column_full(col)
        }
        
        // Find the lowest empty row in this column
        let row = board.get_empty_row(col)
        
        // Place the current player's piece
        board.set_cell_at(row, col, board.player_to_state(board.current_player))
        
        // Check for win
        if board.has_line(board.current_player):
            if board.current_player == 0:
                board.outcome = Outcome.Player1Win
            else:
                board.outcome = Outcome.Player2Win
            return
        
        // Check for draw
        if board.is_full():
            board.outcome = Outcome.Draw
            return
        
        // Next player's turn
        board.next_player()

// Test function to demonstrate gameplay
fun main() -> Int:
    let game = play()
    
    println("Starting a game of Connect Four")
    
    // Player 1 (X) drops in the middle
    game.drop(3)
    println("After Player 1 drops in column 3:")
    println(game.board.to_string())
    
    // Player 2 (O) drops in column 2
    game.drop(2)
    println("After Player 2 drops in column 2:")
    println(game.board.to_string())
    
    // Player 1 drops in column 4
    game.drop(4)
    println("After Player 1 drops in column 4:")
    println(game.board.to_string())
    
    // Player 2 drops in column 1
    game.drop(1)
    println("After Player 2 drops in column 1:")
    println(game.board.to_string())
    
    // Player 1 drops in column 5
    game.drop(5)
    println("After Player 1 drops in column 5:")
    println(game.board.to_string())
    
    // Player 2 drops in column 0
    game.drop(0)
    println("After Player 2 drops in column 0:")
    println(game.board.to_string())
    
    // Player 1 drops in column 6 to win with a diagonal
    game.drop(6)
    println("After Player 1 drops in column 6:")
    println(game.board.to_string())
    
    // Check outcome
    if game.board.outcome == Outcome.Player1Win:
        println("Player 1 wins!")
        return 0
    elif game.board.outcome == Outcome.Player2Win:
        println("Player 2 wins!")
        return 1
    else:
        println("It's a draw!")
        return 2

// Function for machine learning components to display the game state
fun pretty_print(ConnectFour game):
    println(game.board.to_string())

    // Print column numbers for reference
    var col_numbers = ""
    for Int col = 0; col < kCols; col = col + 1:
        col_numbers = col_numbers + to_string(col)
    println(col_numbers)
    
    // Print current state information
    if game.board.outcome == Outcome.Unknown:
        println("Current player: " + to_string(game.board.current_player + 1))
    elif game.board.outcome == Outcome.Player1Win:
        println("Player 1 wins!")
    elif game.board.outcome == Outcome.Player2Win:
        println("Player 2 wins!")
    else:
        println("It's a draw!")

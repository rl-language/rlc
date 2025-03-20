# RUN: rlc %s -o %t -i %stdlib

import serialization.print
import collections.vector
import machine_learning
import action

# Constants for the game
const kRows = 6
const kCols = 7
const kNumCells = 42  # kRows * kCols = 6 * 7 = 42
const kNumPlayers = 2

# Cell states
enum CellState:
    Empty
    Cross  # Player 0
    Nought # Player 1

    fun equal(CellState other) -> Bool:
        return self.value == other.value

    fun not_equal(CellState other) -> Bool:
        return !(self.value == other.value)

# Outcome of the game
enum Outcome:
    Unknown
    Player1Win
    Player2Win
    Draw

    fun equal(Outcome other) -> Bool:
        return self.value == other.value

    fun not_equal(Outcome other) -> Bool:
        return !(self.value == other.value)

# Board representation
cls Board:
    CellState[kNumCells] cells
    Int current_player
    Outcome outcome

    fun init():
        self.current_player = 0
        self.outcome = Outcome::Unknown
        let i = 0
        while i < kNumCells:
            self.cells[i] = CellState::Empty
            i = i + 1

    # Get cell state at specific row and column
    fun cell_at(Int row, Int col) -> CellState:
        return self.cells[row * kCols + col]

    # Set cell state at specific row and column
    fun set_cell_at(Int row, Int col, CellState state):
        self.cells[row * kCols + col] = state

    # Check if a column is full
    fun is_column_full(Int col) -> Bool:
        return self.cell_at(kRows - 1, col) != CellState::Empty

    # Check if the entire board is full
    fun is_full() -> Bool:
        let col = 0
        while col < kCols:
            if !self.is_column_full(col):
                return false
            col = col + 1
        return true

    # Get the lowest empty row in a column
    fun get_empty_row(Int col) -> Int:
        let row = 0
        while row < kRows:
            if self.cell_at(row, col) == CellState::Empty:
                return row
            row = row + 1
        return -1  # Column is full

    # Convert player number to cell state
    fun player_to_state(Int player) -> CellState:
        if player == 0:
            return CellState::Cross
        return CellState::Nought

    # Check if there's a line from a specific position in a specific direction
    fun has_line_from_in_direction(Int player, Int row, Int col, Int drow, Int dcol) -> Bool:
        if row + 3 * drow >= kRows or row + 3 * drow < 0 or col + 3 * dcol >= kCols or col + 3 * dcol < 0:
            return false
        
        let cell_state = self.player_to_state(player)
        let i = 0
        while i < 4:
            if self.cell_at(row + i * drow, col + i * dcol) != cell_state:
                return false
            i = i + 1
        return true

    # Check if player has a line from a specific position
    fun has_line_from(Int player, Int row, Int col) -> Bool:
        return self.has_line_from_in_direction(player, row, col, 0, 1) or self.has_line_from_in_direction(player, row, col, 1, 0) or self.has_line_from_in_direction(player, row, col, 1, 1) or self.has_line_from_in_direction(player, row, col, 1, -1)

    # Check if player has a line anywhere on the board
    fun has_line(Int player) -> Bool:
        let cell_state = self.player_to_state(player)
        let col = 0
        while col < kCols:
            let row = 0
            while row < kRows:
                if self.cell_at(row, col) == cell_state and self.has_line_from(player, row, col):
                    return true
                row = row + 1
            col = col + 1
        return false

    # Print the board
    fun to_string() -> String:
        let result = ""s
        let row = kRows - 1
        while row >= 0:
            let col = 0
            while col < kCols:
                if self.cell_at(row, col) == CellState::Empty:
                    result = result + "."s
                else if self.cell_at(row, col) == CellState::Cross:
                    result = result + "x"s
                else:
                    result = result + "o"s
                col = col + 1
            result = result + "\n"s
            row = row - 1
        return result

    # Switch to the next player
    fun next_player():
        self.current_player = 1 - self.current_player

# The main Connect Four game
@classes
act play() -> Game:
    frm board : Board
    board.outcome = Outcome::Unknown
    
    while board.outcome == Outcome::Unknown:
        act drop(BInt<0, 7> col) {
            col.value >= 0,
            col.value < kCols,
            !board.is_column_full(col.value)
        }
        
        # Find the lowest empty row in this column
        let row = board.get_empty_row(col.value)
        
        # Place the current player's piece
        board.set_cell_at(row, col.value, board.player_to_state(board.current_player))
        
        # Check for win
        if board.has_line(board.current_player):
            if board.current_player == 0:
                board.outcome = Outcome::Player1Win
            else:
                board.outcome = Outcome::Player2Win
            return
        
        # Check for draw
        if board.is_full():
            board.outcome = Outcome::Draw
            return
        
        # Next player's turn
        board.next_player()

# Test function to demonstrate gameplay
fun main() -> Int:
    let game = play()
    
    print("Starting a game of Connect Four"s)
    
    # Player 1 (X) drops in the middle
    let col3 : BInt<0, 7>
    col3.value = 3
    game.drop(col3)
    print("After Player 1 drops in column 3:"s)
    print(game.board.to_string())
    
    # Player 2 (O) drops in column 2
    let col2 : BInt<0, 7>
    col2.value = 2
    game.drop(col2)
    print("After Player 2 drops in column 2:"s)
    print(game.board.to_string())
    
    # Player 1 drops in column 4
    let col4 : BInt<0, 7>
    col4.value = 4
    game.drop(col4)
    print("After Player 1 drops in column 4:"s)
    print(game.board.to_string())
    
    # Player 2 drops in column 1
    let col1 : BInt<0, 7>
    col1.value = 1
    game.drop(col1)
    print("After Player 2 drops in column 1:"s)
    print(game.board.to_string())
    
    # Player 1 drops in column 5
    let col5 : BInt<0, 7>
    col5.value = 5
    game.drop(col5)
    print("After Player 1 drops in column 5:"s)
    print(game.board.to_string())
    
    # Player 2 drops in column 0
    let col0 : BInt<0, 7>
    col0.value = 0
    game.drop(col0)
    print("After Player 2 drops in column 0:"s)
    print(game.board.to_string())
    
    # Player 1 drops in column 6 to win with a diagonal
    let col6 : BInt<0, 7>
    col6.value = 6
    game.drop(col6)
    print("After Player 1 drops in column 6:"s)
    print(game.board.to_string())
    
    # Check outcome
    if game.board.outcome == Outcome::Player1Win:
        print("Player 1 wins!"s)
        return 0
    else if game.board.outcome == Outcome::Player2Win:
        print("Player 2 wins!"s)
        return 1
    else:
        print("It's a draw!"s)
        return 2

# Function for machine learning components to display the game state
fun pretty_print(Game game):
    print(game.board.to_string())

    # Print column numbers for reference
    let col_numbers = ""s
    let col = 0
    while col < kCols:
        col_numbers = col_numbers + to_string(col)
        col = col + 1
    print(col_numbers)
    
    # Print current state information
    if game.board.outcome == Outcome::Unknown:
        print("Current player: "s + to_string(game.board.current_player + 1))
    else if game.board.outcome == Outcome::Player1Win:
        print("Player 1 wins!"s)
    else if game.board.outcome == Outcome::Player2Win:
        print("Player 2 wins!"s)
    else:
        print("It's a draw!"s)

# Return current player or special value if game is done
fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    return g.board.current_player

# Return score for ML training
fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0
    if g.board.outcome == Outcome::Player1Win and player_id == 0:
        return 1.0
    else if g.board.outcome == Outcome::Player2Win and player_id == 1:
        return 1.0
    else if g.board.outcome == Outcome::Player1Win or g.board.outcome == Outcome::Player2Win:
        return -1.0
    return 0.0  # Draw

# Return number of players
fun get_num_players() -> Int:
    return 2

# Function for fuzzing
fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = play()
    let action : AnyGameAction 
    parse_and_execute(state, action, input)

# RUN: rlc %s -o %t -i %stdlib

import serialization.print
import collections.vector
import machine_learning
import action

# Constants for the game
const kDefaultRows = 10
const kDefaultCols = 5
const kNumCells = 50 # kDefaultRows * kDefaultCols

# Cell states
enum CellState:
    Empty
    Paddle
    Ball

    fun equal(CellState other) -> Bool:
        return self.value == other.value

    fun not_equal(CellState other) -> Bool:
        return !(self.value == other.value)

# Outcome of the game
enum Outcome:
    Unknown
    Win
    Loss

    fun equal(Outcome other) -> Bool:
        return self.value == other.value

    fun not_equal(Outcome other) -> Bool:
        return !(self.value == other.value)

# Board representation
cls Board:
    Int num_rows
    Int num_cols
    Int ball_row
    Int ball_col
    Int paddle_col
    Bool initialized
    Outcome outcome

    fun init():
        self.num_rows = kDefaultRows
        self.num_cols = kDefaultCols
        self.ball_row = 0
        self.ball_col = 0
        self.paddle_col = kDefaultCols / 2
        self.initialized = false
        self.outcome = Outcome::Unknown

    # Get cell state at specific row and column
    fun cell_at(Int row, Int col) -> CellState:
        if row == self.num_rows - 1 and col == self.paddle_col:
            return CellState::Paddle
        else if row == self.ball_row and col == self.ball_col:
            return CellState::Ball
        return CellState::Empty

    # Check if the game is terminal
    fun is_terminal() -> Bool:
        return self.initialized and self.ball_row >= self.num_rows - 1

    # Get the current returns (rewards)
    fun returns() -> Float:
        if !self.is_terminal():
            return 0.0
        else if self.ball_col == self.paddle_col:
            return 1.0
        else:
            return -1.0

    # Generate string representation of the board
    fun to_string() -> String:
        let result = ""s
        let row = 0
        while row < self.num_rows:
            let col = 0
            while col < self.num_cols:
                if self.cell_at(row, col) == CellState::Empty:
                    result = result + "."s
                else if self.cell_at(row, col) == CellState::Paddle:
                    result = result + "x"s
                else:
                    result = result + "o"s
                col = col + 1
            result = result + "\n"s
            row = row + 1
        return result

    # Update outcome based on current game state
    fun update_outcome():
        if self.is_terminal():
            if self.ball_col == self.paddle_col:
                self.outcome = Outcome::Win
            else:
                self.outcome = Outcome::Loss

# The main Catch game
@classes
act play() -> Game:
    frm board : Board
    
    # Initialization - chance player selects starting ball column
    act initialize(BInt<0, kDefaultCols> col) {
        !board.initialized
    }
    board.ball_col = col.value
    board.initialized = true
    
    # Game loop - player makes moves until ball reaches bottom
    while !board.is_terminal():
        act move(BInt<0, 2> direction) {
            board.initialized
        }
        
        # Move the ball down one row
        board.ball_row = board.ball_row + 1
        
        # Move the paddle based on player's direction
        # 0 = left, 1 = stay, 2 = right
        let actual_direction = direction.value - 1  # Convert to -1, 0, 1
        let new_paddle_pos = board.paddle_col + actual_direction
        
        # Ensure paddle stays within bounds
        if new_paddle_pos >= 0 and new_paddle_pos < board.num_cols:
            board.paddle_col = new_paddle_pos
    
    # Game is now terminal, update outcome
    board.update_outcome()

# Function for machine learning components to display the game state
fun pretty_print(Game game):
    print(game.board.to_string())
    
    # Print game status
    if game.is_done():
        if game.board.outcome == Outcome::Win:
            print("You win! You caught the ball."s)
        else:
            print("You lose! You missed the ball."s)
    else:
        print("Game in progress. Ball at row "s + to_string(game.board.ball_row) + ", column "s + to_string(game.board.ball_col))
        print("Paddle at column "s + to_string(game.board.paddle_col))

# Return current player or special value if game is done
fun get_current_player(Game g) -> Int:
    if !g.board.initialized:
        return -1  # Chance player
    if g.is_done():
        return -4  # Terminal state
    return 0  # Player 0 (the only player)

# Return score for ML training
fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0
    if g.board.outcome == Outcome::Win:
        return 1.0
    else:
        return -1.0

# Return number of players (always 1 for this game)
fun get_num_players() -> Int:
    return 1

fun fuzz(Vector<Byte> input):
    let state = play()
    let x : AnyGameAction
    let enumeration = enumerate(x)
    let index = 0
    while index + 8 < input.size() and !state.is_done():
        let num_action : Int
        from_byte_vector(num_action, input, index)
        if num_action < 0:
          num_action = num_action * -1 
        if num_action < 0:
          num_action = 0 

        let executable : Vector<AnyGameAction>
        let i = 0
        #print("VALIDS")
        while i < enumeration.size():
          if can apply(enumeration.get(i), state):
            #print(enumeration.get(i))
            executable.append(enumeration.get(i))
          i = i + 1
        #print("ENDVALIDS")
        #if executable.size() == 0:
        #print("zero valid actions")
        #print(state)
        #return

        print(executable.get(num_action % executable.size()))
        apply(executable.get(num_action % executable.size()), state)

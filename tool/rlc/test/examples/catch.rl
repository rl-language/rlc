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

# Board representation
cls Board:
    BInt<2,99> num_rows
    BInt<2,99> num_cols
    BInt<0,98> ball_row
    BInt<0,98> ball_col
    BInt<0,98> paddle_col

    fun init():
        self.num_rows.value = kDefaultRows
        self.num_cols.value = kDefaultCols
        self.paddle_col.value = kDefaultCols / 2
    
    fun init(Int rows, Int cols):
        self.num_rows.value = rows
        self.num_cols.value = cols
        self.paddle_col.value = cols / 2

    # Get cell state at specific row and column
    fun cell_at(Int row, Int col) -> CellState:
        if row == self.num_rows.value - 1 and col == self.paddle_col.value:
            return CellState::Paddle
        else if row == self.ball_row.value and col == self.ball_col.value:
            return CellState::Ball
        return CellState::Empty

    # Check if the game is terminal
    fun is_terminal() -> Bool:
        return self.ball_row.value >= self.num_rows.value - 1

    # Generate string representation of the board
    fun to_string() -> String:
        let result = ""s
        let row = 0
        while row < self.num_rows.value:
            let col = 0
            while col < self.num_cols.value:
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

# The main Catch game
@classes
act play() -> Game:
    frm board : Board
    
    # Initialization - chance player selects starting ball column
    act initialize(BInt<0, kDefaultCols> col) {
        col.value >= 0,
        col.value < kDefaultCols
    }
    board.ball_col.value = col.value
    
    # Game loop - player makes moves until ball reaches bottom
    while !board.is_terminal():
        act move(BInt<0, 2> direction) {
            direction >= 0,
            direction < 3
        }
        
        # Move the ball down one row
        board.ball_row.value = board.ball_row.value + 1
        
        # Move the paddle based on player's direction
        # 0 = left, 1 = stay, 2 = right
        let actual_direction = direction.value - 1  # Convert to -1, 0, 1
        let new_paddle_pos = board.paddle_col.value + actual_direction
        
        # Ensure paddle stays within bounds
        if new_paddle_pos >= 0 and new_paddle_pos < board.num_cols.value:
            board.paddle_col.value = new_paddle_pos

# Function for machine learning components to display the game state
fun pretty_print(Game game):
    print(game.board.to_string())
    
    # Print game status
    if game.is_done():
        if score(game, 0) == 1.0:
            print("You win! You caught the ball."s)
        else:
            print("You lose! You missed the ball."s)
    else:
        print("Game in progress. Ball at row "s + to_string(game.board.ball_row) + ", column "s + to_string(game.board.ball_col))
        print("Paddle at column "s + to_string(game.board.paddle_col))

# Return current player or special value if game is done
fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4  # Terminal state
    return 0  # Player 0 (the only player)

# Return score for ML training
fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0
    if g.board.paddle_col == g.board.ball_col:
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

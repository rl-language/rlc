# RUN: rlc %s -o %t -i %stdlib --shared

import serialization.print
import range
import collections.vector
import machine_learning
import action

# Constants for the game
const NUM_ROWS = 11
const NUM_COLUMS = 5

enum Direction:
    Left
    None
    Right

    fun equal(Direction other) -> Bool:
        return self.value == other.value

    fun not_equal(Direction other) -> Bool:
        return !(self.value == other.value)


using Column = BInt<0, NUM_COLUMS>
using Row = BInt<0, NUM_ROWS>

# The main Catch game
@classes
act play() -> Game:
    frm ball_row : Row 
    frm ball_col : Column
    frm paddle_col : Column
    
    # Initialization - chance player selects starting ball column
    act set_start_location(Column col)
    ball_col.value = col.value
    paddle_col = NUM_COLUMS / 2
    
    # Game loop - player makes moves until ball reaches bottom
    while ball_row != NUM_ROWS - 1:
        act move(Direction direction)         

        ball_row.value = ball_row.value + 1
        
        let actual_direction = direction.value - 1  # Convert to -1, 0, 1
        paddle_col = paddle_col.value + actual_direction

# Cell states
enum CellState:
    Empty
    Paddle
    Ball

    fun equal(CellState other) -> Bool:
        return self.value == other.value

    fun not_equal(CellState other) -> Bool:
        return !(self.value == other.value)

# Get cell state at specific row and column
fun cell_at(Game game, Int row, Int col) -> CellState:
    if row == NUM_ROWS - 1 and col == game.paddle_col.value:
        return CellState::Paddle
    else if row == game.ball_row.value and col == game.ball_col.value:
        return CellState::Ball
    return CellState::Empty

        

# Function for machine learning components to display the game state
fun pretty_print(Game game):
    # Generate string representation of the board
    let result = ""s
    for row in range(NUM_ROWS):
        for col in range(NUM_COLUMS):
            if cell_at(game, row, col) == CellState::Empty:
                result = result + "."s
            else if cell_at(game, row, col) == CellState::Paddle:
                result = result + "x"s
            else:
                result = result + "o"s
        result = result + "\n"s
    print(result)


# Return current player or special value if game is done
fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4  # Terminal state
    let column : Column
    if can g.set_start_location(column):
        return -1
    return 0  # Player 0 (the only player)

# Return score for ML training
fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0
    if g.paddle_col.value == g.ball_col.value:
        return 1.0
    else:
        return 0.0

# Return number of players (always 1 for this game)
fun get_num_players() -> Int:
    return 1


# checkers implementation using the same rules as openspiel implementation (8x8 board, 40 moves without capturs mean draw)
# RUN: rlc %s -o %t -i %stdlib

import action
import enum_utils
import machine_learning

enum CellState:
    empty
    white
    white_king
    black
    black_king

    fun to_char() -> Byte:
        if self == CellState::white:
            return 'w'
        if self == CellState::white_king:
            return 'W'
        if self == CellState::black:
            return 'b'
        if self == CellState::black_king:
            return 'B'
        return ' '

    fun equal(CellState other) -> Bool:
        return self.value == other.value

    fun not_equal(CellState other) -> Bool:
        return self.value != other.value

    fun maybe_upgrade(Int y_location):
        if self == CellState::white and y_location == 7:
            self = CellState::white_king
            return
        if self == CellState::black and y_location == 0:
            self = CellState::black_king
            return

    fun get_player() -> Int:
        if self == CellState::white:
            return 0
        if self == CellState::white_king:
            return 0
        if self == CellState::black:
            return 1
        if self == CellState::black_king:
            return 1
        return -1

    fun acceptable_y_direction() -> Int:
        if self == CellState::white:
            return 1
        if self == CellState::black:
            return -1
        return 0

    fun belongs_to_player(Int player_id) -> Bool:
        if player_id == 0:
            return self == CellState::white or self == CellState::white_king
        if player_id == 1:
            return self == CellState::black or self == CellState::black_king
        return false
cls Board:
    CellState[8][8] slots

fun make_board() -> Board:
    let to_return: Board
    
    to_return.slots[1][0] = CellState::white
    to_return.slots[3][0] = CellState::white
    to_return.slots[5][0] = CellState::white
    to_return.slots[7][0] = CellState::white
                   
    to_return.slots[0][1] = CellState::white
    to_return.slots[2][1] = CellState::white
    to_return.slots[4][1] = CellState::white
    to_return.slots[6][1] = CellState::white
                   
    to_return.slots[1][2] = CellState::white
    to_return.slots[3][2] = CellState::white
    to_return.slots[5][2] = CellState::white
    to_return.slots[7][2] = CellState::white
                   
    to_return.slots[0][5] = CellState::black
    to_return.slots[2][5] = CellState::black
    to_return.slots[4][5] = CellState::black
    to_return.slots[6][5] = CellState::black
                   
    to_return.slots[1][6] = CellState::black
    to_return.slots[3][6] = CellState::black
    to_return.slots[5][6] = CellState::black
    to_return.slots[7][6] = CellState::black
                   
    to_return.slots[0][7] = CellState::black
    to_return.slots[2][7] = CellState::black
    to_return.slots[4][7] = CellState::black
    to_return.slots[6][7] = CellState::black

    return to_return

fun is_capture(Board board, Int target_x, Int target_y, Int x, Int y) -> Bool:
    if abs(target_x - x) == 2 and abs(target_y - y) == 2:
        let cell_player = board.slots[(target_x + x) / 2][(target_y + y) / 2].get_player()
        if cell_player == -1:
            return false 
        if cell_player != board.slots[x][y].get_player():
            return true
    return false

fun target_location_accettable(Board board, Int target_x, Int target_y, Int x, Int y) -> Bool:
    if target_x > 7 or target_y > 7:
        return false
    if 0 > target_x or 0 > target_y:
        return false
    if board.slots[target_x][target_y] != CellState::empty:
        return false
    if abs(target_x - x) >  2 or abs(target_y - y) > 2 or abs(target_x - x) == 0 or abs(target_y - y) == 0:
        return false
    let direction = target_y - y 
    direction = direction / abs(direction)
    let accettable = board.slots[x][y].acceptable_y_direction()
    if accettable != 0 and accettable != direction:
        return false
    if abs(target_x - x) == 1 and abs(target_y - y) == 1:
        return true
    return is_capture(board, target_x, target_y, x, y)

act move(ctx Board board, frm Bool current_player, frm BInt<0, 8> x, frm BInt<0, 8> y) ->  Move:
    act move_to(frm BInt<0, 8> target_x, frm BInt<0, 8> target_y) {
        target_location_accettable(board, target_x.value, target_y.value, x.value, y.value)
    }

    frm is_capture = is_capture(board, target_x.value, target_y.value, x.value, y.value)
    # move the piece
    board.slots[target_x.value][target_y.value] = board.slots[x.value][y.value]
    board.slots[target_x.value][target_y.value].maybe_upgrade(target_y.value)
    board.slots[x.value][y.value] = CellState::empty

fun has_valid_target(Board b, Int x, Int y) -> Bool:
    if target_location_accettable(b, x + 1, y + 1, x, y) or target_location_accettable(b, x - 1, y - 1, x, y) or target_location_accettable(b, x - 1, y + 1, x, y) or target_location_accettable(b, x + 1, y - 1, x, y):
        return true
    if target_location_accettable(b, x + 2, y + 2, x, y) or target_location_accettable(b, x - 2, y - 2, x, y) or target_location_accettable(b, x - 2, y + 2, x, y) or target_location_accettable(b, x + 2, y - 2, x, y):
        return true
    return false

fun has_capture_available(Board b, Int x, Int y) -> Bool:
    if target_location_accettable(b, x + 2, y + 2, x, y):
        return true 
    if target_location_accettable(b, x - 2, y - 2, x, y):
        return true 
    if target_location_accettable(b, x + 2, y - 2, x, y):
        return true 
    if target_location_accettable(b, x - 2, y + 2, x, y):
        return true 
    return false
    
fun has_capture_available(Int player_id, Board b) -> Bool:
    let x = 0
    while x != 8:
        let y = 0
        while y != 8:
            if b.slots[x][y].get_player() != player_id:
                y = y + 1
                continue
            if has_capture_available(b, x, y):
                return true
            y = y + 1
        x = x + 1
    return false

fun can_move_anything(Int player_id, Board b) -> Bool:
    let x = 0
    while x != 8:
        let y = 0
        while y != 8:
            if b.slots[x][y].get_player() != player_id:
                y = y + 1
                continue
            if has_valid_target(b, x, y):
                return true
            y = y + 1
        x = x + 1
    return false

act play() -> Game:
    frm current_player : Bool
    frm board = make_board()
    frm not_taken_turns : BInt<0, 41>

    frm lost_pieces : BInt<0, 13>[2]

    while lost_pieces[int(current_player)] != 12 and not_taken_turns != 40:
        if !can_move_anything(int(current_player), board):
            return
        frm can_capture = has_capture_available(int(current_player), board)
        act select(frm BInt<0, 8> x, frm BInt<0, 8> y){
            board.slots[x.value][y.value].belongs_to_player(int(current_player)),
            has_valid_target(board, x.value, y.value),
            !can_capture or has_capture_available(board, x.value, y.value)
        }
        while true:

            subaction*(board) movement = move(board, current_player, x, y)
            if !movement.is_capture:
                not_taken_turns = not_taken_turns + 1
                current_player = !current_player
                break

            # count kill and remove opponent piece
            board.slots[(movement.target_x.value + movement.x.value) / 2][(movement.target_y.value + movement.y.value) / 2] = CellState::empty
            ref lost_pieces = lost_pieces[(int(current_player) + 1) % 2] 
            lost_pieces = lost_pieces + 1
            not_taken_turns = 0 
            x = movement.target_x
            y = movement.target_y
            if !has_capture_available(board, x.value, y.value):
                current_player = !current_player
                break
            

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    return int(g.current_player)

fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0 
    if g.lost_pieces[(1 + player_id) % 2] == 15:
        return 1.0
    if g.lost_pieces[player_id] == 15:
        return -1.0
    return 0.0

fun get_num_players() -> Int:
    return 2


fun pretty_print(Game g):
    let x = 0
    while x != 8:
        let s : String
        let y = 0
        while y != 8:
            s.append(g.board.slots[x][y].to_char()) 
            y = y + 1
        print(s)
        x = x + 1


fun fuzz(Vector<Byte> input):
    let state = play()
    let x : AnyGameAction
    let enumeration = enumerate(x)
    let index = 0
    while index + 8 < input.size():
        if state.is_done():
            return
        let num_action : Int
        from_byte_vector(num_action, input, index)
        if num_action < 0:
          num_action = num_action * -1 
        if num_action < 0:
          num_action = 0 

        let executable : Vector<AnyGameAction>
        let i = 0
        while i < enumeration.size():
          if can apply(enumeration.get(i), state):
            executable.append(enumeration.get(i))
          i = i + 1
        if executable.empty():
            pretty_print(state)
            assert(false, "found non terminal state with no valid moves")
            return
        print(executable.get(num_action % executable.size()))
        apply(executable.get(num_action % executable.size()), state)

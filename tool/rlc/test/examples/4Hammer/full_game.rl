import rules
import game_utils

# a whole game, including army selection
@classes
act play() -> Game:
    frm board : Board
    subaction*(board) p1 = pick_army(board, false)
    subaction*(board) p2 = pick_army(board, true)
    subaction*(board) battle = battle(board)


fun score(Game g, Int player_id) -> Float:
    return float(g.board.score[player_id].value - g.board.score[1-player_id].value)

fun get_current_player(Game g) -> Int:
    return default_get_current_player(g)

fun log_alive_models_player1(Game g) -> Int:
    return g.board.count_models(0)

fun log_alive_models_player2(Game g) -> Int:
    return g.board.count_models(1)

fun get_num_players() -> Int:
    return 2

fun max_game_lenght() -> Int:
    return 5000

fun pretty_print(Game game):
    print_indented(game)


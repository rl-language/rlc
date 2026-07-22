from rlc.uigen.layout import Direction, FIT
from rlc.uigen.view_types.interaction_context import parse_config_path
from rlc.uigen.game_config import GameConfig
from rlc.uigen.policies import when, random_action
from rlc.uigen.view_types.declarative import Panel, Text, Field
from test.uigen.tic_tac_toe.cell_widget import CellWidget


computer_turn = when(
    lambda state, _program: not state.state.board.playerTurn and not state.is_done(),
    random_action,
)


FIT_BOTH = (FIT(), FIT())

BOARD = Panel(
    Panel(Field("slots", direction=Direction.COLUMN, bg="#C9B79C"),
          sizing=FIT_BOTH),
    Panel(Text("Player Turn: "), Field("playerTurn"),
          direction=Direction.ROW, sizing=FIT_BOTH),
    border=4,
    child_gap=14,
    sizing=FIT_BOTH,
)
GAME = Panel(
    Panel(Text("Tic-Tac-Toe"), sizing=FIT_BOTH),
    Field("board"),
    Panel(Text("Score: "), Field("score"),
          direction=Direction.ROW, sizing=FIT_BOTH),
    border=4,
    child_gap=14,
    sizing=FIT_BOTH,
)

CONFIG = GameConfig(
    title="Tic-Tac-Toe",
    interactions=[
        (parse_config_path("Game/board/slots/$x/$y/on_click"), "mark"),
    ],
    policy=computer_turn,
    renderer_config={
        "Game": GAME,
        "Board": BOARD,
        "BIntT0T3T": CellWidget(),
    },
)

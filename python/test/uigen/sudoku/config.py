from rlc.uigen.layout import Direction, FIT
from rlc.uigen.view_types.interaction_context import parse_config_path
from rlc.uigen.view_types.dispatch_actions import action, dispatch_action as rlc_dispatch
from rlc.uigen.view_types.declarative import Panel, Text, Field
from rlc.uigen.game_config import GameConfig
from test.uigen.sudoku.cell_widget import CellWidget


@action("input_value")
def input_value(program, state, x, y, value):
    if not (value and value.isdigit() and value != "0"):
        return False
    return rlc_dispatch("place", program, state, {"num": int(value), "row": x, "col": y})


BOARD = Panel(
    Panel(Text("Sudoku")),
    Panel(Field("slots", direction=Direction.COLUMN, cell=CellWidget(), bg="#2E2A26"),sizing=(FIT(), FIT())),
    border=4,
    child_gap=14,
    sizing=(FIT(), FIT())
)


CONFIG = GameConfig(
    title="Sudoku",
    interactions=[
        (parse_config_path("Game/board/slots/$x/$y/on_focus"),         "select_cell"),
        (parse_config_path("Game/board/slots/$x/$y/on_key/$value"),    "input_value"),
    ],
    click_mode="focus",
    handles_keys=True,
    renderer_config={
        "Game": Field("board"),
        "Board": BOARD,
    },
)

import random

from rlc.uigen.layout import Direction, FIXED, FIT
from rlc.uigen.view_types.interaction_context import parse_config_path
from rlc.uigen.view_types.declarative import Panel, Text, Field, ActionButton
from rlc.uigen.game_config import GameConfig
from test.uigen.hanabi.card_widget import CardWidget
from test.uigen.hanabi.firework_panel import FireworkPanel
from test.uigen.hanabi.actions import card_click  # noqa: F401  (registers @action handlers)


def _is_draw(action):
    return str(action).startswith("draw_random_card")


def card_selected(game, node):
    board = game.board
    if board.selected < 0:
        return False
    meta = getattr(node, "on_click", None)
    if not meta:
        return False
    args = meta.get("args", {})
    player = 0 if board.selected_is_own else 1
    return args.get("player") == player and args.get("index") == board.selected


def hanabi_policy(state, _program):
    if state.is_done():
        return False
    actions = state.legal_actions or []
    draws = [a for a in actions if _is_draw(a)]
    if draws:
        state.step(random.choice(draws))
        return True
    if state.state.board.current_player.value != 1:
        return False
    others = [a for a in actions if not _is_draw(a)]
    if others:
        state.step(random.choice(others))
        return True
    return False


_LEFT = Panel(
    Panel(Text("Your hand"),
          Field("player_hands", direction=Direction.ROW, bg="#F6F0ED", index=0,
                cell=CardWidget(face_down=False), highlight_when=card_selected),
          child_gap=4),
    Panel(Text("Firework piles"), FireworkPanel(), child_gap=4),
    Panel(
        Text("Status"),
        Panel(
            Field("info_token", label="Info tokens"),
            Field("fuses", label="Fuses"),
            Field("current_player", label="Current player"),
            direction=Direction.ROW,
            child_gap=28,
        ),
        child_gap=4,
    ),
    Panel(Text("Opponent hand"),
          Field("player_hands", direction=Direction.ROW, bg="#F6F0ED", index=1,
                cell=CardWidget(), highlight_when=card_selected),
          child_gap=4),
    Panel(
        ActionButton(label="Play", action="play_selected"),
        ActionButton(label="Discard", action="discard_selected"),
        ActionButton(label="Deselect", action="deselect", bg="#8A7A6A"),
        direction=Direction.ROW,
        child_gap=12,
    ),
    Panel(
        *[ActionButton(label=c.capitalize(), action="clue_color",
                       args={"suit": c}, bg="#5B6BA8")
          for c in ("red", "yellow", "green", "blue", "white")],
        direction=Direction.ROW,
        child_gap=8,
    ),
    Panel(
        *[ActionButton(label=str(n), action="clue_number",
                       args={"value": n}, bg="#5B6BA8")
          for n in (1, 2, 3, 4, 5)],
        direction=Direction.ROW,
        child_gap=8,
    ),
    child_gap=8,
)

BOARD = Panel(
    _LEFT,
    Panel(Text("Log"), Field("info_box", direction=Direction.COLUMN),
          sizing=(FIXED(350), FIT())),
    direction=Direction.ROW,
    border=4,
    child_gap=14,
)


CONFIG = GameConfig(
    title="hanabi",
    policy=hanabi_policy,
    interactions=[
        (parse_config_path("Game/board/player_hands/$player/$index/on_click"), "card_click"),
    ],
    click_mode="dispatch",
    handles_keys=False,
    renderer_config={
        "Game": Field("board"),
        "Board": BOARD,
    },
)

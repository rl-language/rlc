import random

from rlc.uigen.layout import Direction
from rlc.uigen.view_types.interaction_context import parse_config_path
from rlc.uigen.game_config import GameConfig
from rlc.uigen.view_types.declarative import Panel, Text, Field
from test.uigen.blackjack.card_widget import CardWidget


def run_shuffle(state, _program):
    if state.state.shuffling.is_done():
        return False
    state.step(random.choice(state.legal_actions))
    return True


HAND = Field("cards", direction=Direction.ROW, cell=CardWidget(), bg="#F6F0ED")


def game_over(g):
    return g.resume_index == -1


BOARD = Panel(
    Panel(Text("Dealer hand"),
          Field("dealer_hand", visible_when=game_over)),
    Panel(Text("Your hand"), Field("player_hand")),
    Panel(
        Field("hit_button", label="Hit", button=True),
        Field("stand_button", label="Stand", button=True),
        direction=Direction.ROW,
        child_gap=20,
    ),
    Panel(
        Field("player_passed", label="Passed"),
        Field("player_bust", label="Bust"),
        direction=Direction.ROW,
        child_gap=28,
    ),
    border=4,
    child_gap=14,
)


CONFIG = GameConfig(
    title="Blackjack",
    interactions=[
        (parse_config_path("Game/hit_button/on_click"),     "hit"),
        (parse_config_path("Game/stand_button/on_click"), "stand"),
    ],
    policy=run_shuffle,
    renderer_config={
        "Game": BOARD,
        "PlayerHand": HAND,
        "HiddenTBoundedVectorTBIntT0T14TT20TT": Field("value", direction=Direction.ROW,cell=CardWidget(), bg="#EFE7DD"),
    },
)

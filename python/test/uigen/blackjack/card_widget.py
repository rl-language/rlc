from dataclasses import dataclass

from rlc.uigen.layout import Layout, Direction, FIXED, Padding
from rlc.uigen.text import Text
from rlc.uigen.view_types.declarative import Widget
from rlc.uigen.view_types.view_type import to_display_str


CARD_WIDTH = 48
CARD_HEIGHT = 68
_CARD_BG = "#FDFBF7"
_RANK_COLOR = "#1A1A1A"

_FACE = {1: "A", 11: "J", 12: "Q", 13: "K"}


def _rank(value):
    text = to_display_str(value)
    try:
        n = int(text)
    except (TypeError, ValueError):
        return text
    return _FACE.get(n, str(n))


@dataclass
class CardWidget(Widget):
    def build(self, value):
        card = Layout(sizing=(FIXED(CARD_WIDTH), FIXED(CARD_HEIGHT)),
                      direction=Direction.ROW, color=_CARD_BG, border=2,
                      padding=Padding(0, 0, 0, 0), align="center")
        card._no_blend = True
        card.add_child(Text(_rank(value), "Arial", 26, _RANK_COLOR))
        return card

    def refresh(self, card, value):
        label = card.children[0]
        if isinstance(label, Text):
            label.update_text(_rank(value))

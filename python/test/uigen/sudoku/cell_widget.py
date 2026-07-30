from dataclasses import dataclass

from rlc.uigen.layout import Layout, Direction, FIXED, Padding
from rlc.uigen.text import Text
from rlc.uigen.view_types.declarative import Widget
from rlc.uigen.view_types.view_type import to_display_str


CELL_SIZE = 56
_CELL_BG = "#FBF7F0"
_GIVEN_COLOR = "#222222"


def _digit(value):
    text = to_display_str(value)
    return text if text and text != "0" else ""


@dataclass
class CellWidget(Widget):
    def build(self, value):
        cell = Layout(sizing=(FIXED(CELL_SIZE), FIXED(CELL_SIZE)),
                      direction=Direction.ROW, color=_CELL_BG, border=2,
                      padding=Padding(0, 0, 0, 0), align="center")
        cell._no_blend = True
        cell.add_child(Text(_digit(value), "Arial", 30, _GIVEN_COLOR))
        return cell

    def refresh(self, cell, value):
        label = cell.children[0]
        if isinstance(label, Text):
            label.update_text(_digit(value))

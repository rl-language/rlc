from dataclasses import dataclass

from rlc.uigen.layout import Layout, Direction, FIXED, Padding
from rlc.uigen.text import Text
from rlc.uigen.view_types.declarative import Widget
from rlc.uigen.view_types.view_type import to_display_str


CELL_SIZE = 90
_MARKS = {0: "", 1: "X", 2: "O"}
_MARK_COLORS = {1: "#A51C30", 2: "#1F6FB2"}
_CELL_BG = "#FBF7F0"


def _mark(value):
    try:
        n = int(to_display_str(value) or 0)
    except ValueError:
        n = 0
    return _MARKS.get(n, ""), _MARK_COLORS.get(n, "#222222")


@dataclass
class CellWidget(Widget):
    def build(self, value):
        text, color = _mark(value)
        cell = Layout(sizing=(FIXED(CELL_SIZE), FIXED(CELL_SIZE)),
                      direction=Direction.ROW, color=_CELL_BG, border=2,
                      padding=Padding(8, 8, 8, 8), align="center")
        cell._no_blend = True
        cell.add_child(Text(text, "Arial", 52, color))
        return cell

    def refresh(self, cell, value):
        text, color = _mark(value)
        label = cell.children[0]
        if isinstance(label, Text):
            label.update_text(text)
            label.color = color

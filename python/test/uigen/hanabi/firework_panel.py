import os
from dataclasses import dataclass

from rlc.uigen.image import Image
from rlc.uigen.layout import Layout, Direction, FIT, Padding
from rlc.uigen.view_types.declarative import Widget
from rlc.uigen.view_types.view_type import seq_size, seq_at
from test.uigen.hanabi.card_widget import (
    SUIT_NAMES, SUIT_COLORS, CARD_W, CARD_H, _ASSET_DIR, _as_int,
)


def _pile_visual(suit_idx, value):
    suit_name = SUIT_NAMES[suit_idx] if 0 <= suit_idx < len(SUIT_NAMES) else ""
    tint = SUIT_COLORS.get(suit_name, "lightgray")
    if not value:
        return None, "#cfc7b8", suit_name, "-"
    path = os.path.join(_ASSET_DIR, f"{suit_name}_{value}.png")
    return path, tint, suit_name, str(value)


@dataclass
class FireworkPanel(Widget):
    field: str = "highest_card_played"

    def _card(self, suit_idx, value):
        path, tint, suit_name, label = _pile_visual(suit_idx, value)
        return Image((CARD_W, CARD_H), image_path=path, tint=tint,
                     label=label, sub_label=suit_name)

    def build(self, piles):
        row = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW,
                     child_gap=10, color=None, border=0, padding=Padding())
        for suit_idx in range(seq_size(piles)):
            value = _as_int(seq_at(piles, suit_idx))
            card = self._card(suit_idx, value)
            row.children_mapping[suit_idx] = card
            row.add_child(card)
        return row

    def refresh(self, row, piles):
        for suit_idx, card in enumerate(row.children):
            if suit_idx >= seq_size(piles):
                break
            path, tint, suit_name, label = _pile_visual(
                suit_idx, _as_int(seq_at(piles, suit_idx)))
            card.update_image(image_path=path, tint=tint,
                              label=label, sub_label=suit_name)

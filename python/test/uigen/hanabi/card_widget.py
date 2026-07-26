import os
from dataclasses import dataclass

from rlc.uigen.image import Image
from rlc.uigen.view_types.declarative import Widget
from rlc.uigen.view_types.view_type import get_field


SUIT_NAMES = ["red", "yellow", "green", "blue", "white"]
SUIT_COLORS = {
    "white": "#F9F7F3",
    "yellow": "#F7CE5B",
    "green": "#8A9B68",
    "blue": "#5398BE",
    "red": "#A51C30",
}

CARD_W, CARD_H = 80, 120

_ASSET_DIR = os.path.join(os.path.dirname(__file__), "assets")


def _as_int(obj):
    if obj is None:
        return None
    if isinstance(obj, bool):
        return int(obj)
    if isinstance(obj, int):
        return obj
    if isinstance(obj, dict):
        return _as_int(obj.get("value"))
    inner = getattr(obj, "value", None)
    if inner is not None and inner is not obj:
        return _as_int(inner)
    return None


def _suit_value(card):
    return _as_int(get_field(card, "suit")), _as_int(get_field(card, "value"))


def _card_visual(card):
    suit_idx, value = _suit_value(card)
    suit_name = SUIT_NAMES[suit_idx] if suit_idx is not None and 0 <= suit_idx < len(SUIT_NAMES) else None
    tint = SUIT_COLORS.get(suit_name, "lightgray")
    label = str(value) if value is not None else "?"
    image_path = None
    if suit_name is not None and value is not None:
        image_path = os.path.join(_ASSET_DIR, f"{suit_name}_{value}.png")
    return image_path, tint, suit_name or "", label


_FACE_DOWN = (None, "#3B3B3B", "", "?")


@dataclass
class CardWidget(Widget):
    face_down: bool = False

    def _visual(self, card):
        return _FACE_DOWN if self.face_down else _card_visual(card)

    def build(self, card):
        image_path, tint, suit_name, label = self._visual(card)
        return Image((CARD_W, CARD_H), image_path=image_path,
                     tint=tint, label=label, sub_label=suit_name)

    def refresh(self, layout, card):
        if isinstance(layout, Image):
            image_path, tint, suit_name, label = self._visual(card)
            layout.update_image(image_path=image_path, tint=tint,
                                label=label, sub_label=suit_name)

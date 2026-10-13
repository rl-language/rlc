import os
from typing import Any, Dict, Tuple

os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
os.environ.setdefault("SDL_AUDIODRIVER", "dummy")

import pygame

from rlc.uigen.view_backend import ViewBackend


class HeadlessTextBackend(ViewBackend):
    def __init__(self):
        pygame.font.init()
        self._font_cache: Dict[Tuple[str, int], Any] = {}

    def _font(self, font_name: str, font_size: int):
        key = (font_name, font_size)
        f = self._font_cache.get(key)
        if f is None:
            f = pygame.font.SysFont(font_name, font_size)
            self._font_cache[key] = f
        return f

    def get_text_size(self, text: str, font_name: str, font_size: int) -> Tuple[int, int]:
        return self._font(font_name, font_size).size(text)

    def render_text(self, *a, **k):
        return []

    def render_text_lines(self, *a, **k):
        return []

    def draw_rectangle(self, *a, **k):
        return None

    def draw_border(self, *a, **k):
        return None

    def blit_surface(self, *a, **k):
        return None

    def load_image(self, *a, **k):
        return None

    def draw_image(self, *a, **k):
        return False

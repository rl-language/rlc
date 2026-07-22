import math
import os
import time
from typing import Tuple, List, Optional, Iterator

import pygame

from rlc.uigen.view_backend import ViewBackend
from rlc.uigen.window_events import QuitEvent, ResizeEvent, ScrollEvent, ClickEvent, KeyEvent



class PygameRenderer(ViewBackend):
    _image_cache = {}

    def __init__(self, screen):
        self.screen = screen

    def get_text_size(self, text: str, font_name: str, font_size: int) -> Tuple[int, int]:
        font = pygame.font.SysFont(font_name, font_size)
        return font.render(text, True, pygame.Color("black")).get_size()

    def render_text(self, text: str, font_name: str, font_size: int, color: str) -> List:
        font = pygame.font.SysFont(font_name, font_size)
        return [font.render(text, True, pygame.Color(color))]

    def render_text_lines(self, lines: List[str], font_name: str, font_size: int, color: str,
                          anim_start, anim_duration, alpha) -> List:
        if anim_start:
            t = (time.time() - anim_start) / anim_duration
            if t >= 1:
                anim_start = None
                alpha = 255
            else:
                alpha = int(255 * math.sin(math.pi * t))
        font = pygame.font.SysFont(font_name, font_size)
        return [font.render(line, True, pygame.Color(color)) for line in lines]

    def draw_rectangle(self, position: Tuple[int, int], size: Tuple[int, int],
                       color: str, border_size: int = 2):
        x, y = position
        w, h = size
        if color and color.startswith("rgba("):
            parts = color[5:-1].split(',')
            r, g, b, a = map(int, parts)
            surf = pygame.Surface((w, h), pygame.SRCALPHA)
            surf.fill((r, g, b, a))
            self.blit_surface(surf, (x, y))
        else:
            pygame.draw.rect(self.screen,
                             pygame.Color(color if color else "white"),
                             pygame.Rect(x, y, w, h))

    def draw_border(self, position: Tuple[int, int], size: Tuple[int, int],
                    border_color: str = "darkgray", border_size: int = 2):
        if border_size <= 0:
            return
        x, y = position
        w, h = size
        pygame.draw.rect(self.screen, pygame.Color(border_color),
                         pygame.Rect(x, y, w, h), width=border_size)

    def blit_surface(self, surface, position: Tuple[int, int]):
        self.screen.blit(surface, position)

    def load_image(self, path: str, size: Tuple[int, int]) -> Optional[object]:
        w, h = int(size[0]), int(size[1])
        key = (path, w, h)
        if key in self._image_cache:
            return self._image_cache[key]
        surface = None
        if path and os.path.exists(path):
            try:
                raw = pygame.image.load(path).convert_alpha()
                surface = pygame.transform.smoothscale(raw, (w, h))
            except (pygame.error, ValueError):
                surface = None
        self._image_cache[key] = surface
        return surface

    def draw_image(self, path: str, position: Tuple[int, int], size: Tuple[int, int]) -> bool:
        surface = self.load_image(path, size)
        if surface is None:
            return False
        self.blit_surface(surface, (int(position[0]), int(position[1])))
        return True

    def draw_card(self, position: Tuple[int, int], size: Tuple[int, int],
                  tint: str = "white", label: str = "", sub_label: str = ""):
        x, y = int(position[0]), int(position[1])
        w, h = int(size[0]), int(size[1])
        radius = max(4, min(w, h) // 8)
        fill = pygame.Color(tint if tint else "white")
        pygame.draw.rect(self.screen, fill, pygame.Rect(x, y, w, h), border_radius=radius)
        pygame.draw.rect(self.screen, pygame.Color("black"), pygame.Rect(x, y, w, h),
                         width=2, border_radius=radius)
        luminance = 0.299 * fill.r + 0.587 * fill.g + 0.114 * fill.b
        text_color = pygame.Color("black") if luminance > 140 else pygame.Color("white")
        if label:
            font = pygame.font.SysFont("Arial", max(12, h // 2), bold=True)
            surf = font.render(str(label), True, text_color)
            self.screen.blit(surf, surf.get_rect(center=(x + w // 2, y + h // 2)))
        if sub_label:
            font = pygame.font.SysFont("Arial", max(8, h // 6))
            surf = font.render(str(sub_label), True, text_color)
            self.screen.blit(surf, surf.get_rect(midtop=(x + w // 2, y + 3)))


class PygameWindow:
    def __init__(self, title: str, width: int = 1280, height: int = 720):
        pygame.init()
        self._screen = pygame.display.set_mode((width, height), pygame.RESIZABLE)
        pygame.display.set_caption(title)
        self._clock = pygame.time.Clock()

    @property
    def size(self) -> Tuple[int, int]:
        return self._screen.get_size()

    def make_renderer(self) -> PygameRenderer:
        return PygameRenderer(self._screen)

    def pump_events(self) -> Iterator:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                yield QuitEvent()
            elif event.type == pygame.VIDEORESIZE:
                self._screen = pygame.display.set_mode((event.w, event.h), pygame.RESIZABLE)
                yield ResizeEvent(event.w, event.h)
            elif event.type == pygame.MOUSEWHEEL:
                yield ScrollEvent(dx=event.x * 30, dy=event.y * 30)
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                mx, my = pygame.mouse.get_pos()
                yield ClickEvent(mx, my)
            elif event.type == pygame.KEYDOWN:
                yield KeyEvent(event.key)

    def begin_frame(self):
        self._screen.fill("white")

    def end_frame(self):
        pygame.display.flip()

    def tick(self, fps: int = 60) -> float:
        return self._clock.tick(fps) / 1000.0

    def close(self):
        pygame.quit()


def display(build_function):
    import argparse
    from rlc.uigen.layout_logger import LayoutLogger, LayoutLogConfig
    from rlc.uigen.render import render

    parser = argparse.ArgumentParser()
    parser.add_argument("--dump", action="store_true")
    parser.add_argument("--json", action="store_true")
    parser.add_argument("--dump-out", nargs="?", const="logs/", default=None)
    parser.add_argument("--json-out", nargs="?", const="logs/", default=None)
    args = parser.parse_args()

    want_logger = args.dump or args.json or args.dump_out or args.json_out
    logger = LayoutLogger(LayoutLogConfig()) if want_logger else None

    pygame.init()
    screen = pygame.display.set_mode((2000, 700))
    screen.fill((240, 230, 220))
    backend = PygameRenderer(screen)

    root = build_function()
    root.compute_size(logger=logger, backend=backend)
    root.layout(20, 20, logger=logger)

    if logger:
        logger.record_final_tree(root=root)
        if args.dump:
            print(logger.to_text_tree(root))
        if args.json:
            print(logger.to_json())
        if args.json_out:
            path = _auto_name("layout_log", "json", args.json_out)
            logger.write_json(path=path)
            print(f"[saved] json log -> {path}")
        if args.dump_out:
            path = _auto_name("layout_tree", "txt", args.dump_out)
            logger.write_text_tree(path=path, root=root)
            print(f"[saved] text tree -> {path}")

    render(backend, root)
    pygame.display.flip()
    pygame.time.wait(5000)
    pygame.quit()


def _auto_name(prefix: str, ext: str, out=None) -> str:
    ts = time.strftime("%Y%m%d-%H%M%S")
    if not out:
        return os.path.join("logs", f"{prefix}-{ts}.{ext}")
    if out.endswith("/") or (os.path.isdir(out) if os.path.exists(out) else out.endswith(os.sep)):
        return os.path.join(out, f"{prefix}-{ts}.{ext}")
    return out

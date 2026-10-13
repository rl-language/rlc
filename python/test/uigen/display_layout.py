
import argparse
import pygame
import os
import time
from rlc import Layout, Text, Image
from rlc import LayoutLogConfig, LayoutLogger
from rlc import ViewBackend
from typing import Tuple, List
import math

def render(self, backend, position):
        """Called when drawing text."""
        
class PygameRenderer(ViewBackend):
    _image_cache = {}

    def __init__(self, screen):
        self.screen = screen

    def get_text_size(self, text: str, font_name: str, font_size: int) -> Tuple[int, int]:
        font = pygame.font.SysFont(font_name, font_size)
        surface = font.render(text, True, pygame.Color("black"))  # Dummy render for size
        return surface.get_size()

    def render_text(self, text: str, font_name: str, font_size: int, color: str) -> List[pygame.Surface]:
        
        font = pygame.font.SysFont(font_name, font_size)
        return [font.render(text, True, pygame.Color(color))]
    
    def render_text_lines(self, lines: List[str], font_name: str, font_size: int, color: str, anim_start, anim_duration, alpha) -> List[pygame.Surface]:
        if anim_start:
            t = (time.time() - anim_start) / anim_duration
            if t >= 1:
                anim_start = None
                alpha = 255
            else:
                alpha = int(255 * math.sin(math.pi * t))
            alpha = alpha
        font = pygame.font.SysFont(font_name, font_size)
        return [font.render(line, True, pygame.Color(color)) for line in lines]

    def draw_rectangle(self, position: Tuple[int, int], size: Tuple[int, int], color: str, border_size=2):
        x, y = position
        w, h = size
        if color and color.startswith("rgba("):
            parts = color[5:-1].split(',')
            r, g, b, a = map(int, parts)
            surf = pygame.Surface((w, h), pygame.SRCALPHA)
            surf.fill((r, g, b, a))
            self.blit_surface(surf, (x, y))
        else:
            color = pygame.Color(color if color else "white")
            pygame.draw.rect(self.screen, color, pygame.Rect(x, y, w, h))
            
    
    def draw_border(self, position: Tuple[int, int], size: Tuple[int, int],
                    border_color: str = "darkgray", border_size: int = 2):
        x, y = position
        w, h = size

        if border_size <= 0:
            return

        rect = pygame.Rect(x, y, w, h)
        pygame.draw.rect(self.screen, pygame.Color(border_color), rect, width=border_size)


    def blit_surface(self, surface, position: Tuple[int, int]):
        self.screen.blit(surface, position)

    def load_image(self, path: str, size: Tuple[int, int]):
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
            rect = surf.get_rect(center=(x + w // 2, y + h // 2))
            self.screen.blit(surf, rect)
        if sub_label:
            font = pygame.font.SysFont("Arial", max(8, h // 6))
            surf = font.render(str(sub_label), True, text_color)
            rect = surf.get_rect(midtop=(x + w // 2, y + 3))
            self.screen.blit(surf, rect)

def _auto_name(prefix: str, ext: str, out = None) -> str:
    ts = time.strftime("%Y%m%d-%H%M%S")
    if not out:
        return os.path.join("logs", f"{prefix}-{ts}.{ext}")
    if out.endswith("/") or (os.path.isdir(out) if os.path.exists(out) else out.endswith(os.sep)):
        return os.path.join(out, f"{prefix}-{ts}.{ext}")
    # explicit file path provided
    return out

def display(build_function):
    parser = argparse.ArgumentParser()
    parser.add_argument("--dump", action="store_true", help="Dump layout tree as text")
    parser.add_argument("--json", action="store_true", help="Dump layout tree as json")
    parser.add_argument("--dump-out", nargs="?",
    const="logs/", default=None, help="Write text log to this file (or directory). Auto-name if directory is missing")
    parser.add_argument("--json-out", nargs="?",
    const="logs/", default=None, help="Write json log to this file (or directory). Auto-name if directory is missing")
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
            print(f"[saved] json log  -> {path}")
        if args.dump_out:
            path = _auto_name("layout_tree", "txt", args.dump_out)
            logger.write_text_tree(path=path, root=root)
            print(f"[saved] text tree  -> {path}")

    render(backend, root)

    pygame.display.flip()
    pygame.time.wait(5000)
    pygame.quit()


def _dim_color(color, factor=0.45):
    try:
        c = pygame.Color(color if color else "white")
    except (ValueError, TypeError):
        return color
    gray = pygame.Color(160, 160, 160)
    r = int(c.r * factor + gray.r * (1 - factor))
    g = int(c.g * factor + gray.g * (1 - factor))
    b = int(c.b * factor + gray.b * (1 - factor))
    return f"#{r:02x}{g:02x}{b:02x}"


def render(backend, node):
    if isinstance(node, Image):
        draw_image_node(node, backend)
        return
    if isinstance(node, Text):
        write_text(node, backend)
        return
    if isinstance(node, Layout):
        color = node.color if node.enabled else _dim_color(node.color)
        backend.draw_rectangle((node.x, node.y), (node.width, node.height), color, node.border)
        for child in node.children:
            render(backend, child)
        if node.border > 0:
            
            if node.focused:
                backend.draw_border(
                    (node.x, node.y),
                    (node.width, node.height),
                    "yellow",
                    border_size=3
                )
            else:
                backend.draw_border((node.x, node.y), (node.width, node.height), border_color="darkgray", border_size=node.border)
        
        

def draw_image_node(node, backend):
    pos = (node.x, node.y)
    size = (node.width, node.height)
    if node.image_path and backend.draw_image(node.image_path, pos, size):
        return
    backend.draw_card(pos, size, tint=node.tint or "white",
                       label=node.label, sub_label=node.sub_label)


def write_text(node, backend):
    lines = node.wrap_text(backend, node.width)
    color = node.color
    if not node.enabled:
        color = _dim_color(node.color)
    if node.focused:
        color = "yellow"
    surfaces = backend.render_text_lines(lines, node.font_name, node.font_size, color, node.anim_start, node.anim_duration, node.alpha)
    y_offset = 0
    for surface in surfaces:
        surface.set_alpha(node.alpha)
        backend.blit_surface(surface, (node.x, node.y + y_offset))
        y_offset += surface.get_height()
    return



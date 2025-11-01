
import argparse
import pygame
import os
import time
from rlc import Layout, Text
from rlc import LayoutLogConfig, LayoutLogger


def _auto_name(prefix: str, ext: str, out = None) -> str:
    ts = time.strftime("%Y%m%d-%H%M%S")
    if not out:
        # default logs directory
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

    root = build_function()
    root.compute_size(logger=logger)
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

    render(screen, root)

    pygame.display.flip()
    pygame.time.wait(5000)
    pygame.quit()


def render(screen, node):
    if isinstance(node, Text):
        write_text(screen, node)
        return
    if isinstance(node, Layout):
        draw_rectangle(screen, (node.x, node.y), (node.width, node.height), node.color)
        for child in node.children:
            render(screen, child)

def draw_rectangle(screen, position, size, color):
    x, y = position
    w, h = size
    color = pygame.Color(color if color else "white")
    pygame.draw.rect(screen, color, pygame.Rect(x, y, w, h))

def write_text(screen, node):
    y_offset = 0
    for surface in node.text_surfaces:
        screen.blit(surface, (node.x, node.y + y_offset))
        y_offset += surface.get_height()


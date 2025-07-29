
import argparse
import pygame
from rlc import Container, Text

def display(build_function):
    parser = argparse.ArgumentParser()
    parser.add_argument("--dump", action="store_true", help="Dump layout tree as text")
    parser.add_argument("--json", action="store_true", help="Dump layout tree as json")
    args = parser.parse_args()

    pygame.init()
    screen = pygame.display.set_mode((2000, 700))
    screen.fill((240, 230, 220))

    root = build_function()
    root.compute_size()
    root.layout(20, 20)

    if args.dump:
        print("dump_as_text(root)")
    if args.json:
        print("dump_as_json(root)")

    render(screen, root)

    pygame.display.flip()
    pygame.time.wait(5000)
    pygame.quit()


def render(screen, node):
    if isinstance(node, Text):
        write_text(screen, node)
        return
    if isinstance(node, Container):
        draw_rectangle(screen, (node.x, node.y), (node.width, node.height), node.backgroundColor)
        for child in node.children:
            render(screen, child)

def draw_rectangle(screen, position, size, color):
    x, y = position
    w, h = size
    color = pygame.Color(color)
    pygame.draw.rect(screen, color, pygame.Rect(x, y, w, h))

def write_text(screen, node):
    y_offset = 0
    for surface in node.text_surfaces:
        screen.blit(surface, (node.x, node.y + y_offset))
        y_offset += surface.get_height()


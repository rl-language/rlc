from rlc.uigen.layout import Layout
from rlc.uigen.text import Text
from rlc.uigen.image import Image


_NAMED_COLORS = {
    "white": (255, 255, 255), "black": (0, 0, 0), "red": (255, 0, 0),
    "green": (0, 128, 0), "blue": (0, 0, 255), "yellow": (255, 255, 0),
    "gray": (128, 128, 128), "darkgray": (64, 64, 64), "lightgray": (211, 211, 211),
    "orange": (255, 165, 0), "purple": (128, 0, 128), "pink": (255, 192, 203),
    "cyan": (0, 255, 255), "magenta": (255, 0, 255), "brown": (165, 42, 42),
    "seagreen": (46, 139, 87), "royalblue": (65, 105, 225), "crimson": (220, 20, 60),
    "gold": (255, 215, 0), "forestgreen": (34, 139, 34), "dimgray": (105, 105, 105),
}


def _parse_color(color):
    if not color:
        return (255, 255, 255)
    c = color.strip().lower()
    if c in _NAMED_COLORS:
        return _NAMED_COLORS[c]
    if c.startswith("#"):
        c = c[1:]
        if len(c) == 6:
            return (int(c[0:2], 16), int(c[2:4], 16), int(c[4:6], 16))
    return None


def _dim_color(color, factor=0.45):
    rgb = _parse_color(color)
    if rgb is None:
        return color
    gray = 160
    r = int(rgb[0] * factor + gray * (1 - factor))
    g = int(rgb[1] * factor + gray * (1 - factor))
    b = int(rgb[2] * factor + gray * (1 - factor))
    return f"#{r:02x}{g:02x}{b:02x}"


def render(backend, node):
    if isinstance(node, Image):
        _draw_image_node(node, backend)
        return
    if isinstance(node, Text):
        _write_text(node, backend)
        return
    if isinstance(node, Layout):
        color = node.color if node.enabled else _dim_color(node.color)
        backend.draw_rectangle((node.x, node.y), (node.width, node.height), color, node.border)
        for child in node.children:
            render(backend, child)
        if node.border > 0:
            if node.focused:
                backend.draw_border((node.x, node.y), (node.width, node.height),
                                    "yellow", border_size=3)
            else:
                backend.draw_border((node.x, node.y), (node.width, node.height),
                                    border_color="darkgray", border_size=node.border)


def _draw_image_node(node, backend):
    pos = (node.x, node.y)
    size = (node.width, node.height)
    if node.image_path and backend.draw_image(node.image_path, pos, size):
        return
    if hasattr(backend, "draw_card"):
        backend.draw_card(pos, size, tint=node.tint or "white",
                          label=node.label, sub_label=node.sub_label)
    else:
        backend.draw_rectangle(pos, size, node.tint or "white")


def _write_text(node, backend):
    lines = node.wrap_text(backend, node.width)
    color = node.color
    if not node.enabled:
        color = _dim_color(node.color)
    if node.focused:
        color = "yellow"
    surfaces = backend.render_text_lines(lines, node.font_name, node.font_size, color,
                                         node.anim_start, node.anim_duration, node.alpha)
    y_offset = 0
    for surface in surfaces:
        surface.set_alpha(node.alpha)
        backend.blit_surface(surface, (node.x, node.y + y_offset))
        y_offset += surface.get_height()


def clamp_scroll(layout, view_w, view_h, scroll):
    if layout.width <= view_w:
        scroll["x"] = 0
    else:
        scroll["x"] = min(0, max(-(layout.width - view_w), scroll["x"]))
    if layout.height <= view_h:
        scroll["y"] = 0
    else:
        scroll["y"] = min(0, max(-(layout.height - view_h), scroll["y"]))


def relayout(window_size, backend, layout, scroll, margin=20):
    view_w = max(0, window_size[0] - 2 * margin)
    view_h = max(0, window_size[1] - 2 * margin)
    layout.compute_size(available_width=view_w, available_height=view_h,
                        logger=None, backend=backend)
    clamp_scroll(layout, view_w, view_h, scroll)
    layout.layout(margin + scroll["x"], margin + scroll["y"], logger=None)

from rlc.uigen.view_types.dispatch_actions import _EVENT_ATTRS


def node_pixel_bounds(node):
    w = getattr(node, "width", 0) or 0
    h = getattr(node, "height", 0) or 0
    if w <= 0 or h <= 0:
        return None
    return (node.x, node.y, w, h)


def interior_pixel(bounds):
    x, y, w, h = bounds
    return int(x + max(2, w * 0.1)), int(y + max(2, h * 0.1))


def find_all_clickable_nodes(layout):
    found = []

    def walk(node):
        for attr in _EVENT_ATTRS:
            meta = getattr(node, attr, None)
            if meta and meta.get("handler"):
                found.append((node, attr, meta))
                break
        for child in getattr(node, "children", []):
            walk(child)

    walk(layout)
    return found


def find_enabled_clickable_nodes(layout):
    return [(n, a, m) for (n, a, m) in find_all_clickable_nodes(layout)
            if getattr(n, "enabled", True) and node_pixel_bounds(n) is not None]

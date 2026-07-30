from dataclasses import dataclass, field
from typing import Any, Dict


@dataclass
class Intent:
    handler: str
    args: Dict[str, Any] = field(default_factory=dict)
    params: list = field(default_factory=list)


FOCUS = object()


def resolve_click(layout, x, y, focus_mode: bool):
    target = layout.find_target(x, y)
    if focus_mode:
        focusable = target is not None and getattr(target, "on_focus", None)
        return target if focusable else None, FOCUS
    interactive = target is not None and getattr(target, "on_click", None)
    if interactive and getattr(target, "enabled", True):
        meta = target.on_click
        return target, Intent(meta["handler"], dict(meta["args"]),
                              list(meta.get("params", [])))
    return target, None


def resolve_key(layout, value):
    focused = layout.find_focused_node()
    if not (focused and getattr(focused, "on_key", None) and
            getattr(focused, "enabled", True)):
        return focused, None
    meta = focused.on_key
    args = dict(meta["args"])
    for param_name in meta.get("params", []):
        if param_name == "value":
            args["value"] = value
    return focused, Intent(meta["handler"], args, list(meta.get("params", [])))

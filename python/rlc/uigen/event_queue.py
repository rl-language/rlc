import ctypes
from dataclasses import dataclass, field
from typing import Optional, Any, Callable, Dict
from enum import Enum


class SignalKind(Enum):
    ACTION = "action"
    RESIZE = "resize"
    SCROLL = "scroll"
    FOCUS = "focus"


@dataclass
class UpdateSignal:
    kind: SignalKind
    handler_name: Optional[str] = None
    args: Dict[str, Any] = field(default_factory=dict)
    target: Any = None
    width: int = 0
    height: int = 0
    dx: int = 0
    dy: int = 0


def _copy_state(state_obj):
    return state_obj.clone()


def _rlc_string_to_python(rlc_str) -> str:
    vec = rlc_str._data       
    size = vec._size          
    data = vec._data           
    return bytes(
        data[i] if isinstance(data[i], int) else data[i].value
        for i in range(size - 1) 
    ).decode('ascii')

def _seq_size(obj):
    while True:
        if hasattr(obj, "_size"):
            s = obj._size
            return s if isinstance(s, int) else getattr(s, "value", s)
        if hasattr(obj, "_data"):
            obj = obj._data
            continue
        return len(obj)


def _unwrap_scalar(obj):
    fields = getattr(obj, "_fields_", None)
    if fields is None:
        return obj
    public = [f for f in fields if not f[0].startswith("_")]
    if len(public) == 1 and public[0][0] == "value":
        return obj.value
    return obj


def resolve_value(state_obj, sim_path: tuple):
    obj = state_obj
    for seg in sim_path:
        if seg == "#":
            return _seq_size(obj)
        if isinstance(seg, int):
            target = obj
            while hasattr(target, '_data'):
                target = target._data
            obj = _unwrap_scalar(target[seg])
        else:
            obj = getattr(obj, seg) if hasattr(obj, seg) else obj
    return _unwrap_scalar(obj)


class UpdateController:

    def __init__(self, renderer, layout, relayout_fn: Callable, dispatch_fn: Callable,
                  state_obj=None, program_module=None, queued: bool = True):
        self.renderer = renderer
        self.layout = layout
        self.relayout_fn = relayout_fn
        self.dispatch_fn = dispatch_fn

        self._queued = queued
        self._queue = []
        self._processing = False
        self._state_changed = False
        self._needs_relayout = False

        self.scroll = {"x": 0, "y": 0}

        self._program_module = program_module
        if  state_obj is not None:
            self._last_state = _copy_state(state_obj)
        else:
            self._last_state = None

    def cleanup(self):
        self._last_state = None

    def enqueue(self, signal: UpdateSignal):
        if self._queued:
            self._queue.append(signal)
        else:
            self._handle_signal(signal)

    def process(self, state_obj, elapsed: float):
        if self._processing:
            return

        self._processing = True
        self._needs_relayout = False

        try:
            while self._queue:
                signal = self._queue.pop(0)
                self._handle_signal(signal)

            if self._state_changed:
                can_target = (self._last_state is not None
                              and self._program_module is not None
                              )
                if can_target:
                    self._targeted_update(state_obj)
                else:
                    self.renderer.update(self.layout, state_obj)
                    self._needs_relayout = True

            if _refresh_visibility(self.layout, state_obj):
                self._needs_relayout = True

            if self._needs_relayout or _any_child_dirty(self.layout):
                self.relayout_fn()

        finally:
            self._processing = False
            self._state_changed = False

    def _targeted_update(self, state_obj):

        changed = self._program_module.VectorTStringT()
        self._program_module.diff(self._last_state, state_obj, changed)
        num_changed = changed.size()
        if  num_changed == 0:
            return
        for i in range(num_changed):
            path_str = _rlc_string_to_python(changed.get(i).contents)
            sim_path = tuple(
                int(p) if p.lstrip('-').isdigit() else p
                for p in path_str.split('.')
                if p
            )
            if sim_path:
                self._apply_path(state_obj, sim_path)

        self._last_state = _copy_state(state_obj)
        self._needs_relayout = True

    def _apply_path(self, state_obj, sim_path):
        if sim_path[-1] == "#":
            node = self.get_ui_path(self.layout, sim_path[:-1])
            if node is not None and getattr(node, "update_fn", None):
                node.update_fn(resolve_value(state_obj, sim_path[:-1]))
            return
        prefix = sim_path
        while prefix:
            node = self.get_ui_path(self.layout, prefix)
            if node is not None and getattr(node, "update_fn", None):
                node.update_fn(resolve_value(state_obj, prefix))
                return
            prefix = prefix[:-1]

    def get_ui_path(self, layout, sim_path : tuple) :
        node = layout
        for seg in sim_path:
            cm = getattr(node, "children_mapping", None)
            node = cm.get(seg) if cm else None
            if node is None:
                return None
        return node
        
    def notify_state_changed(self):
        self._state_changed = True

    def _handle_signal(self, signal: UpdateSignal):
        if signal.kind == SignalKind.ACTION:
            changed = self.dispatch_fn(signal.handler_name, signal.args)
            if changed:
                self._state_changed = True

        elif signal.kind == SignalKind.FOCUS:
            self.dispatch_fn(signal.handler_name, signal.args)
            self._needs_relayout = True

        elif signal.kind == SignalKind.RESIZE:
            self._needs_relayout = True

        elif signal.kind == SignalKind.SCROLL:
            self.scroll["x"] = min(0, self.scroll["x"] + signal.dx)
            self.scroll["y"] = min(0, self.scroll["y"] + signal.dy)
            self._needs_relayout = True


def _any_child_dirty(layout):
    if getattr(layout, "is_dirty", False):
        layout.is_dirty = False
        return True
    return any(
        _any_child_dirty(c)
        for c in layout.children
        if hasattr(c, "children")
    )


def _refresh_visibility(layout, state_obj):
    changed = False
    predicate = getattr(layout, "_visible_when", None)
    if predicate is not None:
        collapsed = not predicate(state_obj)
        if collapsed != getattr(layout, "collapsed", False):
            layout.collapsed = collapsed
            changed = True
    for child in getattr(layout, "children", []):
        if _refresh_visibility(child, state_obj):
            changed = True
    return changed

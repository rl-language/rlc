from typing import Optional, List, Dict, Any
import time
import itertools
from dataclasses import asdict, dataclass
import json
import os
import tempfile

@dataclass
class LayoutLogConfig:
    include_timestamp: bool = True
    precision: int = 2
    record_events: bool = True  #lifecycle events (before/after compute/layout)
    record_final_tree: bool = True #final tree snapshot

class LayoutLogger:

    _id_counter = itertools.count(1)

    def __init__(self, config: Optional[LayoutLogConfig] = None):
        self.config = config or LayoutLogConfig()
        self._events: List[Dict[str, Any]] = []
        self._node_meta: Dict[int, Dict[str, Any]] = {} #stable identity & static metadata
        self._final_tree: Optional[Dict[str, Any]] = None
        self._start = time.time()

    def attach_id(self, node) -> int:
        nid = getattr(node, "_log_node_id", None)
        if nid is None:
            nid = next(self._id_counter)
            setattr(node, "_log_node_id", nid)
            self._node_meta[nid] = {
                "type" : type(node).__name__
            }
        return nid
    
    def _ts(self) -> float:
        return round(time.time() - self._start, 6) if self.config.include_timestamp else None
    
    def _pad_dict(self, pad) -> Optional[Dict[str, Any]]:
        if pad is None: return None
        return {"top": pad.top,
                "right": pad.right,
                "bottom": pad.bottom,
                "left": pad.left}
    
    def _preview(self, s:str) -> str:
        if not s:
            return ""
        s = s.replace("\n", " ")
        return s 
    
    def _pad_str(self, pad) -> str:
        if pad is None: return "-"
        return f"t{pad.top}/r{pad.right}/b{pad.bottom}/l{pad.left}"
    
    def snapshot(self, node, stage: str,
                 axis: Optional[str] = None,
                 warnings: Optional[str] = None,
                 extra: Optional[Dict[str, Any]] = None) -> None:
        if not self.config.record_events:
            return
        nid = self.attach_id(node)
        e = {
            "timestamp" : self._ts(),
            "node_id" : nid,
            "stage" : stage,
            "position": {"x": getattr(node, "x", None), "y": getattr(node, "y", None)},
            "size": {"w": getattr(node, "width", None), "h": getattr(node, "height", None)},
            "sizing_policy": {
                "width": getattr(node, "sizing", (None, None))[0].mode.value if getattr(node, "sizing", None) else None,
                "height": getattr(node, "sizing", (None, None))[1].mode.value if getattr(node, "sizing", None) else None
                },
            "padding": self._pad_dict(getattr(node, "padding", None)),
            "direction": getattr(getattr(node, "direction", None), "value", None),
            "axis" : axis,
            "warnings" : warnings,
            "extra": extra or {}
        }
        self._events.append(e)

    def record_final_tree(self, root) -> None:
        if not self.config.record_final_tree:
            return
        self._final_tree = self._tree_dict(root)

    def _safe_write_json(self, path: str, data, indent: int = 2) -> None:
        os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
        # Atomic write: write to temp file, then replace
        dir_name = os.path.dirname(path) or "."
        fd, tmp = tempfile.mkstemp(prefix=".tmp-", dir=dir_name, text=True)
        try:
            with os.fdopen(fd, "w", encoding="utf-8", newline="\n") as f:
                json.dump(data, f, indent=indent, ensure_ascii=False)
                f.write("\n")
            os.replace(tmp, path)
        except Exception:
            # Clean up temp on failure
            try:
                os.remove(tmp)
            except OSError:
                pass
            raise
    def _safe_write_text(self, path: str, text: str) -> None:
        os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
        dir_name = os.path.dirname(path) or "."
        fd, tmp = tempfile.mkstemp(prefix=".tmp-", dir=dir_name, text=True)
        try:
            with os.fdopen(fd, "w", encoding="utf-8", newline="\n") as f:
                f.write(text)
                if not text.endswith("\n"): f.write("\n")
            os.replace(tmp, path)
        except Exception:
            try:
                os.remove(tmp)
            except OSError:
                pass
            raise

    def to_json(self, indent: int = 2) -> str:
        data = {
            "config" : asdict(self.config),
            "events" : self._events,
            "final_tree" : self._final_tree
        }
        return json.dumps(data, indent)
    
    def to_text_tree(self, root):
        lines : List[str] = []
        def rec(node, depth: int):
            nid = self.attach_id(node)
            pad = getattr(node, "padding", None)
            sizing = getattr(node, "sizing", (None, None))
            wmode = sizing[0].mode.value if sizing and sizing[0] else "?"
            hmode = sizing[1].mode.value if sizing and sizing[1] else "?"
            dirv  = getattr(getattr(node, "direction", None), "value", "-")
            text = ""
            if type(node).__name__ == "Text":
                txt = getattr(node, "text", "")
                if txt is not None:
                    text = f' text="{self._preview(txt)}"'
            branch = "|___"
            indent = "    " * (depth) + (branch if depth > 0 else branch)
            lines.append(
                f'{indent}.{type(node).__name__}#{nid} '
                f'[{wmode},{hmode}; direction:{dirv}] '
                f'position=({getattr(node,"x",None)}, {getattr(node, "y", None)}) '
                f'size=({getattr(node, "width", None)}, {getattr(node, "height", None)}) '
                f'padding={self._pad_str(pad)}{text}'
            )
            for child in getattr(node, "children", []) or []:
                rec(child, depth+1)
        rec(root, 0)
        return "\n".join(lines)
    
    def write_json(self, path: str, indent: int = 2):
        data = {
            "config" : asdict(self.config),
            "events" : self._events,
            "final_tree" : self._final_tree
        }
        self._safe_write_json(path, data, indent)

    def write_text_tree(self, root, path: str):
        txt = self.to_text_tree(root)
        self._safe_write_text(path, txt)

    def _tree_dict(self, node) -> Dict[str, Any]:
        nid = self.attach_id(node)
        sizing = getattr(node, "sizing", (None, None))
        wm = sizing[0].mode.value if sizing[0] else None
        hm = sizing[1].mode.value if sizing[1] else None
        d : Dict[str, Any] = {
            "node_id" : nid,
            "type": type(node).__name__,
            "direction": getattr(getattr(node, "direction", None), "value", None),
            "position": {"x": getattr(node, "x", None), "y": getattr(node, "y", None)},
            "size": {"w": getattr(node, "width", None), "h": getattr(node, "height", None)},
            "sizing_policy": {
                "width": wm,
                "height": hm
                },
            "padding": self._pad_dict(getattr(node, "padding", None))
        }
        if type(node).__name__ == "Text":
            d["text_preview"] = self._preview(getattr(node, "text", ""))
            surfaces = getattr(node, "text_surfaces", None)
            if surfaces is not None and hasattr(surfaces, "__len__"):
                d["line_count"] = len(surfaces)
        children = getattr(node, "children", None)
        if children:
            d["children"] = [self._tree_dict(c) for c in children]
        return d


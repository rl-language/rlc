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
        self._root_ref = None

    def __del__(self):
        """Destructor: clean up node IDs when logger is garbage-collected."""
        try:
            if self._root_ref:
                self.finalize(self._root_ref)
        except Exception:
            pass

    def _attach_id(self, node) -> int:
        nid = getattr(node, "_log_node_id", None)
        if nid is None:
            nid = next(self._id_counter)
            setattr(node, "_log_node_id", nid)
            self._node_meta[nid] = {
                "type" : type(node).__name__
            }
        return nid
    
    def _remove_id(self, node):
        nid = getattr(node, "_log_node_id", None)
        if nid is not None:
            self._node_meta.pop(nid, None)  # safe remove
            if hasattr(node, "_log_node_id"):
                delattr(node, "_log_node_id")
 
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
    
    def _extract_node_info(self, node) -> Dict[str, Any]:
        sizing = getattr(node, "sizing", (None, None))
        wmode = sizing[0].mode.value if sizing and sizing[0] else None
        hmode = sizing[1].mode.value if sizing and sizing[1] else None
        return {
            "id" : self._attach_id(node),
            "type": type(node).__name__,
            "position": {"x": getattr(node, "x", None), "y": getattr(node, "y", None)},
            "size": {"w": getattr(node, "width", None), "h": getattr(node, "height", None)},
            "sizing_policy": {
                "width": wmode,
                "height": hmode
                },
            "padding": self._pad_dict(getattr(node, "padding", None)),
            "direction": getattr(getattr(node, "direction", None), "value", None)
        }
    
    def _safe_write_json(self, path: str, data, indent: int = 2) -> None:
        os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
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

    def snapshot(self, node, stage: str,
                 axis: Optional[str] = None,
                 warnings: Optional[str] = None,
                 extra: Optional[Dict[str, Any]] = None) -> None:
        if not self.config.record_events:
            return
        info = self._extract_node_info(node)
        e = {
            "timestamp" : self._ts(),
            "node_id" : info['id'],
            "stage" : stage,
            "position": info['position'],
            "size": info['size'],
            "sizing_policy": info['sizing_policy'],
            "padding": info['padding'],
            "direction": info['direction'],
            "axis" : axis,
            "warnings" : warnings,
            "extra": extra or {}
        }
        self._events.append(e)

    def record_final_tree(self, root) -> None:
        if not self.config.record_final_tree:
            return
        self._final_tree = self._tree_dict(root)
        self._root_ref = root

    def finalize(self, root) -> None:
        """Remove all node IDs and metadata after logging is finished."""
        def rec(node):
            self._remove_id(node)
            for child in getattr(node, "children", []) or []:
                rec(child)
        rec(root)
        self._node_meta.clear()

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
            info = self._extract_node_info(node)
            pad = getattr(node, "padding", None)
            text = ""
            if info['type'] == "Text":
                txt = getattr(node, "text", "")
                if txt is not None:
                    text = f' text="{self._preview(txt)}"'
            branch = "|___"
            indent = "    " * (depth) + (branch if depth > 0 else branch)
            lines.append(
                f'{indent}.{info["type"]}#{info["id"]} '
                f'[({info["sizing_policy"]["width"]}, {info["sizing_policy"]["height"]}); direction:{info["direction"]}] '
                f'position=({info["position"]["x"]}, {info["position"]["y"]}) '
                f'size=({info["size"]["w"]}, {info["size"]["h"]}) '
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
        nid = self._attach_id(node)
        sizing = getattr(node, "sizing", (None, None))
        wm = sizing[0].mode.value if sizing[0] else None
        hm = sizing[1].mode.value if sizing[1] else None
        info = self._extract_node_info(node)
        d : Dict[str, Any] = {
            "node_id" : info['id'],
            "type": info['type'],
            "backgroundColor": getattr(node, "backgroundColor", (220, 220, 220)),
            "direction": info['direction'],
            "position": info['position'],
            "size": info['size'],
            "sizing_policy": info['sizing_policy'],
            "padding": info['padding']
        }
        if info['type'] == "Text":
            d["text_preview"] = self._preview(getattr(node, "text", ""))
            surfaces = getattr(node, "text_surfaces", None)
            if surfaces is not None and hasattr(surfaces, "__len__"):
                d["line_count"] = len(surfaces)
        children = getattr(node, "children", None)
        if children:
            d["children"] = [self._tree_dict(c) for c in children]
        return d
    



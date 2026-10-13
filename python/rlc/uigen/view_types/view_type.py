
from abc import ABC, abstractmethod
from rlc.uigen.layout import Layout, Direction, FIT, Padding
from typing import Dict
from dataclasses import dataclass, field, fields, is_dataclass, MISSING
from rlc.uigen.text import Text
import yaml
from rlc.uigen.view_types.interaction_context import InteractionMapping

_renderer_registry = {} 


def get_field(obj, name):
    if isinstance(obj, dict):
        return obj.get(name)
    return getattr(obj, name, None)


def has_field(obj, name) -> bool:
    if isinstance(obj, dict):
        return name in obj
    return hasattr(obj, name)


def seq_size(obj) -> int:
    if isinstance(obj, list):
        return len(obj)
    s = getattr(obj, "_size", None)
    if s is not None:
        return s.value if hasattr(s, "value") else s
    n = getattr(obj, "_length_", None)
    if n is not None:
        return n
    data = getattr(obj, "_data", None)
    if data is not None:
        return seq_size(data)
    return 0


def seq_at(obj, i):
    if isinstance(obj, list):
        return obj[i]
    # ctypes Vector: data lives in `_data`.
    data = getattr(obj, "_data", None)
    if data is not None and not isinstance(data, list):
        return seq_at(data, i)
    return obj[i]


def _is_rlc_string(obj) -> bool:
    return (type(obj).__name__ == "String"
            and hasattr(obj, "_data")
            and hasattr(obj._data, "_size"))


def _decode_rlc_string(obj) -> str:
    vec = obj._data
    size = vec._size if isinstance(vec._size, int) else vec._size.value
    data = vec._data
    chars = []
    for i in range(size):
        b = data[i] if isinstance(data[i], int) else data[i].value
        if b == 0:
            break
        chars.append(b)
    return bytes(chars).decode("ascii", errors="replace")


def to_display_str(obj) -> str:
    if obj is None:
        return ""
    if isinstance(obj, dict):
        inner = obj.get("value")
        if inner is not None:
            return to_display_str(inner)
    if _is_rlc_string(obj):
        return _decode_rlc_string(obj)
    inner = getattr(obj, "value", None)
    if inner is not None and inner is not obj:
        return to_display_str(inner)
    if isinstance(obj, bool):
        return "True" if obj else "False"
    if isinstance(obj, int):
        return str(obj)
    return str(obj)

class ViewTypeDumper(yaml.SafeDumper):
    index = 0
    def generate_anchor(self, node: yaml.Node):

        self.index = self.index + 1
        return str(self.index)

class ViewTypeLoader(yaml.FullLoader):
    pass

def renderable_representer(dumper: ViewTypeDumper, obj: 'ViewType'):
    tag = obj.yaml_tag()
    mapping = []
    for f in fields(obj):
        if f.name.startswith('_'):
            continue
        value = getattr(obj, f.name)

        if f.name == "interaction_mappings":
            if value: 
                mappings_as_dicts = [
                    {
                        "event_type": m.event_type,
                        "handler_name": m.handler_name,
                        "param_vars": m.param_vars,
                        "path": m.path
                    }
                    for m in value
                ]
                mapping.append((f.name, mappings_as_dicts))
            continue

        if f.default is not MISSING and value == f.default:
            continue

        if f.default_factory is not MISSING:
            try:
                default = f.default_factory()
                if value == default:
                    continue
            except TypeError:
                pass

        mapping.append((f.name, value))
    return dumper.represent_mapping(tag, mapping)


def renderable_multi_constructor(loader: ViewTypeLoader, tag_suffix: str, node):
    """
    tag_suffix is the part after the '!' when using add_multi_constructor("!", ...")
    e.g. YAML tag `!FooRenderer` → tag_suffix == "FooRenderer"
    """
    cls = _renderer_registry[tag_suffix] 
    data = loader.construct_mapping(node, deep=True)

    interaction_mappings = None
    if "interaction_mappings" in data:
        mappings_data = data.pop("interaction_mappings")  

        interaction_mappings = []
        for m in mappings_data:
            index_vars = [seg[1:] for seg in m["path"] if isinstance(seg, str) and seg.startswith('$')]

            interaction_mappings.append(InteractionMapping(
                event_type=m["event_type"],
                handler_name=m["handler_name"],
                index_vars=index_vars,
                param_vars=m["param_vars"],
                path=m["path"]
            ))

    instance = cls(**data)

    if interaction_mappings is not None:
        instance.interaction_mappings = interaction_mappings

    return instance

yaml.add_multi_constructor("!", renderable_multi_constructor, Loader=ViewTypeLoader)

def register_renderer(cls):
    _renderer_registry[cls.__name__] = cls
    return cls

@dataclass
class ViewType(ABC):
    rlc_type_name: str
    interaction_mappings: list = field(default_factory=list, repr=False, compare=False, init=False)
    _layout: object = field(default=None, repr=False, compare=False, init=False)

    def make_layout(self, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2), border=3, child_gap=5) -> Layout:
        layout = Layout(sizing=sizing, direction=direction, color=color, padding=padding, border=border, child_gap=child_gap)

        return layout

    def make_text(self, txt, font_name, font_size, color) -> Text:
        text = Text(txt, font_name, font_size, color)
        return text

    @abstractmethod
    def build_layout(self, obj, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)) -> Layout:
        pass

    def _get_deepest_interaction_mappings(self):
        if self.interaction_mappings:
            return self.interaction_mappings

        for child in self._iter_children():
            child_mappings = child._get_deepest_interaction_mappings()
            if child_mappings:
                return child_mappings

        return []

    def _apply_interaction_mappings(self, layout, index_bindings=None):
        if index_bindings is None:
            index_bindings = {}

        for mapping in self.interaction_mappings:
            metadata = {
                "handler": mapping.handler_name,
                "args": index_bindings.copy(),  
                "params": mapping.param_vars 
            }

            if mapping.event_type == "on_click":
                layout.on_click = metadata
            elif mapping.event_type == "on_key":
                layout.on_key = metadata
            elif mapping.event_type == "on_hover":
                layout.on_hover = metadata
            else:
                setattr(layout, mapping.event_type, metadata)

            layout.interactive = True

    def update(self, layout, obj):
        pass

    def on_changed(self, obj, *args):
        if hasattr(obj, "contents"):
            obj = obj.contents
        if not self._layout:
            return

        if not args:
            if self._layout.update_fn:
                self._layout.update_fn(obj)
                self._layout.is_dirty = True
            return

        node = self._layout
        value = obj

        for arg in args:
            cm = getattr(node, "children_mapping", None)
            if cm is None:
                if self._layout.update_fn:
                    self._layout.update_fn(obj)
                    self._layout.is_dirty = True
                return
            if isinstance(arg, str) and arg in cm:
                node = cm[arg]
                value = getattr(value, arg, None)
            elif isinstance(arg, int) and arg in cm:
                node = cm[arg]
                value = value[arg]
            else:
                if self._layout.update_fn:
                    self._layout.update_fn(obj)
                    self._layout.is_dirty = True
                return

        if getattr(node, "update_fn", None):
            node.update_fn(value)
            node.is_dirty = True

    def __call__(self, obj, parent_path=None, **kwds):
        if parent_path is None:
            current_path = [self.rlc_type_name]
        else:
            current_path = list(parent_path)

        layout = self.build_layout(obj=obj, parent_path=current_path, **kwds)
        self._layout = layout

        return layout

    def post_order_types(self):
        frontier = [self]
        seen = set()
        output = []
        while len(frontier) != 0:
            current = frontier.pop(0)
            if id(current) in seen:
                continue
            output.append(current)
            seen.add(id(current))
            if current is not None:
                for child in current._iter_children():
                    frontier.append(child)
        return [x for x in reversed(output)]


    def to_yaml(self):
        return yaml.dump(self.post_order_types(), Dumper=ViewTypeDumper, sort_keys=False)

    @classmethod
    def from_yaml(cls, yaml_text):
        return yaml.load(yaml_text, Loader=ViewTypeLoader)[-1]

    @classmethod
    def yaml_tag(cls) -> str:
        return f"!{cls.__name__}"

    def _iter_children(self):
        return []

yaml.add_multi_representer(ViewType, renderable_representer, Dumper=ViewTypeDumper)

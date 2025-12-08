
from abc import ABC, abstractmethod
from copy import deepcopy
from rlc.layout import Layout, Direction, FIT, Padding
from typing import Dict
from dataclasses import dataclass, field, fields, is_dataclass, MISSING
from rlc.text import Text
import yaml

_renderer_registry = {}  # maps class name → class

class RenderableDumper(yaml.SafeDumper):
    index = 0
    def generate_anchor(self, node: yaml.Node):

        self.index = self.index + 1
        return str(self.index)

class RenderableLoader(yaml.FullLoader):
    pass

def renderable_representer(dumper: RenderableDumper, obj: 'Renderable'):
    tag = obj.yaml_tag()
    mapping = []
    for f in fields(obj):
        value = getattr(obj, f.name)

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


def renderable_multi_constructor(loader: RenderableLoader, tag_suffix: str, node):
    """
    tag_suffix is the part after the '!' when using add_multi_constructor("!", ...)
    e.g. YAML tag `!FooRenderer` → tag_suffix == "FooRenderer"
    """
    cls = _renderer_registry[tag_suffix]  # look up the class
    data = loader.construct_mapping(node, deep=True)
    return cls(**data)

yaml.add_multi_constructor("!", renderable_multi_constructor, Loader=RenderableLoader)

def register_renderer(cls):
    _renderer_registry[cls.__name__] = cls
    return cls

@dataclass
class Renderable(ABC):
    """
    Base abstract renderer type.
    Each subclasss knows how to convert its types object into a Layout tree.
    """

    def make_layout(self, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2), border=3, child_gap=5) -> Layout:
        layout = Layout(sizing=sizing, direction=direction, color=color, padding=padding, border=border, child_gap=child_gap)

        return layout

    def make_text(self, txt, font_name, font_size, color) -> Text:
        text = Text(txt, font_name, font_size, color)
        return text

    @abstractmethod
    def build_layout(self, obj, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)) -> Layout:
        """Construct and return a Layout tree for the given object."""
        pass

    def apply_interactivity(self, layout_child, index=None, parent_obj=None):
        pass

    def update(self, layout, obj, elapsed_time: float = 0.0):
        """
        Update the existing layout tree in place using new data from obj.
        Optionally, use elapsed_time for animations (interpolation).
        """
        pass

    def __call__(self, obj, parent_binding=None, **kwds):
        layout = self.build_layout(obj=obj, **kwds)

        # If build_layout didn't set binding, ensure we add it
        if layout.binding is None:
            layout.binding = {
                "parent": parent_binding,
            }
        else:
            layout.binding["parent"] = parent_binding

        # # Pass our binding to children
        for child in layout.children:
            # Only set if child doesn't already have binding
            if child.binding is None:
                child.binding = {
                    "type": "unknown",
                    "parent": layout.binding
                }
            else:
                child.binding["parent"] = layout.binding

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
            for child in current._iter_children():
                frontier.append(child)
        return [x for x in reversed(output)]


    def to_yaml(self):
        return yaml.dump(self.post_order_types(), Dumper=RenderableDumper, sort_keys=False)

    @classmethod
    def from_yaml(cls, yaml_text):
        return yaml.load(yaml_text, Loader=RenderableLoader)[-1]

    @classmethod
    def yaml_tag(cls) -> str:
        return f"!{cls.__name__}"

    def _iter_children(self):
        """Return iterable of child renderers, if any. Override per subclass."""
        return []

yaml.add_multi_representer(Renderable, renderable_representer, Dumper=RenderableDumper)

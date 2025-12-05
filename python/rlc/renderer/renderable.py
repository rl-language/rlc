
from abc import ABC, abstractmethod
from rlc.layout import Layout, Direction, FIT, Padding
from typing import Dict
from dataclasses import dataclass, field
from rlc.text import Text

_renderer_registry = {}  # maps class name → class

def register_renderer(cls):
    _renderer_registry[cls.__name__] = cls
    return cls

@dataclass(kw_only=True)
class Renderable(ABC):
    """
    Base abstract renderer type.
    Each subclasss knows how to convert its types object into a Layout tree.
    """
    rlc_type_name: str
    style_policy: Dict = field(default_factory=dict)

    def make_layout(self, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2), border=3, child_gap=5) -> Layout:
        layout = Layout(sizing=sizing, direction=direction, color=color, padding=padding, border=border, child_gap=child_gap)

        for attr, value in self.style_policy.items():
            if value is not None and hasattr(layout, attr):
                setattr(layout, attr, value)

        return layout

    def make_text(self, txt, font_name, font_size, color) -> Text:
        text = Text(txt, font_name, font_size, color)

        for attr, value in self.style_policy.items():
            if value is not None and hasattr(text, attr):
                setattr(text, attr, value)

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
                "type": self.rlc_type_name,
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


    def _iter_children(self):
        """Return iterable of child renderers, if any. Override per subclass."""
        return []

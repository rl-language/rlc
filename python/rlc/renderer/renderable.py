
from abc import ABC, abstractmethod
from rlc.layout import Layout, Direction, FIT, Padding
from rlc.text import Text

_renderer_registry = {}  # maps class name → class

def register_renderer(cls):
    _renderer_registry[cls.__name__] = cls
    return cls

class Renderable(ABC):
    """
    Base abstract renderer type.
    Each subclasss knows how to convert its types object into a Layout tree.
    """
    def __init__(self, rlc_type_name, style_policy : dict): 
        self.rlc_type_name = rlc_type_name
        self.style_policy = style_policy

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

    def update(self, layout, obj, elapsed_time: float = 0.0):
        """
        Update the existing layout tree in place using new data from obj.
        Optionally, use elapsed_time for animations (interpolation).
        """
        pass

    def __call__(self, obj, **kwds):
        """Allow calling the object directly"""
        return self.build_layout(obj=obj, **kwds)
    

    # ---------- PUBLIC SERIALIZATION API ----------
    def to_dict(self):
        return {
            "renderer_class": self.__class__.__name__,
            "rlc_type_name": self.rlc_type_name,
            "data": self._to_dict_data(),
            "style_policy" : self.style_policy
        }

    @staticmethod
    def from_dict(d):
        cls_name = d["renderer_class"]
        if cls_name not in _renderer_registry:
            raise ValueError(f"Unknown renderer class '{cls_name}'")
        cls = _renderer_registry[cls_name]
        return cls._from_dict_data(
            d["rlc_type_name"],
            d["data"]
        )
    
    # ---------- SUBCLASS HOOKS ----------
    @abstractmethod
    def _to_dict_data(self):
        """Return subclass-specific dict data."""
        pass

    @classmethod
    @abstractmethod
    def _from_dict_data(cls, rlc_type_name, data):
        """Recreate subclass instance."""
        pass
    
    def print_tree(self, indent: int = 0):
        prefix = "  " * indent
        print(f"{prefix}{self.__class__.__name__}({self._describe_self()})")
        for child in self._iter_children():
            if isinstance(child, Renderable):
                child.print_tree(indent + 1)
            else:
                print(f"{'  ' * (indent + 1)}{child}")

    def _describe_self(self) -> str:
        """Subclasses can override to show additional info."""
        return self.rlc_type_name

    def _iter_children(self):
        """Return iterable of child renderers, if any. Override per subclass."""
        return []
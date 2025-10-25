
from abc import ABC, abstractmethod
from typing import Optional
from rlc.layout import Layout, Direction, FIT, Padding
from rlc.renderer_backend import RendererBackend

class Renderable(ABC):
    """
    Base abstract renderer type.
    Each subclasss knows how to convert its types object into a Layout tree.
    """
    def __init__(self, rlc_type, backend: Optional[RendererBackend] = None):
        self.rlc_type = rlc_type
        self.backend = backend

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
        return getattr(self.rlc_type, "__name__", str(self.rlc_type))

    def _iter_children(self):
        """Return iterable of child renderers, if any. Override per subclass."""
        return []
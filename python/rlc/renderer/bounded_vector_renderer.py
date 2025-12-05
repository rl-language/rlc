from rlc.text import Text
from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import  Direction, FIT, Padding
from dataclasses import dataclass

@register_renderer
@dataclass
class BoundedVectorRenderer(Renderable):
    """
    Renderer for bounded integer structs like BIntT1T10T.
    """
    vector_renderer: Renderable

    def build_layout(self, obj, direction=Direction.COLUMN,
                     color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        value = getattr(obj, "_data", None)
        value_layout = self.vector_renderer(value)
        return value_layout

    def update(self, layout, obj, elapsed_time=0.0):
        value = getattr(obj, "_data")
        self.vector_renderer.update(layout, value, elapsed_time)

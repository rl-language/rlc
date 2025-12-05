from rlc.text import Text
from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import  Direction, FIT, Padding
from dataclasses import dataclass

@register_renderer
class BoundedIntRenderer(Renderable):
    """
    Renderer for bounded integer structs like BIntT1T10T.
    """
    def build_layout(self, obj, direction=Direction.COLUMN,
                     color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        # Extract the 'value' field (the inner c_long)
        value = getattr(obj, "value", None)
        val_str = str(value if isinstance(value, int) else getattr(value, "value", value))

        layout = self.make_text(val_str, "Arial", 16, "black")
        layout.binding = {
            "type": "bounded_int",
            "obj": obj
        }
        return layout

    def update(self, layout, obj, elapsed_time=0.0):
        if isinstance(layout, Text):
            value = getattr(obj, "value", None)
            new_val = str(value if isinstance(value, int) else getattr(value, "value", value))
            layout.update_text(new_val)

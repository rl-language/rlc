
from rlc.renderer.renderable import Renderable, register_renderer
from ctypes import c_long, c_bool
from rlc.text import Text
from rlc.layout import  Direction, FIT, Padding
import time
from dataclasses import dataclass

@register_renderer
@dataclass
class PrimitiveRenderer(Renderable):
    def build_layout(self, obj, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        if self.rlc_type_name == "c_bool":
            text = "True" if obj else "False"
        if self.rlc_type_name == "c_long":
            text = str(obj if isinstance(obj, int) else obj.value)
        else:
            text = str(obj)

        layout = self.make_text(text, "Arial", 16, "black")
        layout.binding = {
            "type": "primitive",
            "value": obj
        }

        return layout

    def update(self, layout, obj, elapsed_time=0.0):
        """Update the text node if the value changed."""
        if isinstance(layout, Text):
            new_value = self._extract_value(obj)
            layout.update_text(new_value)

    def _extract_value(self, obj):
        if self.rlc_type_name == "c_bool":
            text = "True" if obj else "False"
        if self.rlc_type_name == "c_long":
            text = str(obj if isinstance(obj, int) else obj.value)
        else:
            text = str(obj)
        return text


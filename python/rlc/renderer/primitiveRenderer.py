
from rlc.renderer.renderable import Renderable
from ctypes import c_long, c_bool
from rlc.text import Text
from rlc.layout import  Direction, FIT, Padding
import time

class PrimitiveRenderer(Renderable):
    def __init__(self, rlc_type, backend = None):
        super().__init__(rlc_type, backend)

    def _iter_children(self):
        return []

    def build_layout(self, obj, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        if self.rlc_type == c_bool:
            text = "True" if obj else "False"
        if self.rlc_type == c_long:
            text = str(obj if isinstance(obj, int) else obj.value)
        else:
            text = str(obj)

        return Text(text, "Arial", 36, "black")
    
    def update(self, layout, obj, elapsed_time=0.0):
        """Update the text node if the value changed."""
        if isinstance(layout, Text):
            new_value = self._extract_value(obj)
            layout.update_text(new_value)

    def _extract_value(self, obj):
        if self.rlc_type == c_bool:
            text = "True" if obj else "False"
        if self.rlc_type == c_long:
            text = str(obj if isinstance(obj, int) else obj.value)
        else:
            text = str(obj)
        return text
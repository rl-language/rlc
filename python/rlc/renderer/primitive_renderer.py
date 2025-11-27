
from rlc.renderer.renderable import Renderable, register_renderer
from ctypes import c_long, c_bool
from rlc.text import Text
from rlc.layout import  Direction, FIT, Padding
import time

@register_renderer
class PrimitiveRenderer(Renderable):
    def __init__(self, rlc_type_name, style_policy):
        super().__init__(rlc_type_name, style_policy)
        self.style_policy = style_policy

    def _iter_children(self):
        return []

    def build_layout(self, obj, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        if self.rlc_type_name == "c_bool":
            text = "True" if obj else "False"
        if self.rlc_type_name == "c_long":
            text = str(obj if isinstance(obj, int) else obj.value)
        else:
            text = str(obj)

        return self.make_text(text, "Arial", 16, "black")
    
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
    
    def _to_dict_data(self):
        return {
            "style_policy" : self.style_policy
        }  

    @classmethod
    def _from_dict_data(cls, rlc_type_name, data):
        return cls(rlc_type_name, data["style_policy"])
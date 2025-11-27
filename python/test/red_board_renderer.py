from rlc.renderer.struct_renderer import ContainerRenderer
from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import Layout, Direction, FIT, Padding
from rlc.text import Text

@register_renderer
class RedBoard(ContainerRenderer):

    def __init__(self, rlc_type_name, field_renderers, style_policy):
        super().__init__("RedBoard", field_renderers, style_policy)

    def build_layout(self, obj, direction=Direction.ROW, color='red', sizing=(FIT(), FIT()), logger=None, padding=Padding(7,7,7,7)):
        return super().build_layout(obj, direction, color, sizing, logger, padding)
    
    def _to_dict_data(self):
        return {
            "fields": {k: v.to_dict() for k, v in self.field_renderers.items()},
            "style_policy" : self.style_policy
        }

    @classmethod
    def _from_dict_data(cls, rlc_type_name, data):
        fields = {
            k: Renderable.from_dict(v)
            for k, v in data["fields"].items()
        }
        return cls(rlc_type_name, fields, data["style_policy"])
    
from rlc.renderer.struct_renderer import ContainerRenderer
from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import Layout, Direction, FIT, Padding
from rlc.text import Text
from dataclasses import dataclass

@register_renderer
@dataclass
class RedBoard(ContainerRenderer):
    def build_layout(self, obj, direction=Direction.ROW, color='red', sizing=(FIT(), FIT()), logger=None, padding=Padding(7,7,7,7)):
        return super().build_layout(obj, direction, color, sizing, logger, padding)


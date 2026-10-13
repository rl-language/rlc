from rlc.uigen.view_types.view_type import ViewType, register_renderer, to_display_str
from rlc.uigen.text import Text
from rlc.uigen.layout import Direction, FIT, Padding
from dataclasses import dataclass


@register_renderer
@dataclass
class PrimitiveViewType(ViewType):
    def build_layout(self, obj, parent_path, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2), index_bindings=None):
        if index_bindings is None:
            index_bindings = {}

        layout = self.make_text(to_display_str(obj), "Arial", 16, "black")
        layout.render_path = parent_path
        layout.update_fn = lambda v: self.update(layout, v)
        self._apply_interaction_mappings(layout, index_bindings)
        return layout

    def update(self, layout, obj):
        if isinstance(layout, Text):
            layout.update_text(to_display_str(obj))


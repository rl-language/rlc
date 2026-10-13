from rlc.uigen.view_types.view_type import ViewType, register_renderer
from rlc.uigen.layout import  Direction, FIT, Padding
from dataclasses import dataclass

@register_renderer
@dataclass
class BoundedVectorViewType(ViewType):
    vector_view_type: ViewType

    def build_layout(self, obj, parent_path, direction=Direction.COLUMN,
                     color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2), index_bindings=None):
        if index_bindings is None:
            index_bindings = {}

        value = obj if isinstance(obj, list) else getattr(obj, "_data", None)

        return self.vector_view_type(
            value,
            parent_path=parent_path,
            direction=direction,
            color=color,
            sizing=sizing,
            logger=logger,
            padding=padding,
            index_bindings=index_bindings,
        )

    def update(self, layout, obj):
        value = obj if isinstance(obj, list) else getattr(obj, "_data")
        self.vector_view_type.update(layout, value)

    def on_changed(self, obj, *args):
        if hasattr(obj, "contents"):
            obj = obj.contents
        if self._layout:
            self.update(self._layout, obj)

    def _iter_children(self) :
        return [self.vector_view_type]

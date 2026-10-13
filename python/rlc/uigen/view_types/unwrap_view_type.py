from dataclasses import dataclass

from rlc.uigen.view_types.view_type import ViewType, register_renderer, get_field


@register_renderer
@dataclass
class UnwrapViewType(ViewType):

    field_name: str
    inner: ViewType

    def _unwrap(self, obj):
        if obj is None:
            return None
        if hasattr(obj, "contents"):
            obj = obj.contents
        inner = get_field(obj, self.field_name)
        return inner if inner is not None else obj

    def build_layout(self, obj, parent_path, **kwargs):
        layout = self.inner(self._unwrap(obj), parent_path=parent_path, **kwargs)
        layout.update_fn = lambda v: self.update(layout, v)
        layout.children_mapping[self.field_name] = layout
        return layout

    def update(self, layout, obj):
        self.inner.update(layout, self._unwrap(obj))

    def on_changed(self, obj, *args):
        if hasattr(obj, "contents"):
            obj = obj.contents
        if self._layout:
            self.update(self._layout, obj)

    def _iter_children(self):
        return [self.inner]

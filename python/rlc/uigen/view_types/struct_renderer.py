from rlc.uigen.view_types.view_type import ViewType, register_renderer, get_field, has_field
from rlc.uigen.layout import Layout, FIT, Direction, Padding
from rlc.uigen.text import Text
from dataclasses import dataclass


@register_renderer
@dataclass
class ContainerViewType(ViewType):
    field_renderers: dict  


    @staticmethod
    def create_fields(factory, rlc_type, rlc_path, config, interaction_ctx):

        fields = {}
        for fname, ftype in getattr(rlc_type, "_fields_", []):
            child_path = rlc_path + [fname]
            child_renderer = factory(ftype, config, interaction_ctx, child_path)
            if child_renderer is None:
                continue
            fields[fname] = (fname, child_renderer)
        return fields

    def build_layout(self, obj, parent_path, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(7,7,7,7), index_bindings=None):
        if index_bindings is None:
            index_bindings = {}

        layout = self.make_layout(sizing=sizing, direction=direction, child_gap=5, color=color, border=5, padding=padding)
        layout.binding = {"type": "struct"}
        layout.render_path = parent_path

        self._apply_interaction_mappings(layout, index_bindings)

        for display_name, field_data in self.field_renderers.items():
            if field_data is None:
                continue

            actual_field_name, field_renderer = field_data

            if not has_field(obj, actual_field_name):
                raise AttributeError(
                    f"Field '{actual_field_name}' missing on {type(obj).__name__}; "
                    f"display name '{display_name}'."
                )

            value = get_field(obj, actual_field_name)
            row_layout = self.make_layout(sizing=(FIT(), FIT()), direction=Direction.ROW, child_gap=5, color=None, border=5, padding=Padding(10,10,10,10))
            row_layout.render_path = parent_path
            label = self.make_text(display_name + ": ", "Arial", 16, "black")
            label.render_path = parent_path
            value_layout = field_renderer(
                value,
                parent_path=parent_path + [actual_field_name],
                index_bindings=index_bindings)
            row_layout.add_child(label)
            row_layout.add_child(value_layout)
            layout.children_mapping[actual_field_name] = value_layout
            layout.add_child(row_layout)
        layout.update_fn = lambda v : self.update(layout, v)
        return layout

    def update(self, layout, obj):
        for (display_name, field_data), child_layout in zip(self.field_renderers.items(), layout.children):
            if field_data is None:
                continue

            actual_field_name, field_renderer = field_data

            if not has_field(obj, actual_field_name):
                raise AttributeError(
                    f"Field '{actual_field_name}' missing on {type(obj).__name__}; "
                    f"display name '{display_name}'."
                )

            value = get_field(obj, actual_field_name)
            field_renderer.update(child_layout.children[-1], value)

    def __getattr__(self, name):
        if name != "field_renderers" and hasattr(self, "field_renderers"):
            fr = self.field_renderers
            if name in fr and fr[name] is not None:
                return fr[name][1]
            for field_data in fr.values():
                if field_data and field_data[0] == name:
                    return field_data[1]
        raise AttributeError(f"'{type(self).__name__}' has no field renderer '{name}'")

    def _iter_children(self):
        return [renderer for field_data in self.field_renderers.values() if field_data is not None for _, renderer in [field_data]]


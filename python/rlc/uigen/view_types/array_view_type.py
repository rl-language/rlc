from rlc.uigen.view_types.view_type import ViewType, register_renderer
from rlc.uigen.layout import Direction, FIT, Padding
from dataclasses import dataclass

@register_renderer
@dataclass
class ArrayViewType(ViewType):
    length: int
    element_renderer: ViewType

    def build_layout(self, obj, parent_path, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2), index_bindings=None):
        if index_bindings is None:
            index_bindings = {}

        layout = self.make_layout(sizing=sizing, direction=direction, color=color, padding=padding, border=3, child_gap=5)
        layout.binding = {"type": "array"}
        layout.render_path = parent_path

        self._apply_interaction_mappings(layout, index_bindings)

        color = 'lightgray'
        if self.element_renderer is not None:
            index_var_name = None
            deepest_mappings = self.element_renderer._get_deepest_interaction_mappings()
            if deepest_mappings:
                interaction_mapping = deepest_mappings[0]
                num_bound = len(index_bindings)
                if num_bound < len(interaction_mapping.index_vars):
                    index_var_name = interaction_mapping.index_vars[num_bound]

            for i in range(self.length):
                item = obj[i]
                next_dir = (
                    Direction.ROW if direction == Direction.COLUMN else Direction.COLUMN
                )
                child_index_bindings = index_bindings.copy()
                if index_var_name:
                    child_index_bindings[index_var_name] = i

                child = self.element_renderer(
                    item,
                    parent_path=parent_path + [i],
                    direction=next_dir,
                    logger=logger,
                    color='white',
                    sizing=(FIT(), FIT()),
                    padding=Padding(2,2,2,2),
                    index_bindings=child_index_bindings,
                )
                layout.children_mapping[i] = child
                layout.add_child(child)
            layout.update_fn = lambda v : self.update(layout, v)
        return layout

    def update(self, layout, obj):
        for i, child in enumerate(layout.children):
            item = obj[i]
            self.element_renderer.update(child, item)

    def _iter_children(self):
        return [self.element_renderer]


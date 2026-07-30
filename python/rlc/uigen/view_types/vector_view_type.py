from rlc.uigen.view_types.view_type import ViewType, register_renderer, seq_size, seq_at
from rlc.uigen.layout import Layout, Direction, FIT, Padding
from dataclasses import dataclass

@register_renderer
@dataclass
class VectorViewType(ViewType):
    element_renderer: ViewType

    def build_layout(self, obj, parent_path, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(5, 5, 5, 5), index_bindings=None):
        if index_bindings is None:
            index_bindings = {}

        size = seq_size(obj)

        layout = self.make_layout(
            sizing=sizing,
            direction=direction,
            child_gap=5,
            padding=padding,
            color=color
        )
        layout.render_path = parent_path
        self._apply_interaction_mappings(layout, index_bindings)

        index_var_name = None
        deepest_mappings = self.element_renderer._get_deepest_interaction_mappings()
        if deepest_mappings:
            interaction_mapping = deepest_mappings[0]
            num_bound = len(index_bindings)
            if num_bound < len(interaction_mapping.index_vars):
                index_var_name = interaction_mapping.index_vars[num_bound]

        layout._index_bindings = index_bindings
        layout._index_var_name = index_var_name

        if size <= 0:
            layout.update_fn = lambda v : self.update(layout, v)
            layout.resize_fn = lambda n : self.resize(layout, n)
            return layout

        next_dir = (
            Direction.ROW if direction == Direction.COLUMN else Direction.COLUMN
        )

        for i in range(size):
            item = seq_at(obj, i)
            child_index_bindings = index_bindings.copy()
            if index_var_name:
                child_index_bindings[index_var_name] = i
            child_layout = self.element_renderer(
                item,
                parent_path=parent_path + [i],
                direction=next_dir,
                color="white",
                sizing=(FIT(), FIT()),
                logger=logger,
                index_bindings=child_index_bindings,
            )
            layout.children_mapping[i] = child_layout
            layout.add_child(child_layout)
        layout.update_fn = lambda v : self.update(layout, v)
        layout.resize_fn = lambda n : self.resize(layout, n)
        return layout

    def _grow_to(self, layout, new_size, item_at):
        old_size = len(layout.children)
        parent_path = getattr(layout, "render_path", None) or []
        next_dir = (
            Direction.ROW if layout.direction == Direction.COLUMN else Direction.COLUMN
        )
        index_bindings = getattr(layout, "_index_bindings", {})
        index_var_name = getattr(layout, "_index_var_name", None)
        if new_size > old_size:
            for i in range(old_size, new_size):
                child_index_bindings = index_bindings.copy()
                if index_var_name:
                    child_index_bindings[index_var_name] = i
                child_layout = self.element_renderer(
                    item_at(i),
                    parent_path=parent_path + [i],
                    direction=next_dir,
                    color="lightgray",
                    sizing=(FIT(), FIT()),
                    index_bindings=child_index_bindings,
                )
                layout.add_child(child_layout)
                layout.children_mapping[i] = child_layout
            layout.is_dirty = True
        elif new_size < old_size:
            for i in range(new_size, old_size):
                layout.children_mapping.pop(i, None)
            layout.children = layout.children[:new_size]
            layout.is_dirty = True

    def resize(self, layout, new_size):
        old_size = len(layout.children)
        if new_size < old_size:
            for i in range(new_size, old_size):
                layout.children_mapping.pop(i, None)
            layout.children = layout.children[:new_size]
        layout.is_dirty = True

    def update(self, layout, obj):
        new_size = seq_size(obj)
        self._grow_to(layout, new_size, lambda i: seq_at(obj, i))
        for i in range(new_size):
            self.element_renderer.update(layout.children[i], seq_at(obj, i))

    def _iter_children(self):
        return [self.element_renderer]


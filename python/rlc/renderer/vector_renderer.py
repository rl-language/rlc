from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import Layout, Direction, FIT, Padding
from dataclasses import dataclass

@register_renderer
@dataclass
class VectorRenderer(Renderable):
    element_renderer: Renderable

    def build_layout(self, obj, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(5, 5, 5, 5)):
        data_ptr = getattr(obj, "_data", None)
        size = getattr(obj, "_size", None)

        if size is None and hasattr(obj, "_length_"):
            size = obj._length_
        size = size or 0

        layout = self.make_layout(
            sizing=sizing,
            direction=direction,
            child_gap=5,
            padding=padding,
            color=color
        )
        layout.binding = {"type": "vector"}
        if not data_ptr or size <= 0:
            return layout

        # Alternate direction for the next nesting level
        next_dir = (
            Direction.ROW if direction == Direction.COLUMN else Direction.COLUMN
        )

        # Iterate over elements
        for i in range(size):
            item = data_ptr[i]
            item_binding = {
                "type": "vector_item",
                "index": i,
                "parent": layout.binding
            }
            child_layout = self.element_renderer(
                item,
                parent_binding=item_binding,
                direction=next_dir,
                color="lightgray",
                sizing=(FIT(), FIT()),
                logger=logger,
            )

            child_layout.binding = item_binding
            layout.add_child(child_layout)

        return layout

    def update(self, layout, obj, elapsed_time=0.0):
        new_size = obj.size()
        old_size = len(layout.children)

        if new_size > old_size:
            for i in range(old_size, new_size):
                item = obj.get(i).contents
                child_layout = self.element_renderer(
                    item,
                    direction=layout.direction,
                    color="lightgray",
                    sizing=(FIT(), FIT()),
                )
                layout.add_child(child_layout)
            layout.is_dirty = True
        elif new_size < old_size:
            layout.children = layout.children[:new_size]
            layout.is_dirty = True

        for i in range(min(new_size, old_size)):
            item = obj.get(i).contents
            self.element_renderer.update(layout.children[i], item, elapsed_time)

    def _iter_children(self):
        return [self.element_renderer]


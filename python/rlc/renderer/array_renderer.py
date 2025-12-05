from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import Layout, Direction, FIT, Padding
from dataclasses import dataclass

@register_renderer
@dataclass
class ArrayRenderer(Renderable):
    length: int
    element_renderer: Renderable

    def build_layout(self, obj, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        layout = self.make_layout(sizing=sizing, direction=direction, color=color, padding=padding, border=3, child_gap=5)
        layout.binding = {"type": "array"}
        color = 'lightgray'
        if self.element_renderer is not None:
            for i in range(self.length):
                item = obj[i]
                # Alternate direction for the next depth
                next_dir = (
                    Direction.ROW if direction == Direction.COLUMN else Direction.COLUMN
                )
                next_color = 'lightblue'
                item_binding = {
                    "type": "array_item",
                    "index": i,
                    "parent": layout.binding
                }
                child = self.element_renderer(
                    item,
                    parent_binding=item_binding,
                    direction=next_dir,
                    logger=logger,
                    color=next_color,
                    sizing=(FIT(), FIT()),
                    padding=Padding(2,2,2,2),
                )
                child.binding = item_binding
                layout.add_child(child)
        return layout

    def update(self, layout, obj, elapsed_time=0.0):
        for i, child in enumerate(layout.children):
            item = obj[i]
            self.element_renderer.update(child, item, elapsed_time)

    def _iter_children(self):
        return [self.element_renderer]

    def apply_interactivity(self, layout_child, index=None, parent_obj=None):
        """Hook for subclasses to mark children interactive."""
        return None


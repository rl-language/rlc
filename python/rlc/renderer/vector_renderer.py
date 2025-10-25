from rlc.renderer.renderable import Renderable
from rlc.layout import Layout, Direction, FIT, Padding

class VectorRenderer(Renderable):
    def __init__(self, rlc_type, element_renderer: Renderable, backend = None):
        self.element_renderer = element_renderer
        super().__init__(rlc_type, backend)

    def build_layout(self, obj, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(5, 5, 5, 5)):
        data_ptr = getattr(obj, "_data", None)
        size = getattr(obj, "_size", 0)

        layout = Layout(
            sizing=sizing,
            direction=direction,
            child_gap=5,
            padding=padding,
            color=color
        )
        if not data_ptr or size <= 0:
            return layout

        # Alternate direction for the next nesting level
        next_dir = (
            Direction.ROW if direction == Direction.COLUMN else Direction.COLUMN
        )

        # Iterate over elements
        for i in range(size):
            item = data_ptr[i]
            child_layout = self.element_renderer(
                item,
                direction=next_dir,
                color="lightgray",
                sizing=(FIT(), FIT()),
                logger=logger,
            )
            layout.add_child(child_layout)

        return layout
    
    def _iter_children(self):
        return [self.element_renderer]

    def _describe_self(self):
        return f"{self.rlc_type.__name__}(vector)"
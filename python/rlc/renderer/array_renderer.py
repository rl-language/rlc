from rlc.renderer.renderable import Renderable
from rlc.layout import Layout, Direction, FIT, Padding

class ArrayRenderer(Renderable):
    def __init__(self, rlc_type, element_rendere: Renderable, backend = None):
        super().__init__(rlc_type, backend)
        self.element_renderer = element_rendere

    def build_layout(self, obj, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        layout = Layout(sizing=sizing, direction=direction, color=color, padding=padding, border=3, child_gap=5)
        color = 'lightgray'
        for i in range(self.rlc_type._length_):  
            item = obj[i]
            # Alternate direction for the next depth
            next_dir = (
                Direction.ROW if direction == Direction.COLUMN else Direction.COLUMN
            )
            next_color = 'lightblue'
            child = self.element_renderer(item, direction=next_dir, logger=logger, color=next_color, sizing=(FIT(), FIT()), padding=Padding(2,2,2,2))
            layout.add_child(child)
        return layout
    
    def _iter_children(self):
        return [self.element_renderer]

    def _describe_self(self):
        return f"{self.rlc_type.__name__}[len={getattr(self.rlc_type, '_length_', '?')}]"
    

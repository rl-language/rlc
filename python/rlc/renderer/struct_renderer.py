from rlc.renderer.renderable import Renderable
from rlc.layout import Layout, FIT, Direction, Padding
from rlc.text import Text


class ContainerRenderer(Renderable):
    def __init__(self, rlc_type, field_renderers, backend = None):
        super().__init__(rlc_type, backend)
        self.field_renderers = field_renderers

    def build_layout(self, obj, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(7,7,7,7)):
        layout = Layout(sizing=sizing, direction=direction, child_gap=5, color=color, border=5, padding=padding)
        for field_name, field_renderer in self.field_renderers.items():
            # Create a row for "name: value"
            value = getattr(obj, field_name)
            row_layout = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW, child_gap=5, color=None, border=5, padding=Padding(10,10,10,10))
            label = Text(field_name + ": ", "Arial", 36, "black")
            value_layout = field_renderer(value)
            row_layout.add_child(label)
            row_layout.add_child(value_layout)
            layout.add_child(row_layout)
            
        return layout
    
    def update(self, layout, obj, elapsed_time=0.0):
        for (field_name, field_renderer), child_layout in zip(self.field_renderers.items(), layout.children):
            # print(field_name + ": ")
            # child_layout.print_layout()
            value = getattr(obj, field_name)
            field_renderer.update(child_layout.children[-1], value, elapsed_time)
    
    def _iter_children(self):
        # Only return child renderers (ignore field names)
        return list(self.field_renderers.values())

    def _describe_self(self):
        field_names = ", ".join(self.field_renderers.keys())
        return f"{self.rlc_type.__name__} fields=[{field_names}]"

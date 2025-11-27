from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import Layout, FIT, Direction, Padding
from rlc.text import Text

@register_renderer
class ContainerRenderer(Renderable):
    def __init__(self, rlc_type_name, field_renderers, style_policy):
        super().__init__(rlc_type_name, style_policy)
        self.field_renderers = field_renderers
        self.style_policy = style_policy

    def build_layout(self, obj, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(7,7,7,7)):
        layout = self.make_layout(sizing=sizing, direction=direction, child_gap=5, color=color, border=5, padding=padding)
        for field_name, field_renderer in self.field_renderers.items():
            if field_renderer is None:
                continue
            # Create a row for "name: value"
            value = getattr(obj, field_name)
            row_layout = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW, child_gap=5, color=None, border=5, padding=Padding(10,10,10,10))
            label = self.make_text(field_name + ": ", "Arial", 16, "black")
            value_layout = field_renderer(value)
            row_layout.add_child(label)
            row_layout.add_child(value_layout)
            layout.add_child(row_layout)
            
        return layout
    
    def update(self, layout, obj, elapsed_time=0.0):
        for (field_name, field_renderer), child_layout in zip(self.field_renderers.items(), layout.children):
            if field_renderer is None:
                continue
            value = getattr(obj, field_name)
            field_renderer.update(child_layout.children[-1], value, elapsed_time)
    
    def _iter_children(self):
        # Only return child renderers (ignore field names)
        return [r for r in self.field_renderers.values() if r is not None]

    def _describe_self(self):
        field_names = ", ".join(self.field_renderers.keys())
        return f"{self.rlc_type_name} fields=[{field_names}]"
    
    def _to_dict_data(self):
        return {
            "fields": {k: v.to_dict() for k, v in self.field_renderers.items() if v is not None},
            "style_policy" : self.style_policy
        }

    @classmethod
    def _from_dict_data(cls, rlc_type_name, data):
        fields = {
            k: Renderable.from_dict(v)
            for k, v in data["fields"].items()
        }
        return cls(rlc_type_name, fields, data["style_policy"])

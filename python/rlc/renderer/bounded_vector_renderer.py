from rlc.text import Text
from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import  Direction, FIT, Padding

@register_renderer
class BoundedVectorRenderer(Renderable):
    """
    Renderer for bounded integer structs like BIntT1T10T.
    """
    def __init__(self, rlc_type_name, vector_renderer, style_policy):
        super().__init__(rlc_type_name, style_policy)
        name = rlc_type_name
        self.vector_renderer = vector_renderer
        self.bounds = self._parse_bounds_from_name(name)
        self.style_policy = style_policy

    def _parse_bounds_from_name(self, name: str):
        # Extract numbers between 'T' markers if present
        # e.g., "BoundedVectorT20T" -> (1, 10)
        parts = [p for p in name.split("T") if p.isdigit()]
        if len(parts) >= 2:
            return int(parts[-1])
        return None

    def build_layout(self, obj, direction=Direction.COLUMN,
                     color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        value = getattr(obj, "_data", None)
        value_layout = self.vector_renderer(value)
        return value_layout
        
    
    def update(self, layout, obj, elapsed_time=0.0):
        value = getattr(obj, "_data")
        self.vector_renderer.update(layout, value, elapsed_time)

    def _describe_self(self):
        return f"{self.rlc_type_name}(bounded)"
    
    def _to_dict_data(self):
        return {
            "bounds": self.bounds,
            "field": self.vector_renderer.to_dict(),
            "style_policy" : self.style_policy
        }

    @classmethod
    def _from_dict_data(cls, rlc_type_name, data):
        vector_renderer = Renderable.from_dict(data["field"])
        obj = cls(rlc_type_name, vector_renderer, data["style_policy"])
        obj.bounds = data["bounds"]
        
        return obj

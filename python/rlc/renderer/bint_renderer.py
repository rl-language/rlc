from rlc.text import Text
from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import  Direction, FIT, Padding

@register_renderer
class BoundedIntRenderer(Renderable):
    """
    Renderer for bounded integer structs like BIntT1T10T.
    """
    def __init__(self, rlc_type_name, style_policy):
        super().__init__(rlc_type_name, style_policy)
        # Optionally parse bounds from type name: BIntT1T10T -> 1,10
        name = rlc_type_name
        self.bounds = self._parse_bounds_from_name(name)
        self.style_policy = style_policy

    def _parse_bounds_from_name(self, name: str):
        # Extract numbers between 'T' markers if present
        # e.g., "BIntT1T10T" -> (1, 10)
        parts = [p for p in name.split("T") if p.isdigit()]
        if len(parts) >= 2:
            return (int(parts[0]), int(parts[1]))
        return None

    def build_layout(self, obj, direction=Direction.COLUMN,
                     color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(2,2,2,2)):
        # Extract the 'value' field (the inner c_long)
        value = getattr(obj, "value", None)
        val_str = str(value if isinstance(value, int) else getattr(value, "value", value))

        # Include bounds if known
        # if self.bounds:
        #     low, high = self.bounds
        #     display = f"{val_str} [{low}-{high}]"
        # else:
        #     display = val_str
        layout = self.make_text(val_str, "Arial", 16, "black")
        layout.binding = {
            "type": "bounded_int",
            "obj": obj
        }
        return layout
    
    def update(self, layout, obj, elapsed_time=0.0):
        if isinstance(layout, Text):
            value = getattr(obj, "value", None)
            new_val = str(value if isinstance(value, int) else getattr(value, "value", value))
            layout.update_text(new_val)

    def _describe_self(self):
        return f"{self.rlc_type_name + str(self.style_policy)}(bounded)"
    
    def _to_dict_data(self):
        return {
            "bounds": self.bounds,
            "style_policy" : self.style_policy
        }

    @classmethod
    def _from_dict_data(cls, rlc_type_name, data):
        obj = cls(rlc_type_name, data["style_policy"])
        obj.bounds = tuple(data["bounds"])
        return obj

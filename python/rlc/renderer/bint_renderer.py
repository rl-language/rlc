from rlc.text import Text
from rlc.renderer.renderable import Renderable
from rlc.layout import  Direction, FIT, Padding

class BoundedIntRenderer(Renderable):
    """
    Renderer for bounded integer structs like BIntT1T10T.
    """
    def __init__(self, rlc_type, backend=None):
        super().__init__(rlc_type, backend)
        # Optionally parse bounds from type name: BIntT1T10T -> 1,10
        name = getattr(rlc_type, "__name__", "")
        self.bounds = self._parse_bounds_from_name(name)

    def _parse_bounds_from_name(self, name: str):
        # Extract numbers between 'T' markers if present
        # e.g., "BIntT1T10T" -> (1, 10)
        parts = [p for p in name.split("T") if p.isdigit()]
        if len(parts) >= 2:
            return (int(parts[0]), int(parts[1]))
        return None

    def build_layout(self, obj, direction=Direction.COLUMN,
                     color="white", sizing=(FIT(), FIT()), logger=None):
        # Extract the 'value' field (the inner c_long)
        value = getattr(obj, "value", None)
        val_str = str(value if isinstance(value, int) else getattr(value, "value", value))

        # Include bounds if known
        # if self.bounds:
        #     low, high = self.bounds
        #     display = f"{val_str} [{low}-{high}]"
        # else:
        #     display = val_str

        return Text(val_str, "Arial", 36, "black")

    def _describe_self(self):
        return f"{self.rlc_type.__name__}(bounded)"

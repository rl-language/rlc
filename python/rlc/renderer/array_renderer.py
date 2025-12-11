from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import Layout, Direction, FIT, Padding

@register_renderer
class ArrayRenderer(Renderable):
    def __init__(self, rlc_type_name, length, element_rendere: Renderable, style_policy):
        super().__init__(rlc_type_name, style_policy)
        self.length = length
        self.element_renderer = element_rendere
        self.style_policy = style_policy

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

    def _describe_self(self):
        return f"{self.rlc_type_name + str(self.style_policy)}[len={self.length}]"
    
    def _to_dict_data(self):
        return {
            "length": self.length,
            "element": self.element_renderer.to_dict() if self.element_renderer is not None else None,
            "style_policy" : self.style_policy
        }

    @classmethod
    def _from_dict_data(cls, rlc_type_name, data):
        length = data["length"]
        element = Renderable.from_dict(data["element"]) if data["element"] is not None else None
        return cls(rlc_type_name, length, element, data["style_policy"])
    
    def apply_interactivity(self, layout_child, index=None, parent_obj=None):
        """Hook for subclasses to mark children interactive."""
        return None
    

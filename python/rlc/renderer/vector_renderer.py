from rlc.renderer.renderable import Renderable, register_renderer
from rlc.layout import Layout, Direction, FIT, Padding

@register_renderer
class VectorRenderer(Renderable):
    def __init__(self, rlc_type_name, element_renderer: Renderable, style_policy):
        self.element_renderer = element_renderer
        self.style_policy = style_policy
        super().__init__(rlc_type_name, style_policy)

    def build_layout(self, obj, direction=Direction.ROW, color="white", sizing=(FIT(), FIT()), logger=None, padding=Padding(5, 5, 5, 5)):
        data_ptr = getattr(obj, "_data", None)
        size = getattr(obj, "_size", None)
        # Fallback for bounded vectors that expose `_length_` / `_type_` rather than `_size`
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
    
    def update(self, layout, obj, elapsed_time=0.0):
        data_ptr = getattr(obj, "_data", None)
        new_size = getattr(obj, "_size", None)
        
        if data_ptr is None:
            return
        old_size = len(layout.children)

        if new_size > old_size:
            for i in range(old_size, new_size):
                item = data_ptr[i]
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
            item = data_ptr[i]
            self.element_renderer.update(layout.children[i], item, elapsed_time)
    
    def _iter_children(self):
        return [self.element_renderer]

    def _describe_self(self):
        return f"{self.rlc_type_name}(vector)"
    
    def _to_dict_data(self):
        return {
            "element": self.element_renderer.to_dict(),
            "style_policy": self.style_policy
        }

    @classmethod
    def _from_dict_data(cls, rlc_type_name, data):
        element = Renderable.from_dict(data["element"])
        return cls(rlc_type_name, element, data["style_policy"])

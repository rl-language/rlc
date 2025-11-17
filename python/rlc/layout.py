from enum import Enum
from typing import Optional, Tuple, List
import copy
from .renderer_backend import RendererBackend
# //utility for dumping layout

class Direction(Enum):
    ROW = "row"
    COLUMN = "column"

class SizePolicies(Enum):
    FIXED = "fixed"
    FIT = "fit"
    GROW = "grow"

class SizePolicy:
    def __init__(self, size_policy: SizePolicies, value: Optional[float] = None):
        self.size_policy = size_policy
        self.value : Optional[int] = value
    def __repr__(self) -> str:
        return f"{self.size_policy.value}({self.value})" if self.value is not None else self.size_policy.value
    
def FIXED(value) -> SizePolicy:
    return SizePolicy(SizePolicies.FIXED, value)
def FIT() -> SizePolicy:
    return SizePolicy(SizePolicies.FIT, 0)
def GROW() -> SizePolicy:
    return SizePolicy(SizePolicies.GROW, 0)

class Padding:
    def __init__(self, top=0, bottom=0, left=0, right=0):
        self.top = top
        self.bottom = bottom
        self.left = left
        self.right = right


class Layout:
    def __init__(
            self, 
            sizing : Tuple[SizePolicy, SizePolicy] = (FIT(), FIT()), 
            padding : Padding = Padding(0, 0, 0, 0),
            direction : Direction = Direction.ROW,
            border: float = 0,
            child_gap : float = 0,
            color: Optional[str] = None):
        self.sizing : Tuple[SizePolicy, SizePolicy] = sizing
        self.direction : Direction = direction
        self.border = border
        self.child_gap : float = child_gap
        self.padding : Padding = padding
        self.color: Optional[str] = color
        self.children : List['Layout'] = []
        self.x = 0
        self.y = 0
        self.is_dirty = False
        self.width = sizing[0].value if sizing[0].size_policy == SizePolicies.FIXED else 0
        self.height = sizing[1].value if sizing[1].size_policy == SizePolicies.FIXED else 0    
         
    def add_child(self, child: 'Layout') -> None:
        self.children.append(copy.deepcopy(child))

    def _inner_dims(self) -> Tuple[float, float]:
        iw = max(0, self.width - self.padding.left - self.padding.right)
        ih = max(0, self.height - self.padding.top - self.padding.bottom)
        return iw, ih

    def child_size(self,logger: Optional['LayoutLogger'] = None, backend: Optional[RendererBackend] = None) -> None:
        # Always size children — even if self has fixed size
        inner_available_width = self.width - self.padding.left - self.padding.right
        inner_available_height = self.height - self.padding.top - self.padding.bottom
        
        for child in self.children:
            child_width_policy, child_height_policy = child.sizing
            # Only pass constraints if needed
            child_width = (
                inner_available_width if child_width_policy.size_policy != SizePolicies.FIXED else child_width_policy.value
            )
            child_height = (
                inner_available_height if child_height_policy.size_policy != SizePolicies.FIXED else child_height_policy.value
            )
            child.compute_size(child_width, child_height,logger, backend=backend)


     # Sizing  
    
    def compute_size(self, available_width=None, available_height=None, logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> None:
        if logger: logger.snapshot(self, "before_compute")
        if self.direction == Direction.ROW:
            self._compute_width(logger, backend)
            self._compute_grow_width()
            self.child_size(logger, backend)
            if logger: logger.snapshot(self, "after_children_sizing")
            self._compute_height(logger, backend)
            self._compute_grow_height()
        if self.direction == Direction.COLUMN:
            self._compute_height(logger, backend)
            self._compute_grow_height()
            self.child_size(logger, backend)
            if logger: logger.snapshot(self, "after_children_sizing")
            self._compute_width(logger, backend)
            self._compute_grow_width()
        if logger: logger.snapshot(self, "after_compute")
                
    def _compute_width(self, logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> None:
        width_policy, _ = self.sizing
        if width_policy.size_policy == SizePolicies.FIXED:
            self.width = width_policy.value
        elif width_policy.size_policy == SizePolicies.FIT: 
            self.width = self._compute_fit_width(logger, backend)       
        
    def _compute_height(self, logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> None:
        _, height_policy = self.sizing
        if height_policy.size_policy == SizePolicies.FIXED:
            self.height = height_policy.value
        elif height_policy.size_policy == SizePolicies.FIT:
            self.height = self._compute_fit_height(logger, backend)

    def _compute_fit_width(self, logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> float:
        self.child_size(logger, backend)
        if self.direction == Direction.ROW:
            content_width = sum(c.width for c in self.children)
            content_width += (len(self.children) - 1) * self.child_gap 

        if self.direction == Direction.COLUMN:
            content_width = max((c.width for c in self.children), default=0)
        
        return content_width + self.padding.left + self.padding.right
    
    def _compute_fit_height(self, logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> float:
        self.child_size(logger, backend)
        if self.direction == Direction.COLUMN:
            content_height = sum(c.height for c in self.children)
            content_height += (len(self.children) - 1) * self.child_gap 

        if self.direction == Direction.ROW:
            content_height = max((c.height for c in self.children), default=0)
        
        return content_height + self.padding.top + self.padding.bottom

    def _compute_grow_width(self) -> None:
        if self.direction == Direction.ROW:
            # Horizontal GROW
            total_fixed_width = sum(child.width for child in self.children if child.sizing[0].size_policy != SizePolicies.GROW)
            total_gap = (len(self.children) - 1) * self.child_gap
            remaining_width = self.width - self.padding.left - self.padding.right - total_fixed_width - total_gap
            self._grow_children_evenly(self.children, remaining_width, axis=0)

        if self.direction == Direction.COLUMN:
            # cross-axis GROW
            remaining_width = self.width - self.padding.left - self.padding.right
            for child in self.children:
                if child.sizing[0].size_policy == SizePolicies.GROW:
                    child.width = remaining_width 

    def _compute_grow_height(self) -> None:
        if self.direction == Direction.ROW:
            # cross-axis GROW
            remaining_height = self.height - self.padding.top - self.padding.bottom
            for child in self.children:
                if child.sizing[1].size_policy == SizePolicies.GROW:
                    child.height = remaining_height 

        if self.direction == Direction.COLUMN:
            # Vertical GROW
            total_fixed_height = sum(child.height for child in self.children if child.sizing[1].size_policy != SizePolicies.GROW)
            total_gap = self.child_gap * (len(self.children) - 1)
            remaining_height = self.height - self.padding.top - self.padding.bottom - total_fixed_height - total_gap
            self._grow_children_evenly(self.children, remaining_height, axis=1)
    
    def _grow_children_evenly(self, children: List['Layout'], remaining: float, axis: int):
        growable = [c for c in children if c.sizing[axis].size_policy == SizePolicies.GROW]
        while growable and remaining > 0:
            growable.sort(key=lambda c: c.width if axis == 0 else c.height)
            smallest = growable[0]
            min_val = smallest.width if axis == 0 else smallest.height
            second_smallest = float('inf')
            for c in growable:
                val = c.width if axis == 0 else c.height
                if val > min_val:
                    second_smallest = min(second_smallest, val)
            if second_smallest == float('inf'):
                # All same size, distribute equally
                to_add = remaining // len(growable)
                for c in growable:
                    if axis == 0:
                        c.width += to_add
                    else:
                        c.height += to_add
                remaining -= to_add * len(growable)
                break
            delta = second_smallest - min_val
            group = [c for c in growable if (c.width if axis == 0 else c.height) == min_val]
            grow_amount = min(delta, remaining // len(group))
            for c in group:
                if axis == 0:
                    c.width += grow_amount
                else:
                    c.height += grow_amount
                remaining -= grow_amount
            growable = [c for c in growable if (c.width if axis == 0 else c.height) < second_smallest]
        shrinkable = [c for c in children if c.sizing[axis].size_policy != SizePolicies.FIXED]
        while shrinkable and remaining < 0:
            shrinkable.sort(key=lambda c: c.width if axis == 0 else c.height, reverse=True)
            largest = shrinkable[0]
            to_remove = remaining
            max_val = largest.width if axis == 0 else largest.height
            second_largest = 0
            second_largest_val = 0
            if len(shrinkable) > 1:
                second_largest = shrinkable[1]
                second_largest_val = second_largest.width if axis == 0 else second_largest.height
        
            if second_largest_val == 0:
                # All same size, distribute equally
                to_remove =  (-remaining) // len(shrinkable)
                for c in shrinkable:
                    if axis == 0:
                        c.width -= to_remove
                    else:
                        c.height -= to_remove
                remaining += to_remove * len(shrinkable)
                
                break
            delta = max_val - second_largest_val
            group = [c for c in shrinkable if (c.width if axis == 0 else c.height) == max_val]
            shrink_amount = min(delta, (-remaining) // len(group))
            for c in group:
                if axis == 0:
                    c.width -= shrink_amount
                else:
                    c.height -= shrink_amount
                remaining += shrink_amount
            shrinkable = [c for c in shrinkable if (c.width if axis == 0 else c.height) > second_largest_val]

    # Position
    def layout(self, x: int=0, y: int=0, logger: Optional['LayoutLogger']=None) -> None:
        self.x = x
        self.y = y
        if logger: logger.snapshot(self, "before_layout")
        if self.direction == Direction.ROW:
            self._layout_row_children()
        if self.direction == Direction.COLUMN:
            self._layout_column_children()
        if logger: logger.snapshot(self, "after_layout")

    def _layout_row_children(self) -> None:
        left_offset = self.padding.left
        for child in self.children:
            child_pos_x = self.x + left_offset
            child_pos_y =  self.y + self.padding.top
            child.layout(child_pos_x, child_pos_y)
            left_offset += child.width + self.child_gap

    def _layout_column_children(self) -> None:
        top_offset = self.padding.top
        for child in self.children:
            child_pos_x = self.x + self.padding.left
            child_pos_y = self.y + top_offset
            child.layout(child_pos_x, child_pos_y)
            top_offset += child.height + self.child_gap

    def print_layout(self, depth: int = 0):
        indent = "  " * depth
        print(f"{indent}{self.__class__.__name__} (dir={self.direction.value if self.direction else '-'}, size=({self.width}*{self.height}), pos=({self.x},{self.y}), policy=({self.sizing[0].size_policy},{self.sizing[1].size_policy}), color={self.color if self.color else '-'})")
        for child in self.children:
            child.print_layout(depth + 1)



        




   
from dataclasses import dataclass
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
        self.value : Optional[float] = value
    def __repr__(self) -> str:
        return f"{self.size_policy.value}({self.value})" if self.value is not None else self.size_policy.value
    
def FIXED(value) -> SizePolicy:
    return SizePolicy(SizePolicies.FIXED, value)
def FIT() -> SizePolicy:
    return SizePolicy(SizePolicies.FIT, 0)
def GROW() -> SizePolicy:
    return SizePolicy(SizePolicies.GROW, 0)

@dataclass(frozen=True)
class Padding:
    top: float = 0
    bottom: float = 0
    left: float = 0
    right: float = 0


ZERO_PADDING = Padding()

class Layout:
    def __init__(
            self, 
            sizing : Tuple[SizePolicy, SizePolicy] = (FIT(), FIT()), 
            padding : Optional[Padding] = None,
            direction : Direction = Direction.ROW,
            border: float = 0,
            child_gap : float = 0,
            color: Optional[str] = None):
        self.sizing : Tuple[SizePolicy, SizePolicy] = sizing
        self.direction : Direction = direction
        self.border = border
        self.child_gap : float = child_gap
        self.padding : Padding = padding or ZERO_PADDING
        self.color: Optional[str] = color
        self.children : List['Layout'] = []
        self.x = 0
        self.y = 0
        self.is_dirty = False
        self._children_sized = False
        self.width = sizing[0].value if sizing[0].size_policy == SizePolicies.FIXED else 0
        self.height = sizing[1].value if sizing[1].size_policy == SizePolicies.FIXED else 0    
         
    def add_child(self, child: 'Layout') -> None:
        self.children.append(copy.deepcopy(child))

    def _axis_size(self, axis: int) -> float:
        return self.width if axis == 0 else self.height

    def _set_axis_size(self, axis: int, value: float) -> None:
        if axis == 0:
            self.width = value
        else:
            self.height = value

    def _padding_for_axis(self, axis: int) -> Tuple[float, float]:
        return (
            (self.padding.left, self.padding.right)
            if axis == 0
            else (self.padding.top, self.padding.bottom)
        )

    def _inner_size(self, axis: int) -> float:
        start, end = self._padding_for_axis(axis)
        return max(0, self._axis_size(axis) - start - end)

    def child_size(self, logger: Optional['LayoutLogger'] = None, backend: Optional[RendererBackend] = None) -> None:
        if self._children_sized:
            return
        # Always size children — even if self has fixed size
        inner_available_width = self._inner_size(0)
        inner_available_height = self._inner_size(1)
        
        for child in self.children:
            child_width_policy, child_height_policy = child.sizing
            # Only pass constraints if needed; leave GROW unset so parent-driven sizing is preserved.
            if child_width_policy.size_policy == SizePolicies.FIXED:
                child_width = child_width_policy.value
            elif child_width_policy.size_policy == SizePolicies.GROW:
                child_width = None
            else:
                child_width = inner_available_width

            if child_height_policy.size_policy == SizePolicies.FIXED:
                child_height = child_height_policy.value
            elif child_height_policy.size_policy == SizePolicies.GROW:
                child_height = None
            else:
                child_height = inner_available_height

            child.compute_size(child_width, child_height,logger, backend=backend)
        self._children_sized = True


     # Sizing  
    
    def compute_size(self, available_width=None, available_height=None, logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> None:
        self._children_sized = False
        if logger: logger.snapshot(self, "before_compute")
        available = (available_width, available_height)
        primary_axis = 0 if self.direction == Direction.ROW else 1
        self._compute_axis(primary_axis, available, logger, backend)
        self._compute_grow_axis(primary_axis)
        self.child_size(logger, backend)
        if logger: logger.snapshot(self, "after_children_sizing")
        cross_axis = 1 - primary_axis
        self._compute_axis(cross_axis, available, logger, backend)
        self._compute_grow_axis(cross_axis)
        if logger: logger.snapshot(self, "after_compute")
                
    def _compute_axis(self, axis: int, available: Tuple[Optional[float], Optional[float]], logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> None:
        policy = self.sizing[axis]
        if policy.size_policy == SizePolicies.FIXED:
            self._set_axis_size(axis, policy.value)
        elif policy.size_policy == SizePolicies.FIT:
            self._set_axis_size(axis, self._compute_fit(axis, logger, backend))
        elif policy.size_policy == SizePolicies.GROW:
            available_size = available[axis]
            # Only fill from available space if we haven't already been sized
            if available_size is not None and self._axis_size(axis) <= 0:
                self._set_axis_size(axis, available_size)

    def _compute_fit(self, axis: int, logger: Optional['LayoutLogger']=None, backend: Optional[RendererBackend] = None) -> float:
        self.child_size(logger, backend)
        if self.direction == Direction.ROW:
            content = (
                sum(c.width for c in self.children) + (len(self.children) - 1) * self.child_gap
                if axis == 0 else
                max((c.height for c in self.children), default=0)
            )
        else:
            content = (
                sum(c.height for c in self.children) + (len(self.children) - 1) * self.child_gap
                if axis == 1 else
                max((c.width for c in self.children), default=0)
            )
        padding_start, padding_end = self._padding_for_axis(axis)
        return content + padding_start + padding_end

    def _compute_grow_axis(self, axis: int) -> None:
        if not self.children:
            return
        is_primary_axis = (
            (self.direction == Direction.ROW and axis == 0)
            or (self.direction == Direction.COLUMN and axis == 1)
        )
        inner_size = self._inner_size(axis)
        if is_primary_axis:
            total_gap = (len(self.children) - 1) * self.child_gap
            fixed_total = sum(
                self._child_axis_size(child, axis)
                for child in self.children
                if child.sizing[axis].size_policy != SizePolicies.GROW
            )
            remaining = inner_size - fixed_total - total_gap
            self._grow_children_evenly(self.children, remaining, axis)
        else:
            for child in self.children:
                if child.sizing[axis].size_policy == SizePolicies.GROW:
                    self._set_child_axis_size(child, axis, inner_size)
    
    def _child_axis_size(self, child: 'Layout', axis: int) -> float:
        return child.width if axis == 0 else child.height

    def _set_child_axis_size(self, child: 'Layout', axis: int, value: float) -> None:
        if axis == 0:
            child.width = value
        else:
            child.height = value

    def _grow_children_evenly(self, children: List['Layout'], remaining: float, axis: int):
        if not children or remaining == 0:
            return
        if remaining > 0:
            self._distribute_positive_space([c for c in children if c.sizing[axis].size_policy == SizePolicies.GROW], remaining, axis)
        else:
            self._distribute_negative_space([c for c in children if c.sizing[axis].size_policy != SizePolicies.FIXED], -remaining, axis)

    def _distribute_positive_space(self, growable: List['Layout'], space: float, axis: int) -> None:
        if not growable or space <= 0:
            return
        # Level items up to the next size tier before splitting evenly.
        while growable and space > 0:
            growable.sort(key=lambda c: self._child_axis_size(c, axis))
            smallest_size = self._child_axis_size(growable[0], axis)
            next_larger = next(
                (self._child_axis_size(c, axis) for c in growable if self._child_axis_size(c, axis) > smallest_size),
                None
            )
            if next_larger is None:
                share = space // len(growable)
                for child in growable:
                    self._set_child_axis_size(child, axis, self._child_axis_size(child, axis) + share)
                return
            group = [c for c in growable if self._child_axis_size(c, axis) == smallest_size]
            increment = min(next_larger - smallest_size, space / len(group))
            if increment <= 0:
                share = space // len(group)
                for child in group:
                    self._set_child_axis_size(child, axis, self._child_axis_size(child, axis) + share)
                return
            for child in group:
                self._set_child_axis_size(child, axis, self._child_axis_size(child, axis) + increment)
            space -= increment * len(group)
            growable = [c for c in growable if self._child_axis_size(c, axis) < next_larger]

    def _distribute_negative_space(self, shrinkable: List['Layout'], space: float, axis: int) -> None:
        if not shrinkable or space <= 0:
            return
        # Trim the largest items first to avoid skewing sizes.
        while shrinkable and space > 0:
            shrinkable.sort(key=lambda c: self._child_axis_size(c, axis), reverse=True)
            largest_size = self._child_axis_size(shrinkable[0], axis)
            next_smaller = next(
                (self._child_axis_size(c, axis) for c in shrinkable if self._child_axis_size(c, axis) < largest_size),
                None
            )
            if next_smaller is None:
                share = space // len(shrinkable)
                for child in shrinkable:
                    self._set_child_axis_size(child, axis, max(0, self._child_axis_size(child, axis) - share))
                return
            group = [c for c in shrinkable if self._child_axis_size(c, axis) == largest_size]
            decrement = min(largest_size - next_smaller, space / len(group))
            if decrement <= 0:
                share = space // len(group)
                for child in group:
                    self._set_child_axis_size(child, axis, max(0, self._child_axis_size(child, axis) - share))
                return
            for child in group:
                self._set_child_axis_size(child, axis, max(0, self._child_axis_size(child, axis) - decrement))
            space -= decrement * len(group)
            shrinkable = [c for c in shrinkable if self._child_axis_size(c, axis) > next_smaller]

    # Position
    def layout(self, x: int=0, y: int=0, logger: Optional['LayoutLogger']=None) -> None:
        self.x = x
        self.y = y
        if logger: logger.snapshot(self, "before_layout")
        axis = 0 if self.direction == Direction.ROW else 1
        self._layout_children(axis)
        if logger: logger.snapshot(self, "after_layout")

    def _layout_children(self, axis: int) -> None:
        offset = self.padding.left if axis == 0 else self.padding.top
        for child in self.children:
            child_x = self.x + offset if axis == 0 else self.x + self.padding.left
            child_y = self.y + self.padding.top if axis == 0 else self.y + offset
            child.layout(child_x, child_y)
            offset += self._child_axis_size(child, axis) + self.child_gap

    def print_layout(self, depth: int = 0):
        indent = "  " * depth
        print(f"{indent}{self.__class__.__name__} (dir={self.direction.value if self.direction else '-'}, size=({self.width}*{self.height}), pos=({self.x},{self.y}), policy=({self.sizing[0].size_policy},{self.sizing[1].size_policy}), color={self.color if self.color else '-'})")
        for child in self.children:
            child.print_layout(depth + 1)

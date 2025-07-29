from enum import Enum
import pygame
import copy
# //utility for dumping layout

class Direction(Enum):
    ROW = "row"
    COLUMN = "column"

class SizePolicies(Enum):
    FIXED = "fixed"
    FIT = "fit"
    GROW = "grow"

class SizePolicy:
    def __init__(self, mode, value=None):
        self.mode = mode
        self.value = value
    def __repr__(self):
        return f"{self.mode}({self.value})" if self.value is not None else self.mode
    
def FIXED(value):
    return SizePolicy(SizePolicies.FIXED, value)
def FIT():
    return SizePolicy(SizePolicies.FIT, 0)
def GROW():
    return SizePolicy(SizePolicies.GROW, 0)

class Padding:
    def __init__(self, top=0, bottom=0, left=0, right=0):
        self.top = top
        self.bottom = bottom
        self.left = left
        self.right = right


class Container:
    def __init__(self, backgroundColor="white", sizing=(FIT(), FIT()), padding=Padding(0, 0, 0, 0), direction=Direction.ROW, child_gap=0):
        self.sizing = sizing
        self.backgroundColor = backgroundColor
        self.direction = direction
        self.child_gap = child_gap
        self.padding = padding
        self.children = []
        self.x = 0
        self.y = 0
        self.width = 0
        self.height = 0    
         
    def add_child(self, child):
        self.children.append(copy.deepcopy(child))

    def child_size(self):
        # Always size children — even if self has fixed size
        inner_available_width = self.width - self.padding.left - self.padding.right
        inner_available_height = self.height - self.padding.top - self.padding.bottom
        for child in self.children:
            if isinstance(child, Text):
                child_width_policy, child_height_policy = child.sizing

                # Only pass constraints if needed
                child_width = (
                    inner_available_width if child_width_policy.mode != SizePolicies.FIXED else child_width_policy.value
                )
                child_height = (
                    inner_available_height if child_height_policy.mode != SizePolicies.FIXED else child_height_policy.value
                )
                child.compute_size(child_width, child_height)
            else: child.compute_size()

     # Sizing  
    
    def compute_size(self):
        if self.direction == Direction.ROW:
            self.compute_width()
            self.compute_grow_width()
            self.child_size()
            self.compute_height()
            self.compute_grow_height()
        if self.direction == Direction.COLUMN:
            self.compute_height()
            self.compute_grow_height()
            self.child_size()
            self.compute_width()
            self.compute_grow_width()
                
    def compute_width(self):
        width_policy, _ = self.sizing
        if width_policy.mode == SizePolicies.FIXED:
            self.width = width_policy.value
        elif width_policy.mode == SizePolicies.FIT: 
            self.width = self.compute_fit_width()
        elif width_policy.mode == SizePolicies.GROW:
            self.width = 0        
        
    def compute_height(self):
        _, height_policy = self.sizing
        if height_policy.mode == SizePolicies.FIXED:
            self.height = height_policy.value
        elif height_policy.mode == SizePolicies.FIT:
            self.height = self.compute_fit_height()
        elif height_policy.mode == SizePolicies.GROW:
            self.height = 0

    def compute_fit_width(self):
        self.child_size()
        if self.direction == Direction.ROW:
            content_width = sum(c.width for c in self.children)
            content_width += (len(self.children) - 1) * self.child_gap 

        if self.direction == Direction.COLUMN:
            content_width = max((c.width for c in self.children), default=0)
        
        return content_width + self.padding.left + self.padding.right
    
    def compute_fit_height(self):
        self.child_size()
        if self.direction == Direction.COLUMN:
            content_height = sum(c.height for c in self.children)
            content_height += (len(self.children) - 1) * self.child_gap 

        if self.direction == Direction.ROW:
            content_height = max((c.height for c in self.children), default=0)
        
        return content_height + self.padding.top + self.padding.bottom

    def compute_grow_width(self):
        if self.direction == Direction.ROW:
            # Horizontal GROW
            total_fixed_width = sum(child.width for child in self.children if child.sizing[0].mode != SizePolicies.GROW)
            total_gap = (len(self.children) - 1) * self.child_gap
            remaining_width = self.width - self.padding.left - self.padding.right - total_fixed_width - total_gap
            self.grow_children_evenly(self.children, remaining_width, axis=0)

        if self.direction == Direction.COLUMN:
            # cross-axis GROW
            remaining_width = self.width - self.padding.left - self.padding.right
            for child in self.children:
                if child.sizing[0].mode == SizePolicies.GROW:
                    child.width = remaining_width 

    def compute_grow_height(self):
        if self.direction == Direction.ROW:
            # cross-axis GROW
            remaining_height = self.height - self.padding.top - self.padding.bottom
            for child in self.children:
                if child.sizing[1].mode == SizePolicies.GROW:
                    child.height = remaining_height 

        if self.direction == Direction.COLUMN:
            # Vertical GROW
            total_fixed_height = sum(child.height for child in self.children if child.sizing[1].mode != SizePolicies.GROW)
            total_gap = self.child_gap * (len(self.children) - 1)
            remaining_height = self.height - self.padding.top - self.padding.bottom - total_fixed_height - total_gap
            self.grow_children_evenly(self.children, remaining_height, axis=1)
    
    def grow_children_evenly(self, children, remaining, axis):
        growable = [c for c in children if c.sizing[axis].mode == SizePolicies.GROW]
        while growable and remaining > 0:
            print("In GROW")
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

        shrinkable = [c for c in children if c.sizing[axis].mode != SizePolicies.FIXED]
        while shrinkable and remaining < 0:
            print("In Shrink")
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
    def layout(self, x, y):
        self.x = x
        self.y = y
        if self.direction == Direction.ROW:
            self.layout_row_children()
        if self.direction == Direction.COLUMN:
            self.layout_column_children()

    def layout_row_children(self):
        left_offset = self.padding.left
        for child in self.children:
            child_pos_x = self.x + left_offset
            child_pos_y =  self.y + self.padding.top
            child.layout(child_pos_x, child_pos_y)
            left_offset += child.width + self.child_gap

    def layout_column_children(self):
        top_offset = self.padding.top
        for child in self.children:
            child_pos_x = self.x + self.padding.left
            child_pos_y = self.y + top_offset
            child.layout(child_pos_x, child_pos_y)
            top_offset += child.height + self.child_gap


class Text(Container):
    def __init__(self, text, font_name, font_size, color="black"):
        super().__init__() 
        self.text = text
        self.color = pygame.Color(color)
        self.font_name = font_name
        self.font_size = font_size
    def compute_size(self, available_width=None, available_height=None):
        font = pygame.font.SysFont(self.font_name, self.font_size)
        if available_width:
            lines = self.wrap_text(font, available_width)
        else:
            lines = [self.text]
            
        self.text_surfaces = [font.render(line, True, self.color) for line in lines]
        self.width = max(s.get_width() for s in self.text_surfaces)
        self.height = sum(s.get_height() for s in self.text_surfaces)

    def wrap_text(self, font, max_width):
        words = self.text.split(" ")
        lines = []
        current_line = ""

        for word in words:
            line = current_line + (" " if current_line else "") + word
            if font.size(line)[0] <= max_width:
                current_line = line
            else:
                if current_line:
                    lines.append(current_line)
                current_line = word
        if current_line:
            lines.append(current_line)
        return lines
        




   
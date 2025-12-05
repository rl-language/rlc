from typing import Dict, Tuple, List, Optional, Any
from .layout import Layout, Direction, FIT, FIXED, GROW, Padding
from .text import Text
from ctypes import c_long, Array, c_bool
import math  # For grid dimension calculation


# For non-square, could add more logic (e.g., check nested arrays for 2D)

class SceneNode:
    def __init__(self, kind: str, children: Optional[List['SceneNode']] = None, value: Any = None, label: str = "", meta: dict = None):
        self.kind = kind          
        self.children = children if children is not None else []
        self.value = value                 
        self.meta = meta or {} 

    def add_child(self, child: 'SceneNode'):
        self.children.append(child)

def detect_grid_dimensions(typ) -> Optional[Tuple[int, ...]]:
    """
    Recursively detect ctypes.Array grid dimensions.
    Works for nested arrays like Array[3][3], Array[2][3][4], etc.

    Returns:
        A tuple of dimensions (e.g., (3, 3) or (2, 3, 4)).
        Returns None if not an Array.
    """
    if not issubclass(typ, Array):
        return None

    dims = [typ._length_]
    inner = typ._type_
    while issubclass(inner, Array):
        dims.append(inner._length_)
        inner = inner._type_
    return tuple(dims)


# def detect_grid_dimensions(typ) -> Optional[Tuple[int, int]]:
#     """Pattern matching for grid-like arrays: Assume square if length is perfect square."""
#     if not issubclass(typ, Array):
#         return None
    
#     def total_inner_length(a: type) -> int:
#         """Return the total product of lengths for nested Array types."""
#         if not issubclass(a, Array):
#             return 1
#         inner_len = a._length_
#         # if inner element is itself an Array multiply by its total inner length
#         if issubclass(a._type_, Array):
#             return inner_len * total_inner_length(a._type_)
#         return inner_len
    
#     # Case 1: nested array: Array[rows][...]
#     if issubclass(typ._type_, Array):
#         rows = typ._length_
#         # If the immediate inner is an Array, compute its total flattened length
#         cols = total_inner_length(typ._type_)
#         return (rows, cols)

#     # Case 2: flat 1D array: try to see if it forms a square grid
#     length = typ._length_
#     sqrt_len = int(math.sqrt(length))
#     if sqrt_len * sqrt_len == length:
#         return (sqrt_len, sqrt_len)

#     # length = typ._length_
#     # sqrt_len = int(math.sqrt(length))
#     # if sqrt_len * sqrt_len == length:
#     #     return (sqrt_len, sqrt_len)  
#     # if issubclass(typ._type_, Array):
#     #     inner_dims = detect_grid_dimensions(typ._type_)
#     #     if inner_dims:
#     #         return (length, inner_dims[0]) 
#     return None  # Fallback to linear

def state_to_scene(state, typ, context: dict = None) -> SceneNode:
    """Recursively transform game state into a generic, reusable scene graph.
    
    Args:
        state: The game state or value to transform.
        typ: The RLC type of the state.
        context: Optional dictionary for configuration (e.g., field names).
    
    Returns:
        A SceneNode representing the state.
    """
    context = context or {}
    if hasattr(typ, "_fields_"):  # Struct
        node = SceneNode("struct", label=typ.__name__, meta={"context": context.get("struct_meta", {})})
        for field_name, field_type in typ._fields_:
            field_value = getattr(state, field_name)
            child_context = context.copy()
            child_context["field_name"] = field_name
            child = state_to_scene(field_value, field_type, child_context)
            label_node = SceneNode("label", value=field_name)
            node.add_child(label_node)
            node.add_child(child)
        return node
    elif issubclass(typ, Array):  # Array
        grid_dims = detect_grid_dimensions(typ)
        node = SceneNode("array", label=typ.__name__, meta={
            "context": context.get("array_meta", {}),
            "dims": grid_dims,
            "rows": grid_dims[0] if grid_dims else None,
            "cols": grid_dims[1] if grid_dims and len(grid_dims) > 1 else None,
        })
        # grid_dims = detect_grid_dimensions(typ)
        if grid_dims:
            if len(grid_dims) == 1:
                # 1D array
                for i in range(typ._length_):
                    child_context = {**context, "index": i}
                    child = state_to_scene(state[i], typ._type_, child_context)
                    node.add_child(child)
            else:
                rows = grid_dims[0]
                cols = grid_dims[1] if len(grid_dims) > 1 else 1
                for i in range(typ._length_):
                    row, col = divmod(i, cols)
                    child_node = SceneNode("cell", value=state[i], meta={
                        "row": row, 
                        "col": col, 
                    })
                    child_context = {**context, "index": i}
                    child_context["index"] = i
                    child_node.add_child(state_to_scene(state[i], typ._type_, child_context))
                    node.add_child(child_node)
        else:
            for i in range(typ._length_):
                child_context = context.copy()
                child_context["index"] = i
                child = state_to_scene(state[i], typ._type_, child_context)
                node.add_child(child)
        return node
    if typ == c_bool:
        print("primitive state of bool --> ", state)
        node = SceneNode(kind="value", value="True" if state else "False", meta={"context": context})
        print("primitive state --> ", node.value)
        return node
    if typ == c_long:
        print("primitive state of long --> ", state)
        node = SceneNode(kind="value", value=str(state), meta={"context": context})
        print("primitive state --> ", node.value)
        return node
    else:  # Primitive (int, bool, str)
        print("primitive state --> ", state)
        node = SceneNode(kind="value", value=state, meta={"context": context.get("value_meta", {})})
        if context.get("editable", False):
            node.meta["editable"] = True  # Flag for editing interfaces
        print("primitive state --> ", node.value)
        return node
    

def build_grid_layout(node: SceneNode, state_path: Tuple[str, ...]) -> Layout:
    """
    Build a layout for multi-dimensional arrays.
    - Handles 1D, 2D, and nested grids (e.g., 3×3, 2×3×4).
    """
    dims = node.meta.get("dims") or (
        [node.meta.get("rows"), node.meta.get("cols")]
        if node.meta.get("rows") and node.meta.get("cols")
        else None
    )

    # No dimensions → fallback column layout
    if not dims:
        layout = Layout(sizing=(FIT(), FIT()), direction=Direction.COLUMN)
        for i, child in enumerate(node.children):
            layout.add_child(scene_to_layout(child, state_path + (i,)))
        return layout

    # Base 1D case
    if len(dims) == 1:
        layout = Layout(
            sizing=(FIT(), FIT()),
            direction=Direction.ROW,
            padding=Padding(5, 5, 5, 5),
            child_gap=2,
            color="lightblue",
        )
        for i, child in enumerate(node.children):
            layout.add_child(scene_to_layout(child, state_path + (i,)))
        return layout

    # 2D or higher: render first dimension as rows
    rows = dims[0]
    sub_dims = dims[1:]
    total_children = len(node.children)
    cols = total_children // rows if rows else 0

    # cell_size = FIXED(60)
    layout = Layout(
        sizing=(FIT(), FIT()),
        direction=Direction.ROW,
        padding=Padding(5, 5, 5, 5),
        child_gap=3,
        color="lightblue",
    )
    

    for r in range(rows):
        row_layout = Layout(
            sizing=(FIT(), FIT()),
            direction=Direction.COLUMN,
            padding=Padding(3, 3, 3, 3),
            child_gap=2,
            color="lightblue",
        )
        for c in range(cols):
            idx = r * cols + c
            if idx >= total_children:
                break
            child_path = state_path + (idx,)
            child_node = node.children[idx]

            # Recurse: nested grid → nested layout
            if len(sub_dims) > 1:
                nested_layout = build_grid_layout(child_node, child_path)
                row_layout.add_child(nested_layout)
            else:
                row_layout.add_child(scene_to_layout(child_node, child_path))
        layout.add_child(row_layout)

    return layout

# def build_grid_layout(node, state_path):
#     """General grid builder for detected grid dimensions."""
#     children = node.children
#     rows = node.meta.get("rows", 0)
#     cols = node.meta.get("cols", 0)
#     if not rows or not cols:
#         return Layout(sizing=(FIT(), FIT()), direction=Direction.COLUMN)
#     layout = Layout(sizing=(FIT(), FIT()), direction=Direction.COLUMN, color="lightblue")
#     cell_size = FIXED(60)  # Adjustable based on game/type
#     for row in range(rows):
#         row_layout = Layout(sizing=(FIT(), cell_size), direction=Direction.ROW, padding=Padding(5,5,5,5), child_gap=2, color="lightblue")
#         for col in range(cols):
#             index = row * cols + col
#             child_path = state_path + (index,)
#             child = scene_to_layout(children[index], child_path)
#             row_layout.add_child(child)
#         layout.add_child(row_layout)
#     return layout

def scene_to_layout(node: SceneNode, state_path: Tuple[str, ...] = ()) -> Layout:
    """Transform a generic scene graph into a concrete layout for rendering."""
    if node.kind == "array":
        grid_dims = (node.meta.get("rows", 0), node.meta.get("cols", 0))
        if grid_dims[0] and grid_dims[0] > 0 and grid_dims[1] and grid_dims[1] > 0:
            print('found grid')
            return build_grid_layout(node, state_path)
        
        layout = Layout(sizing=(FIT(), FIT()), direction=Direction.COLUMN, child_gap=5, padding=Padding(5,5,5,5), color="pink", border=2)
        for i, child in enumerate(node.children):
            child_layout = scene_to_layout(child, state_path + (i,))
            layout.add_child(child_layout)
        return layout
    elif node.kind == "struct":
        layout = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW, child_gap=5, border=2, color="lightgray")
        for i in range(0, len(node.children), 2):  # Process label and value pairs
            if i + 1 < len(node.children) and node.children[i].kind == "label":
                label_node = node.children[i]
                value_node = node.children[i + 1]
                member_layout = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW, color="pink")
                if(label_node.value != "value"):
                    label_text = Text(str(label_node.value) + ": ", "Arial", 26, "black")
                    member_layout.add_child(label_text)
                value_layout = scene_to_layout(value_node, state_path + (i + 1,))
                member_layout.add_child(value_layout)
                layout.add_child(member_layout)
        return layout
    elif node.kind == "value":
        return Text(str(node.value), "Arial", 26, "black")
    elif node.kind == "cell":
        cell_layout = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW, padding=Padding(5, 5, 5, 5), color="darkblue")
        if node.children:
            child_layout = scene_to_layout(node.children[0], state_path)
            cell_layout.add_child(child_layout)
        return cell_layout
    else:
        # Fallback for unrecognized kinds
        return Layout(sizing=(FIT(), FIT()))
    

    
def print_scene(node: SceneNode, depth: int = 0, prefix: str = ""):
    """Print the scene graph in a hierarchical, readable format.
    
    Args:
        node: The SceneNode to print.
        depth: The current indentation level.
        prefix: The prefix for the current node (e.g., for multi-line values).
    """
    indent = "  " * depth
    meta_str = f", meta={node.meta}" if node.meta else ""
    print(f"{indent}{prefix}{node.kind} (value={node.value}{meta_str})")
    for child in node.children:
        print_scene(child, depth + 1)
from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
from typing import Type
from numbers import Number
from ctypes import c_long, Array, c_bool
from rlc.layout import Layout, Direction, FIT, Padding, GROW, FIXED
from rlc.text import Text
import pygame
from rlc.renderer.primitiveRenderer import PrimitiveRenderer
from rlc.renderer.array_renderer import ArrayRenderer
from rlc.renderer.vector_renderer import VectorRenderer
from rlc.renderer.struct_renderer import ContainerRenderer
from rlc.renderer.bint_renderer import BoundedIntRenderer
from test.display_layout import  render, PygameRenderer
from rlc import LayoutLogConfig, LayoutLogger
from rlc.scene_graph import state_to_scene, scene_to_layout, print_scene



def opposite_direction(current_direction):
    return Direction.ROW if current_direction == Direction.COLUMN else Direction.COLUMN

def create_layout_from_type(rlc_type, obj, direction=Direction.COLUMN, color="white", sizing=(FIT(), FIT()), logger=None):
    # Handle primitives
    if rlc_type == c_bool:
        return Text("True" if obj else "False", "Arial", 36, "black")
    if rlc_type == c_long:
        return Text(str(obj if isinstance(obj, int) else obj.value), "Arial", 36, "black")
    
    # Unwrap single-field containers
    typ, accessor = make_single_element_container_accessor(rlc_type)
    obj = accessor(obj)  # Apply accessor to get inner obj

    # Create container layout
    layout = Layout(sizing=sizing, direction=direction, child_gap=5, padding=Padding(5,5,5,5), color=color)
    # print(f"Layout '{rlc_type.__name__}', obj={obj}")
    if issubclass(typ, Array):
        # print(f"Processing array of length {typ._length_}")
        child_type = typ._type_
        if typ.__name__ == "BIntT0T3T_Array_9":  # Check for Board slots
            board_layout = Layout(sizing=(FIT(), FIT()), direction=Direction.COLUMN, child_gap=2, color="lightgray")
            # Create a 3x3 grid
            for row in range(3):  # 3 rows
                row_layout = Layout(sizing=(FIT(), FIXED(70)), direction=Direction.ROW,padding=Padding(5,5,5,5), child_gap=2, color="white")
                for col in range(3):  # 3 columns
                    index = row * 3 + col
                    item = obj[index] 
                    symbol = str(item.value)
                    cell_text = Text(symbol, "Arial", 48, "black")
                    cell_box = Layout(sizing=(FIT(), FIXED(60)), direction=Direction.ROW,padding=Padding(5,5,5,5), child_gap=10, color="white")
                    cell_box.add_child(cell_text)
                    row_layout.add_child(cell_box)
                board_layout.add_child(row_layout)
            layout.add_child(board_layout)
        else:
            for i in range(typ._length_):  # Use _length_ for fixed-size arrays
                item = obj[i]
                child = create_layout_from_type(child_type, item, opposite_direction(direction), logger=logger, color='lightgray', sizing=(FIXED(50), FIXED(50)))
                layout.add_child(child)
    if hasattr(typ, "_fields_"):  # Struct
        for field_name, field_type in typ._fields_:
            # Create a row for "name: value"
            member_layout = Layout(sizing=(FIT(), FIT()), direction=Direction.ROW, child_gap=5, color=None)  # No bg for label row
            label = Text(field_name + ": ", "Arial", 36, "black")
            member_layout.add_child(label)
            
            value = getattr(obj, field_name)
            child = create_layout_from_type(field_type, value, opposite_direction(direction), logger=logger)
            member_layout.add_child(child)
            
            layout.add_child(member_layout)
    
    return layout




def make_array_accessor(index):
    def access(obj):
        return obj[index]
    return access

def make_object_accessor(name):
    def access(obj):
        return getattr(obj, name)
    return access

def make_single_element_container_accessor(rlc_type, name=None):
    if not hasattr(rlc_type, "_fields_") or len(rlc_type._fields_) == 0:
        if name is None:
            return (rlc_type, lambda x: x)
        return (rlc_type, make_object_accessor(name))
    if len(rlc_type._fields_) > 1:  # Multi-field struct, return original
        return (rlc_type, lambda x: x)
    (name, typ) = rlc_type._fields_[0]
    accessor = make_object_accessor(name)
    while hasattr(typ, "_fields_") and len(typ._fields_) == 1:
        rlc_type = typ
        (name, typ) = rlc_type._fields_[0]
        newacc = lambda obj: make_object_accessor(name)(accessor(obj))
        accessor = newacc
    return (typ, accessor)

def dump_rlc_type(rlc_type: Type, depth=0):
    print("-" * depth, rlc_type.__name__)
    
    if issubclass(rlc_type, Array):
        return dump_rlc_type(rlc_type._type_, depth+1)
    if rlc_type == c_bool:
        return
    if rlc_type == c_long:
        return
    if hasattr(rlc_type, "_type_"):
        (typ, accessor) = make_single_element_container_accessor(rlc_type._type_)
        dump_rlc_type(accessor(typ), depth+1)
    if hasattr(rlc_type, "_fields_") :
        for field in rlc_type._fields_:
            dump_rlc_type(field[1], depth+1)

_renderer_cache = {}
def create_renderer(rlc_type, backend=None):
    if rlc_type in _renderer_cache:
        return _renderer_cache[rlc_type]
    
    name = getattr(rlc_type, "__name__", str(rlc_type))
    
    # Primitive 
    if rlc_type == c_bool or rlc_type == c_long:
        renderer = PrimitiveRenderer(rlc_type=rlc_type, backend=backend)
    
    # Bounded integer
    if name.startswith("BInt"):
        renderer = BoundedIntRenderer(rlc_type, backend)

    # Vector
    elif "Vector" in name:
        # find the _data field to determine element type
        data_field = next((f for f in getattr(rlc_type, "_fields_", []) if f[0] == "_data"), None)
        if data_field:
            element_type = getattr(data_field[1], "_type_", None)
            if element_type is None:
                element_type = data_field[1]
            element_renderer = create_renderer(element_type, backend)
            renderer = VectorRenderer(rlc_type, element_renderer, backend)
        else:
            renderer = PrimitiveRenderer(rlc_type, backend)

    
    # Array
    elif issubclass(rlc_type, Array):
        element_rendere = create_renderer(rlc_type._type_, backend)
        renderer = ArrayRenderer(rlc_type, element_rendere, backend)
    
    # Struct
    elif hasattr(rlc_type, "_fields_"):
        field_renderer = {
            name: create_renderer(field_type, backend) for name, field_type in rlc_type._fields_
        }
        renderer = ContainerRenderer(rlc_type, field_renderer, backend)
    else:
        renderer = PrimitiveRenderer(rlc_type=rlc_type, backend=backend)
    _renderer_cache[rlc_type] = renderer
    return renderer

if __name__ == "__main__":
    parser = make_rlc_argparse("game_display", description="Display game state")
    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:
        state = program.start()

        # dump_rlc_type(program.module.Game)
        # print(f"State object: {state.state}")

        logger = LayoutLogger(LayoutLogConfig())
        logger = None
        
        # layout.print_layout()
        # root = create_layout_from_type(program.module.Game, state.state, logger=logger)
        # scene_root = state_to_scene(state=state.state, typ=program.module.Game)
        # print("=== Abstract Scene Graph (Reusable Structure) ===")
        # print_scene(scene_root)
        # print("=== End Scene Graph ===\n")
        # root = scene_to_layout(scene_root)
        # print(f"Root size: {root.width}x{root.height}, children={[c.height for c in root.children]}")
        
        
        pygame.init()  
        screen = pygame.display.set_mode((1280, 720))
        screen.fill("white")
        clock = pygame.time.Clock()
        running = True
        backend = PygameRenderer(screen)

        renderer = create_renderer(program.module.Game)
        renderer.print_tree()
        layout = renderer(state.state)

        layout.compute_size(logger=logger, backend=backend)
        layout.layout(20, 20, logger=logger)
        print(f"Root size: {layout.width}x{layout.height}, children={[c.height for c in layout.children]}")
        if logger: 
            logger.record_final_tree(root=layout)
            # print(logger.to_text_tree(layout))
        
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
            
            
            render(backend, layout)
            pygame.display.flip()
            clock.tick(60)
        
        pygame.quit()


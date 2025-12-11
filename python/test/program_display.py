from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
from typing import Type
from typing import Dict
from ctypes import c_long, Array, c_bool
import pygame, time, random
from python.rlc.renderer.primitive_renderer import PrimitiveRenderer
from rlc.renderer.array_renderer import ArrayRenderer
from rlc.renderer.vector_renderer import VectorRenderer
from rlc.renderer.struct_renderer import ContainerRenderer
from rlc.renderer.bint_renderer import BoundedIntRenderer
from test.display_layout import  render, PygameRenderer
from rlc import LayoutLogConfig, LayoutLogger
from test.red_board_renderer import RedBoard
import json


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
def create_renderer(rlc_type, config : Dict[type, type]):
    if rlc_type in _renderer_cache:
        return _renderer_cache[rlc_type]
    
    name = getattr(rlc_type, "__name__", str(rlc_type))

    if rlc_type in config:
        pytyp = config.get(rlc_type)
        if hasattr(rlc_type, "_fields_"):
            field_renderer = {
                name: create_renderer(field_type, config) for name, field_type in rlc_type._fields_
            }
            renderer = pytyp(rlc_type, field_renderer)
            return renderer

    # Vector
    elif "Vector" in name:
        # find the _data field to determine element type
        data_field = next((f for f in getattr(rlc_type, "_fields_", []) if f[0] == "_data"), None)
        if data_field:
            element_type = getattr(data_field[1], "_type_", None)
            if element_type is None:
                element_type = data_field[1]
            element_renderer = create_renderer(element_type, config)
            renderer = VectorRenderer(name, element_renderer)
        else:
            renderer = PrimitiveRenderer(name)

    
    # Array
    elif hasattr(rlc_type, "_length_") and hasattr(rlc_type, "_type_"):
        element_rendere = create_renderer(rlc_type._type_, config)
        renderer = ArrayRenderer(name,rlc_type._length_, element_rendere)
    
    # Primitive 
    elif rlc_type == c_bool or rlc_type == c_long:
        renderer = PrimitiveRenderer(name)
    
    # Bounded integer
    elif name.startswith("BInt"):
        renderer = BoundedIntRenderer(name)

    # Struct
    elif hasattr(rlc_type, "_fields_"):
        field_renderer = {
            name: create_renderer(field_type, config) for name, field_type in rlc_type._fields_
        }
        renderer = ContainerRenderer(name, field_renderer)
    else:
        renderer = PrimitiveRenderer(name)
    _renderer_cache[rlc_type] = renderer
    return renderer

def any_child_dirty(layout):
    if getattr(layout, "is_dirty", False):
        layout.is_dirty = False
        return True
    return any(any_child_dirty(c) for c in layout.children if hasattr(c, "children"))


if __name__ == "__main__":
    parser = make_rlc_argparse("game_display", description="Display game state")
    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:

        pygame.init()  
        screen = pygame.display.set_mode((1280, 720))
        screen.fill("white")
        clock = pygame.time.Clock()
        backend = PygameRenderer(screen)
        running = True

        dump_rlc_type(program.module.Game)

        config = {
            program.module.RedBoard : RedBoard
        }

        renderer = create_renderer(program.module.Game, config)
        renderer.print_tree()
        json_str = json.dumps(renderer.to_dict(), indent=4)
        print(json_str)
        iterations = -1
        current = 0
        STEP_DELAY = 0.9  # seconds per state
        logger = LayoutLogger(LayoutLogConfig())
        logger = None
        state = None


        while running and current < iterations:
            print(f"\n=== Iteration {current + 1}/{iterations} ===")
            if hasattr(state, "reset"):
                state.reset()
            else:
                state = program.start()
            layout = renderer(state.state)
            actions = state.legal_actions
            layout.compute_size(logger=logger, backend=backend)
            layout.layout(20, 20, logger=logger)

            if logger: 
                logger.record_final_tree(root=layout)
                # print(logger.to_text_tree(layout))
        
            last_update = time.time()
            accumulated_time = 0.0
            while running:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        running = False

                elapsed = clock.tick(60) / 1000.0
                accumulated_time += elapsed
                
                if accumulated_time >= STEP_DELAY:
                    accumulated_time = 0.0
                    if not state.is_done():
                        actions = state.legal_actions
                        if len(actions) != 0:
                            action = random.choice(actions)
                            state.step(action)
                            # state.state.place(program.module.make_num(4), program.module.make_pos(0), program.module.make_pos(0))
                            new_state = state.state
                            print(action, len(actions))
                            renderer.update(layout, new_state, elapsed)
                            if layout.is_dirty or any_child_dirty(layout):
                                layout.compute_size(logger=logger, backend=backend)
                                layout.layout(20, 20, logger=logger)
                        else:
                           print("No legal actions left.")
                           break
                    else:
                        print("Game done.")
                        break
                screen.fill("white")
                render(backend, layout)
                pygame.display.flip()
            current += 1
            time.sleep(1.0)
        
    pygame.quit()


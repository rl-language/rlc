from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
from rlc.renderer.factory import RendererFactory
import pygame, time, random
from test.display_layout import  render, PygameRenderer
from rlc import LayoutLogConfig, LayoutLogger
from test.event_dispatcher import EventDispatcher
from simulate import new_timing_bucket, relayout, any_child_dirty, print_timings

def key_press(key, node, state, program):
    # key is pygame keycode, convert to digit
    print(key)
    if pygame.K_h == key :
        state.state.hit()
    elif pygame.K_s == key:
        state.state.stand()
    else:
        return False
    return True


if __name__ == "__main__":
    parser = make_rlc_argparse("game_display", description="Display game state")
    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:

        config = {}
        renderer = RendererFactory.from_rlc_type(program.module.Game, config)

        pygame.init()  
        screen = pygame.display.set_mode((1280, 720), pygame.RESIZABLE)
        screen.fill("white")
        clock = pygame.time.Clock()
        backend = PygameRenderer(screen)
        running = True
        
        renderer.print_tree()
        iterations = 1
        current = 0
        STEP_DELAY = 0.9  # seconds per state
        logger = LayoutLogger(LayoutLogConfig())
        logger = None
        state = None
        scroll = {"x": 0, "y": 0}


        while running and current < iterations:
            compute_times = new_timing_bucket()
            layout_times = new_timing_bucket()
            print(f"\n=== Iteration {current + 1}/{iterations} ===")
            if hasattr(state, "reset"):
                state.reset()
            else:
                state = program.start()
            layout = renderer(state.state)
            layout.propagate_interactive()
            actions = state.legal_actions
            relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)

            if logger: 
                logger.record_final_tree(root=layout)

            handlers = {}
            selection = None
            dispatcher = EventDispatcher(state, renderer, layout, program, selection, handlers)
        
            last_update = time.time()
            accumulated_time = 0.0
            elapsed = 0.0
            while running:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        running = False
                    if event.type == pygame.VIDEORESIZE:
                        screen = pygame.display.set_mode((event.w, event.h), pygame.RESIZABLE)
                        backend = PygameRenderer(screen)
                        relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)
                    if event.type == pygame.MOUSEWHEEL:
                        # y is vertical wheel, x is horizontal wheel; positive y = scroll up
                        scroll["y"] += event.y * 30
                        scroll["x"] += event.x * 30
                        relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)
                    if event.type == pygame.MOUSEBUTTONDOWN:
                        mx, my = pygame.mouse.get_pos()
                        target = layout.find_target(mx, my)
                        if target:
                            changed = dispatcher.handle_click(target, elapsed_time=elapsed)
                            if changed:
                                layout.is_dirty = True
                            if layout.is_dirty or any_child_dirty(layout):
                                relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)
                    if event.type == pygame.KEYDOWN:
                        changed = dispatcher.handle_key(event.key, handler=key_press)
                        if changed:
                            layout.is_dirty = True
                        if layout.is_dirty or any_child_dirty(layout):
                            relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)

                elapsed = clock.tick(60) / 1000.0
                accumulated_time += elapsed
                
                if accumulated_time >= STEP_DELAY:
                    accumulated_time = 0.0
                    if not state.is_done():
                        if state.state.shuffling.to_shuffle > 0:
                            dispatcher.play_random_turn(elapsed_time=elapsed)
                        else:
                            pass
                    else:
                        print("Game done.")
                        break
                screen.fill("white")
                render(backend, layout)
                pygame.display.flip()
            current += 1
            print_timings(f"iteration {current}", compute_times, layout_times)
            time.sleep(1.0)
        
    pygame.quit()
        
import os
from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
import pygame, time, random
from test.display_layout import  render, PygameRenderer
from rlc import LayoutLogConfig, LayoutLogger
from rlc.serialization.renderer_serializer import load_renderer

  
def any_child_dirty(layout):
    if getattr(layout, "is_dirty", False):
        layout.is_dirty = False
        return True
    return any(any_child_dirty(c) for c in layout.children if hasattr(c, "children"))

def _new_timing_bucket():
    # count, total_seconds, max_seconds
    return {"count": 0, "total": 0.0, "max": 0.0}

def _record_timing(bucket, elapsed):
    bucket["count"] += 1
    bucket["total"] += elapsed
    bucket["max"] = max(bucket["max"], elapsed)

def _print_timings(label, compute_bucket, layout_bucket):
    def fmt(b):
        if b["count"] == 0:
            return "0 runs"
        avg = (b["total"] / b["count"]) * 1000
        return f"{b['count']} runs | avg {avg:.3f} ms | max {b['max']*1000:.3f} ms"
    print(f"[timing] {label} | compute_size: {fmt(compute_bucket)} | layout: {fmt(layout_bucket)}")

def _clamp_scroll(layout, screen, scroll, margin):
    view_w = max(0, screen.get_width() - 2 * margin)
    view_h = max(0, screen.get_height() - 2 * margin)
    max_x = max(0, layout.width - view_w)
    max_y = max(0, layout.height - view_h)
    scroll["x"] = min(0, max(-max_x, scroll["x"]))
    scroll["y"] = min(0, max(-max_y, scroll["y"]))

def _relayout(screen, backend, layout, logger, compute_times, layout_times, scroll, margin=20):
    """Resize-aware layout: fit inside the window minus a margin and apply scroll offsets."""
    avail_w = max(0, screen.get_width() - 2 * margin)
    avail_h = max(0, screen.get_height() - 2 * margin)
    t0 = time.perf_counter()
    layout.compute_size(available_width=avail_w, available_height=avail_h, logger=logger, backend=backend)
    _record_timing(compute_times, time.perf_counter() - t0)
    _clamp_scroll(layout, screen, scroll, margin)
    t0 = time.perf_counter()
    layout.layout(margin + scroll["x"], margin + scroll["y"], logger=logger)
    _record_timing(layout_times, time.perf_counter() - t0)


if __name__ == "__main__":
    parser = make_rlc_argparse("game_display", description="Display game state")
    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:

        pygame.init()  
        screen = pygame.display.set_mode((1280, 720), pygame.RESIZABLE)
        screen.fill("white")
        clock = pygame.time.Clock()
        backend = PygameRenderer(screen)
        running = True
        source_file = args.source_file 
        base_name = os.path.splitext(os.path.basename(source_file))[0] if source_file else "renderer"
        load_path = os.path.join("./logs", f"{base_name}.json")

        renderer = load_renderer(load_path)
        renderer.print_tree()
        iterations = 1
        current = 0
        STEP_DELAY = 0.9  # seconds per state
        logger = LayoutLogger(LayoutLogConfig())
        logger = None
        state = None
        scroll = {"x": 0, "y": 0}


        while running and current < iterations:
            compute_times = _new_timing_bucket()
            layout_times = _new_timing_bucket()
            print(f"\n=== Iteration {current + 1}/{iterations} ===")
            if hasattr(state, "reset"):
                state.reset()
            else:
                state = program.start()
            layout = renderer(state.state)
            actions = state.legal_actions
            _relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)

            if logger: 
                logger.record_final_tree(root=layout)
                # print(logger.to_text_tree(layout))
        
            last_update = time.time()
            accumulated_time = 0.0
            while running:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        running = False
                    if event.type == pygame.VIDEORESIZE:
                        screen = pygame.display.set_mode((event.w, event.h), pygame.RESIZABLE)
                        backend = PygameRenderer(screen)
                        _relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)
                    if event.type == pygame.MOUSEWHEEL:
                        # y is vertical wheel, x is horizontal wheel; positive y = scroll up
                        scroll["y"] += event.y * 30
                        scroll["x"] += event.x * 30
                        _relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)

                elapsed = clock.tick(60) / 1000.0
                accumulated_time += elapsed
                
                if accumulated_time >= STEP_DELAY:
                    accumulated_time = 0.0
                    if not state.is_done():
                        actions = state.legal_actions
                        if len(actions) != 0:
                            action = random.choice(actions)
                            state.step(action)
                            new_state = state.state
                            # print(action, len(actions))
                            # print(new_state)
                            renderer.update(layout, new_state, elapsed)
                            if layout.is_dirty or any_child_dirty(layout):
                                _relayout(screen, backend, layout, logger, compute_times, layout_times, scroll)
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
            _print_timings(f"iteration {current}", compute_times, layout_times)
            time.sleep(1.0)
        
    pygame.quit()

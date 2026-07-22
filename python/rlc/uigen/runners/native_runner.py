import time

from command_line import load_program_from_args, make_rlc_argparse
from rlc.uigen.view_types.factory import ViewTypeTreeFactory
from rlc.uigen.view_types.interaction_context import InteractionContext
from rlc.uigen.view_types.dispatch_actions import ActionGate, dispatch_action as rlc_dispatch
from rlc.uigen.ui_dispatch import resolve_click, resolve_key, FOCUS
from rlc.uigen.event_queue import _any_child_dirty
from rlc.uigen.render import render, relayout
from rlc.uigen.game_config import GameConfig
from rlc.uigen.window_events import QuitEvent, ResizeEvent, ScrollEvent, ClickEvent, KeyEvent


def run(cfg: GameConfig, window, backend, args):
    with load_program_from_args(args) as program:
        state = program.start()

        pyobject_slots = ViewTypeTreeFactory.collect_pyobject_slots(program.module.Game)
        ViewTypeTreeFactory.install_noop_callbacks(state.state, pyobject_slots)

        cfg.run_policy(state, program)

        wires = ViewTypeTreeFactory.collect_callback_wires(program.module.Game)

        renderer = ViewTypeTreeFactory.from_rlc_type(
            program.module.Game, cfg.renderer_config,
            interaction_ctx=InteractionContext(config_rules=cfg.interactions),
            rlc_path=["Game"],
        )
        layout = renderer(state.state, parent_path=[])

        ViewTypeTreeFactory.apply_callback_wires(wires, state.state, renderer)

        gate = ActionGate(state)
        scroll = {"x": 0, "y": 0}

        def do_relayout():
            relayout(window.size, backend, layout, scroll)

        gate.apply(layout)
        do_relayout()

        running = True

        while running:
            needs_relayout = False

            for event in window.pump_events():
                if isinstance(event, QuitEvent):
                    running = False
                elif isinstance(event, ResizeEvent):
                    backend = window.make_renderer()
                    needs_relayout = True
                elif isinstance(event, ScrollEvent):
                    scroll["x"] += event.dx
                    scroll["y"] += event.dy
                    needs_relayout = True
                elif isinstance(event, ClickEvent) and not state.is_done():
                    target, intent = resolve_click(layout, event.x, event.y,
                                                   cfg.click_mode == "focus")
                    if intent is FOCUS:
                        layout.set_focus(target)
                        needs_relayout = True
                    elif intent is not None:
                        rlc_dispatch(intent.handler, program, state, intent.args)
                        needs_relayout = True
                elif isinstance(event, KeyEvent) and cfg.handles_keys and not state.is_done():
                    _focused, intent = resolve_key(layout, event.char)
                    if intent is not None:
                        rlc_dispatch(intent.handler, program, state, intent.args)
                        needs_relayout = True

            if cfg.policy is not None:
                before = str(state.state)
                cfg.run_policy(state, program)
                if str(state.state) != before:
                    needs_relayout = True

            gate.apply(layout)

            if needs_relayout or _any_child_dirty(layout):
                do_relayout()

            window.tick(60)
            window.begin_frame()
            render(backend, layout)
            window.end_frame()

            if state.is_done():
                time.sleep(2.0)
                running = False

        window.close()

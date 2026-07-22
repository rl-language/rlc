import time

from command_line import load_program_from_args, make_rlc_argparse
from rlc.uigen.view_types.factory import ViewTypeTreeFactory
from rlc.uigen.view_types.interaction_context import InteractionContext
from rlc.uigen.view_types.dispatch_actions import ActionGate, dispatch_action as rlc_dispatch
from rlc.uigen.ui_dispatch import resolve_click, resolve_key, FOCUS
from rlc.uigen.event_queue import UpdateController, UpdateSignal, SignalKind
from rlc.uigen.render import render, relayout
from rlc.uigen.game_config import GameConfig
from rlc.uigen.window_events import QuitEvent, ResizeEvent, ScrollEvent, ClickEvent, KeyEvent


def run(cfg: GameConfig, window, backend, args):
    with load_program_from_args(args, optimize=True) as program:
        interaction_ctx = InteractionContext(config_rules=cfg.interactions)
        renderer = ViewTypeTreeFactory.from_rlc_type(
            program.module.Game, cfg.renderer_config,
            interaction_ctx=interaction_ctx,
            rlc_path=["Game"],
        )

        state = program.start()

        pyobject_slots = ViewTypeTreeFactory.collect_pyobject_slots(program.module.Game)
        ViewTypeTreeFactory.install_noop_callbacks(state.state, pyobject_slots)

        cfg.run_policy(state, program)

        layout = renderer(state.state, parent_path=[])

        gate = ActionGate(state)

        def dispatch_action(handler_name, action_args):
            return rlc_dispatch(handler_name, program, state, action_args)

        def do_relayout():
            relayout(window.size, backend, layout, controller.scroll)

        controller = UpdateController(
            renderer, layout, do_relayout, dispatch_action,
            state_obj=state.state,
            program_module=program.module,
            queued=True,
        )
        gate.apply(layout)
        do_relayout()

        running = True

        while running:
            for event in window.pump_events():
                if isinstance(event, QuitEvent):
                    running = False
                elif isinstance(event, ResizeEvent):
                    backend = window.make_renderer()
                    controller.enqueue(UpdateSignal(kind=SignalKind.RESIZE))
                elif isinstance(event, ScrollEvent):
                    controller.enqueue(UpdateSignal(
                        kind=SignalKind.SCROLL, dy=event.dy, dx=event.dx))
                elif isinstance(event, ClickEvent):
                    target, intent = resolve_click(layout, event.x, event.y,
                                                   cfg.click_mode == "focus")
                    if intent is FOCUS:
                        layout.set_focus(target)
                    elif intent is not None:
                        controller.enqueue(UpdateSignal(
                            kind=SignalKind.ACTION,
                            handler_name=intent.handler, args=intent.args))
                elif isinstance(event, KeyEvent) and cfg.handles_keys:
                    _focused, intent = resolve_key(layout, event.char)
                    if intent is not None:
                        controller.enqueue(UpdateSignal(
                            kind=SignalKind.ACTION,
                            handler_name=intent.handler, args=intent.args))

            elapsed = window.tick(60)

            controller.process(state.state, elapsed)
            cfg.run_policy(state, program)
            controller.notify_state_changed()
            controller.process(state.state, elapsed)

            gate.apply(layout)

            window.begin_frame()
            render(backend, layout)
            window.end_frame()

            if state.is_done():
                time.sleep(6.0)
                running = False

        controller.cleanup()
        window.close()

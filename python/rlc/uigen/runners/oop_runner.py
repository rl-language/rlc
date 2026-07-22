import argparse
import os
import time

from command_line import load_program_from_args, make_rlc_argparse
from rlc.uigen.view_types.factory import ViewTypeTreeFactory
from rlc.uigen.view_types.interaction_context import InteractionContext
from rlc.uigen.view_types.dispatch_actions import ActionGate, dispatch_action as rlc_dispatch
from rlc.uigen.ui_dispatch import resolve_click, resolve_key, FOCUS
from rlc.uigen.event_queue import _rlc_string_to_python, _refresh_visibility
from rlc.uigen.render import render, relayout
from rlc.uigen.game_config import GameConfig
from rlc.uigen.runners.oop_protocol import (
    Transport, connect, listen_one, walk_layout, resolve_value,
)
from rlc.uigen.window_events import QuitEvent, ResizeEvent, ScrollEvent, ClickEvent, KeyEvent


def _handle_action(gate, program, state, name, action_args):
    if gate.can_apply(name, action_args) is False:
        return False, "precondition not satisfied"
    try:
        ok = rlc_dispatch(name, program, state, action_args)
        if ok is False:
            return False, "precondition not satisfied"
        return True, None
    except Exception as e:
        return False, str(e)


def run_sim(cfg: GameConfig, transport: Transport, args):
    print(f"[sim pid={os.getpid()}] starting", flush=True)

    with load_program_from_args(args) as program:
        state = program.start()

        pyobject_slots = ViewTypeTreeFactory.collect_pyobject_slots(program.module.Game)
        ViewTypeTreeFactory.install_noop_callbacks(state.state, pyobject_slots)

        cfg.run_policy(state, program)

        gate = ActionGate(state)

        def broadcast_state():
            transport.send({"type": "state", "blob": state.as_byte_vector()})
            transport.send({"type": "valid_actions",
                            "set": [str(a) for a in state.legal_actions]})

        broadcast_state()

        running = True

        while running:
            msg = transport.recv(timeout=0.05)
            if msg is not None:
                kind = msg.get("type")
                if kind == "shutdown":
                    running = False
                    continue
                if kind == "action":
                    aid = msg.get("id")
                    ok, err = _handle_action(gate, program, state,
                                             msg["name"], msg.get("args", {}))
                    transport.send({"type": "ack", "id": aid, "ok": ok, "error": err})
                    if ok:
                        broadcast_state()

            before_policy = str(state.state)
            cfg.run_policy(state, program)
            if str(state.state) != before_policy:
                broadcast_state()

            if state.is_done():
                transport.send({"type": "game_over"})

    print(f"[sim pid={os.getpid()}] exit", flush=True)


def run_ui(cfg: GameConfig, window, backend, transport: Transport, args):
    print(f"[ui pid={os.getpid()}] starting", flush=True)

    program = load_program_from_args(args)
    state = program.start()
    ViewTypeTreeFactory.install_noop_callbacks(
        state.state, ViewTypeTreeFactory.collect_pyobject_slots(program.module.Game))

    renderer = ViewTypeTreeFactory.from_rlc_type(
        program.module.Game, cfg.renderer_config,
        interaction_ctx=InteractionContext(config_rules=cfg.interactions),
        rlc_path=["Game"],
    )
    layout = renderer(state.state, parent_path=[])
    gate = ActionGate(state)
    valid_keys = set()
    last_state = state.state.clone()

    def apply_path(path):
        if path[-1] == "#":
            node = walk_layout(layout, path[:-1])
            if node is not None and getattr(node, "resize_fn", None):
                node.resize_fn(resolve_value(state.state, path))
            return
        prefix = path
        while prefix:
            node = walk_layout(layout, prefix)
            if node is not None and getattr(node, "update_fn", None):
                node.update_fn(resolve_value(state.state, prefix))
                return
            prefix = prefix[:-1]

    def apply_state(blob):
        nonlocal last_state
        state.load_byte_vector(blob)
        changed = program.module.VectorTStringT()
        program.module.diff(last_state, state.state, changed)
        for i in range(changed.size()):
            path_str = _rlc_string_to_python(changed.get(i).contents)
            path = tuple(int(p) if p.lstrip("-").isdigit() else p
                         for p in path_str.split(".") if p)
            if path:
                apply_path(path)
        last_state = state.state.clone()

    scroll = {"x": 0, "y": 0}

    def do_relayout():
        relayout(window.size, backend, layout, scroll)

    do_relayout()

    next_action_id = 0
    running = True
    game_over = False

    while running:
        needs_relayout = False

        while True:
            msg = transport.recv_nowait()
            if msg is None:
                break
            kind = msg["type"]
            if kind == "state":
                apply_state(msg["blob"])
                needs_relayout = True
            elif kind == "valid_actions":
                valid_keys = ActionGate.parse_valid_strings(msg["set"])
            elif kind == "ack":
                if not msg["ok"]:
                    print(f"[ui] action {msg['id']} rejected: {msg.get('error')}", flush=True)
            elif kind == "game_over":
                game_over = True

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
            elif isinstance(event, ClickEvent) and not game_over:
                target, intent = resolve_click(layout, event.x, event.y,
                                               cfg.click_mode == "focus")
                if intent is FOCUS:
                    layout.set_focus(target)
                    needs_relayout = True
                elif intent is not None:
                    transport.send({"type": "action", "id": next_action_id,
                                    "name": intent.handler, "args": intent.args})
                    next_action_id += 1
            elif isinstance(event, KeyEvent) and not game_over and cfg.handles_keys:
                _focused, intent = resolve_key(layout, event.char)
                if intent is not None:
                    transport.send({"type": "action", "id": next_action_id,
                                    "name": intent.handler, "args": intent.args})
                    next_action_id += 1

        gate.apply_valid(layout, valid_keys)

        if _refresh_visibility(layout, state.state):
            needs_relayout = True

        if needs_relayout:
            do_relayout()

        window.tick(60)
        window.begin_frame()
        render(backend, layout)
        window.end_frame()

        if game_over:
            time.sleep(2.0)
            running = False

    transport.send({"type": "shutdown"})
    del layout
    window.close()


def _parse_endpoint(spec: str):
    if ":" not in spec:
        raise argparse.ArgumentTypeError(f"expected host:port, got {spec!r}")
    host, port = spec.rsplit(":", 1)
    return host, int(port)


def sim_main(cfg: GameConfig):
    parser = make_rlc_argparse(cfg.title + " sim", description=cfg.title + " (sim)")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=5000)
    args = parser.parse_args()

    print(f"[sim] listening on {args.host}:{args.port}", flush=True)
    transport = listen_one(args.host, args.port)
    print("[sim] UI connected", flush=True)
    try:
        run_sim(cfg, transport, args)
    finally:
        transport.close()
    os._exit(0)


def ui_main(cfg: GameConfig, window, backend):
    parser = make_rlc_argparse(cfg.title + " UI", description=cfg.title + " (UI)")
    parser.add_argument("--connect", required=True, type=_parse_endpoint,
                        help="Sim endpoint as host:port")
    args = parser.parse_args()

    host, port = args.connect
    print(f"[ui] connecting to {host}:{port}", flush=True)
    transport = connect(host, port)
    print("[ui] connected", flush=True)
    try:
        run_ui(cfg, window, backend, transport, args)
    finally:
        transport.close()
    os._exit(0)

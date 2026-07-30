import os
import socket
import subprocess
import sys
import threading
import time

from rlc.uigen.runners import oop_runner
from rlc.uigen.runners.oop_protocol import connect


def _free_port():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("127.0.0.1", 0))
    port = s.getsockname()[1]
    s.close()
    return port
from rlc.uigen.window_events import ClickEvent, KeyEvent
from rlc.uigen.view_types.dispatch_actions import ActionGate

from test.uigen.fuzz.deployments.window import HeadlessWindow
from test.uigen.fuzz.util.state_signature import signature
from test.uigen.fuzz.util.layout_geometry import (
    find_enabled_clickable_nodes, node_pixel_bounds, interior_pixel,
)


class _UiSession:
    def __init__(self):
        self.state = None
        self.layout = None
        self.state_ready = threading.Event()
        self.layout_ready = threading.Event()

    def capture_state(self, state):
        self.state = state
        self.state_ready.set()

    def capture_layout(self, layout):
        self.layout = layout
        self.layout_ready.set()


class _CaptureTarget:
    session = None
    program = None
    state = None


_CAPTURE = _CaptureTarget()


class _ProgramProxy:
    def __init__(self, program, state):
        self._program = program
        self._state = state

    def start(self):
        return self._state

    def __getattr__(self, name):
        return getattr(self._program, name)


def _patch_ui_hooks():
    real_render = oop_runner.render
    real_loader = oop_runner.load_program_from_args
    real_time = oop_runner.time

    def capturing_render(backend, node):
        if _CAPTURE.session is not None:
            _CAPTURE.session.capture_layout(node)
        return real_render(backend, node)

    def reusing_loader(*a, **k):
        if _CAPTURE.program is None:
            _CAPTURE.program = real_loader(*a, **k)
            _CAPTURE.state = _CAPTURE.program.start()
        if _CAPTURE.session is not None:
            _CAPTURE.session.capture_state(_CAPTURE.state)
        return _ProgramProxy(_CAPTURE.program, _CAPTURE.state)

    class _NoSleep:
        def sleep(self, _):
            return None

        def __getattr__(self, name):
            return getattr(real_time, name)

    oop_runner.render = capturing_render
    oop_runner.load_program_from_args = reusing_loader
    oop_runner.time = _NoSleep()

    def restore():
        oop_runner.render = real_render
        oop_runner.load_program_from_args = real_loader
        oop_runner.time = real_time
        _CAPTURE.session = None
    return restore


class OopFuzzer:
    cheap_restore = False

    def __init__(self, cfg, args, rl_source, game, *, host="127.0.0.1",
                 port=5099, window_size=(1280, 720)):
        self.cfg = cfg
        self.args = args
        self.rl_source = rl_source
        self.game = game
        self.host = host
        self.port = port
        self.window = HeadlessWindow(window_size)
        self.session = _UiSession()
        self._sim = None
        self._thread = None
        self._restore = None
        self._transport = None
        self._valid_keys = set()

    def start(self):
        self._path = []
        self._restore = _patch_ui_hooks()
        self._spawn_and_connect()

    def _spawn_and_connect(self):
        self.port = _free_port()
        self._sim = subprocess.Popen(
            [sys.executable, "-m", "test.uigen.run", self.game,
             "--mode", "oop-sim", self.rl_source,
             "--host", self.host, "--port", str(self.port)],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            env={**os.environ, "PYTHONPATH": os.getcwd()},
        )
        self._transport = self._wait_connect()
        _CAPTURE.session = self.session

        backend = self.window.make_renderer()

        def target():
            oop_runner.run_ui(self.cfg, self.window, backend,
                              self._transport, self.args)

        self._thread = threading.Thread(target=target, daemon=True)
        self._thread.start()
        if not self.session.state_ready.wait(timeout=15):
            raise RuntimeError("UI state never captured (program load stalled?)")
        self.window.advance_frame([])
        if not self.session.layout_ready.wait(timeout=15):
            raise RuntimeError("UI layout never rendered")
        self._await_enabled()

    def _await_enabled(self, max_frames=60):
        for _ in range(max_frames):
            if self.enabled_clickable_nodes():
                return
            self.window.advance_frame([])
            time.sleep(0.01)

    def _wait_connect(self, timeout=30):
        deadline = time.time() + timeout
        while time.time() < deadline:
            try:
                return connect(self.host, self.port)
            except OSError:
                time.sleep(0.1)
        raise RuntimeError("could not connect to sim")

    def is_done(self):
        return self.session.state.state.resume_index == -1

    def snapshot(self):
        return tuple(self._path)

    def restore(self, path):
        self._restart_sim()
        for px in path:
            self.click(*px)
            self._settle()
        self._path = list(path)

    def _restart_sim(self):
        self._teardown_ui_and_sim()
        self.window = HeadlessWindow(self.window.size)
        self.session = _UiSession()
        self._path = []
        self._spawn_and_connect()

    def _teardown_ui_and_sim(self):
        self.window.request_quit()
        if self._thread:
            self._thread.join(timeout=5.0)
        if self._transport:
            self._transport.close()
        if self._sim:
            self._sim.terminate()
            try:
                self._sim.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self._sim.kill()

    def settle(self, max_frames=12):
        return self._settle(max_frames)

    def restart(self):
        self._restart_sim()
        self._await_enabled()

    def _settle(self, max_frames=12):
        last = self.current_state_signature()
        stable = 0
        for _ in range(max_frames):
            self.window.advance_frame([])
            time.sleep(0.01)
            now = self.current_state_signature()
            if now == last:
                stable += 1
                if stable >= 3:
                    break
            else:
                stable = 0
            last = now
        return self.current_state_signature()

    def current_state_signature(self):
        return signature(self.session.state)

    def enabled_clickable_nodes(self):
        return find_enabled_clickable_nodes(self.session.layout)

    def pixel_for_node(self, node):
        bounds = node_pixel_bounds(node)
        return interior_pixel(bounds) if bounds else None

    def click(self, x, y):
        self.window.advance_frame([ClickEvent(x, y)])

    def press_key(self, char):
        self.window.advance_frame([KeyEvent(0, char)])

    def perform(self, interaction):
        px = self.pixel_for_node(interaction.node)
        if px is None:
            return
        self.click(*px)
        if interaction.key_char is not None:
            self.press_key(interaction.key_char)

    def stop(self):
        self._teardown_ui_and_sim()
        if self._restore:
            self._restore()

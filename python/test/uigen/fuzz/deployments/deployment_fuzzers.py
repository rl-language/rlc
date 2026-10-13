import threading
from contextlib import contextmanager

from command_line import load_program_from_args
from rlc.uigen.window_events import ClickEvent, KeyEvent
from rlc.uigen.runners import inproc_runner, native_runner
from rlc.uigen.view_types.factory import ViewTypeTreeFactory

from test.uigen.fuzz.deployments.window import HeadlessWindow
from test.uigen.fuzz.util.state_signature import signature
from test.uigen.fuzz.util.layout_geometry import (
    find_enabled_clickable_nodes, node_pixel_bounds, interior_pixel,
)
from test.uigen.fuzz.util.action_index import legal_action_keys_of_state


class _CapturedSession:
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


class _ProgramProxy:
    def __init__(self, program, session):
        self._program = program
        self._session = session

    def start(self):
        state = self._program.start()
        state.is_done = lambda: False
        self._session.capture_state(state)
        return state

    def __getattr__(self, name):
        return getattr(self._program, name)


@contextmanager
def loaded_program(args):
    with load_program_from_args(args, optimize=True) as program:
        yield program


class _NoSleepTime:
    def __init__(self, real):
        self._real = real

    def sleep(self, _seconds):
        return None

    def __getattr__(self, name):
        return getattr(self._real, name)


def _patch(runner_module, session, shared_program):
    real_render = runner_module.render
    real_loader = runner_module.load_program_from_args
    real_time = runner_module.time

    def capturing_render(backend, node):
        session.capture_layout(node)
        return real_render(backend, node)

    @contextmanager
    def reusing_loader(*args, **kwargs):
        yield _ProgramProxy(shared_program, session)

    runner_module.render = capturing_render
    runner_module.load_program_from_args = reusing_loader
    runner_module.time = _NoSleepTime(real_time)

    def restore():
        runner_module.render = real_render
        runner_module.load_program_from_args = real_loader
        runner_module.time = real_time

    return restore


class LocalDeploymentFuzzer:
    runner_module = None
    cheap_restore = True

    def __init__(self, cfg, args, program, *, window_size=(1280, 720)):
        self.cfg = cfg
        self.args = args
        self.program = program
        self.window = HeadlessWindow(window_size)
        self.session = _CapturedSession()
        self._thread = None
        self._restore = None

    def start(self):
        self._restore = _patch(self.runner_module, self.session, self.program)
        self._slots = ViewTypeTreeFactory.collect_pyobject_slots(
            self.program.module.Game)
        backend = self.window.make_renderer()

        def target():
            self.runner_module.run(self.cfg, self.window, backend, self.args)

        self._thread = threading.Thread(target=target, daemon=True)
        self._thread.start()
        self.session.state_ready.wait()
        self.window.advance_frame([])
        self.session.layout_ready.wait()

    def snapshot(self):
        return self.session.state.state.clone()

    def restore(self, snap):
        self.session.state.state = snap.clone()
        ViewTypeTreeFactory.install_noop_callbacks(
            self.session.state.state, self._slots)
        self.window.advance_frame([])

    def settle(self):
        from test.uigen.fuzz.strategies import settle as _settle
        return _settle(self)

    def current_state_signature(self):
        return signature(self.session.state)

    def is_done(self):
        return self.session.state.state.resume_index == -1

    def legal_actions(self):
        return legal_action_keys_of_state(self.session.state)

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
        if self._thread and self._thread.is_alive():
            self.window.request_quit()
            self._thread.join(timeout=10)
        if self._restore:
            self._restore()


class InprocFuzzer(LocalDeploymentFuzzer):
    runner_module = inproc_runner


class NativeFuzzer(LocalDeploymentFuzzer):
    runner_module = native_runner


DEPLOYMENTS = {
    "inproc": InprocFuzzer,
    "native": NativeFuzzer,
}


def deployment_names():
    return list(DEPLOYMENTS) + ["oop"]

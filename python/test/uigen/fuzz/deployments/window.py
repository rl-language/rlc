import threading

from test.uigen.fuzz.util.headless_backend import HeadlessTextBackend
from rlc.uigen.window_events import QuitEvent


class HeadlessWindow:
    def __init__(self, size=(1280, 720)):
        self._size = size
        self._backend = HeadlessTextBackend()

        self._lock = threading.Lock()
        self._pending = []
        self._events_ready = threading.Event()
        self._frame_done = threading.Event()
        self._closed = False

    @property
    def size(self):
        return self._size

    def make_renderer(self):
        return self._backend

    def pump_events(self):
        self._events_ready.wait()
        with self._lock:
            self._events_ready.clear()
            events = self._pending
            self._pending = []
        for event in events:
            yield event

    def begin_frame(self):
        pass

    def end_frame(self):
        self._frame_done.set()

    def tick(self, fps=60):
        return 1.0 / fps

    def close(self):
        with self._lock:
            self._closed = True
        self._events_ready.set()
        self._frame_done.set()

    @property
    def closed(self):
        with self._lock:
            return self._closed

    def submit_events(self, events):
        with self._lock:
            self._pending.extend(events)
            self._events_ready.set()

    def advance_frame(self, events):
        if self.closed:
            return
        self._frame_done.clear()
        self.submit_events(events)
        self._frame_done.wait()

    def request_quit(self):
        self.advance_frame([QuitEvent()])

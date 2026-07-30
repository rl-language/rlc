from dataclasses import dataclass
from typing import Optional


@dataclass
class Interaction:
    node: object
    key_char: Optional[str] = None
    action_key: Optional[tuple] = None
    expect_legal: bool = True


def settle(fuzzer, max_frames=4):
    last = fuzzer.current_state_signature()
    for _ in range(max_frames):
        if fuzzer.is_done() or fuzzer.window.closed:
            break
        fuzzer.window.advance_frame([])
        now = fuzzer.current_state_signature()
        if now == last:
            return now
        last = now
    return fuzzer.current_state_signature()

from dataclasses import dataclass, field
from typing import Any, Callable, List, Optional, Tuple


@dataclass
class GameConfig:
    title: str
    interactions: List[Tuple[Any, str]] = field(default_factory=list)
    policy: Optional[Callable] = None
    click_mode: str = "dispatch"
    handles_keys: bool = False
    renderer_config: dict = field(default_factory=dict)

    def run_policy(self, state, program, max_steps: int = 1000) -> None:
        if self.policy is None:
            return
        steps = 0
        before = str(state.state)
        while self.policy(state, program):
            steps += 1
            after = str(state.state)
            if after == before:
                break
            before = after
            if steps >= max_steps:
                break

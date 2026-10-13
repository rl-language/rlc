from dataclasses import dataclass
from typing import Optional


@dataclass
class Seed:
    snapshot: object
    signature: str
    score: float = 1.0
    retired: bool = False


class Corpus:
    def __init__(self, rng):
        self.rng = rng
        self.seeds = []
        self._signatures = set()

    def __len__(self):
        return len(self.seeds)

    def add(self, snapshot, signature) -> Optional[Seed]:
        if signature in self._signatures:
            return None
        self._signatures.add(signature)
        seed = Seed(snapshot, signature)
        self.seeds.append(seed)
        return seed

    def select(self) -> Optional[Seed]:
        active = [s for s in self.seeds if not s.retired]
        if not active:
            return None
        weights = [s.score for s in active]
        return self.rng.choices(active, weights=weights, k=1)[0]

    def reward(self, seed: Seed):
        seed.score += 1.0

    def decay(self, seed: Seed):
        seed.score = max(0.1, seed.score * 0.7)

    def retire(self, seed: Seed):
        seed.retired = True

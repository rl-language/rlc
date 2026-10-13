import random
import time
from dataclasses import dataclass, field
from typing import Set

from test.uigen.fuzz.exploration.corpus import Corpus
from test.uigen.fuzz.util.action_index import (
    map_actions_to_clickable_nodes, legal_action_keys_of_state,
)
from test.uigen.fuzz.util.game_bindings import make_bindings


@dataclass
class ExplorationResult:
    strategy: str
    seed: int
    branches_attempted: int = 0
    branches_landed: int = 0
    gate_leaks: int = 0
    elapsed: float = 0.0
    states_seen: Set[str] = field(default_factory=set)
    leak_examples: list = field(default_factory=list)
    action_universe: Set = field(default_factory=set)
    actions_fired: Set = field(default_factory=set)
    unreachable_legal: Set = field(default_factory=set)

    @property
    def distinct_states(self):
        return len(self.states_seen)

    @property
    def states_per_second(self):
        return self.distinct_states / self.elapsed if self.elapsed > 0 else 0.0

    @property
    def action_coverage(self):
        if not self.action_universe:
            return 0.0
        return len(self.actions_fired & self.action_universe) / len(self.action_universe)


class Explorer:
    def __init__(self, fuzzer, game, choose_next, *, seed=0, max_depth=30):
        self.fuzzer = fuzzer
        self.game = game
        self.choose_next = choose_next
        self.rng = random.Random(seed)
        self.seed = seed
        self.max_depth = max_depth

    def run(self, *, budget):
        fuzzer = self.fuzzer
        bindings = make_bindings(self.game, fuzzer.session.state)
        result = ExplorationResult(strategy=self.choose_next.name, seed=self.seed)
        result.states_seen.add(fuzzer.current_state_signature())
        self._covered = {}

        t0 = time.monotonic()
        if getattr(fuzzer, "cheap_restore", True):
            self._run_corpus(fuzzer, bindings, result, budget)
        else:
            self._run_forward(fuzzer, bindings, result, budget)
        result.elapsed = time.monotonic() - t0
        return result

    def _run_corpus(self, fuzzer, bindings, result, budget):
        corpus = Corpus(self.rng)
        corpus.add(fuzzer.snapshot(), fuzzer.current_state_signature())
        rounds = 0
        while rounds < budget:
            seed = corpus.select()
            if seed is None:
                break
            fuzzer.restore(seed.snapshot)
            produced_new = self._rollout(fuzzer, bindings, result, corpus)
            corpus.reward(seed) if produced_new else corpus.decay(seed)
            rounds += 1

    def _run_forward(self, fuzzer, bindings, result, budget):
        for _ in range(budget):
            if fuzzer.is_done() or not fuzzer.enabled_clickable_nodes():
                fuzzer.restart()
                continue
            self._rollout(fuzzer, bindings, result, None)

    def _rollout(self, fuzzer, bindings, result, corpus):
        produced_new = False
        for _ in range(self.max_depth):
            if fuzzer.is_done():
                break
            here = fuzzer.current_state_signature()
            index = map_actions_to_clickable_nodes(fuzzer.session.layout, bindings)
            legal = legal_action_keys_of_state(fuzzer.session.state)
            self._record_universe(index, legal, result)
            local_covered = self._covered.setdefault(here, set())
            interaction = self.choose_next.pick(fuzzer, bindings, self.rng,
                                                local_covered, index, legal)
            if interaction is None:
                break
            if interaction.action_key is not None:
                local_covered.add(interaction.action_key)

            result.branches_attempted += 1
            fuzzer.perform(interaction)
            after = fuzzer.settle()

            if after != here:
                result.branches_landed += 1
                if interaction.action_key is not None:
                    result.actions_fired.add(interaction.action_key)
                if not interaction.expect_legal:
                    result.gate_leaks += 1
                    if len(result.leak_examples) < 5:
                        result.leak_examples.append(str(interaction.action_key))
                if after not in result.states_seen:
                    result.states_seen.add(after)
                    if corpus is not None and not fuzzer.is_done():
                        corpus.add(fuzzer.snapshot(), after)
                    produced_new = True
        return produced_new

    def _record_universe(self, index, legal, result):
        keys = set(index)
        result.action_universe |= keys
        result.unreachable_legal |= (legal - keys)

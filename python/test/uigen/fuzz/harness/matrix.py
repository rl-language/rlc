import json
import os
import subprocess
import sys
from dataclasses import dataclass, field
from typing import Dict, Optional, Tuple


@dataclass
class CellResult:
    strategy: str
    distinct_states: int = 0
    branches_attempted: int = 0
    branches_landed: int = 0
    gate_leaks: int = 0
    elapsed: float = 0.0
    crashed: bool = False
    leak_examples: list = field(default_factory=list)
    action_universe_size: int = 0
    actions_fired_count: int = 0
    unreachable_legal: list = field(default_factory=list)

    @property
    def states_per_second(self):
        return self.distinct_states / self.elapsed if self.elapsed > 0 else 0.0

    @property
    def action_coverage(self):
        if not self.action_universe_size:
            return None
        return self.actions_fired_count / self.action_universe_size


@dataclass
class FuzzMatrix:
    game: str
    budget: int
    seed: int
    cells: Dict[Tuple[str, str], CellResult] = field(default_factory=dict)

    def get(self, deployment, strategy):
        return self.cells.get((deployment, strategy))

    def records(self):
        rows = []
        for (deployment, strategy), r in self.cells.items():
            rows.append({
                "game": self.game,
                "deployment": deployment,
                "strategy": strategy,
                "budget": self.budget,
                "seed": self.seed,
                "distinct_states": r.distinct_states,
                "states_per_second": round(r.states_per_second, 3),
                "branches_attempted": r.branches_attempted,
                "branches_landed": r.branches_landed,
                "gate_leaks": r.gate_leaks,
                "action_universe_size": r.action_universe_size,
                "actions_fired": r.actions_fired_count,
                "action_coverage": (round(r.action_coverage, 4)
                                    if r.action_coverage is not None else None),
                "unreachable_legal_count": len(r.unreachable_legal),
                "elapsed_s": round(r.elapsed, 3),
                "crashed": r.crashed,
            })
        return rows


def run_cell(game, source_file, deployment, strategy, *, budget, seed,
             repo_root):
    read_fd, write_fd = os.pipe()
    cmd = [
        sys.executable, "-m", "test.uigen.fuzz.harness.cell_runner",
        source_file,
        "--game", game, "--deployment", deployment, "--strategy", strategy,
        "--budget", str(budget), "--seed", str(seed),
        "--repo-root", repo_root, "--result-fd", str(write_fd),
    ]
    proc = subprocess.Popen(
        cmd, pass_fds=(write_fd,),
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        env={**os.environ, "PYTHONPATH": repo_root + "/python"},
    )
    os.close(write_fd)
    payload = _read_all(read_fd)
    proc.wait()

    if not payload:
        return CellResult(strategy=strategy, crashed=True)
    data = json.loads(payload)
    return CellResult(
        strategy=data["strategy"],
        distinct_states=data["distinct_states"],
        branches_attempted=data["branches_attempted"],
        branches_landed=data["branches_landed"],
        gate_leaks=data["gate_leaks"],
        elapsed=data["elapsed"],
        leak_examples=data.get("leak_examples", []),
        action_universe_size=data.get("action_universe_size", 0),
        actions_fired_count=data.get("actions_fired_count", 0),
        unreachable_legal=data.get("unreachable_legal", []),
    )


def _read_all(fd):
    chunks = []
    while True:
        chunk = os.read(fd, 4096)
        if not chunk:
            break
        chunks.append(chunk)
    os.close(fd)
    return b"".join(chunks).decode().strip()


def run_matrix(game, source_file, deployments, strategies, *, budget, seed,
               repo_root, oop_budget, on_cell=None):
    matrix = FuzzMatrix(game=game, budget=budget, seed=seed)
    for deployment in deployments:
        cell_budget = oop_budget if deployment == "oop" else budget
        for strategy in strategies:
            matrix.cells[(deployment, strategy)] = run_cell(
                game, source_file, deployment, strategy,
                budget=cell_budget, seed=seed, repo_root=repo_root)
            if on_cell is not None:
                on_cell(matrix)
    return matrix

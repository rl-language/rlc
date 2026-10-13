import json
import os
import sys

from command_line import make_rlc_argparse

from test.uigen.fuzz.deployments.deployment_fuzzers import (
    DEPLOYMENTS, loaded_program,
)
from test.uigen.fuzz.deployments.oop_fuzzer import OopFuzzer
from test.uigen.fuzz.exploration.explorer import Explorer
from test.uigen.fuzz.strategies.registry import make_strategy
from test.uigen.fuzz.util.game_bindings import load_config, rl_source


def _run(game, args, deployment, strategy, budget, seed, repo_root):
    cfg = load_config(game)
    chooser = make_strategy(strategy)
    if deployment == "oop":
        fuzzer = OopFuzzer(cfg, args, rl_source(game, repo_root), game)
    else:
        fuzzer = DEPLOYMENTS[deployment](cfg, args, loaded_program(args).__enter__())
    fuzzer.start()
    try:
        return Explorer(fuzzer, game, chooser, seed=seed).run(budget=budget)
    finally:
        try:
            fuzzer.stop()
        except Exception:
            pass


def main():
    parser = make_rlc_argparse("cell", description="run one fuzz matrix cell")
    parser.add_argument("--game", required=True)
    parser.add_argument("--deployment", required=True)
    parser.add_argument("--strategy", required=True)
    parser.add_argument("--budget", type=int, required=True)
    parser.add_argument("--seed", type=int, default=0)
    parser.add_argument("--repo-root", required=True)
    parser.add_argument("--result-fd", type=int, required=True)
    args = parser.parse_args()

    result = _run(args.game, args, args.deployment, args.strategy,
                  args.budget, args.seed, args.repo_root)
    payload = json.dumps({
        "strategy": result.strategy,
        "distinct_states": result.distinct_states,
        "branches_attempted": result.branches_attempted,
        "branches_landed": result.branches_landed,
        "gate_leaks": result.gate_leaks,
        "elapsed": result.elapsed,
        "leak_examples": result.leak_examples,
        "action_universe_size": len(result.action_universe),
        "actions_fired_count": len(result.actions_fired & result.action_universe),
        "unreachable_legal": sorted(str(k) for k in result.unreachable_legal),
    })
    os.write(args.result_fd, (payload + "\n").encode())
    sys.stdout.flush()
    sys.stderr.flush()
    os._exit(0)


if __name__ == "__main__":
    main()

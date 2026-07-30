import json
import os
import sys

from command_line import make_rlc_argparse

from test.uigen.fuzz.deployments.deployment_fuzzers import deployment_names
from test.uigen.fuzz.strategies.registry import strategy_names
from test.uigen.fuzz.harness.matrix import run_matrix
from test.uigen.fuzz.harness import report
from test.uigen.fuzz.util.game_bindings import GAMES, rl_source


def _repo_root():
    here = os.path.abspath(__file__)
    return here[:here.index("/python/")]


def main(argv=None):
    parser = make_rlc_argparse(
        "fuzz", description="UI state-reachability fuzzer for RLC games")
    parser.add_argument("--game", choices=GAMES, required=True)
    parser.add_argument("--config", default="all",
                        choices=["all"] + deployment_names())
    parser.add_argument("--strategy", default="all",
                        choices=["all"] + strategy_names())
    parser.add_argument("--budget", type=int, default=1000)
    parser.add_argument("--oop-budget", type=int, default=60,
                        help="rounds for the oop cell (restarts cost ~2s each)")
    parser.add_argument("--seed", type=int, default=0)
    parser.add_argument("--out", default=None,
                        help="directory to persist per-run results as JSON "
                             "(for the export tables/plots)")
    args = parser.parse_args(argv)

    deployments = (deployment_names() if args.config == "all"
                   else [args.config])
    strategies = (strategy_names() if args.strategy == "all"
                  else [args.strategy])

    out_path = None
    if args.out:
        os.makedirs(args.out, exist_ok=True)
        out_path = os.path.join(
            args.out,
            f"{args.game}_{args.config}_b{args.budget}_s{args.seed}.json")

    def _persist(matrix):
        with open(out_path, "w") as f:
            json.dump(matrix.records(), f, indent=2)

    matrix = run_matrix(args.game, args.source_file, deployments, strategies,
                        budget=args.budget, seed=args.seed,
                        repo_root=_repo_root(), oop_budget=args.oop_budget,
                        on_cell=_persist if out_path else None)
    print(report.render(matrix, deployments, strategies), flush=True)
    if out_path:
        print(f"\n  [out] wrote {out_path}", flush=True)
    return 0


if __name__ == "__main__":
    args = sys.argv[1:]
    if all(not a.endswith(".rl") for a in args):
        game = None
        for i, a in enumerate(args):
            if a == "--game" and i + 1 < len(args):
                game = args[i + 1]
        if game:
            args = [rl_source(game, _repo_root())] + args
    code = main(args)
    sys.stdout.flush()
    sys.stderr.flush()
    os._exit(code)

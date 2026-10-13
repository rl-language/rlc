import argparse
import csv
import glob
import json
import os
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap

# CVD-safe categorical palette (dataviz skill, light mode, fixed order).
PALETTE = ["#2a78d6", "#eb6834", "#1baf7a", "#eda100", "#e87ba4", "#008300"]
GOOD = "#008300"
CRITICAL = "#e34948"
INK = "#0b0b0b"
MUTED = "#52514e"
GRID = "#dddddd"

plt.rcParams.update({
    "figure.dpi": 120,
    "font.size": 10,
    "axes.edgecolor": MUTED,
    "axes.linewidth": 0.8,
    "axes.labelcolor": INK,
    "text.color": INK,
    "xtick.color": MUTED,
    "ytick.color": MUTED,
    "axes.spines.top": False,
    "axes.spines.right": False,
})


def load_records(results_dir):
    rows = []
    for path in sorted(glob.glob(os.path.join(results_dir, "*.json"))):
        with open(path) as f:
            rows.extend(json.load(f))
    return rows


def write_csv(rows, out):
    if not rows:
        return
    path = os.path.join(out, "results.csv")
    cols = list(rows[0].keys())
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=cols)
        w.writeheader()
        w.writerows(rows)
    print(f"  [csv] {path}")


def _save(fig, out, name):
    for ext in ("pdf", "png"):
        fig.savefig(os.path.join(out, f"{name}.{ext}"), bbox_inches="tight")
    plt.close(fig)
    print(f"  [plot] {name}.pdf / .png")


def _order(values, preferred):
    present = [v for v in preferred if v in values]
    return present + sorted(v for v in values if v not in preferred)


DEPLOY_ORDER = ["inproc", "native", "oop"]
STRAT_ORDER = ["from_all_actions", "from_enabled_widgets", "from_random_pixels"]
STRAT_LABEL = {"from_all_actions": "all actions",
               "from_enabled_widgets": "enabled widgets",
               "from_random_pixels": "random pixels"}


def _latest_per_cell(rows, metric):
    """Pick the largest-budget row per (game, deployment, strategy)."""
    best = {}
    for r in rows:
        key = (r["game"], r["deployment"], r["strategy"])
        if key not in best or r["budget"] > best[key]["budget"]:
            best[key] = r
    return best


def _games(rows):
    return _order({r["game"] for r in rows}, [])


def bars_by_deployment(rows, out, metric, ylabel, name):
    # One bar chart per (game, deployment): bars = strategies.
    best = _latest_per_cell(rows, metric)
    for game in _games(rows):
        deployments = _order(
            {d for (g, d, s) in best if g == game}, DEPLOY_ORDER)
        for dep in deployments:
            strategies = _order(
                {s for (g, d, s) in best if g == game and d == dep},
                STRAT_ORDER)
            if not strategies:
                continue
            fig, ax = plt.subplots(figsize=(5.2, 3.4))
            for si, strat in enumerate(strategies):
                cell = best.get((game, dep, strat))
                val = cell.get(metric) if cell else None
                ax.bar(si, val if val is not None else 0, width=0.62,
                       color=PALETTE[si % len(PALETTE)],
                       label=STRAT_LABEL.get(strat, strat), zorder=3)
            ax.set_xticks(range(len(strategies)))
            ax.set_xticklabels([STRAT_LABEL.get(s, s) for s in strategies],
                               fontsize=8)
            ax.set_ylabel(ylabel)
            ax.set_title(f"{game} — {dep}", loc="left", color=INK,
                         fontweight="bold")
            ax.yaxis.grid(True, color=GRID, linewidth=0.8, zorder=0)
            ax.set_axisbelow(True)
            _save(fig, out, f"{name}_{game}_{dep}")


def distinct_bars(rows, out):
    bars_by_deployment(rows, out, "distinct_states", "distinct states",
                       "distinct_states")


def action_coverage_bars(rows, out):
    bars_by_deployment(rows, out, "action_coverage",
                       "action coverage (fired / reachable)", "action_coverage")


def states_per_sec_bars(rows, out):
    bars_by_deployment(rows, out, "states_per_second", "new states / second",
                       "states_per_sec")


def convergence_by_deployment(rows, out, metric, ylabel, name):
    # One line plot per (game, deployment): lines = strategies, x = budget.
    by_gd = defaultdict(lambda: defaultdict(dict))  # (game,dep) -> strat -> {budget:val}
    for r in rows:
        by_gd[(r["game"], r["deployment"])][r["strategy"]][r["budget"]] = r[metric]
    for (game, dep), per_strat in by_gd.items():
        if max((len(v) for v in per_strat.values()), default=0) < 2:
            continue
        fig, ax = plt.subplots(figsize=(5.6, 3.6))
        for strat in _order(list(per_strat), STRAT_ORDER):
            budgets = sorted(per_strat[strat])
            ys = [per_strat[strat][b] for b in budgets]
            ci = STRAT_ORDER.index(strat) if strat in STRAT_ORDER else 0
            ax.plot(budgets, ys, "-", color=PALETTE[ci % len(PALETTE)],
                    linewidth=2, marker="o", markersize=4,
                    label=STRAT_LABEL.get(strat, strat))
        ax.set_xlabel("budget (rounds)")
        ax.set_ylabel(ylabel)
        ax.set_title(f"{game} — {dep}", loc="left", fontweight="bold")
        ax.grid(True, color=GRID, linewidth=0.8)
        ax.set_axisbelow(True)
        ax.legend(frameon=False, fontsize=8)
        _save(fig, out, f"{name}_{game}_{dep}")


def convergence(rows, out):
    convergence_by_deployment(rows, out, "distinct_states", "distinct states",
                              "convergence")


def states_per_sec_convergence(rows, out):
    convergence_by_deployment(rows, out, "states_per_second",
                              "new states / second", "states_per_sec_conv")


def leaks_heatmap(rows, out):
    best = _latest_per_cell(rows, "gate_leaks")
    # aggregate leaks per (game, deployment) across strategies
    agg = defaultdict(int)
    games, deployments = set(), set()
    for (g, d, s), r in best.items():
        agg[(g, d)] += r["gate_leaks"]
        games.add(g)
        deployments.add(d)
    games = _order(games, [])
    deployments = _order(deployments, DEPLOY_ORDER)
    if not games or not deployments:
        return

    import numpy as np
    grid = np.array([[agg.get((g, d), 0) for d in deployments]
                     for g in games], dtype=float)
    cmap = LinearSegmentedColormap.from_list("leaks", [GOOD, "#f4f4f0",
                                                       CRITICAL])
    fig, ax = plt.subplots(figsize=(1.4 + 1.2 * len(deployments),
                                    0.7 + 0.5 * len(games)))
    vmax = max(1.0, grid.max())
    ax.imshow(grid, cmap=cmap, vmin=0, vmax=vmax, aspect="auto")
    ax.set_xticks(range(len(deployments)))
    ax.set_xticklabels(deployments)
    ax.set_yticks(range(len(games)))
    ax.set_yticklabels(games)
    for i in range(len(games)):
        for j in range(len(deployments)):
            v = int(grid[i, j])
            ax.text(j, i, str(v), ha="center", va="center",
                    color=INK if v == 0 else "#ffffff", fontsize=9)
    ax.set_title("gate leaks  (0 = correct)", loc="left", fontweight="bold")
    _save(fig, out, "leaks_heatmap")


def main():
    parser = argparse.ArgumentParser(
        description="Export fuzz results as CSV + thesis plots")
    parser.add_argument("results_dir")
    parser.add_argument("--out", default=None,
                        help="output dir for csv/plots (default: results_dir)")
    args = parser.parse_args()
    out = args.out or args.results_dir
    os.makedirs(out, exist_ok=True)

    rows = load_records(args.results_dir)
    if not rows:
        print(f"no *.json results in {args.results_dir}")
        return
    print(f"loaded {len(rows)} cell records")

    write_csv(rows, out)
    distinct_bars(rows, out)
    action_coverage_bars(rows, out)
    states_per_sec_bars(rows, out)
    convergence(rows, out)
    states_per_sec_convergence(rows, out)
    leaks_heatmap(rows, out)


if __name__ == "__main__":
    main()

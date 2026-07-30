def render(matrix, deployments, strategies):
    lines = []
    lines.append("")
    lines.append(f"{'=' * 64}")
    lines.append(f"  fuzz: {matrix.game}   (budget={matrix.budget}, "
                 f"seed={matrix.seed})")
    lines.append(f"{'=' * 64}")

    lines.append("")
    lines.append("  distinct states reached")
    lines += _grid(matrix, deployments, strategies,
                   lambda r: "CRASH" if r.crashed else str(r.distinct_states))

    lines.append("")
    lines.append("  new states / second")
    lines += _grid(matrix, deployments, strategies,
                   lambda r: "-" if r.crashed else f"{r.states_per_second:.1f}")

    lines.append("")
    lines.append("  action coverage (fired / UI-reachable)")
    lines += _grid(matrix, deployments, strategies, _coverage_cell)

    unreachable = _collect_unreachable(matrix)
    lines.append("")
    if unreachable:
        lines.append(f"  !!! UNREACHABLE LEGAL ACTIONS: {len(unreachable)}"
                     f" (legal but no UI node fires them)")
        for key in sorted(unreachable)[:5]:
            lines.append(f"      {key}")
    else:
        lines.append("  unreachable legal actions: 0"
                     "  (every legal action has a UI node)")

    total_leaks = sum(r.gate_leaks for r in matrix.cells.values())
    lines.append("")
    if total_leaks:
        lines.append(f"  !!! GATE LEAKS: {total_leaks} "
                     f"(illegal actions changed state)")
        for (dep, strat), r in matrix.cells.items():
            if r.gate_leaks:
                examples = getattr(r, "leak_examples", [])
                ex = f"  e.g. {examples[0]}" if examples else ""
                lines.append(f"      {dep}/{strat}: {r.gate_leaks}{ex}")
    else:
        lines.append("  gate leaks: 0  (every illegal action was blocked)")

    lines.append(f"{'=' * 64}")
    return "\n".join(lines)


def _coverage_cell(r):
    if r.crashed:
        return "-"
    total = r.action_universe_size
    if total == 0:
        return "n/a"
    fired = r.actions_fired_count
    return f"{fired}/{total} {fired / total:.0%}"


def _collect_unreachable(matrix):
    out = set()
    for r in matrix.cells.values():
        if not r.crashed:
            out |= set(r.unreachable_legal)
    return out


def _grid(matrix, deployments, strategies, cell_fn):
    col_w = max(12, max((len(s) for s in strategies), default=12) + 1)
    head = "  " + " " * 22 + "".join(f"{s:>{col_w}}" for s in strategies)
    rows = [head, "  " + "-" * (22 + col_w * len(strategies))]
    for dep in deployments:
        cells = []
        for strat in strategies:
            r = matrix.get(dep, strat)
            cells.append(cell_fn(r) if r is not None else "-")
        rows.append(f"  {dep:<22}" + "".join(f"{c:>{col_w}}" for c in cells))
    return rows

#!/usr/bin/env bash
#
# Run the full fuzz matrix (all games x all deployments x all strategies)
# across a budget sweep, persist per-run JSON, and generate CSV + thesis plots.
#
# Usage:
#   ./run.sh                      # everything, default settings
#   GAMES="sudoku hanabi" ./run.sh
#   BUDGETS="500 1000 2000" ./run.sh
#   CONFIGS="inproc native" ./run.sh          # skip the slow oop cells
#   OUT=~/thesis/data ./run.sh
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../../.." && pwd)"
PY_DIR="$REPO_ROOT/python"

# oop is slow (subprocess + TCP, esp. sudoku/hanabi focus+key) — opt in with
# CONFIGS="inproc native oop" (and keep OOP_BUDGET low).
GAMES="${GAMES:-tic_tac_toe sudoku blackjack hanabi}"
CONFIGS="${CONFIGS:-inproc native}"
STRATEGY="${STRATEGY:-all}"
BUDGETS="${BUDGETS:-250 500 1000}"
OOP_BUDGET="${OOP_BUDGET:-20}"
SEED="${SEED:-0}"
OUT="${OUT:-$SCRIPT_DIR/results/$(date +%Y%m%d-%H%M%S)}"
PYTHON="${PYTHON:-python3}"

RESULTS="$OUT/json"
PLOTS="$OUT/plots"
mkdir -p "$RESULTS" "$PLOTS"

echo "=================================================================="
echo "  fuzz full run"
echo "  games:    $GAMES"
echo "  configs:  $CONFIGS"
echo "  strategy: $STRATEGY"
echo "  budgets:  $BUDGETS   (oop budget: $OOP_BUDGET)"
echo "  seed:     $SEED"
echo "  out:      $OUT"
echo "=================================================================="

cd "$PY_DIR"
export PYTHONPATH="$PY_DIR"

for game in $GAMES; do
  for budget in $BUDGETS; do
    for config in $CONFIGS; do
      echo ""
      echo ">>> $game / $config / budget=$budget"
      "$PYTHON" -m test.uigen.fuzz.runner \
        --game "$game" \
        --config "$config" \
        --strategy "$STRATEGY" \
        --budget "$budget" \
        --oop-budget "$OOP_BUDGET" \
        --seed "$SEED" \
        --out "$RESULTS" \
        || echo "    !! $game/$config/b$budget failed (continuing)"
    done
  done
done

echo ""
echo "=================================================================="
echo "  generating tables + plots"
echo "=================================================================="
"$PYTHON" -m test.uigen.fuzz.harness.export "$RESULTS" --out "$PLOTS"

echo ""
echo "=================================================================="
echo "  DONE"
echo "  json:  $RESULTS"
echo "  csv:   $PLOTS/results.csv"
echo "  plots: $PLOTS/*.pdf  *.png"
echo "=================================================================="

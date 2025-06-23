#!/usr/bin/env python3
"""pack.py

Build and stage Python distribution artefacts for a CMake-driven project.

Features
~~~~~~~~
* **--dry-run** prints each command instead of executing it.
* Automatically appends an appropriate ``--plat-name`` to ``bdist_wheel``:

  * macOS → ``macosx_11_0_arm64`` (Apple Silicon)
  * Windows → ``win_amd64``
  * Other platforms keep the default produced by *wheel*.

Steps performed (unless ``--dry-run``):
1. Delete any previously staged ``dist`` folder from the *binary* directory.
2. Run ``setup.py sdist bdist_wheel [--plat-name=…]`` inside the source dir.
3. Copy the freshly-built wheels/SDists into the *binary* directory.
4. Remove ``dist`` and ``build`` left in the source tree.

Typical CMake invocation::

    add_custom_target(
        pip_package
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/pack.py \
                --source-dir ${CMAKE_SOURCE_DIR}/python \
                --binary-dir ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
"""

from __future__ import annotations

import argparse
import platform
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Iterable, List

# ---------------------------------------------------------------------------
# Utility helpers
# ---------------------------------------------------------------------------


def _echo(cmd: str) -> None:  # noqa: D401
    """Uniformly print a shell-style command that will (or would) run."""
    print("+", cmd, flush=True)


def _run(cmd: List[str], cwd: Path, dry: bool) -> None:
    shell_line = " ".join(str(c) for c in cmd)
    _echo(f"(cwd={cwd}) {shell_line}")
    if not dry:
        subprocess.check_call(cmd, cwd=cwd)


def _rm_rf(path: Path, dry: bool) -> None:
    _echo(f"rm -rf {path}")
    if not dry and path.exists():
        shutil.rmtree(path, ignore_errors=True)


def _cp_r(src: Path, dst: Path, dry: bool) -> None:
    _echo(f"cp -r {src} {dst}")
    if not dry:
        shutil.copytree(src, dst, dirs_exist_ok=True)


# ---------------------------------------------------------------------------
# Platform helper
# ---------------------------------------------------------------------------


def _plat_name_for_current_host() -> str | None:
    """Return the wheel *plat-name* tag for this host or ``None`` if default."""
    sys_plat = sys.platform
    if sys_plat == "darwin":
        return "macosx_11_0_arm64"  # assume Apple Silicon (arm64)
    if sys_plat.startswith("win"):
        return "win_amd64"
    return None


# ---------------------------------------------------------------------------
# Core actions
# ---------------------------------------------------------------------------


def build_package(src_dir: Path, python: str, dry: bool) -> None:
    """Run ``setup.py sdist bdist_wheel`` in *src_dir* with proper plat-name."""
    cmd: list[str] = [python, "setup.py", "sdist", "bdist_wheel"]

    plat_name = _plat_name_for_current_host()
    if plat_name:
        cmd.append(f"--plat-name={plat_name}")

    _run(cmd, cwd=src_dir, dry=dry)


def copy_dist(src_dir: Path, bin_dir: Path, dry: bool) -> None:
    """Copy the ``dist`` folder from *src_dir* to *bin_dir*."""
    _cp_r(src_dir / "dist", bin_dir / "dist", dry=dry)


def clean_artifacts(paths: Iterable[Path], dry: bool) -> None:
    for p in paths:
        _rm_rf(p, dry=dry)


# ---------------------------------------------------------------------------
# CLI handling
# ---------------------------------------------------------------------------


def _parse_args(argv: list[str] | None = None) -> argparse.Namespace:  # noqa: ANN001
    p = argparse.ArgumentParser(description="Build & stage Python sdists/wheels for CMake")
    p.add_argument("--source-dir", required=True, type=Path, help="Path containing setup.py")
    p.add_argument("--binary-dir", required=True, type=Path, help="CMake binary dir (stage area)")
    p.add_argument("--python", default=sys.executable, help="Python interpreter to run setup.py")
    p.add_argument("--dry-run", action="store_true", help="Print commands only; do nothing")
    return p.parse_args(argv)


def main(argv: list[str] | None = None) -> None:  # noqa: ANN001
    args = _parse_args(argv)

    src_dir = args.source_dir.resolve()
    bin_dir = args.binary_dir.resolve()
    dry = bool(args.dry_run)

    # 1. Clear out any previously staged artefacts
    clean_artifacts([bin_dir / "dist"], dry)

    # 2. Build new artefacts
    build_package(src_dir, args.python, dry)

    # 3. Stage them into the build tree
    copy_dist(src_dir, bin_dir, dry)

    # 4. Clean up temporary build folders inside the source tree
    clean_artifacts([src_dir / "dist", src_dir / "build"], dry)

    msg = (
        "(dry run) Packaging steps simulated"
        if dry
        else f"Packaging complete → {bin_dir / 'dist'}"
    )
    print(msg)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)

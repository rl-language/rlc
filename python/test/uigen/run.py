import argparse
import importlib
import sys

from rlc.uigen.game_config import GameConfig

_MODES = ("inproc", "native", "oop-sim", "oop-ui")
_BACKENDS = ("pygame",)


def load_config(game: str) -> GameConfig:
    if ":" in game:
        module_name, attr = game.rsplit(":", 1)
    else:
        module_name, attr = f"test.uigen.{game}.config", "CONFIG"
    module = importlib.import_module(module_name)
    cfg = getattr(module, attr)
    if not isinstance(cfg, GameConfig):
        raise TypeError(f"{module_name}:{attr} is not a GameConfig")
    return cfg


def dispatch_run(cfg: GameConfig, default_mode: str = "inproc") -> None:
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--mode", choices=_MODES, default=default_mode)
    parser.add_argument("--backend", choices=_BACKENDS, default="pygame")
    known, rest = parser.parse_known_args()
    sys.argv = [sys.argv[0]] + rest

    if known.mode == "oop-sim":
        from rlc.uigen.runners.oop_runner import sim_main
        sim_main(cfg)
        return

    if known.backend == "pygame":
        from test.uigen.backends.pygame_backend import PygameWindow
        window = PygameWindow(cfg.title)
        backend = window.make_renderer()
    else:
        raise ValueError(f"unknown backend: {known.backend!r}")

    if known.mode == "inproc":
        from rlc.uigen.runners.inproc_runner import run
        from command_line import make_rlc_argparse
        args = make_rlc_argparse(cfg.title, description=cfg.title).parse_args()
        run(cfg, window, backend, args)
    elif known.mode == "native":
        from rlc.uigen.runners.native_runner import run
        from command_line import make_rlc_argparse
        args = make_rlc_argparse(cfg.title, description=cfg.title).parse_args()
        run(cfg, window, backend, args)
    elif known.mode == "oop-ui":
        from rlc.uigen.runners.oop_runner import ui_main
        ui_main(cfg, window, backend)


def main() -> None:
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("game",
                        help="game name (e.g. tic_tac_toe) or module:attr")
    known, rest = parser.parse_known_args()
    sys.argv = [sys.argv[0]] + rest
    dispatch_run(load_config(known.game))


if __name__ == "__main__":
    main()

import importlib

from rlc.uigen.view_types.dispatch_actions import _parse_action_str


class HandlerBinding:
    def __init__(self, name, *, ui_only=False, dispatch_guarded=False,
                 target=None, bind=None, free=None, targets=()):
        self.name = name
        self.ui_only = ui_only
        self.dispatch_guarded = dispatch_guarded
        self._target = target
        self.bind = dict(bind or {})
        self.free = dict(free or {})
        self.targets = tuple(targets)

    def target_action(self, node_args):
        if callable(self._target):
            return self._target(node_args)
        return self._target

    def bound_args(self, node_args):
        return {rl: node_args[nd] for nd, rl in self.bind.items()
                if nd in node_args}


class GameBindings:
    def __init__(self, state):
        self.state = state
        self.overrides = {}
        self._by_name = None
        self._expand_cache = {}

    def ui_only(self, handler):
        self.overrides[handler] = HandlerBinding(handler, ui_only=True)

    def dispatch_guarded(self, handler):
        self.overrides[handler] = HandlerBinding(handler, dispatch_guarded=True)

    def is_dispatch_guarded(self, handler):
        binding = self.overrides.get(handler)
        return binding is not None and binding.dispatch_guarded

    def is_guarded_target(self, name):
        if name in self.overrides and self.overrides[name].dispatch_guarded:
            return True
        return name in self._guarded_targets()

    def _guarded_targets(self):
        targets = set()
        for binding in self.overrides.values():
            if binding.dispatch_guarded and binding.targets:
                targets.update(binding.targets)
        return targets

    def map(self, handler, target, *, bind=None, free=None, guarded=False,
            targets=()):
        self.overrides[handler] = HandlerBinding(
            handler, target=target, bind=bind, free=free,
            dispatch_guarded=guarded, targets=targets)

    def key_value_arg(self, handler):
        binding = self.overrides.get(handler)
        if binding is None or not binding.free:
            return None
        return next(iter(binding.free.values()))

    def _universe_by_name(self):
        if self._by_name is None:
            grouped = {}
            for a in self.state.actions:
                name, args = _parse_action_str(str(a))
                grouped.setdefault(name, []).append(dict(args))
            self._by_name = grouped
        return self._by_name

    def actions_for_handler(self, handler, node_args):
        binding = self.overrides.get(handler)
        if binding is None:
            yield from self._infer(handler, node_args)
            return
        if binding.ui_only:
            return
        if binding.dispatch_guarded:
            yield (handler, dict(node_args))
            return
        target = binding.target_action(node_args)
        if target is None:
            return
        yield from self._expand(target, binding.bound_args(node_args), binding.free)

    def _infer(self, handler, node_args):
        yield (handler, dict(node_args))

    def _expand(self, target, bound, free):
        if not free:
            yield (target, bound)
            return
        cache_key = (target, frozenset(bound.items()))
        cached = self._expand_cache.get(cache_key)
        if cached is None:
            cached = [
                (target, args) for args in self._universe_by_name().get(target, ())
                if all(args.get(k) == v for k, v in bound.items())
            ]
            self._expand_cache[cache_key] = cached
        yield from cached


def make_bindings(game, state):
    bindings = GameBindings(state)
    try:
        module = importlib.import_module(f"test.uigen.{game}.fuzz")
    except ModuleNotFoundError:
        return bindings
    module.register(bindings)
    return bindings


_RL_SOURCES = {
    "tic_tac_toe": "tool/rlc/test/tic_tac_toe.rl",
    "blackjack":   "tool/rlc/test/black_jack.rl",
    "sudoku":      "tool/rlc/test/sudoku_vector.rl",
    "hanabi":      "tool/rlc/test/hanabi.rl",
}

GAMES = tuple(_RL_SOURCES)


def load_config(game):
    module = importlib.import_module(f"test.uigen.{game}.config")
    return module.CONFIG


def rl_source(game, repo_root):
    return f"{repo_root}/{_RL_SOURCES[game]}"

import re

_ACTION_STR_RE = re.compile(r"^(\w+)\s*(\{.*\})?\s*$")

_EVENT_ATTRS = ("on_click", "on_key", "on_hover")

ACTION_REGISTRY = {}

def _parse_action_str(s):
    m = _ACTION_STR_RE.match(s.strip())
    if not m:
        return s.strip(), ()
    name, body = m.group(1), m.group(2)
    pairs = []
    if body:
        for pair in body.strip("{}").split(","):
            if ":" in pair:
                k, v = pair.split(":", 1)
                k, v = k.strip(), v.strip()
                try:
                    v = int(v)
                except ValueError:
                    pass
                pairs.append((k, v))
    return name, tuple(sorted(pairs))


def _args_key(args):
    out = []
    for k, v in (args or {}).items():
        try:
            out.append((k, int(v)))
        except (TypeError, ValueError):
            out.append((k, v))
    return tuple(sorted(out, key=lambda kv: kv[0]))


class ActionGate:
    def __init__(self, state):
        self.state = state
        self._gatable = {
            _parse_action_str(str(a)): a for a in state.actions
        }

    def _valid_set(self):
        return {_parse_action_str(str(a)) for a in self.state.legal_actions}

    def resolve(self, handler, args):
        return self._gatable.get((handler, _args_key(args)))

    def can_apply(self, handler, args):
        action = self.resolve(handler, args)
        if action is None:
            return None
        return self.state.can_apply(action)

    def _meta_key(self, meta):
        if not meta:
            return None
        return (meta.get("handler"), _args_key(meta.get("args")))

    def is_gatable(self, meta):
        key = self._meta_key(meta)
        return key is not None and key in self._gatable

    def apply(self, layout):
        self._walk(layout, self._valid_set())

    def apply_valid(self, layout, valid_keys):
        self._walk(layout, valid_keys)

    @staticmethod
    def parse_valid_strings(strings):
        return {_parse_action_str(s) for s in strings}

    def _walk(self, node, valid):
        keys = [
            self._meta_key(getattr(node, attr, None))
            for attr in _EVENT_ATTRS
        ]
        gatable_keys = [k for k in keys if k is not None and k in self._gatable]
        if gatable_keys:
            node.enabled = any(k in valid for k in gatable_keys)
        for child in getattr(node, "children", []):
            self._walk(child, valid)


def action(name):
    def wrapper(fn):
        ACTION_REGISTRY[name] = fn
        return fn
    return wrapper


def _convert_arg(value, expected_type):
    if expected_type is None or isinstance(value, expected_type):
        return value
    try:
        inst = expected_type()
    except Exception:
        return value
    if hasattr(inst, "value"):
        inst.value = value
        return inst
    return value


def dispatch_action(handler_name, program, state, args):
    if handler_name in ACTION_REGISTRY:
        return ACTION_REGISTRY[handler_name](program, state, **args)

    method = getattr(state.state, handler_name, None)
    if method is None:
        raise KeyError(
            f"No handler '{handler_name}' (not in ACTION_REGISTRY, "
            f"no RLC action of that name on the game state)"
        )

    annotations = getattr(method, "__annotations__", {}) or {}
    converted = {}
    for name, value in args.items():
        ann = annotations.get(name)
        target_type = getattr(program.module, ann, None) if isinstance(ann, str) else None
        converted[name] = _convert_arg(value, target_type)

    if _resolved_can_apply(state, handler_name, args) is False:
        return False

    method(**converted)
    return True


def _resolved_can_apply(state, handler_name, args):
    key = (handler_name, _args_key(args))
    for a in state.actions:
        if _parse_action_str(str(a)) == key:
            return state.can_apply(a)
    return None
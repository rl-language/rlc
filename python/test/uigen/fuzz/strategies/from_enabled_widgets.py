from test.uigen.fuzz.strategies import Interaction
from test.uigen.fuzz.util.action_index import make_action_key


class FromEnabledWidgets:
    name = "from_enabled_widgets"

    def pick(self, fuzzer, bindings, rng, tried, index, legal):
        nodes = fuzzer.enabled_clickable_nodes()
        if not nodes:
            return None
        node, _attr, _meta = rng.choice(nodes)
        return Interaction(node=node, key_char=_key_char(node, bindings, rng),
                           action_key=_action_key_for(node, bindings))


def _key_char(node, bindings, rng):
    on_key = getattr(node, "on_key", None)
    if not on_key:
        return None
    return str(rng.randint(1, 9))


def _action_key_for(node, bindings):
    from rlc.uigen.view_types.dispatch_actions import _EVENT_ATTRS
    for attr in _EVENT_ATTRS:
        meta = getattr(node, attr, None)
        if meta and meta.get("handler"):
            for name, args in bindings.actions_for_handler(
                    meta["handler"], dict(meta.get("args", {}))):
                return make_action_key(name, args)
    return None

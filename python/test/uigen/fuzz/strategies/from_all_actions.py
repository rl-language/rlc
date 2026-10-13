from test.uigen.fuzz.strategies import Interaction


class FromAllActions:
    name = "from_all_actions"

    def pick(self, fuzzer, bindings, rng, covered, index, legal):
        if not index:
            return None
        universe = list(index)
        uncovered = [k for k in universe if k not in covered]
        action_key = rng.choice(sorted(uncovered or universe))
        name = action_key[0]
        expect_legal = action_key in legal or bindings.is_guarded_target(name)
        return _interaction_for(action_key, index[action_key], bindings,
                                expect_legal=expect_legal)


def _interaction_for(action_key, sites, bindings, *, expect_legal):
    node, _attr = sites[0]
    key_char = _key_char(node, action_key, bindings)
    return Interaction(node=node, key_char=key_char,
                       action_key=action_key, expect_legal=expect_legal)


def _key_char(node, action_key, bindings):
    on_key = getattr(node, "on_key", None)
    if not on_key:
        return None
    handler = on_key.get("handler")
    key_arg = bindings.key_value_arg(handler)
    if key_arg is None:
        return None
    _name, args = action_key
    value = dict(args).get(key_arg)
    return None if value is None else str(value)

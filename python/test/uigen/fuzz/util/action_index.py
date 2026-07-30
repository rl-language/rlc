from collections import defaultdict

from rlc.uigen.view_types.dispatch_actions import (
    _EVENT_ATTRS, _parse_action_str, _args_key,
)

from test.uigen.fuzz.util.layout_geometry import node_pixel_bounds


def make_action_key(name, args):
    return (name, _args_key(args))


def legal_action_keys_of_state(state):
    return {_parse_action_str(str(a)) for a in state.legal_actions}


def all_action_keys_of_state(state):
    return {_parse_action_str(str(a)) for a in state.actions}


def map_actions_to_clickable_nodes(layout, bindings):
    index = defaultdict(list)

    def walk(node):
        if node_pixel_bounds(node) is not None:
            for attr in _EVENT_ATTRS:
                meta = getattr(node, attr, None)
                if not meta or not meta.get("handler"):
                    continue
                handler = meta["handler"]
                args = dict(meta.get("args", {}))
                for name, resolved in bindings.actions_for_handler(handler, args):
                    index[make_action_key(name, resolved)].append((node, attr))
        for child in getattr(node, "children", []):
            walk(child)

    walk(layout)
    return index

import random

from rlc.uigen.view_types.dispatch_actions import _parse_action_str


def _action_name(action):
    return _parse_action_str(str(action))[0]


def random_action(state, program):
    if state.is_done():
        return False
    actions = state.legal_actions or []
    if not actions:
        return False
    state.step(random.choice(actions))
    return True


def action_by_name(name):
    def _policy(state, program):
        if state.is_done():
            return False
        for a in state.legal_actions or []:
            if _action_name(a) == name:
                state.step(a)
                return True
        return False
    return _policy


def when(predicate, inner):
    def _policy(state, program):
        if predicate(state, program):
            return inner(state, program)
        return False
    return _policy


def sequence(*policies):
    def _policy(state, program):
        for p in policies:
            if p(state, program):
                return True
        return False
    return _policy

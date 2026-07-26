from rlc.uigen.view_types.dispatch_actions import action, dispatch_action as rlc_dispatch


@action("card_click")
def card_click(program, state, player, index, **_):
    if player == 0:
        return rlc_dispatch("select_own", program, state, {"index": index})
    return rlc_dispatch("select_opponent", program, state, {"index": index})


def _step_matching(state, predicate):
    for a in state.legal_actions:
        if str(a).startswith("give_info") and predicate(str(a)):
            state.step(a)
            return True
    return False


@action("clue_color")
def clue_color(program, state, suit, **_):
    return _step_matching(state, lambda s: f"suit: {suit}" in s)


@action("clue_number")
def clue_number(program, state, value, **_):
    return _step_matching(state, lambda s: f"value: {value}" in s)

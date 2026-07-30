from test.uigen.fuzz.strategies.from_all_actions import FromAllActions
from test.uigen.fuzz.strategies.from_enabled_widgets import FromEnabledWidgets
from test.uigen.fuzz.strategies.from_random_pixels import FromRandomPixels


STRATEGIES = {
    cls.name: cls
    for cls in (FromAllActions, FromEnabledWidgets, FromRandomPixels)
}


def strategy_names():
    return list(STRATEGIES)


def make_strategy(name):
    return STRATEGIES[name]()

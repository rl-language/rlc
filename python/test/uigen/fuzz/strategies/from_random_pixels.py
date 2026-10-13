from test.uigen.fuzz.strategies import Interaction


class _PixelNode:
    def __init__(self, x, y):
        self.x, self.y = x, y
        self.width = self.height = 1


class FromRandomPixels:
    name = "from_random_pixels"

    def pick(self, fuzzer, bindings, rng, tried, index, legal):
        width, height = fuzzer.window.size
        x = rng.randint(0, width)
        y = rng.randint(0, height)
        key_char = str(rng.randint(1, 9)) if rng.random() < 0.5 else None
        return Interaction(node=_PixelNode(x, y), key_char=key_char)

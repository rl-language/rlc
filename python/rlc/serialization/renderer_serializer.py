import json
from rlc.renderer.renderable import Renderable

def save_renderer(renderable, path):
    with open(path, "w") as f:
        if renderable is not None:
            json.dump(renderable.to_dict(), f, indent=4)

def load_renderer(path):
    with open(path) as f:
        data = json.load(f)
    return Renderable.from_dict(data)
from rlc.uigen.view_types.view_type import ViewType

def save_renderer(viewType, path):
    with open(path, "w") as f:
        if viewType is not None:
            yaml_str = viewType.to_yaml()
            f.write(yaml_str)

def load_renderer(path):
    with open(path, 'r') as f:
        yaml_str = f.read()
    return ViewType.from_yaml(yaml_str)
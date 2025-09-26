import json
from pathlib import Path
import pygame
import pytest
from rlc import Layout, Text, Padding, Direction, FIXED, FIT
from rlc import LayoutLogger, LayoutLogConfig
from graphviz import Digraph

@pytest.fixture(autouse=True, scope="session")
def init_pygame():
    pygame.init()
    pygame.font.init()
    yield
    pygame.quit()

def build_simple_tree():
    root = Layout(sizing=(FIT(), FIT()), padding=Padding(5, 5, 5, 5), direction=Direction.ROW, color="white")
    child1 = Layout(sizing=(FIXED(100), FIXED(50)), color="blue")
    child2 = Text("Hello World", "Arial", 16)
    root.add_child(child1)
    root.add_child(child2)
    return root


@pytest.mark.parametrize("indent", [2, 4])
def test_json_and_text_serialization(indent, tmp_path):
    root = build_simple_tree()
    logger = LayoutLogger(LayoutLogConfig())

    # compute and layout to fill logger events
    root.compute_size(logger=logger)
    root.layout(0, 0, logger=logger)
    logger.record_final_tree(root)

    # --- JSON string ---
    js = logger.to_json()
    data = json.loads(js)
    print(data)
    assert "config" in data
    assert "events" in data
    assert "final_tree" in data
    assert data["final_tree"]["type"] == "Layout"
    assert data["final_tree"]["color"] == "white"

    # --- Write JSON file ---
    json_path = tmp_path / "layout.json"
    logger.write_json(str(json_path), indent=indent)
    assert json_path.exists()
    parsed = json.loads(json_path.read_text())
    assert parsed["final_tree"]["type"] == "Layout"
    assert parsed["final_tree"]["color"] == "white"

    # --- Text tree string ---
    txt = logger.to_text_tree(root)
    assert "Layout" in txt
    assert "Text" in txt

    # --- Write text tree file ---
    txt_path = tmp_path / "layout.txt"
    logger.write_text_tree(root, str(txt_path))
    assert txt_path.exists()
    contents = txt_path.read_text()
    assert "Layout" in contents
    assert "Text" in contents

def test_to_dot_conversion():
    """Ensure to_dot method doesn't raise exceptions for Layout and Text nodes."""
    root = build_simple_tree()
    root.compute_size()
    root.layout(0, 0)
    dot = Digraph('TestTree')
    dot.attr("node", shape="box", fontname="Arial", fontsize="10")
    
    # Test Layout node
    root.to_dot(dot)
    
    # Test Text node
    text_node = root.children[1]  # The Text node from build_simple_tree
    text_node.to_dot(dot)
    
    # No assertions on content, just ensure no exceptions
    assert True
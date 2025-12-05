from graphviz import Digraph
import json
import sys
from typing import Optional
import argparse

def visualize_layout_tree(data, output_path: str = "-", format: str = "png") -> Optional[str]:
    if isinstance(data, str):
        data = json.load(data)

    tree = data['final_tree']

    dot = Digraph('LayoutTree', filename='layout_tree' if output_path == '-' else output_path, format=format)
    dot.attr("node", shape="box", fontname="Arial", fontsize="10")

    def add_node(node):
        nid = str(node["node_id"])
        label = f'{node["type"]}#{node["node_id"]}\n'
        label += f'{node["sizing_policy"]["width"]}×{node["sizing_policy"]["height"]}\n'
        label += f'dir={node.get("direction", "-")}\n'
        label += f'size=({node["size"]["w"]}×{node["size"]["h"]})\n'
        label += f'pos=({node["position"]["x"]},{node["position"]["y"]})'
        color = node.get("color", "#dddddd")
        if node["type"] == "Text":
            txt = node.get("text_preview", "")
            preview = (txt[:20] + "…") if len(txt) > 20 else txt
            label += f'\n"{preview}"'
            color = "#dddddd"
        dot.node(nid, label, style="filled", fillcolor=color)
        for c in node.get("children", []):
            cid = str(c["node_id"])
            dot.edge(nid, cid)
            add_node(c)
    add_node(tree)
    if output_path == '-':
        return dot.source
    else:
        dot.render(cleanup=True)
        return None


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Visualize layout tree from a layout JSON file.")
    parser.add_argument("input",
                        help="Input JSON layout log (e.g., logs/layout-log.json)", nargs="?", default="-")
    parser.add_argument("-o", "--out", default='-',
                        help="Output file path or '-' for stdout (default: '-')")
    parser.add_argument("--format", default="png", choices=["png", "svg", "pdf"],
                        help="Output format (default: png)")

    args = parser.parse_args()
    if args.input != "-":
        with open(args.input, 'r', encoding='utf-8') as f:
            data = json.load(f)
    else:
        data = json.load("\n".join(sys.stdin.readlines()))

    result = visualize_layout_tree(data, args.out, args.format)
    if result:
        print(result)

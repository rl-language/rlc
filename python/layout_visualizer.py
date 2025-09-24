from graphviz import Digraph
import json
import argparse

def tree_visualizer(json_path: str, output_path: str = "logs/layout_tree", format: str = "png"):
    print(output_path, format)
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    tree = data['final_tree']

    dot = Digraph('LayoutTree', filename=output_path, format=format)
    dot.attr("node", shape="box", fontname="Arial", fontsize="10")

    def add_node(node):
        nid = str(node["node_id"])
        label = f'{node["type"]}#{node["node_id"]}\n'
        label += f'{node["sizing_policy"]["width"]}×{node["sizing_policy"]["height"]}\n'
        label += f'dir={node.get("direction", "-")}\n'
        label += f'size=({node["size"]["w"]}×{node["size"]["h"]})\n'
        label += f'pos=({node["position"]["x"]},{node["position"]["y"]})'
        if node["type"] == "Text":
            txt = node.get("text_preview", "")
            preview = (txt[:20] + "…") if len(txt) > 20 else txt
            label += f'\n"{preview}"'
        color = node.get("backgroundColor", "#dddddd")
        dot.node(nid, label, style="filled", fillcolor=color)
        for c in node.get("children", []):
            cid = str(c["node_id"])
            dot.edge(nid, cid)
            add_node(c)
    add_node(tree)
    dot.render(cleanup=True)
    print(f"[✓] Layout tree rendered to {output_path}.{format}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Visualize layout tree from a layout JSON file.")
    parser.add_argument("--in", dest="input", required=True,
                        help="Input JSON layout log (e.g., logs/layout-log.json)")
    parser.add_argument("--out", dest="output", default="layout_tree",
                        help="Output file path without extension (default: layout_tree)")
    parser.add_argument("--format", default="png", choices=["png", "svg", "pdf"],
                        help="Output format (default: png)")

    args = parser.parse_args()

    tree_visualizer(json_path=args.input)
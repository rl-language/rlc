from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Dict, Iterable, List, Optional


class NodeKind(Enum):
    """All kinds that can head a graph record."""
    ENTRY        = "entry"
    EXIT         = "exit"
    CALL         = "call*"
    CALL_ONCE    = "call"
    ACTION       = "action"
    ALTERNATIVE  = "alternative"


@dataclass
class Node:
    """
    A vertex of the call graph.

    Attributes
    ----------
    id:          the (hex) handle that uniquely identifies the node.
    kind:        semantic class of the node.
    name:        descriptive name that follows the id in the dump.
    successors:  ids of nodes reached via an **s** edge.
    callees:     ids of nodes reached via a **c** edge.
    """
    id: str
    kind: NodeKind
    name: str
    successors: List[str] = field(default_factory=list)
    callees: List[str]    = field(default_factory=list)

    # convenience helpers ­– not strictly needed for parsing
    def __str__(self) -> str:
        return f"{self.kind.value} {self.id} {self.name}"

    def __hash__(self):
        return hash(self.id)


@dataclass
class CallGraph:
    """The whole graph, addressable by node id."""
    nodes: Dict[str, Node] = field(default_factory=dict)

    def add(self, node: Node) -> None:
        if node.id in self.nodes:
            raise ValueError(f"duplicate node id {node.id}")
        self.nodes[node.id] = node

    # optional – resolve string edges to real objects after parsing
    def link(self) -> None:
        for n in self.nodes.values():
            n.successors = [self.nodes[i] for i in n.successors]
            n.callees    = [self.nodes[i] for i in n.callees]


def parse_call_graph(lines: Iterable[str]) -> CallGraph:
    """
    Build a CallGraph from the textual format:

        KIND ID NAME
        (s|c) ID
        (s|c) ID
        ...

    A new KIND line starts a new node; *s* and *c* lines that follow
    (until the next KIND) are attached to that node.
    """
    graph = CallGraph()
    current: Optional[Node] = None     # node we are currently filling

    def flush() -> None:
        nonlocal current
        if current is not None:
            graph.add(current)
            current = None

    for raw in lines:
        line = raw.strip()
        if not line:
            continue                          # ignore blank lines

        head, *rest = line.split(maxsplit=2)

        # ------------------------------------------------------------------
        # Edge records ------------------------------------------------------
        # ------------------------------------------------------------------
        if head in ("s", "c"):
            if current is None:
                raise ValueError("edge outside of a node header")
            target_id = rest[0]
            (current.successors if head == "s" else current.callees).append(target_id)
            continue

        # ------------------------------------------------------------------
        # Node header -------------------------------------------------------
        # ------------------------------------------------------------------
        flush()                               # previous node is finished
        if len(rest) < 1:
            raise ValueError(f"malformed node header: {line}")

        node_id: str       = rest[0]
        node_name: str     = rest[1] if len(rest) == 2 else ""

        try:
            kind = NodeKind(head)
        except ValueError:                    # unknown KIND → OTHER
            kind = NodeKind.OTHER

        current = Node(id=node_id, kind=kind, name=node_name)

    flush()                                   # flush the last pending node
    return graph




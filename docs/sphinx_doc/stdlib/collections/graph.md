# graph.rl

## Class Edge

### Fields

### Methods
- **Function**: `get_target_id()  -> Int`
- **Function**: `get_source_id()  -> Int`
- **Function**: `get_label()  -> ref EdgeLabel`

## Class Node

### Fields

### Methods
- **Function**: `add_edge(Node<NodeLabel, EdgeLabel> other) `
- **Function**: `add_edge(Node<NodeLabel, EdgeLabel> other, EdgeLabel label) `
- **Function**: `get_outgoing(Int index)  -> ref Edge<EdgeLabel>`
- **Function**: `get_size_outgoing()  -> Int`
- **Function**: `get_size_incomming()  -> Int`
- **Function**: `get_incomming(Int index)  -> Int`
- **Function**: `get_label()  -> ref NodeLabel`
- **Function**: `get_id()  -> Int`

## Class Graph

### Fields

### Methods
- **Function**: `add_node(Node<NodeLabel, EdgeLabel> n) `
- **Function**: `add_node() `
- **Function**: `remove_edge(Node<NodeLabel, EdgeLabel> node, Int edge_id) `
- **Function**: `erase_outgoing_edges(Node<NodeLabel, EdgeLabel> n) `
- **Function**: `erase_incomming_edges(Node<NodeLabel, EdgeLabel> n) `
- **Function**: `remove_node(Node<NodeLabel, EdgeLabel> n) `
- **Function**: `get_node(Int node_id)  -> ref Node<NodeLabel, EdgeLabel>`
- **Function**: `get_nodes_size()  -> Int`
- **Function**: `add_node(Int source, Int to) `
- **Function**: `add_node(Node<NodeLabel, EdgeLabel> source, Node<NodeLabel, EdgeLabel> to) `
- **Function**: `get_size_successors(Node<NodeLabel, EdgeLabel> n)  -> Int`
- **Function**: `get_successors(Node<NodeLabel, EdgeLabel> n, Int index_successor)  -> Node<NodeLabel, EdgeLabel>`
- **Function**: `get_size_predecessors(Node<NodeLabel, EdgeLabel> n)  -> Int`
- **Function**: `get_predecessor(Node<NodeLabel, EdgeLabel> n, Int index_successor)  -> Node<NodeLabel, EdgeLabel>`


# Copyright 2024 Massimo Fioravanti
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import collections.vector 

cls<EdgeLabel> Edge:
  Int _target_id
  Int _source_id
  EdgeLabel _label

  fun get_target_id() -> Int:
    return self._target_id

  fun get_source_id() -> Int:
    return self._source_id

  fun get_label() -> ref EdgeLabel:
    return self._label

cls<NodeLabel, EdgeLabel> Node:
  Int _id
  NodeLabel _label 
  Vector<Int> _incomming 
  Vector<Edge<EdgeLabel>> _outgoings

  fun add_edge(Node<NodeLabel, EdgeLabel> other):
    let edge : Edge<EdgeLabel>
    edge._target_id = other._id
    edge._source_id = self._id
    self._outgoings.append(edge)
    other._incomming.append(self._id)

  fun add_edge(Node<NodeLabel, EdgeLabel> other, EdgeLabel label):
    let edge : Edge<EdgeLabel>
    edge._label = label
    edge._target_id = other._id
    edge._source_id = self._id
    self._outgoings.append(edge)
    other._incomming.append(self._id)

  fun get_outgoing(Int index) -> ref Edge<EdgeLabel> {index < self._outgoings.size(), 0 <= index}:
    return self._outgoings.get(index)

  fun get_size_outgoing() -> Int:
    return self._id

  fun get_size_incomming() -> Int:
    return self._incomming.size()

  fun get_incomming(Int index) -> Int:
    return self._incomming.get(index)

  fun _set_incomming(Int index, Int new_value):
    self._incomming.get(index) = new_value 

  fun _set_outgoing(Int index, Int new_value):
    self._outgoings.get(index)._target_id = new_value 
  
  fun get_label() -> ref NodeLabel:
    return self._label

  fun get_id() -> Int:
    return self._id

cls<NodeLabel, EdgeLabel> Graph:
  Vector<Node<NodeLabel, EdgeLabel>> _nodes

  fun add_node(Node<NodeLabel, EdgeLabel> n):
    self._nodes.append(n)
    self._nodes.back()._id = self._nodes.size() - 1

  fun add_node():
    let x : Node<NodeLabel, EdgeLabel>
    x._id = self._nodes.size()
    self._nodes.append(x)

  fun _renumerate_node(Int current, Int new_id):
    ref node = self.get_node(current)
    node._id = new_id

    # replace the id of every outgoing edge and those saved in the successor nodes
    let i = 0
    while i != node.get_size_outgoing():
      node.get_outgoing(i)._source_id = new_id
      let y = 0
      ref destination = self.get_node(node.get_outgoing(i).get_target_id())
      while y != destination.get_size_incomming():
        if destination.get_incomming(y) == current: 
          destination._set_incomming(y, new_id)
        y = y + 1
      
      i = i + 1

    # replace the id of every incomming edge and those saved in the predecessor nodes
    i = 0
    while i != node.get_size_incomming():
      ref incomming = self.get_node(node.get_incomming(i))
      let y = 0
      while y != incomming.get_size_outgoing():
        if incomming.get_outgoing(y).get_source_id() == current: 
          incomming._set_outgoing(y, new_id)
        y = y + 1
      
      i = i + 1

  fun _remove_incomming(Node<NodeLabel, EdgeLabel> node, Int source_node):
    let i = 0
    while i != node.get_size_incomming():
      if node.get_incomming(i) == source_node:
        node._incomming.erase(i)
        return
      i = i + 1 

  fun remove_edge(Node<NodeLabel, EdgeLabel> node, Int edge_id):
    let target_id = node._outgoings.get(edge_id).get_target_id()
    self._remove_incomming(self._nodes.get(target_id), node.get_id())
    node._outgoings.erase(edge_id)


  fun erase_outgoing_edges(Node<NodeLabel, EdgeLabel> n):
    let i = 0
    while i != n.get_size_outgoing():
      self.remove_edge(n, i)
      i = i + 1

  fun erase_incomming_edges(Node<NodeLabel, EdgeLabel> n):
    let i = 0
    while i != n.get_size_incomming():
      ref incomming = self.get_node(n.get_incomming(i))
      let y = 0
      while y != incomming.get_size_outgoing():
        if incomming.get_outgoing(y).get_target_id() == n.get_id():
          self.remove_edge(incomming, y)
        y = y + 1
      i = i + 1

  fun remove_node(Node<NodeLabel, EdgeLabel> n):
    self.erase_incomming_edges(n)
    self.erase_outgoing_edges(n)

    let i = n.get_id() + 1
    while i != self._nodes.size():
      self._renumerate_node(i, i - 1)
      i = i + 1
    self._nodes.erase(n.get_id())

  fun get_node(Int node_id) -> ref Node<NodeLabel, EdgeLabel>:
    return self._nodes.get(node_id)

  fun get_nodes_size() -> Int:
    return self._nodes.size()

  fun add_node(Int source, Int to):
    self._nodes.get(source).add_edge(self._nodes.get(to))

  fun add_node(Node<NodeLabel, EdgeLabel> source, Node<NodeLabel, EdgeLabel> to):
    source.add_edge(to)

  fun get_size_successors(Node<NodeLabel, EdgeLabel> n) -> Int:
    return n.get_size_outgoing()

  fun get_successors(Node<NodeLabel, EdgeLabel> n, Int index_successor) -> Node<NodeLabel, EdgeLabel>:
    return self._nodes.get(n.get_outgoing(index_successor).get_target_id())

  fun get_size_predecessors(Node<NodeLabel, EdgeLabel> n) -> Int:
    return n.get_size_incomming()

  fun get_predecessor(Node<NodeLabel, EdgeLabel> n, Int index_successor) -> Node<NodeLabel, EdgeLabel>:
    return self._nodes.get(n.get_incomming(index_successor))

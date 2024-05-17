# RUN: rlc %s -o %t -i %stdlib -g
# RUN: %t

import collections.graph

fun main() -> Int:
  let x : Graph<Int, Int>
  x.add_node()
  if x.get_nodes_size() != 1:
    return -1
  x.add_node()
  x.get_node(0).add_edge(x.get_node(1))
  if x.get_node(0).get_outgoing(0).get_target_id() != 1:
    return -2

  x.remove_node(x.get_node(0))
  x.remove_node(x.get_node(0))
  return x.get_nodes_size()

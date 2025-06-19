# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import machine_learning
import collections.vector

const NODE_SIZE = 1024

cls NodeID:
  Int id

cls Graph:
  Bool[1024] nodes
  Hidden<Vector<NodeID>[1024]> edges
  BoundedVector<NodeID, 2>[1024] in_edges
  Hidden<Int> pebble_reserve
  Hidden<Int> max_used_pebble

  fun add_edge(Int source, Int target):
    let target_id : NodeID
    target_id.id = source
    self.in_edges[target].append(target_id)

    target_id.id = target
    self.edges.value[source].append(target_id)

fun main() -> Int:
  let x : Graph
  x.add_edge(100, 200)
  return x.edges.value[16].size()


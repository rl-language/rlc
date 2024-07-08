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

cls<T1,T2> Pair:
  T1 first
  T2 second

fun<T1, T2> zip(Vector<T1> x,  Vector<T2> y) -> Vector<Pair<T1, T2>>:
  let to_return : Vector<Pair<T1, T2>>
  let i = 0
  let min_size = x.size()
  if min_size > y.size():
    min_size = y.size()
  while i != min_size:
    let pair : Pair<T1,T2>
    pair.first = x.get(i)
    pair.second = y.get(i)
    to_return.append(pair)
    i = i + 1
  return to_return

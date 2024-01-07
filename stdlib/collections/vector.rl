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

ent<T> Vector:
	OwningPtr<T> data
	Int size
	Int capacity

fun<T> _grow(Vector<T> v):
	if v.capacity > v.size:
		return

	let new_data = __builtin_malloc_do_not_use<T>(v.size * 2)
	let counter = 0
	while counter < v.size * 2:
		let new_element : T
		new_data[counter] = new_element
		counter = counter + 1

	counter = 0
	while counter < v.size:
		new_data[counter] = v.data[counter] 
		__builtin_destroy_do_not_use(v.data[counter])
		counter = counter + 1

	__builtin_free_do_not_use(v.data)
	v.capacity = v.size * 2
	v.data = new_data

fun<T> init(Vector<T> v):
	v.size = 0
	v.capacity = 4
	v.data = __builtin_malloc_do_not_use<T>(4)
	let counter = 0
	while counter < v.capacity:
		let new_element : T
		v.data[counter] = new_element
		counter = counter + 1

fun<T> drop(Vector<T> v):
	let counter = 0
	while counter < v.capacity:
		__builtin_destroy_do_not_use(v.data[counter])
		counter = counter + 1
	__builtin_free_do_not_use(v.data)
	v.size = 0
	v.capacity = 0

fun<T> assign(Vector<T> lhs, Vector<T> rhs) -> Vector<T>:
	drop(lhs)	
	init(lhs)	
	let counter = 0
	while counter < rhs.size:
		lhs.append(rhs.get(counter))
		counter = counter + 1

	return lhs

fun<T> get(Vector<T> v, Int index) -> ref T:
	return v.data[index]

fun<T> set(Vector<T> v, Int index, T value):
	v.data[index] = value

fun<T> append(Vector<T> v, T value):
	v._grow()
	v.data[v.size] = value
	v.size = v.size + 1

fun<T> empty(Vector<T> v) -> Bool:
	return v.size == 0

fun<T> clear(Vector<T> v):
	while !v.empty():
		v.pop()

fun<T> pop(Vector<T> v) -> T:
	let to_return = v.data[v.size - 1]
	__builtin_destroy_do_not_use(v.data[v.size])
	v.size = v.size - 1
	return to_return

fun<T> erase(Vector<T> v, Int index):
	let counter = index
	while counter < v.size - 1: 
		v.data[counter]	= v.data[counter + 1]
	v.pop()	

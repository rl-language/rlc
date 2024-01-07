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

trait<T> ByteVectorSerializable:
	fun append_to_vector(T to_add, Vector<Byte> output)

fun append_to_vector(Int to_add, Vector<Byte> output):
	let array = __builtin_to_array(to_add)	
	let counter = 0
	while counter < 8:
		output.append(array[counter])
		counter = counter + 1 

fun append_to_vector(Float to_add, Vector<Byte> output):
	let array = __builtin_to_array(to_add)	
	let counter = 0
	while counter < 8:
		output.append(array[counter])
		counter = counter + 1 

fun append_to_vector(Bool to_add, Vector<Byte> output):
	let array = __builtin_to_array(to_add)	
	output.append(array[0])

fun append_to_vector(Byte to_add, Vector<Byte> output):
	output.append(to_add)

fun<T> append_to_vector(Vector<T> to_add, Vector<Byte> output):
	append_to_vector(to_add.size, output)
	let counter = 0
	while counter < to_add.size:
		_to_vector_impl(to_add.data[counter], output)
		counter = counter + 1 

fun<T, Int X> append_to_vector(T[X] to_add, Vector<Byte> output):
	let counter = 0
	while counter < X:
		_to_vector_impl(to_add[counter], output)
		counter = counter + 1

fun<T> _to_vector_impl(T to_add, Vector<Byte> output):
	if to_add is ByteVectorSerializable:
		to_add.append_to_vector(output)
	else:
		if to_add is Alternative:
			let counter = 0
			for field of to_add:
				using Type = type(field)
				if to_add is Type:
					counter._to_vector_impl(output)
					to_add._to_vector_impl(output)
				counter = counter + 1
		else:
			for field of to_add:
				field._to_vector_impl(output)

fun<T> as_byte_vector(T to_convert) -> Vector<Byte>:
	let vec : Vector<Byte>
	to_convert._to_vector_impl(vec)
	return vec

trait<T> ByteVectorParsable:
	fun parse_from_vector(T result, Vector<Byte> input, Int index) 

fun parse_from_vector(Int result, Vector<Byte> input, Int index):
	let to_parse : Byte[8]
	let counter = 0
	while counter < 8:
		to_parse[counter] = input.get(index)
		index = index + 1	
		counter = counter + 1 
	result = __builtin_from_array<Int>(to_parse)	

fun parse_from_vector(Float result, Vector<Byte> input, Int index):
	let to_parse : Byte[8]
	let counter = 0
	while counter < 8:
		to_parse[counter] = input.get(index)
		index = index + 1	
		counter = counter + 1 
	result = __builtin_from_array<Float>(to_parse)	

fun parse_from_vector(Bool result, Vector<Byte> input, Int index):
	let to_parse : Byte[1]
	to_parse[0] = input.get(index)
	index = index + 1	
	result = __builtin_from_array<Bool>(to_parse)	

fun parse_from_vector(Byte result, Vector<Byte> input, Int index):
	result = input.get(index)
	index = index + 1

fun<T> parse_from_vector(Vector<T> output, Vector<Byte> input, Int index):
	let size : Int
	parse_from_vector(size, input, index)
	let counter = 0
	while counter < size:
		let raw : T
		_from_vector_impl(raw, input, index)
		output.append(raw)
		counter = counter + 1 

fun<T, Int X> parse_from_vector(T[X] to_add, Vector<Byte> input, Int index):
	let counter = 0
	while counter < X:
		_from_vector_impl(to_add[counter], input, index)
		counter = counter + 1

fun<T> _from_vector_impl(T to_add, Vector<Byte> input, Int index):
	if to_add is ByteVectorParsable:
		to_add.parse_from_vector(input, index)
	else:
		if to_add is Alternative:
			let counter = 0
			counter._from_vector_impl(input, index)
			for field of to_add:
				if counter == 0:
					using Type = type(field)
					let to_parse : Type
					to_parse._from_vector_impl(input, index)
					to_add = to_parse
					return
				counter = counter - 1
		else:
			for field of to_add:
				field._from_vector_impl(input, index)

fun<T> from_byte_vector(T result, Vector<Byte> input):
	_from_vector_impl(result, input, 0)

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
	output.append(array[1])

fun append_to_vector(Byte to_add, Vector<Byte> output):
	let array = __builtin_to_array(to_add)	
	output.append(array[1])

fun<T> append_to_vector(Vector<T> to_add, Vector<Byte> output):
	append_to_vector(to_add.size, output)
	let counter = 0
	while counter < to_add.size:
		_to_vector_impl(to_add.data[counter], output)
		counter = counter + 1 

fun<T> _to_vector_impl(T to_add, Vector<Byte> output):
	if to_add is ByteVectorSerializable:
		to_add.append_to_vector(output)
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
	to_parse[1] = input.get(index)
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

fun<T> _from_vector_impl(T to_add, Vector<Byte> input, Int index):
	if to_add is ByteVectorParsable:
		to_add.parse_from_vector(input, index)
	else:
		for field of to_add:
			field._from_vector_impl(input, index)

fun<T> from_byte_vector(T result, Vector<Byte> input):
	_from_vector_impl(result, input, 0)


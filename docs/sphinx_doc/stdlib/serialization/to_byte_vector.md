# to_byte_vector.rl

## Free Functions

- **Function**: `append_to_vector(Int to_add, Vector<Byte> output) `
- **Function**: `append_to_vector(Float to_add, Vector<Byte> output) `
- **Function**: `append_to_vector(Bool to_add, Vector<Byte> output) `
- **Function**: `append_to_vector(Byte to_add, Vector<Byte> output) `
- **Function**: `append_to_vector<T : Enum>(T to_add, Vector<Byte> output) `
- **Function**: `append_to_vector<T>(Vector<T> to_add, Vector<Byte> output) `
- **Function**: `append_to_vector<T, X : Int>(T[X] to_add, Vector<Byte> output) `
- **Function**: `append_to_byte_vector<T>(T to_convert, Vector<Byte> out) `

```text
 converts `to_convert` to a sequence of bytes and adds it
 to `out`.
```

 - **Function**: `as_byte_vector<T>(T to_convert)  -> Vector<Byte>`

```text
 converts `to_convert` to a sequence of bytes 
```

 - **Function**: `parse_from_vector(Int result, Vector<Byte> input, Int index)  -> Bool`
- **Function**: `parse_from_vector(Float result, Vector<Byte> input, Int index)  -> Bool`
- **Function**: `parse_from_vector(Bool result, Vector<Byte> input, Int index)  -> Bool`
- **Function**: `parse_from_vector(Byte result, Vector<Byte> input, Int index)  -> Bool`
- **Function**: `parse_from_vector<T>(Vector<T> output, Vector<Byte> input, Int index)  -> Bool`
- **Function**: `parse_from_vector<X : Enum>(X to_add, Vector<Byte> input, Int index)  -> Bool`
- **Function**: `parse_from_vector<T, X : Int>(T[X] to_add, Vector<Byte> input, Int index)  -> Bool`
- **Function**: `from_byte_vector<T>(T result, Vector<Byte> input)  -> Bool`

```text
 converts the bytes in `input` into a T and 
 assigns the value to `result`. Returns false if the conversion failed.
```

 - **Function**: `from_byte_vector<T>(T result, Vector<Byte> input, Int read_bytes)  -> Bool`

```text
 converts the bytes in `input` starting at `read_bytes` into a T and 
 assigns the value to `result`. Returns false if the conversion failed.
 read_bytes is advanced up to the index of the first bytes not used to
 parse `result`
```

 
## Traits

## Trait ByteVectorSerializable


```text
 Trait that must be implemented by a type 
 to override the standad way it is added to
 a vector of bytes
```

 - **Function**: `append_to_vector(T to_add, Vector<Byte> output) `

## Trait ByteVectorParsable


```text
 Trait that can be implemented to override the default conversion
 from a array of bytes to a object.
 It must be implemented if the trait has implemented ByteVectorSerializable
```

 - **Function**: `parse_from_vector(T result, Vector<Byte> input, Int index)  -> Bool`



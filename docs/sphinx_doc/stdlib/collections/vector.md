# vector.rl

## Class Vector


```text
 A memory contiguous datastructure similar to cpp vector.
 A vector is a list of elements. Just like cpp vector,
 the contents may be reallocated when added or deleated,
 so references elements are invalidated if the 
 vector is modified.
```

 ### Fields

### Methods
- **Function**: `init() `
- **Function**: `drop() `
- **Function**: `assign(Vector<T> other) `
- **Function**: `resize(Int new_size) `

```text
 changes the size of the vector
 to be equal to `new_size`
 if the original size is larger
 than the new size, the extra 
 elements are destroyed
 
 if the original size is smaller
 than the new size, extra elements
 are constructed
```

 - **Function**: `back()  -> ref T`

```text
 returns a reference to the
 last element of the vector
```

 - **Function**: `get(Int index)  -> ref T`

```text
 returns a reference to the element
 with the provided index
```

 - **Function**: `set(Int index, T value) `

```text
 assigns `value` to the element
 of the vector at the provided
 index
```

 - **Function**: `append(T value) `

```text
 appends `value` to the 
 end of the vector
```

 - **Function**: `empty()  -> Bool`

```text
 returns true if the
 size of the vector is equal
 to zero
```

 - **Function**: `clear() `

```text
 erases all the elements
 of the vector
```

 - **Function**: `pop()  -> T`

```text
 removes the last element
 of the vector and returns
 it by copy
```

 - **Function**: `drop_back(Int quantity) `

```text
 removes `quantity` elements
 from the back of the vector
```

 - **Function**: `erase(Int index) `

```text
 erase the element with the provided
 `index`
```

 - **Function**: `size()  -> Int`

## Class BoundedVector


```text
 A bounded vector is a wrapper
 around a vector that prevents 
 the size of the vector to ever 
 exced `max_size`
 this class is usefull when used
 for machine learning techniques, 
 since it allows the machine learning
 to know the maximal possible size
 of this vector when converted to
 a tensor
```

 ### Fields

### Methods
- **Function**: `assign(BoundedVector<T, max_size> other) `
- **Function**: `resize(Int new_size) `

```text
 identical to Vector::resize,
 except `new_size` is clamped
 to be at most `max_size`
```

 - **Function**: `max_size()  -> Int`

```text
 returns the maximal possible
 size of this vector
```

 - **Function**: `back()  -> ref T`

```text
 same as vector::back
```

 - **Function**: `get(Int index)  -> ref T`

```text
 same as vector::get
```

 - **Function**: `set(Int index, T value) `

```text
 same as vector::set
```

 - **Function**: `append(T value) `

```text
 append `value` to the end
 of the vector, but only if
 doing so would not excede
 max vector maximal size
```

 - **Function**: `empty()  -> Bool`

```text
 same as vector::empty
```

 - **Function**: `clear() `

```text
 same as vector::clear
```

 - **Function**: `pop()  -> T`

```text
 same as vector::pop
```

 - **Function**: `drop_back(Int quantity) `

```text
 same as vector::drop_back
```

 - **Function**: `erase(Int index) `

```text
 same as vector::erase
```

 - **Function**: `size()  -> Int`

```text
 same as vector::size
```

 

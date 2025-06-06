# dictionary.rl

## Class Entry

### Fields
- `Bool occupied`
- `Int hash`
- `KeyType key`
- `ValueType value`

## Class Dict

### Fields

### Methods
- **Function**: `init() `
- **Function**: `insert(KeyType key, ValueType value)  -> Bool`
- **Function**: `get(KeyType key)  -> ValueType`

```text
 Add safety counter
 Create local copies of key and value to avoid modifying the input parameters
 Add safety check to prevent infinite loops
 Update the actual entry in entries
 Update the actual entry in entries
 Update the swapped entry

```

- **Function**: `contains(KeyType key)  -> Bool`

```text
 Quick return for empty dictionary
 Add safety counter
 Add safety check to prevent infinite loops

```

- **Function**: `remove(KeyType key)  -> Bool`

```text
 Quick return for empty dictionary
 Add safety counter
 Add safety check to prevent infinite loops

```

- **Function**: `keys()  -> Vector<KeyType>`

```text
 Add safety counter
 Add safety check to prevent infinite loops
 Perform backward-shift operation
 Shift elements until we find an empty slot or an element with probe distance 0
 Calculate probe distance of the next element
 If probe distance is 0, it's already at its ideal position
 Move the element back
 Move to next positions

```

- **Function**: `values()  -> Vector<ValueType>`
- **Function**: `empty()  -> Bool`

```text
 returns true if the
 size of the dictionary is equal
 to zero

```

- **Function**: `size()  -> Int`

```text
 returns true if the
 size of the dictionary is equal
 to zero

```

- **Function**: `clear() `

```text
 erases all the elements
 of the dictionary

```

- **Function**: `drop() `

```text
 Create new, larger entries array
 Ensures growth by at least 1
 Initialize new entries
 Copy old entries to new array, but only scan up to old_capacity
 Insert directly without triggering another growth
 Clean up old entries

```



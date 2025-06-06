# bounded_arg.rl

## Class BInt


```text
 A integer with a max and min, so that
 enumerate will return the range of values
 between the two. Machine learning serialization
 will serialize this class as a one-hot vector

```

### Fields
- `Int value`

### Methods
- **Function**: `init() `
- **Function**: `equal(Int other)  -> Bool`
- **Function**: `equal(BInt<min, max> other)  -> Bool`
- **Function**: `less(BInt<min, max> other)  -> Bool`
- **Function**: `less(Int other)  -> Bool`
- **Function**: `greater(BInt<min, max> other)  -> Bool`
- **Function**: `greater(Int other)  -> Bool`
- **Function**: `greater_equal(BInt<min, max> other)  -> Bool`
- **Function**: `greater_equal(Int other)  -> Bool`
- **Function**: `less_equal(BInt<min, max> other)  -> Bool`
- **Function**: `less_equal(Int other)  -> Bool`
- **Function**: `assign(Int other) `
- **Function**: `not_equal(Int other)  -> Bool`
- **Function**: `not_equal(BInt<min, max> other)  -> Bool`
- **Function**: `add(Int val)  -> BInt<min, max>`
- **Function**: `add(BInt<min, max> other)  -> BInt<min, max>`
- **Function**: `mul(BInt<min, max> other)  -> BInt<min, max>`
- **Function**: `reminder(Int val)  -> BInt<min, max>`
- **Function**: `reminder(BInt<min, max> val)  -> BInt<min, max>`
- **Function**: `mul(Int val)  -> BInt<min, max>`
- **Function**: `sub(BInt<min, max> other)  -> BInt<min, max>`
- **Function**: `sub(Int val)  -> BInt<min, max>`

## Class LinearlyDistributedInt


```text
 A integer with a max and min, so that
 enumerate will return the range of values
 between the two, and machine learning serialization 
 will serialize it as a single float with normalized value 
 (real_value - ((max - min) / 2)) / (max - min). 
 This class makes sense when it is used to rappresent
 integers that appear with the same frequency for
 each possible value.

```

### Fields
- `Int value`

### Methods
- **Function**: `init() `
- **Function**: `equal(Int other)  -> Bool`
- **Function**: `equal(LinearlyDistributedInt<min, max> other)  -> Bool`
- **Function**: `less(LinearlyDistributedInt<min, max> other)  -> Bool`
- **Function**: `less(Int other)  -> Bool`
- **Function**: `greater(LinearlyDistributedInt<min, max> other)  -> Bool`
- **Function**: `greater(Int other)  -> Bool`
- **Function**: `greater_equal(LinearlyDistributedInt<min, max> other)  -> Bool`
- **Function**: `greater_equal(Int other)  -> Bool`
- **Function**: `less_equal(LinearlyDistributedInt<min, max> other)  -> Bool`
- **Function**: `less_equal(Int other)  -> Bool`
- **Function**: `assign(Int other) `
- **Function**: `not_equal(Int other)  -> Bool`
- **Function**: `not_equal(LinearlyDistributedInt<min, max> other)  -> Bool`
- **Function**: `add(Int val)  -> LinearlyDistributedInt<min, max>`
- **Function**: `add(LinearlyDistributedInt<min, max> other)  -> LinearlyDistributedInt<min, max>`
- **Function**: `mul(LinearlyDistributedInt<min, max> other)  -> LinearlyDistributedInt<min, max>`
- **Function**: `reminder(Int val)  -> LinearlyDistributedInt<min, max>`
- **Function**: `reminder(LinearlyDistributedInt<min, max> val)  -> LinearlyDistributedInt<min, max>`
- **Function**: `mul(Int val)  -> LinearlyDistributedInt<min, max>`
- **Function**: `sub(LinearlyDistributedInt<min, max> other)  -> LinearlyDistributedInt<min, max>`
- **Function**: `sub(Int val)  -> LinearlyDistributedInt<min, max>`

## Free Functions

- **Function**: `max<min : Int, max : Int>(BInt<min, max> l, BInt<min, max> r)  -> BInt<min, max>`
- **Function**: `min<min : Int, max : Int>(BInt<min, max> l, BInt<min, max> r)  -> BInt<min, max>`
- **Function**: `append_to_vector<min : Int, max : Int>(BInt<min, max> to_add, Vector<Byte> output) `
- **Function**: `parse_from_vector<min : Int, max : Int>(BInt<min, max> to_add, Vector<Byte> output, Int index)  -> Bool`
- **Function**: `append_to_string<min : Int, max : Int>(BInt<min, max> to_add, String output) `
- **Function**: `parse_string<min : Int, max : Int>(BInt<min, max> to_add, String input, Int index)  -> Bool`
- **Function**: `enumerate<min : Int, max : Int>(BInt<min, max> to_add, Vector<BInt<min, max>> output) `
- **Function**: `tensorable_warning<min : Int, max : Int>(BInt<min, max> x, String out) `
- **Function**: `max<min : Int, max : Int>(LinearlyDistributedInt<min, max> l, LinearlyDistributedInt<min, max> r)  -> LinearlyDistributedInt<min, max>`
- **Function**: `min<min : Int, max : Int>(LinearlyDistributedInt<min, max> l, LinearlyDistributedInt<min, max> r)  -> LinearlyDistributedInt<min, max>`
- **Function**: `append_to_vector<min : Int, max : Int>(LinearlyDistributedInt<min, max> to_add, Vector<Byte> output) `
- **Function**: `parse_from_vector<min : Int, max : Int>(LinearlyDistributedInt<min, max> to_add, Vector<Byte> output, Int index)  -> Bool`
- **Function**: `append_to_string<min : Int, max : Int>(LinearlyDistributedInt<min, max> to_add, String output) `
- **Function**: `parse_string<min : Int, max : Int>(LinearlyDistributedInt<min, max> to_add, String input, Int index)  -> Bool`
- **Function**: `enumerate<min : Int, max : Int>(LinearlyDistributedInt<min, max> to_add, Vector<LinearlyDistributedInt<min, max>> output) `
- **Function**: `tensorable_warning<min : Int, max : Int>(LinearlyDistributedInt<min, max> x, String out) `


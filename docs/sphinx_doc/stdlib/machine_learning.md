# machine_learning.rl

## Class Hidden


```text
 A Hidden object rapresents a object that is not visibile to machine
 learning algorithms. For example a machine learning algorithms learning
 to play black jack should not have access to the deck of cards, so it can be wrapped into a hidden to achieve that effect

```

### Fields
- `T value`

### Methods
- **Function**: `assign(T content) `

## Class HiddenInformation

### Fields
- `T value`
- `Int owner`

### Methods
- **Function**: `assign(T content) `

## Free Functions

- **Function**: `write_in_observation_tensor<T>(Hidden<T> obj, Int observer_id, Vector<Float> output, Int index) `

```text
 since the underlying object is hidden, this function does nothing.

```

- **Function**: `size_as_observation_tensor<T>(Hidden<T> obj)  -> Int`

```text
 since the underlying object is hidden, always returns zero 

```

- **Function**: `append_to_vector<T>(Hidden<T> to_add, Vector<Byte> output) `
- **Function**: `parse_from_vector<T>(Hidden<T> to_add, Vector<Byte> output, Int index)  -> Bool`
- **Function**: `append_to_string<T>(Hidden<T> to_add, String output) `
- **Function**: `parse_string<T>(Hidden<T> to_add, String input, Int index)  -> Bool`
- **Function**: `write_in_observation_tensor<T>(HiddenInformation<T> obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor<T>(HiddenInformation<T> obj)  -> Int`


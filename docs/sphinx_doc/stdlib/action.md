# action.rl

## Free Functions

- **Function**: `apply<FrameType, ActionType>(Vector<ActionType> action, FrameType frame)  -> Bool`
- **Function**: `parse_and_execute<FrameType, AllActionsVariant>(FrameType state, AllActionsVariant variant, Vector<Byte> input, Int read_bytes) `
- **Function**: `parse_actions<AllActionsVariant>(AllActionsVariant variant, Vector<Byte> input, Int read_bytes)  -> Vector<AllActionsVariant>`
- **Function**: `parse_actions<AllActionsVariant>(AllActionsVariant variant, String input)  -> Vector<AllActionsVariant>`
- **Function**: `parse_action_optimized<AllActionsVariant>(AllActionsVariant variant, Vector<Byte> input, Int read_bytes)  -> Bool`

```text
 parses actions taking only one byte and by taking taking the reminder of the parsed number divided by the number of actions, so that no action is ever marked as invalid

```

- **Function**: `parse_actions<AllActionsVariant>(AllActionsVariant variant, Vector<Byte> input)  -> Vector<AllActionsVariant>`
- **Function**: `parse_and_execute<FrameType, AllActionsVariant>(FrameType state, AllActionsVariant variant, Vector<Byte> input) `
- **Function**: `make_valid_actions_vector<FrameType, ActionType>(Vector<ActionType> all_actions, FrameType state)  -> Vector<Byte>`
- **Function**: `get_valid_actions<FrameType, ActionType>(Vector<Byte> valid_actions, Vector<ActionType> all_actions, FrameType state) `
- **Function**: `gen_python_methods<FrameType, AllActionsVariant>(FrameType state, AllActionsVariant variant) `

```text
 method that bust be present in binary to ensure that all methods 
 required by rlc-learn are available

```

- **Function**: `load_action_vector_file<ActionType>(String file_name, Vector<ActionType> out)  -> Bool`
- **Function**: `enumerate(Bool b, Vector<Bool> output) `
- **Function**: `enumeration_error(Int x, String out, Vector<String> context) `
- **Function**: `enumeration_error(Float x, String out, Vector<String> context) `
- **Function**: `enumeration_error<T>( x, String out, Vector<String> context) `
- **Function**: `enumeration_error<T, size : Int>(T[size] x, String out, Vector<String> context) `
- **Function**: `enumeration_error<T>(Vector<T> x, String out, Vector<String> context) `
- **Function**: `get_enumeration_errors_impl<T>(T obj, String out, Vector<String> context) `
- **Function**: `get_enumeration_errors<T>(T obj)  -> String`
- **Function**: `print_enumeration_errors<T>(T obj)  -> Bool`
- **Function**: `enumerate<T : Enum>(T b, Vector<T> output) `
- **Function**: `enumerate<T>(T obj)  -> Vector<T>`
- **Function**: `write_in_observation_tensor(Int value, Int min, Int max, Vector<Float> output, Int index) `
- **Function**: `write_in_observation_tensor(Int obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor(Int obj)  -> Int`
- **Function**: `write_in_observation_tensor(Float obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor(Float obj)  -> Int`
- **Function**: `write_in_observation_tensor(Bool obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor(Bool obj)  -> Int`
- **Function**: `write_in_observation_tensor(Byte obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor(Byte obj)  -> Int`
- **Function**: `write_in_observation_tensor<T, X : Int>(T[X] obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor<T, X : Int>(T[X] obj)  -> Int`
- **Function**: `write_in_observation_tensor<T>(Vector<T> obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor<T>(Vector<T> obj)  -> Int`
- **Function**: `write_in_observation_tensor<T, max_size : Int>(BoundedVector<T, max_size> obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor<T, max_size : Int>(BoundedVector<T, max_size> obj)  -> Int`
- **Function**: `write_in_observation_tensor<min : Int, max : Int>(BInt<min, max> obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor<min : Int, max : Int>(BInt<min, max> obj)  -> Int`
- **Function**: `write_in_observation_tensor<min : Int, max : Int>(LinearlyDistributedInt<min, max> obj, Int observer_id, Vector<Float> output, Int index) `
- **Function**: `size_as_observation_tensor<min : Int, max : Int>(LinearlyDistributedInt<min, max> obj)  -> Int`
- **Function**: `to_observation_tensor<T>(T obj, Int observer_id, Vector<Float> output) `
- **Function**: `to_observation_tensor<T>(T obj, Int observer_id, Vector<Float> output, Int written_bytes) `
- **Function**: `to_observation_tensor<T>(T obj, Int observer_id)  -> Vector<Float>`
- **Function**: `observation_tensor_size<T>(T obj)  -> Int`
- **Function**: `write_tensor_warning_context(String out, Vector<String> context) `
- **Function**: `tensorable_warning(Int x, String out, Vector<String> context) `
- **Function**: `tensorable_warning(Float x, String out, Vector<String> context) `
- **Function**: `tensorable_warning<T>( x, String out, Vector<String> context) `
- **Function**: `tensorable_warning<T, size : Int>(T[size] x, String out, Vector<String> context) `
- **Function**: `tensorable_warning<T>(Vector<T> x, String out, Vector<String> context) `
- **Function**: `to_observation_tensor_warnings<T>(T obj)  -> String`
- **Function**: `emit_observation_tensor_warnings<T>(T obj) `

## Traits

## Trait ApplicableTo

- **Function**: `apply(ActionType action, FrameType frame) `

## Trait Enumerable


```text
 trait that must be implemented by a type to enumerate all
 possible values of that types (ex: enumerate(bool) return 
 {true, false})
 
 this is used by machine learning techniques that need to
 enumerate all possible actions.

```

- **Function**: `enumerate(T obj, Vector<T> output) `

## Trait CustomEnumerationError

- **Function**: `enumeration_error(T x, String out, Vector<String> context) `

## Trait Tensorable


```text
 trait that must be implemented to specify how
 a given type is to be converted into a tensor
 for machine learning consumptions. The encoding
 should be, when possible, one-hot encoding.

```

- **Function**: `write_in_observation_tensor(T obj, Int observer_id, Vector<Float> output, Int counter) `
- **Function**: `size_as_observation_tensor(T obj)  -> Int`

## Trait CustomTensorWarnings

- **Function**: `tensorable_warning(T x, String out, Vector<String> context) `



# rng.rl

## Class RNG

### Fields
- `Int[4] s`

### Methods
- **Function**: `randint(Int min, Int max)  -> Int`
- **Function**: `set_seed(Int new_seed) `
- **Function**: `next()  -> Int`

## Actions

- **Action**: `configure_rng() -> ConfigureRNG`
  - **Action Statement**: `set_rng_bit(Bool bit)`

## Free Functions

- **Function**: `make_rng(Int seed)  -> RNG`
- **Function**: `size_as_observation_tensor(RNG obj)  -> Int`
- **Function**: `size_as_observation_tensor(ConfigureRNG obj)  -> Int`
- **Function**: `write_in_observation_tensor(RNG obj, Int observer_id, Vector<Float> output, Int counter) `
- **Function**: `write_in_observation_tensor(ConfigureRNG obj, Int observer_id, Vector<Float> output, Int counter) `


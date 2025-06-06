# to_hash.rl

## Free Functions

- **Function**: `compute_hash(Int value)  -> Int`

```text
 Specialized implementations for basic types

```

- **Function**: `compute_hash(Float value)  -> Int`
- **Function**: `compute_hash(Bool value)  -> Int`
- **Function**: `compute_hash(Byte value)  -> Int`
- **Function**: `compute_hash(String str)  -> Int`

```text
 Add String hashing - assuming a String type exists
 Using FNV-1a hash algorithm which is fast and has good distribution

```

- **Function**: `compute_hash<T>(Vector<T> vector)  -> Int`

```text
 Implementations for collections

```

- **Function**: `compute_hash<T, N : Int>(T[N] array)  -> Int`
- **Function**: `compute_hash_of<T>(T value)  -> Int`

```text
 The public interface

```


## Traits

## Trait Hashable

- **Function**: `compute_hash(T to_hash)  -> Int`



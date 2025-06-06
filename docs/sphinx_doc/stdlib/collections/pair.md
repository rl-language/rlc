# pair.rl

## Class Pair


```text
 A parameterized container with two fields, first and second

```

### Fields
- `T1 first`
- `T2 second`

## Free Functions

- **Function**: `zip<T1, T2>(Vector<T1> x, Vector<T2> y)  -> Vector<Pair<T1, T2>>`

```text
 Accepts two vectors x, y of types T1, T2 of any length and returns 
 a vector with length equal to the shortest of the lengths of the 
 two input vectors. output.get(i).first == x.get(i) and 
 output.get(i).second == y.get(i) for all i from 0 to output.length()

```



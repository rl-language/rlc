# string.rl

## Class String


```text
 a c style string, composed of a series of ascii characters
 terminated by a null terminator. If the string changes, it 
 may be reallocated, causing all references to its 
 characters to be invalidated.

```

### Fields

### Methods
- **Function**: `init() `
- **Function**: `append(Byte b) `

```text
 append the byte b, interpreted as a ascii
 character to the string

```

- **Function**: `get(Int index)  -> ref Byte`

```text
 returns a reference to the character
 at the `index` location

```

- **Function**: `substring_matches(StringLiteral lit, Int pos)  -> Bool`

```text
 returns true if the current string contains the
 string `lit` starting from the character at `pos`

```

- **Function**: `size()  -> Int`

```text
 returns the size of the string, excluding
 the null terminator

```

- **Function**: `count(Byte b)  -> Int`

```text
 returns the number of occurences
 a certain character appears in the string

```

- **Function**: `append(StringLiteral str) `

```text
 appens `str` to the current string

```

- **Function**: `append(String str) `

```text
 appens `str` to the current string

```

- **Function**: `append_quoted(String str) `
- **Function**: `add(String other)  -> String`

```text
 returns the concatenation of this
 string and `other`, without modifying
 this string.

```

- **Function**: `equal(StringLiteral other)  -> Bool`

```text
 returns true if every character of this
 string is equal to every character of
 the `other` string

```

- **Function**: `equal(String other)  -> Bool`

```text
 returns true if every character of this
 string is equal to every character of
 the `other` string

```

- **Function**: `not_equal(String other)  -> Bool`
- **Function**: `not_equal(StringLiteral other)  -> Bool`
- **Function**: `drop_back(Int quantity) `

```text
 removes `quantity` characters from the
 end of the string

```

- **Function**: `back()  -> ref Byte`

```text
 returns a reference the last character before 
 the null terminator of the string

```

- **Function**: `reverse() `

```text
 reverses inplace the string

```

- **Function**: `to_indented_lines()  -> String`

```text
 returns a string such that every parantesys
 in this string generates a new line indented
 as many times as the number of 
 parentesys not yet closed

```


## Free Functions

- **Function**: `s(StringLiteral literal)  -> String`

```text
 transforms a string literal into a String
 ex: let str = "hey"s.add(" bye")

```

- **Function**: `append_to_string(StringLiteral x, String output) `
- **Function**: `load_file(String file_name, String out)  -> Bool`
- **Function**: `append_to_string(Int x, String output) `
- **Function**: `append_to_string(Byte x, String output) `
- **Function**: `append_to_string(Float x, String output) `
- **Function**: `append_to_string(String x, String output) `
- **Function**: `append_to_string(Bool x, String output) `
- **Function**: `append_to_string<T, X : Int>(T[X] to_add, String output) `
- **Function**: `append_to_string<T>(Vector<T> to_add, String output) `
- **Function**: `append_to_string<T, dim : Int>(BoundedVector<T, dim> to_add, String output) `
- **Function**: `append_to_string<T : Enum>(T to_add, String output) `
- **Function**: `to_string<T>(T to_stringyfi, String output) `
- **Function**: `to_string<T>(T to_stringyfi)  -> String`

```text
 convert a object type into a string 

```

- **Function**: `is_space(Byte b)  -> Bool`
- **Function**: `is_alphanumeric(Byte b)  -> Bool`
- **Function**: `is_open_paren(Byte b)  -> Bool`
- **Function**: `is_close_paren(Byte b)  -> Bool`
- **Function**: `parse_string(Int result, String buffer, Int index)  -> Bool`
- **Function**: `parse_string(Byte result, String buffer, Int index)  -> Bool`
- **Function**: `parse_string(Float result, String buffer, Int index)  -> Bool`
- **Function**: `length(StringLiteral literal)  -> Int`

```text
 returns the length of a string literal

```

- **Function**: `parse_string(String result, String buffer, Int index)  -> Bool`
- **Function**: `parse_string(Bool result, String buffer, Int index)  -> Bool`
- **Function**: `parse_string<T, X : Int>(T[X] result, String buffer, Int index)  -> Bool`
- **Function**: `parse_string<T>(Vector<T> result, String buffer, Int index)  -> Bool`
- **Function**: `parse_string<T, x : Int>(BoundedVector<T, x> result, String buffer, Int index)  -> Bool`
- **Function**: `parse_string<T : Enum>(T result, String buffer, Int index)  -> Bool`
- **Function**: `from_string<T>(T result, String buffer)  -> Bool`
- **Function**: `from_string<T>(T result, String buffer, Int index)  -> Bool`

```text
 given a String *buffer* and a offset into that string index,
 parses buffer starting from *index* and uses those information
 to set the value of *result*.

```


## Traits

## Trait StringSerializable


```text
 Trait that can be implemented by a type to override
 the regular conversion to string for that type.

```

- **Function**: `append_to_string(T to_add, String output) `

## Trait CustomGetTypeName


```text
 Trait that can be implemented by a type to override
 which name is displayed in serialization of alternative 
 types

```

- **Function**: `get_type_name(T to_add)  -> StringLiteral`

## Trait StringParsable


```text
 Trait that must be implemented to allow to convert
 string into a type when StringSerializable has been
 overwritten as well.

```

- **Function**: `parse_string(T result, String buffer, Int index)  -> Bool`



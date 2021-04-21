# Hashing support

## FNV-1a hashing algorithm introduced in the library
Implemented in header [Hash.hpp](../cpp/Hash.hpp)

## Forward list
### Definition:
```cpp
template < typename HashType, typename Type = void >
struct hash;

template < typename HashType, typename Type >
requires (std::input_iterator<Type>)
struct hash<HashType, Type>; // Iterable specification

template < typename HashType, typename Type >
requires (is_integral<Type>::value)
struct hash<HashType, Type>; // Integral specification
```

### Public members:
| Alias | Type |
| :---- | :--- |
| `ResultType` | `HashType` |

### Member functions:
| Method | Arguments | Return type | Description |
| :----- | :-------- | :---------- | :---------- |
| `get_hash` | `Type begin` | `ResultType` | Returns the hash from the start of the sequence to `\0` assuming it is a character one |
| `get_hash` | `Type begin, Type end` | `ResultType` | Returns the hash from the start of the sequence to the end of it |
| (Integral specification) `get_hash` | `Type number` | `ResultType` | Returns back the number |
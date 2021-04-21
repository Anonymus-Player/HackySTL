# Utilities support

## Basic utilities introduced in the library
Implemented in header [Utility.hpp](../cpp/Utility.hpp)

| Function | Arguments | Return type | Description |
| :------- | :-------- | :---------- | :---------- |
| `move` | `Type&& val` | `remove_reference_t<Type>&&` | Move the value by casting the value into a pure r-value reference |
| `move` | `InIter first`, `InIter last`, `OutIter dest` | `OutIter` | Sequentially moves the values in the range of `first -> last` to the destination, `dest` |
| `release` | `Type&& val` | `remove_reference_t<Type>` | Creates a copy by moving the value |
| `forward` | `remove_reference<Type>&`/`&& val` | `Type&&` | Passes the value(s) from the called to the invoked function |
| `construct_at` | `Type* ptr`, `Args...&& args` | `void` | Creates a new object at a memory address (see in-place construction) |
| `exchange` | `Type& target`, `UType&& new_val` | `Type` | Overwrites the `target` with `new_val` and returns the old state of the `target` |
| `addressof` | `const`/`Type& value` | `const`/`Type*` | Obtains the real address of the `Type` object |
| `swap` | `Type& first`, `Type& second` | `void` | Swaps to values by performing 3 moves |
| `copy` | `InIter first`, `InIter last`, `OutIter dest` | `OutIter` | Sequentially copies the values in the range of `first -> last` to the destination, `dest` |
| `copy_n` | `InIter first`, `usize n`, `OutIter dest` | `OutIter` | Sequentially copies the values in the range of `0 -> n` to the destination, `dest` |
| `copy_if` | `InIter first`, `InIter last`, `OutIter dest`, `Predicate pred` | `OutIter` | Sequentially copies the values in the range of `first -> last` to the destination, `dest` if the predicate, `pred` is evaluated to `true` |
| `set` | `InIter first`, `InIter last`, `Type val` | `void` | Sequentially sets the values in the range of `first -> last` with a defined value, `val` |
| `begin` | `Type (&arr)[Count]` | `Type*` | Gets the address of the first element of an array |
| `end` | `Type (&arr)[Count]` | `Type*` | Gets the address of the last element of an array |

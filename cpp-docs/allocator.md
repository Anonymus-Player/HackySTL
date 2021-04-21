# Allocators support

## Multiple allocation strategies introduced in the library
Implemented in header [Allocator.hpp](../cpp/Allocator.hpp)

## Allocators:

### Error type:
#### Definition:
```cpp
class allocator_error
{
private:
    const char* _err = nullptr;
    
public:
    HSD_CONSTEXPR allocator_error(const char* error)
        : _err{error}
    {}
    
    const char* operator()() const
    {
        return _err;
    }
};
```

### Buffered allocator
#### Definition:
```cpp
template <typename Type>
class buffered_allocator;
```

#### Description:
Allocates a block of memory in a buffer with a defined size by using block structure with the definition of:
```cpp
struct block 
{
    usize size{};
    bool in_use{};
    uchar data[]; // flexible array for accessing the data
};
```

#### Public members:
| Member | Value/Type |
| :----- | :--------- |
| `pointer_type` | `Type*` |
| `value_type` | `Type` |

#### Member functions:
| Method | Arguments | Return type | Description |
| :----- | :-------- | :---------- | :---------- |
| `buffered_allocator` | `uchar* buf`, `usize size` | `/*compiler defined*/` | Sets the internal buffer for allocations and size |
| `buffered_allocator` | `const buffered_allocator& other` | `/*compiler defined*/` | Copy constructor |
| `buffered_allocator` | `buffered_allocator&& other` | `/*compiler defined*/` | Move constructor |
| `operator=` | `const buffered_allocator<UType>& rhs` | `buffered_allocator&` | Copy attribution |
| `allocate` | `usize size` | `Result<Type*, allocator_error>` | Allocates a block with a size of `sizeof(Type) * size` inside the buffer |
| `deallocate` | `Type* ptr`, `usize` | `Result<void, allocator_error>` | Deallocates a block with a size defined is its structured inside the buffer |
| `print_buffer` | `N/A` | `void` | If it is compiled in debug mode this method is enabled and prints the buffer in a matrix configuration |


#### Example:

```cpp
#include <Allocator.hpp>
#include <Io.hpp>

template <typename T>
class uses_allocator
    : private hsd::buffered_allocator<T>
{
private:
    usize _size = 0;

public:
    template <typename U = T>
    uses_allocator(const T& value, usize size, const hsd::buffered_allocator<U>& alloc)
        : _size{size}, hsd::buffered_allocator<T>(alloc)
    {
        // you need to use this explicitly
        this->_data = this->allocate(size).unwrap();
        hsd::set(this->_data, this->_data + this->_size, hsd::forward<T>(value));
    }

    template <typename U = T>
    uses_allocator(T&& value, usize size, const hsd::buffered_allocator<U>& alloc)
        : _size{size}, hsd::buffered_allocator<T>(alloc)
    {
        // you need to use this explicitly
        this->_data = this->allocate(size).unwrap();
        hsd::set(this->_data, this->_data + this->_size, hsd::forward<T>(value));
    }

    ~uses_allocator()
    {
        for(usize _index = 0; _index < this->_size; _index++)
            this->_data[_index].~T();
        
        this->deallocate(this->_data, this->_size);
    }

    T* get_pointer()
    {
        return this->_data;
    }

    T& get_reference(usize index)
    {
        return get_pointer()[index];
    }
};

int main()
{
    uchar buffer[1024]{};
    hsd::buffered_allocator<char> alloc{buffer, 1024};

    uses_allocator<char> a{'\0', 5, alloc};
    a.get_reference(0) = 't';
    a.get_reference(1) = 'e';
    a.get_reference(2) = 's';
    a.get_reference(3) = 't';
    hsd::io::print<"{}">(a.get_pointer);

    return 0;
}
```

### Heap allocator
#### Definition:
```cpp
template <typename Type>
class allocator;
```

#### Public members:
| Member | Value/Type |
| :----- | :--------- |
| `pointer_type` | `Type*` |
| `value_type` | `Type` |

#### Description:
Allocates memory on the heap allocation `::operator new`

#### Member functions:
| Method | Arguments | Return type | Description |
| :----- | :-------- | :---------- | :---------- |
| `allocator` | `N/A` | `/*implementation defined*/` | Sets the type size and the alignment according to `Type` |
| `allocator` | `const allocator& other` | `/*implementation defined*/` | Copy constructor |
| `allocator` | `allocator&& other` | `/*implementation defined*/` | Move constructor |
| `operator=` | `const allocator<UType>& rhs` | `allocator&` | Copy attribution |
| `allocate` | `usize size` | `Result<Type*, allocator_error>` | Allocates a block with a size of `type_size * size` and with an `alignment` on the heap |
| `deallocate` | `Type* ptr`, `usize size` | `Result<void, allocator_error>` | Deallocates a block from the heap using the `size`, `type_size` and `alignment` as helping parameters |

#### Example:

```cpp
#include <Allocator.hpp>
#include <Io.hpp>

template <typename T>
class uses_allocator
    : private hsd::allocator<T>
{
private:
    usize _size = 0;

public:
    uses_allocator(const T& value, usize size)
        : _size{size}
    {
        // you need to use this explicitly
        this->_data = this->allocate(size).unwrap();
        hsd::set(this->_data, this->_data + this->_size, hsd::forward<T>(value));
    }

    uses_allocator(T&& value, usize size)
        : _size{size}
    {
        // you need to use this explicitly
        this->_data = this->allocate(size).unwrap();
        hsd::set(this->_data, this->_data + this->_size, hsd::forward<T>(value));
    }

    ~uses_allocator()
    {
        for(usize _index = 0; _index < this->_size; _index++)
            this->_data[_index].~T();
        
        this->deallocate(this->_data, this->_size);
    }

    T* get_pointer()
    {
        return this->_data;
    }

    T& get_reference(usize index)
    {
        return get_pointer()[index];
    }
};

int main()
{
    uses_allocator<char> a{'\0', 5};
    a.get_reference(0) = 't';
    a.get_reference(1) = 'e';
    a.get_reference(2) = 's';
    a.get_reference(3) = 't';
    hsd::io::print<"{}">(a.get_pointer);

    return 0;
}
```

### Constexpr allocator
#### Definition:
```cpp
template < typename Type, usize MaxSize >
class constexpr_allocator;
```

#### Public members:
| Member | Value/Type |
| :----- | :--------- |
| `pointer_type` | `stack_array<Type, MaxSize>` |
| `value_type` | `Type` |

#### Description:
It is a wrapper around `stack_array` which can be used at compile time

#### Member functions:
| Method | Arguments | Return type | Description |
| :----- | :-------- | :---------- | :---------- |
| `constexpr_allocator` | `N/A` | `/*implementation defined*/` | Initialize the internal array |
| `constexpr_allocator` | `const constexpr_allocator&` | `/*implementation defined*/` | Copy constructor (deleted) |
| `constexpr_allocator` | `constexpr_allocator&&` | `/*implementation defined*/` | Move constructor (deleted) |
| `limit` | `N/A` | `usize` | Returns the `MaxSize` template parameter |

#### Example:

```cpp
#include <Allocator.hpp>
#include <Io.hpp>

template <typename T, usize N>
class uses_allocator
    : private hsd::constexpr_allocator<T, N>
{
private:
    usize _size = 0;

public:
    constexpr uses_allocator(const T& value, usize size)
        : _size{size}
    {
        // you need to use this explicitly
        hsd::set(this->_data, this->_data + this->_size, hsd::forward<T>(value));
    }

    constexpr uses_allocator(T&& value, usize size)
        : _size{size}
    {
        // you need to use this explicitly
        hsd::set(this->_data, this->_data + this->_size, hsd::forward<T>(value));
    }

    constexpr T* get_pointer()
    {
        return &this->_data[0];
    }

    constexpr T& get_reference(usize index)
    {
        return get_pointer()[index];
    }
};

int main()
{
    uses_allocator<char, 10> a{'\0', 5};
    a.get_reference(0) = 't';
    a.get_reference(1) = 'e';
    a.get_reference(2) = 's';
    a.get_reference(3) = 't';
    hsd::io::print<"{}">(a.get_pointer);

    return 0;
}
```
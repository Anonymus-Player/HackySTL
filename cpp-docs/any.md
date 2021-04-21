# Universal type class support

## Universal type class introduced in the library
Implemented in header [Any.hpp](../cpp/Any.hpp)

## Any
### Definition:
```cpp
class any;
```

### Error type:
#### Definition:
```cpp
struct bad_any_cast
{
    const char* operator()() 
    {
        return "Illegal casting:\n"
        "Cannot cast into different type";
    }
};
```

### Description:
It is an data type which can hold *any* type imaginable thanks to some polymorphism

### Member function:

| Function | Arguments | Return type | Description |
| :------- | :-------- | :---------- | :---------- |
| `any` | `N/A` | `/*compiler defined*/` | Constructs an empty type |
| `any` | `(Copyable) Type other` | `/*compiler defined*/` | Constructs a new object by value with the condition of the value being able to copy |
| `any` | `const any& other` | `/*compiler defined*/` | Copy constructor |
| `any` | `any&& other` | `/*compiler defined*/` | Move constructor |
| `operator=` | `any rhs` | `any` | Copy attribution |
| `cast_to` | `N/A` | `Result<Type, bad_any_cast>` | Cast the underlying type to the new type, `Type` if it stores the good one, if it isn't it raises an `bad_any_cast` error |
| `cast_if` | `N/A` | `Type*` | Returns the address of the value stored by the underlying type if it is the good one, if it isn't it returns `null` |
| `holds_type` | `N/A` | `bool` | Checks if the the underlying values hols the type, `Type` |
| `type` | `N/A` | `std::type_info&` | If `HSD_ANY_ENABLE_TYPEINFO` is defined this function returns the Run-Time Type Information (RTTI) of the underlying value |
| `swap` | `any& other` | `void` | Swaps the local value with the `other` one |
| `has_value` | `N/A` | `bool` | Checks if the internal value is not empty |
| `reset` | `N/A` | `void` | Nullifies the value thus resetting it |

### Example:
```cpp
#include <Io.hpp>

#define HSD_ANY_ENABLE_TYPEINFO
#include <Any.hpp>

struct S
{
    int _a;
    char _b;
    float _c;
    const char* _d;

    S(int a, char b, float c, const char* d) : _a(a), _b(b), _c(c), _d(d) {}
};

int main()
{
    // any type
    hsd::any a = 1;
    hsd::io::print<"{}: {}\n">(a.type().name(), a.cast_to<int>().unwrap());
    a = 3.14;
    hsd::io::print<"{}: {}\n">(a.type().name(), a.cast_to<double>().unwrap());
    a = true;
    hsd::io::print<"{}: {}\n">(a.type().name(), a.cast_to<bool>().unwrap());
 
    // bad cast
    hsd::io::print<"{}\n">(a.cast_to<float>().unwrap_err()());
 
    // has value
    a = 1;
    if (a.has_value())
    {
        hsd::io::print<"{}\n">(a.type().name());
    }
 
    // reset
    a.reset();
    if (!a.has_value())
    {
        hsd::io::print<"no value\n">();
    }
 
    // pointer to contained data
    a = 1;
    if (int* i = a.cast_if<int>())
        hsd::io::print<"{}\n">(*i);

    // holds a given type
    a.emplace<S>(1, 'c', 4.2f, "str");
    
    if (a.holds_type<S>())
        hsd::io::print<"Holds an S\n">();
    
    if (a.holds_type<int>())
        hsd::io::print<"Holds an int\n">();
}
```
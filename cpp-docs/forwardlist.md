# Forward linked lists support

## Forward list introduced in the library
Implemented in header [ForwardList.hpp](../cpp/ForwardList.hpp)

## Forward list
### Definition:
```cpp
template <typename Type>
class forward_list;
```

### Public members:
| Alias | Type |
| :---- | :--- |
| `iterator` | `forward_list_detail::iterator<Type>` |
| `const_iterator` | `const iterator` |

### Member functions:
| Method | Arguments | Return type | Description |
| :----- | :-------- | :---------- | :---------- |
| `forward_list` | `N/A` | `/*compiler defined*/` | Default construction |
| `forward_list` | `const Type (&arr)[N]` | `/*compiler defined*/` | Copy array initialization |
| `forward_list` | `Type (&&arr)[N]` | `/*compiler defined*/` | Move array initialization |
| `forward_list` | `const forward_list& other` | `/*compiler defined*/` | Copy constructor |
| `forward_list` | `forward_list&& other` | `/*compiler defined*/` | Move constructor |
| `~forward_list` | `N/A` | `/*compiler defined*/` | Destructor |
| `operator=` | `const Type (&arr)[N]` | `forward_list&` | Copy array attribution |
| `operator=` | `Type (&&arr)[N]` | `forward_list&` | Move array attribution |
| `operator=` | `const forward_list& other` | `forward_list&` | Copy attribution |
| `operator=` | `forward_list&& other` | `forward_list&` | Move attribution |
| `erase` | `const_iterator pos` | `Result<iterator, runtime_error>` | Removes an element at the position `pos` from the list |
| `push_back` | `const Type& value` | `void` | Adds an element to the back of the list by copy |
| `push_back` | `Type&& value` | `void` | Adds an element to the back of the list by move |
| `emplace_back` | `Arguments&&... args` | `void` | Constructs a new element into the back of the list using `args` |
| `push_front` | `const Type& value` | `void` | Adds an element to the front of the list by copy |
| `push_front` | `Type&& value` | `void` | Adds an element to the front of the list by move |
| `emplace_front` | `Arguments&&... args` | `void` | Constructs a new element into the front of the list using `args` |
| `pop_front` | `N/A` | `void` | Removes the first element of the list |
| `clear` | `N/A` | `void` | Removes all elements of the list |
| `empty` | `N/A` | `bool` | Checks if the list is empty |
| `front` | `N/A` | `Type&` | Gets the first element of the list |
| `back` | `N/A` | `Type&` | Gets the last element of the list |
| `begin/end` | `N/A` | `(const_)iterator` | Gets the position of the first element and the one after last of the list |

### Example
```cpp
#include <ForwardList.hpp>
#include <Io.hpp>

struct S
{
    int a;
    float b;
    char c;

    S() = default;
    S(int aa, float bb, char cc)
        : a{aa}, b {bb}, c{cc}
    {}
};

int main()
{
    hsd::forward_list<S> ls;
    ls.emplace_back(12, 3.3f, 'a');
    ls.emplace_back(1, 3.2f, 'd');

    for(auto it : ls)
    {
        hsd::io::print<"{}\n">(it.a);
    }

    hsd::io::print<"==========\n">();
    ls.pop_front();

    for(auto it : ls)
    {
        hsd::io::print<"{}\n">(it.a);
    }

    hsd::io::print<"==========\n">();
    ls.emplace_front(3, 23.22f, 'e');

    for(auto it : ls)
    {
        hsd::io::print<"{}\n">(it.a);
    }

    hsd::io::print<"==========\n">();

    for(auto iter = ls.begin(); iter != ls.end();)
    {
        if(iter->a == 3)
        {
            iter = ls.erase(iter).unwrap();
        }
        else
        {
            iter++;
        }
    }
}
```
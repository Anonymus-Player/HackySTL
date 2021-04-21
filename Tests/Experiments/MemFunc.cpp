#include <TypeTraits.hpp>

template <typename T>
struct TestStruct
{
    T* data = nullptr;
    hsd::usize sz = 0;

    TestStruct(T* ptr, hsd::usize size)
        : data{ptr}, sz{size}
    {}

    T* begin()
    {
        return data;
    }

    T* end()
    {
        return data + sz;
    }
};

template <typename T>
concept IsContainer = requires 
{
    hsd::declval<T>().begin();
};

int main()
{
    static_assert(IsContainer<TestStruct<int>>);
}
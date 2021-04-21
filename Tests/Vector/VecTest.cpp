#include <Vector.hpp>
#include <cstdio>
#include <iterator>

struct S
{
    int _a;
    float _b;
    char _c;

    S(int a, float b, char c)
        : _a(a), _b(b), _c(c) 
    {}
};

struct verbose {
    verbose() {
        std::puts("default constructor");
    }
    ~verbose() {
        std::puts("destructor");
    }
    verbose(verbose const&) {
        std::puts("copy constructor");
    }
    verbose(verbose&&) {
        std::puts("move constructor");
    }
    auto& operator=(verbose const&) {
        std::puts("copy assignment");
        return *this;
    }
    auto& operator=(verbose&&) {
        std::puts("move assignment");
        return *this;
    }
};

template <typename T>
hsd::vector<T> gen_range(const hsd::vector<T>& orig, size_t start, size_t end)
{
    assert(start <= end);
    assert(end <= orig.size());
    hsd::vector<T> vec;
    vec.reserve(end - start);
    hsd::copy(orig.cbegin() + start, orig.cbegin() + end, std::back_inserter(vec));
    return vec;
}

template <typename T>
void test(hsd::vector<T>&&)
{

}

constexpr auto make_constexpr_vec()
{
    hsd::static_vector<int, 20> v;
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    v.emplace_back(4);
    v.emplace_back(5);
    v.emplace_back(6);
    v.emplace_back(7);
    v.emplace_back(8);
    v.emplace_back(9);
    v.emplace_back(10);

    return v;
}

int main()
{
    {
        // for function type deduction
        test(hsd::vector{{1, 2, 3, 4, 5, 6}});
        // alternatively you can do
        test<int>({{1, 2, 3, 4, 5, 6}});
        // it does the same thing
        test(hsd::make_vector(1, 2, 3, 4, 5, 6));
        // let's test constexpr vector (it works)
        constexpr auto v = make_constexpr_vec();

        for(auto& val : v)
            printf("%d\n", val);

        puts("==========");
    }

    {
        hsd::vector e = {{1, 2, 3, 4, 5, 6}};

        for(auto iter = e.begin(); iter != e.end();)
        {
            if(*iter == 6)
            {
                iter = e.erase(iter).unwrap();
            }
            else
            {
                printf("%d\n", *iter);
                iter++;
            }
        }

        puts("==========");
    }

    {
        // let's test the buffred vector
        hsd::uchar buf[1000]{};
        hsd::buffered_allocator<hsd::uchar> alloc = {buf, 200};
        hsd::buffered_vector<int> vec{alloc};
        hsd::buffered_vector<int> vec2{alloc};
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        vec.push_back(4);
        vec.push_back(5);

        vec2.push_back(5);
        vec2.push_back(4);
        vec2.push_back(3);
        vec2.push_back(2);
        vec2.push_back(1);

        for(auto& val : vec)
            printf("%d\n", val);

        puts("==========");
        
        for(auto& val : vec2)
            printf("%d\n", val);

        puts("==========");
    }

    {
        // you have to do this: {{...}}
        hsd::vector e = {{1, 2, 3, 4, 5, 6}};

        for (auto val : e)
        {
            printf("%d\n", val);
        }

        std::puts("==========");
        auto e2 = gen_range(e, 2, 5);

        for (auto val : e2)
        {
            std::printf("%d\n", val);
        }

        std::puts("==========");
        e2 = {{12, 21, 123}};

        for (auto val : e2)
        {
            std::printf("%d\n", val);
        }
    }

    std::puts("==========");
    {
        hsd::vector<S> ls;
        ls.emplace_back(12, 3.3f, 'a');
    }

    std::puts("==========");
    {
        auto ls = hsd::make_vector(1, 2, 3.5f, 4.04, 1.0, -0.0, true);

        for (auto val : ls)
        {
            std::printf("%d\n", val);
        }
    }

    std::puts("==========");
    {
        std::puts("--- init 4");
        hsd::vector<verbose> verb(4); // value initialize 4 elements
        // add 2 elements
        std::puts("--- add element");
        verb.emplace_back();
        std::puts("--- add element");
        verb.emplace_back();

        {
            std::puts("--- copy");
            auto verb2 = verb; // copy
        }
        {
            std::puts("--- move");
            auto verb3 = hsd::move(verb); // move
        }
    }
}
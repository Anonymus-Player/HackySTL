#include <ForwardList.hpp>
#include <stdio.h>

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
        printf("%d\n", it.a);
    }

    puts("==========");
    ls.pop_front();

    for(auto it : ls)
    {
        printf("%d\n", it.a);
    }

    puts("==========");
    ls.emplace_front(3, 23.22f, 'e');

    for(auto it : ls)
    {
        printf("%d\n", it.a);
    }

    puts("==========");

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
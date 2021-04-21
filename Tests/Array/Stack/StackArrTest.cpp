#include <stdio.h>
#include <StackArray.hpp>

int main()
{
    hsd::stack_array sarr = {1, 2, 3, 4, 5};

    for(auto i : sarr)
    {
        printf("%d\n", i);
    }

    auto sarr2 = sarr.gen_range<0, 3>();
    puts("========");

    for(auto i : sarr2)
    {
        printf("%d\n", i);
    }
}
#include <String.hpp>
#include <stdio.h>

int main()
{
    hsd::string str = "this is a test";
    auto str2 = str;
    
    printf("%s\n", hsd::to_string(-4.12f).c_str());
    
    printf("%s\n", (",\nno it\'s not\n" + str).data());
    
    printf("%s\n", str.gen_range<0, 4>().unwrap().data());
    
    printf("%s\n", str2.data());

    printf("%zu\n", str.find('z'));
    return 0;
}
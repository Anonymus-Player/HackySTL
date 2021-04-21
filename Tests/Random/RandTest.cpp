#include <Random.hpp>
#include <Io.hpp>

int main()
{
    hsd::mt19937_32 engine;

    for(hsd::u16 i = 0; i < 65000; i++)
    {
        hsd::io::print<L"{}\n">(engine.generate(1, 4).unwrap());
    }
}
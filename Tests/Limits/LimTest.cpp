#include <Limits.hpp>
#include <Io.hpp>

int main()
{
    hsd::io::print<"Upper limits:\n">();

    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::i16>::max);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::i32>::max);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::i64>::max);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::f32>::max);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::f64>::max);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::f128>::max);

    hsd::io::print<"Lower limits: {}\n">(static_cast<hsd::i32>(hsd::limits<hsd::f32>::infinity));

    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::i16>::min);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::i32>::min);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::i64>::min);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::f32>::min);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::f64>::min);
    hsd::io::print<"the limit is: {}\n">(hsd::limits<hsd::f128>::min);
}
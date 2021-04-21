#include <Logging.hpp>
#include <stdio.h>

template <hsd::usize N>
auto test_trace(hsd::stack_trace tr, const char* msg)
    -> hsd::Result<void, hsd::stack_trace_error>
{
    invoke_stacktrace_func(test_trace<N - 1>, msg).unwrap();
    return {};
}

template <>
auto test_trace<0>(hsd::stack_trace tr, const char* msg)
    -> hsd::Result<void, hsd::stack_trace_error>
{
    return hsd::stack_trace_error{};
}

static void test_profiler(hsd::profiler tr, hsd::usize N)
{
    if(N == 0)
        return;

    char buffer[1024];
    auto* file = fopen("/dev/urandom", "r");
    fread(buffer, 1024, 1, file);
    fclose(file);

    invoke_profiler_func(test_profiler, N - 1);
}

int main()
{
    //invoke_stacktrace_func(test_trace<120>, "hello").unwrap();   
    invoke_profiler_func(test_profiler, 32);
}
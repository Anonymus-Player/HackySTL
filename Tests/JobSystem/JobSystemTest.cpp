#include <JobSystem.hpp>

#include <Io.hpp>

void print()
{
    hsd::io::print<L"hello, and other words\n">();
}

// Example function that takes args
void foo(int a, float b)
{
    auto z = a / b;

    z = (a & (a + 1)) + b - z/a;
    
    hsd::io::print<L"Result: {}">(z);

    return;
}

int main()
{
    // Generic example with no parameters
    hsd::Job j = hsd::job_system.create_job([](hsd::Job) 
    {
        print();
    });


    // Schedules 10 jobs
    for(int i = 0; i < 10; i++)
    {
        hsd::job_system.schedule_job(j);
    }
    
    // Pauses current function of the caller until all jobs are done
    hsd::job_system.wait();

//                              \\//

    //Example where args are passed
    int x = 10;
    float y = 5.942f;

    void* data[] = { &x, &y };

    j = hsd::job_system.create_job([](hsd::Job j)    // Technically could just be a fnptr, but a lambda is more readable here
    {
        // Get the variables from the data array
        auto& s = *static_cast<int*>(j.data[0]);    //Can be mutable
        auto t = *static_cast<float*>(j.data[1]);   // Or a copy

        // Passing the values into the function
        foo(s, t);

    }, data);

    for(int i = 0; i < 100; ++i)
    {
        hsd::job_system.schedule_job(j);
    }

    hsd::job_system.wait();
}
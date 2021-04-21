#pragma once

#include "Time.hpp"
#include "Vector.hpp"

namespace hsd
{
    namespace detail
    {
        class source_location
        {
        private:
            const char* _file_name = "unknown";
            const char* _func = "unknown";
            usize _line = 0;

        public:
            constexpr source_location(
                const char* file_name = __builtin_FILE(),
                const char* func = __builtin_FUNCTION(), 
                usize line = __builtin_LINE()) noexcept
                : _file_name{file_name}, _func{func}, _line{line}
            {}

            constexpr const char* file_name() noexcept
            {
                return _file_name;
            }

            constexpr const char* function_name() noexcept
            {
                return _func;
            }

            constexpr usize line() noexcept
            {
                return _line;
            }
        };

        class profiler_value
        {
        private:
            const char* _file_name = "unknown";
            const char* _func = "unknown";
            usize _line = 0;
            precise_clock _clk{};

        public:
            profiler_value(
                const char* file_name = __builtin_FILE(),
                const char* func = __builtin_FUNCTION(), 
                usize line = __builtin_LINE()) noexcept
                : _file_name{file_name}, _func{func}, _line{line}
            {}

            const char* file_name() noexcept
            {
                return _file_name;
            }

            const char* function_name() noexcept
            {
                return _func;
            }

            usize line() noexcept
            {
                return _line;
            }

            precise_clock elapsed_time()
            {
                return _clk.elapsed_time();
            }
        };        
    } // namespace detail
    
    class stack_trace
    {
    private:
        static inline hsd::vector<detail::source_location> _stack;
        using stack_iterator = hsd::vector<detail::source_location>::iterator;

    public:

        stack_trace() noexcept = default;
        stack_trace(const stack_trace&) noexcept = default;

        ~stack_trace() noexcept
        {
            _stack.pop_back();
        }

        stack_trace& add(
            const char* func, usize line = __builtin_LINE(),
            const char* file_name = __builtin_FILE()) noexcept
        {
            _stack.emplace_back(file_name, func, line);
            return *this;
        }

        void print_stack()
        {
            for(auto it = rbegin(); it != rend(); it--)
            {
                fprintf(
                    stderr, "Info: %s:%zu\n\tFunction: %s\n",
                    it->file_name(), it->line(), 
                    it->function_name()
                );
            }
        }
        
        detail::source_location& get() noexcept
        {
            return _stack.back();
        }

        stack_iterator rbegin() noexcept
        {
            return _stack.end() - 1;
        }

        stack_iterator rend() noexcept
        {
            return _stack.begin() - 1;
        }
    };

    class profiler
    {
    private:
        using iterator = hsd::vector<detail::profiler_value>::iterator;
        static inline hsd::vector<detail::profiler_value> _stack;

    public:

        profiler() noexcept = default;
        profiler(const profiler&) noexcept = default;

        ~profiler() noexcept
        {
            if(_stack.size() > 0)
            {
                printf(
                    "Info: %s:%zu\n\tFunction: %s, time taken: %lluus\n", 
                    get().file_name(), get().line(), get().function_name(),
                    get().elapsed_time().to_microseconds()
                );
            }
            _stack.pop_back();
        }

        profiler& add(
            const char* func, usize line = __builtin_LINE(),
            const char* file_name = __builtin_FILE()) noexcept
        {
            _stack.emplace_back(file_name, func, line);
            return *this;
        }
        
        detail::profiler_value& get() noexcept
        {
            return _stack.back();
        }

        iterator rbegin() noexcept
        {
            return _stack.end() - 1;
        }

        iterator rend() noexcept
        {
            return _stack.begin() - 1;
        }
    };

    static inline stack_trace exec_stack;
    static inline profiler profiler_stack;
    using source_location = detail::source_location;

    #define invoke_profiler_func(func, ...) \
        func(hsd::profiler_stack.add(HSD_FUNCTION_NAME), __VA_ARGS__)
    #define invoke_stacktrace_func(func, ...) \
        func(hsd::exec_stack.add(HSD_FUNCTION_NAME), __VA_ARGS__)

    struct stack_trace_error
    {
        const char* operator()() const
        {   
            exec_stack.print_stack();
            fputc('\n', stderr);
            return "The stack was unwined up there";
        }
    };
} // namespace hsd
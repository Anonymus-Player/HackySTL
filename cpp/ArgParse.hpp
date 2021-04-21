#pragma once

#include "UnorderedMap.hpp"
#include "Functional.hpp"
#include "_SStreamDetail.hpp"
#include "StringView.hpp"

namespace hsd
{
    class parser_stream
    {
    private:
        vector<pair<const char*, usize>> _args_buf;

    public:
        // It's a non-movable and copyable type
        inline parser_stream(const parser_stream&) = delete;
        inline parser_stream(parser_stream&&) = delete;
        inline parser_stream& operator=(const parser_stream&) = delete;
        inline parser_stream& operator=(parser_stream&&) = delete;
        inline parser_stream() = default;
        inline ~parser_stream() = default;

        template <typename... Args>
        Result<void, runtime_error> set_data(Args&... args)
        {
            using sstream_detail::_parse;
            constexpr usize num_args = sizeof...(Args);

            if(num_args > _args_buf.size())
            {
                return runtime_error{"Input too small to parse"};
            }
            else
            {
                [&]<usize... Ints>(index_sequence<Ints...>)
                {
                    (_parse(_args_buf[Ints], args), ...);
                }(make_index_sequence<num_args>{});
            }

            return {};
        }

        void emplace(const string_view& arg)
        {
            _args_buf.emplace_back(arg.data(), arg.size());
        }

        void clear()
        {
            _args_buf.clear();
        }
    };

    class argument_parser
    {
    private:
        using function_type = function<void(hsd::parser_stream&)>;
        using action_type = pair<function_type, usize>;
        unordered_map<string_view, action_type> _actions;
        vector<string_view> _informations;

    public:
        inline argument_parser(const string_view& info)
        {
            _informations.emplace_back(info);
        }
        
        inline argument_parser(const argument_parser&) = delete;
        inline argument_parser(argument_parser&&) = delete;
        inline argument_parser& operator=(const argument_parser&) = delete;
        inline argument_parser& operator=(argument_parser&&) = delete;
        inline ~argument_parser() = default;

        inline void add(const string_view& argument, usize num_args,
            function_type&& func, const string_view& help)
        {
            _actions.emplace(argument, func, num_args);
            _informations.emplace_back(help);
        }

        inline void parse(i32 argc, const char** argv)
        {
            if(argc == 1)
            {
                return;
            }
            else if(cstring::compare(argv[1], "-h") == 0 || 
                cstring::compare(argv[1], "--help") == 0)
            {
                for(auto& _tips : _informations)
                    puts(_tips.data());
            }
            else
            {
                //       0          1      2     3    4     5      6
                // launch_program clear command arg1 arg2 arg3 command2 
                // num_args = 3
                // index = 2

                i32 _index = 1, _incrementor;
                parser_stream _buf;

                while(_index < argc)
                {
                    auto& [_function, _num_args] = _actions
                        .at(argv[_index]).unwrap(HSD_FUNCTION_NAME);
                    _incrementor = _num_args;
                    _index += 1;
                    
                    for(; _incrementor != 0; _incrementor--)
                        _buf.emplace(argv[_index + _num_args - _incrementor]);

                    _function(_buf).unwrap(HSD_FUNCTION_NAME);
                    _index += _num_args;
                    _buf.clear();
                }
            }
        }
    };
} // namespace hsd

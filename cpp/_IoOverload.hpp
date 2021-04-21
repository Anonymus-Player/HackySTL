#pragma once

#include <stdio.h>
#include <wchar.h>

#include "Pair.hpp"
#include "StringLiteral.hpp"

namespace hsd
{
    namespace sstream_detail
    {
        template <typename CharT>
        inline void _parse(pair<const CharT*, usize>&, auto&) = delete;

        template <string_literal str>
        inline i32 _write(auto, pair<char*, usize>) = delete;

        template <wstring_literal str>
        inline i32 _write(auto, pair<wchar*, usize>) = delete;
    } // namespace sstream_detail

    namespace io_detail
    {
        template <string_literal str>
        inline void _print(auto, FILE*) = delete;

        template <wstring_literal str>
        inline void _print(auto, FILE*) = delete;
    } // namespace io_detail
} // namespace hsd

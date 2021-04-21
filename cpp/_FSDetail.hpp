#pragma once

#include "String.hpp"
#include <stdlib.h>

#if defined(HSD_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace hsd
{
    namespace filesystem
    {
        namespace fs_detail
        {
            struct Location
            {
                #if defined(HSD_PLATFORM_POSIX)
                string _relative;
                string _absolute;
                #elif defined(HSD_PLATFORM_WINDOWS)
                wstring _relative;
                wstring _absolute;
                #endif

                inline Location() = default;
                inline ~Location() = default;

                #if defined(HSD_PLATFORM_POSIX)
                inline Location(const char* relative_loc)
                #elif defined(HSD_PLATFORM_WINDOWS)
                inline Location(const wchar* relative_loc)
                #endif
                    : _relative{relative_loc}
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    char _buffer[2048]{};
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    wchar _buffer[2048]{};
                    #endif
                    
                    #if defined(HSD_PLATFORM_POSIX)
                    realpath(_relative.c_str(), _buffer);
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    GetFullPathNameW(_relative.c_str(), 2048, _buffer, nullptr);
                    #endif
                    
                    #if defined(HSD_PLATFORM_POSIX)
                    _absolute = string{_buffer};
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    _absolute = wstring{_buffer};
                    #endif
                }

                #if defined(HSD_PLATFORM_POSIX)
                inline Location(const string& relative_loc)
                #elif defined(HSD_PLATFORM_WINDOWS)
                inline Location(const wstring& relative_loc)
                #endif
                    : _relative{relative_loc}
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    char _buffer[2048]{};
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    wchar _buffer[2048]{};
                    #endif
                    
                    #if defined(HSD_PLATFORM_POSIX)
                    realpath(_relative.c_str(), _buffer);
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    GetFullPathNameW(_relative.c_str(), 2048, _buffer, nullptr);
                    #endif

                    #if defined(HSD_PLATFORM_POSIX)
                    _absolute = string{_buffer};
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    _absolute = wstring{_buffer};
                    #endif
                }

                #if defined(HSD_PLATFORM_POSIX)
                inline Location(string&& relative_loc)
                #elif defined(HSD_PLATFORM_WINDOWS)
                inline Location(wstring&& relative_loc)
                #endif
                    : _relative{move(relative_loc)}
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    char _buffer[2048]{};
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    wchar _buffer[2048]{};
                    #endif
                    
                    #if defined(HSD_PLATFORM_POSIX)
                    realpath(_relative.c_str(), _buffer);
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    GetFullPathNameW(_relative.c_str(), 2048, _buffer, nullptr);
                    #endif

                    #if defined(HSD_PLATFORM_POSIX)
                    _absolute = string{_buffer};
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    _absolute = wstring{_buffer};
                    #endif
                }

                inline Location(const Location& other)
                    : _relative{other._relative},
                      _absolute{other._absolute}
                {}

                inline Location(Location&& other)
                    : _relative{move(other._relative)},
                      _absolute{move(other._absolute)}
                {}

                inline Location& operator=(const Location& rhs)
                {
                    _relative = rhs._relative;
                    _absolute = rhs._absolute;
                    return *this;
                }

                inline Location& operator=(Location&& rhs)
                {
                    _relative = move(rhs._relative);
                    _absolute = move(rhs._absolute);
                    return *this;
                }
            };
        } // namespace fs_detail
    } // namespace filesystem
} // namespace hsd
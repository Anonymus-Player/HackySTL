#pragma once

#include "Vector.hpp"
#include "FStatus.hpp"
#include "_FSDetail.hpp"

#include <dirent.h>
#include <utime.h>
#include <fcntl.h>
#include <errno.h>

namespace hsd
{
    namespace filesystem
    {
        namespace fs_detail
        {
            #if defined(HSD_PLATFORM_POSIX)
            inline auto copy_file(const char* from, const char* to)
                -> Result<void, runtime_error>
            {
                i32 _value;
                using file_type = FILE*;
                file_type _from_file = fopen(from, "r");
                file_type _to_file = fopen(to, "w");
                
                if(_from_file == nullptr || _to_file == nullptr)
                {
                    return runtime_error{"File not found"};
                }

                for (_value = fgetc(_from_file); _value != EOF; _value = fgetc(_from_file)) 
                {
                    if (fputc(_value, _to_file) == EOF)
                    {
                        fclose(_to_file);
                        fclose(_from_file);
                        return runtime_error{"Couldn't write into the file"};
                    }
                }

                fclose(_to_file);
                fclose(_from_file);
                return {};
            }
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline auto copy_file(const wchar* from, const wchar* to)
                -> Result<void, runtime_error>
            {
                i32 _value;
                using file_type = FILE*;
                file_type _from_file = _wfopen(from, L"r");
                file_type _to_file = _wfopen(to, L"w");
                
                if(_from_file == nullptr || _to_file == nullptr)
                {
                    return runtime_error{"File not found"};
                }

                for (_value = fgetc(_from_file); _value != EOF; _value = fgetc(_from_file)) 
                {
                    if (fputc(_value, _to_file) == EOF)
                    {
                        fclose(_to_file);
                        fclose(_from_file);
                        return runtime_error{"Couldn't write into the file"};
                    }
                }

                fclose(_to_file);
                fclose(_from_file);
                return {};
            }
            #endif
        } // namespace fs_detail
        

        class path
        {
        private:
            #if defined(HSD_PLATFORM_POSIX)
            DIR* _directory = nullptr;
            #elif defined(HSD_PLATFORM_WINDOWS)
            _WDIR* _directory = nullptr;
            #endif
            fs_detail::Location _location;

            static constexpr auto _default_perms = 
                fs_perms_mask::owner_all;

            void _destroy()
            {
                if (_directory != nullptr)
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    closedir(_directory);
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    _wclosedir(_directory);
                    #endif
                }
            }

            void _reconstruct()
            {
                _destroy();
                
                #if defined(HSD_PLATFORM_POSIX)
                _directory = opendir(absolute_name().c_str());
                #elif defined(HSD_PLATFORM_WINDOWS)
                _directory = _wopendir(absolute_name().c_str());
                #endif
            }

        public:
            inline path()
                #if defined(HSD_PLATFORM_POSIX)
                : path{string(".", 1)}
                #elif defined(HSD_PLATFORM_WINDOWS)
                : path{wstring(L".", 1)}
                #endif
            {
                // Opens the directory at the
                // location of the executable
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path(const char* raw_path)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path(const wchar* raw_path)
            #endif
                : _location{raw_path}
            {
                #if defined(HSD_PLATFORM_POSIX)
                _directory = opendir(absolute_name().c_str());
                #elif defined(HSD_PLATFORM_WINDOWS)
                _directory = _wopendir(absolute_name().c_str());
                #endif
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path(const string& raw_path)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path(const wstring& raw_path)
            #endif
                : _location{raw_path}
            {
                #if defined(HSD_PLATFORM_POSIX)
                _directory = opendir(absolute_name().c_str());
                #elif defined(HSD_PLATFORM_WINDOWS)
                _directory = _wopendir(absolute_name().c_str());
                #endif
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path(string&& raw_path)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path(wstring&& raw_path)
            #endif
                : _location{move(raw_path)}
            {
                #if defined(HSD_PLATFORM_POSIX)
                _directory = opendir(absolute_name().c_str());
                #elif defined(HSD_PLATFORM_WINDOWS)
                _directory = _wopendir(absolute_name().c_str());
                #endif
            }

            inline path(const path& other)
                : _location{other._location}
            {
                #if defined(HSD_PLATFORM_POSIX)
                _directory = opendir(absolute_name().c_str());
                #elif defined(HSD_PLATFORM_WINDOWS)
                _directory = _wopendir(absolute_name().c_str());
                #endif
            }

            inline path(path&& other)
                : _location{move(other._location)}
            {
                swap(_directory, other._directory);
            }

            inline ~path()
            {
                _destroy();
            }

            path& operator=(const path& rhs)
            {
                _location = rhs._location;
                _destroy();
                
                #if defined(HSD_PLATFORM_POSIX)
                _directory = opendir(absolute_name().c_str());
                #elif defined(HSD_PLATFORM_WINDOWS)
                _directory = _wopendir(absolute_name().c_str());
                #endif
                
                return *this;
            }

            path& operator=(path&& rhs)
            {
                swap(_location, rhs._location);
                swap(_directory, rhs._directory);
                return *this;
            }

            inline bool operator==(const path& rhs) const
            {
                return absolute_name() == rhs.absolute_name();
            }

            inline bool operator!=(const path& rhs) const
            {
                return absolute_name() != rhs.absolute_name();
            }

            inline bool operator<(const path& rhs) const
            {
                return absolute_name() < rhs.absolute_name();
            }

            inline bool operator<=(const path& rhs) const
            {
                return absolute_name() <= rhs.absolute_name();
            }

            inline bool operator>(const path& rhs) const
            {
                return absolute_name() > rhs.absolute_name();
            }

            inline bool operator>=(const path& rhs) const
            {
                return absolute_name() >= rhs.absolute_name();
            }

            static inline path root()
            {
                #if defined(HSD_PLATFORM_POSIX)
                return path{string("/", 1)};
                #elif defined(HSD_PLATFORM_WINDOWS)
                return path{wstring(L"C:\\", 3)};
                #endif
            }

            inline path parent()
            {
                if(*this == root())
                    return root();

                #if defined(HSD_PLATFORM_POSIX)
                return path{(relative_name() + "/..")};
                #elif defined(HSD_PLATFORM_WINDOWS)
                return path{(relative_name() + L"\\..")};
                #endif
            }

            inline auto list()
                -> Result<hsd::vector<path>, runtime_error>
            {
                if(_directory != nullptr)
                {
                    hsd::vector<path> _paths;
                    
                    #if defined(HSD_PLATFORM_POSIX)
                    dirent* _dir_entry = nullptr;
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    _wdirent* _dir_entry = nullptr;
                    #endif

                    #if defined(HSD_PLATFORM_POSIX)
                    while ((_dir_entry = readdir(_directory)))
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    while ((_dir_entry = _wreaddir(_directory)))
                    #endif
                    {
                        #if defined(HSD_PLATFORM_POSIX)
                        if(cstring::compare(_dir_entry->d_name, ".") != 0 && 
                            cstring::compare(_dir_entry->d_name, "..") != 0)
                        #elif defined(HSD_PLATFORM_WINDOWS)
                        if(wcstring::compare(_dir_entry->d_name, L".") != 0 && 
                            wcstring::compare(_dir_entry->d_name, L"..") != 0)
                        #endif
                        {
                            #if defined(HSD_PLATFORM_POSIX)
                            if(*(relative_name().end() - 1) == '/')
                            #elif defined(HSD_PLATFORM_WINDOWS)
                            if(*(relative_name().end() - 1) == L'\\')
                            #endif
                            {
                                _paths.emplace_back(relative_name() + 
                                    _dir_entry->d_name);
                            }
                            else
                            {
                                #if defined(HSD_PLATFORM_POSIX)
                                _paths.emplace_back(relative_name() + 
                                    "/" + _dir_entry->d_name);
                                #elif defined(HSD_PLATFORM_WINDOWS)
                                _paths.emplace_back(relative_name() + 
                                    L"\\" + _dir_entry->d_name);
                                #endif
                            }
                        }
                    }

                    _reconstruct();
                    return move(_paths);
                }

                return runtime_error{"No such directory"};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline const string& relative_name() const
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline const wstring& relative_name() const
            #endif
            {
                return _location._relative;
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline const string& absolute_name() const
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline const wstring& absolute_name() const
            #endif
            {
                return _location._absolute;
            }

            inline auto change_permissions(const fs_perms_mask& mask)
                -> Result<void, runtime_error>
            {
                if(!status().exists())
                {
                    return runtime_error{"File/Directory not found"};
                }
                else
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    int _res = chmod(absolute_name().c_str(), static_cast<u32>(mask));
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    int _res = _wchmod(absolute_name().c_str(), static_cast<u32>(mask));
                    #endif

                    if(_res == -1)
                    {
                        return runtime_error{
                            "Couldn't change permissions of the file"
                        };
                    }   
                }

                return {};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline auto rename(const string& new_name)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline auto rename(const wstring& new_name)
            #endif
                -> Result<void, runtime_error> 
            {
                if(!status().exists())
                {
                    return runtime_error{"File/Directory not found"};
                }
                else
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    usize _slash_pos = relative_name().rfind('/');
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    usize _slash_pos = relative_name().rfind(L'\\');
                    #endif

                    int _res = 0;

                    #if defined(HSD_PLATFORM_POSIX)
                    if(_slash_pos == string::npos)
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    if(_slash_pos == wstring::npos)
                    #endif
                    {
                        path _new_path;
                        _new_path /= new_name;

                        #if defined(HSD_PLATFORM_POSIX)
                        _res = ::rename(
                            absolute_name().c_str(), 
                            _new_path.absolute_name().c_str()
                        );
                        #elif defined(HSD_PLATFORM_WINDOWS)
                        _res = ::_wrename(
                            absolute_name().c_str(), 
                            _new_path.absolute_name().c_str()
                        );
                        #endif

                        if(_res == -1)
                            return runtime_error{strerror(errno)};

                        _new_path._destroy();
                        swap(*this, _new_path);
                    }
                    else
                    {
                        #if defined(HSD_PLATFORM_POSIX)
                        path _new_path{string(relative_name().c_str(), _slash_pos)};
                        #elif defined(HSD_PLATFORM_WINDOWS)
                        path _new_path{wstring(relative_name().c_str(), _slash_pos)};
                        #endif
                        
                        _new_path /= new_name;

                        #if defined(HSD_PLATFORM_POSIX)
                        _res = ::rename(
                            absolute_name().c_str(), 
                            _new_path.absolute_name().c_str()
                        );
                        #elif defined(HSD_PLATFORM_WINDOWS)
                        _res = ::_wrename(
                            absolute_name().c_str(), 
                            _new_path.absolute_name().c_str()
                        );
                        #endif

                        if(_res == -1)
                            return runtime_error{strerror(errno)};

                        _new_path._destroy();
                        
                        #if defined(HSD_PLATFORM_POSIX)
                        _new_path._directory = opendir(
                            relative_name().c_str()
                        );
                        #elif defined(HSD_PLATFORM_WINDOWS)
                        _new_path._directory = _wopendir(
                            relative_name().c_str()
                        );
                        #endif

                        swap(*this, _new_path);
                    }
                }

                return {};
            }

            inline auto extension() const
            {
                if(status().is_regular_file().unwrap())
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    usize _point_pos = relative_name().rfind('.');
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    usize _point_pos = relative_name().rfind(L'.');
                    #endif

                    #if defined(HSD_PLATFORM_POSIX)
                    if(_point_pos != string::npos)
                    {
                        const char* _str_data = relative_name().c_str();
                        usize _str_size = relative_name().size();
                        return string (
                            _str_data + _point_pos, 
                            _str_size - _point_pos
                        );
                    }

                    return string{};
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    if(_point_pos != wstring::npos)
                    {
                        const wchar* _str_data = relative_name().c_str();
                        usize _str_size = relative_name().size();
                        return wstring (
                            _str_data + _point_pos, 
                            _str_size - _point_pos
                        );
                    }

                    return wstring{};
                    #endif
                }
                
                // will return an empty string instead
                #if defined(HSD_PLATFORM_POSIX)
                return string{};
                #elif defined(HSD_PLATFORM_WINDOWS)
                return wstring{};
                #endif
            }

            inline auto copy(const path& to)
                -> Result<void, runtime_error> 
            {
                if(status().is_directory().unwrap() &&
                    to.status().is_directory().unwrap())
                {
                    for(auto& _item : list().unwrap())
                    {
                        #if defined(HSD_PLATFORM_POSIX)
                        const char* _item_name =
                            _item.absolute_name().c_str() +
                            _item.absolute_name().rfind('/');
                        #elif defined(HSD_PLATFORM_WINDOWS)
                        const wchar* _item_name =
                            _item.absolute_name().c_str() +
                            _item.absolute_name().rfind(L'\\');
                        #endif

                        path _new_to = to + _item_name;

                        if(_item.status().is_directory().unwrap())
                        {
                            #if defined(HSD_PLATFORM_POSIX)
                            mkdir(
                                _new_to.absolute_name().c_str(), 
                                static_cast<u32>(_default_perms)
                            );
                            #elif defined(HSD_PLATFORM_WINDOWS)
                            _wmkdir(
                                _new_to.absolute_name().c_str()
                            );
                            #endif
                        }
                        else
                        {
                            #if defined(HSD_PLATFORM_POSIX)
                            auto* _dummy_file = 
                                fopen(_new_to.absolute_name().c_str(), "w+");
                            #elif defined(HSD_PLATFORM_WINDOWS)
                            auto* _dummy_file = 
                                _wfopen(_new_to.absolute_name().c_str(), L"w+");
                            #endif

                            fclose(_dummy_file);
                        }

                        _item.copy(_new_to).unwrap();
                    }
                }
                else if(status().is_regular_file().unwrap() &&
                    to.status().is_regular_file().unwrap())
                {
                    return fs_detail::copy_file(
                        absolute_name().c_str(),
                        to.absolute_name().c_str()
                    );
                }
                else if(status().is_regular_file().unwrap() &&
                    to.status().is_directory().unwrap())
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    const char* _item_name =
                        absolute_name().c_str() +
                        absolute_name().rfind('/');
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    const wchar* _item_name =
                        absolute_name().c_str() +
                        absolute_name().rfind(L'\\');
                    #endif

                    path _new_to = to + _item_name;
                    return fs_detail::copy_file(
                        absolute_name().c_str(), 
                        _new_to.absolute_name().c_str()
                    );
                }
                else if(status().is_directory().unwrap() &&
                    to.status().is_regular_file().unwrap())
                {
                    return runtime_error{
                        "Cannot copy a directory into a file"
                    };
                }
                else
                {
                    return runtime_error{"Unreachable code"};
                }
                
                return {};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline auto create_directory(const string& name) const
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline auto create_directory(const wstring& name) const
            #endif
                -> Result<void, runtime_error>
            {
                if(name.rfind('/') < name.size() - 1)
                {
                    return runtime_error{
                        "Cannot create multiple directories"
                    };
                }

                #if defined(HSD_PLATFORM_POSIX)
                mkdir(
                    (absolute_name() + "/" + name).c_str(), 
                    static_cast<u32>(_default_perms)
                );
                #elif defined(HSD_PLATFORM_WINDOWS)
                _wmkdir(
                    (absolute_name() + L"\\" + name).c_str()
                );
                #endif

                return {};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path operator+(const char* rhs) const
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path operator+(const wchar* rhs) const
            #endif
            {
                return path{relative_name() + rhs};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path operator+(const string& rhs) const
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path operator+(const wstring& rhs) const
            #endif
            {
                return path{relative_name() + rhs};
            }

            inline path operator+(const path& rhs) const
            {
                return path{relative_name() + rhs.relative_name()};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path& operator+=(const char* rhs)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path& operator+=(const wchar* rhs)
            #endif
            {
                *this = move(*this + rhs);
                return *this;
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path& operator+=(const string& rhs)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path& operator+=(const wstring& rhs)
            #endif
            {
                *this = move(*this + rhs);
                return *this;
            }

            inline path& operator+=(const path& rhs)
            {
                *this = move(*this + rhs);
                return *this;
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path operator/(const char* rhs) const
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path operator/(const wchar* rhs) const
            #endif
            {
                #if defined(HSD_PLATFORM_POSIX)
                if(rhs[0] != '/')
                    return path{relative_name() + "/" + rhs};
                #elif defined(HSD_PLATFORM_WINDOWS)
                if(rhs[0] != L'\\')
                    return path{relative_name() + L"\\" + rhs};
                #endif

                return path{relative_name() + rhs};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path operator/(const string& rhs) const
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path operator/(const wstring& rhs) const
            #endif
            {
                #if defined(HSD_PLATFORM_POSIX)
                if(rhs[0] != '/')
                    return path{relative_name() + "/" + rhs};
                #elif defined(HSD_PLATFORM_WINDOWS)
                if(rhs[0] != L'\\')
                    return path{relative_name() + L"\\" + rhs};
                #endif

                return path{relative_name() + rhs};
            }

            inline path operator/(const path& rhs) const
            {
                #if defined(HSD_PLATFORM_POSIX)
                if(rhs.relative_name()[0] != '/')
                    return path{relative_name() + "/" + rhs.relative_name()};
                #elif defined(HSD_PLATFORM_WINDOWS)
                if(rhs.relative_name()[0] != L'\\')
                    return path{relative_name() + L"\\" + rhs.relative_name()};
                #endif

                return path{relative_name() + rhs.relative_name()};
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path& operator/=(const char* rhs)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path& operator/=(const wchar* rhs)
            #endif
            {
                *this = move(*this / rhs);
                return *this;
            }

            #if defined(HSD_PLATFORM_POSIX)
            inline path& operator/=(const string& rhs)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline path& operator/=(const wstring& rhs)
            #endif
            {
                *this = move(*this / rhs);
                return *this;
            }

            inline path& operator/=(const path& rhs)
            {
                *this = move(*this / rhs);
                return *this;
            }

            inline fs_status status() const
            {
                return {relative_name().c_str()};
            }

            inline auto parents()
            {
                usize _index, _pos;

                #if defined(HSD_PLATFORM_POSIX)
                vector<string> _parents;
                _parents.emplace_back("/", 1);
                #elif defined(HSD_PLATFORM_WINDOWS)
                vector<wstring> _parents;
                _parents.emplace_back(L"C:\\", 3);
                #endif

                #if defined(HSD_PLATFORM_POSIX)
                for (_index = absolute_name().find('/', 1), _pos = 1; 
                    _index != string::npos; _index = absolute_name().find('/', _pos))
                #elif defined(HSD_PLATFORM_WINDOWS)
                for (_index = absolute_name().find(L'\\', 1), _pos = 1; 
                    _index != string::npos; _index = absolute_name().find(L'\\', _pos))
                #endif
                {
                    _parents.emplace_back(
                        absolute_name().c_str() 
                        + _pos, _index - _pos
                    );
                    _pos = _index + 1;
                }

                return _parents;
            }
        };
    } // namespace filesystem
    //#endif
} // namespace hsd

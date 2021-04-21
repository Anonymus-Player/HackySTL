#pragma once

#include "Result.hpp"

#include <unistd.h>
#include <sys/stat.h>

namespace hsd
{
    namespace filesystem
    {
        #define S_IRWXA (S_IRWXO | S_IRWXU | S_IRWXG)

        enum class fs_perms_mask
        {
            owner_read   = S_IRUSR,
            owner_write  = S_IWUSR,
            owner_exec   = S_IXUSR,
            owner_all    = S_IRWXU,
            group_read   = S_IRGRP,
            group_write  = S_IWGRP,
            group_exec   = S_IXGRP,    
            group_all    = S_IRWXG,  
            others_read  = S_IROTH,
            others_write = S_IWOTH,
            others_exec  = S_IXOTH,
            others_all   = S_IRWXO,
            all_perms    = S_IRWXA
        };

        constexpr fs_perms_mask operator|(
            const fs_perms_mask& lhs, const fs_perms_mask& rhs)
        {
            return static_cast<fs_perms_mask>(
                static_cast<i32>(lhs) | static_cast<i32>(rhs)
            );
        }

        struct fs_permissions
        {
            bool can_owner_read;            // File owner has read permission
            bool can_owner_write;           // File owner has write permission
            bool can_owner_exec;            // File owner has execute/search permission
            bool can_owner_do_everything;	// File owner has read, write, and execute/search permissions

            bool can_group_read;	        // The file's user group has read permission
            bool can_group_write;           // The file's user group has write permission
            bool can_group_exec;	        // The file's user group has execute/search permission
            bool can_group_do_everything;   // The file's user group has read, write, and execute/search permissions

            bool can_others_read;           // Other users have read permission
            bool can_others_write;          // Other users have write permission
            bool can_others_exec;           // Other users have execute/search permission
            bool can_others_do_everything;  // Other users have read, write, and execute/search permissions

            bool can_all_do_everything;     // Everyone can read write and execute the file
        };

        class fs_status
        {
        private:
            struct stat _status;
            bool _exists = false;

        public:
            #if defined(HSD_PLATFORM_POSIX)
            inline fs_status(const char* pathname)
            #elif defined(HSD_PLATFORM_WINDOWS)
            inline fs_status(const wchar* pathname)
            #endif
            {
                #if defined(HSD_PLATFORM_POSIX)
                _exists = lstat(pathname, &_status) != -1;
                #elif defined(HSD_PLATFORM_WINDOWS)
                _exists = wstat(pathname, &_status) != -1;
                #endif
            }

            inline bool operator==(const fs_status& rhs) const
            {
                return (
                    #if defined(HSD_PLATFORM_POSIX)
                    _status.st_atim.tv_nsec == rhs._status.st_atim.tv_nsec &&
                    _status.st_ctim.tv_nsec == rhs._status.st_ctim.tv_nsec &&
                    _status.st_mtim.tv_nsec == rhs._status.st_mtim.tv_nsec &&
                    _status.st_atim.tv_sec  == rhs._status.st_atim.tv_sec  &&
                    _status.st_ctim.tv_sec  == rhs._status.st_ctim.tv_sec  &&
                    _status.st_mtim.tv_sec  == rhs._status.st_mtim.tv_sec  &&
                    _status.st_blksize      == rhs._status.st_blksize      &&
                    _status.st_blocks       == rhs._status.st_blocks       &&
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    _status.st_atime        == rhs._status.st_atime        &&
                    _status.st_ctime        == rhs._status.st_ctime        &&
                    _status.st_mtime        == rhs._status.st_mtime        &&
                    _status.st_atime        == rhs._status.st_atime        &&
                    _status.st_ctime        == rhs._status.st_ctime        &&
                    _status.st_mtime        == rhs._status.st_mtime        &&
                    #endif
                    _status.st_nlink        == rhs._status.st_nlink        &&
                    _status.st_mode         == rhs._status.st_mode         &&
                    _status.st_rdev         == rhs._status.st_rdev         &&
                    _status.st_size         == rhs._status.st_size         &&
                    _status.st_dev          == rhs._status.st_dev          &&
                    _status.st_gid          == rhs._status.st_gid          &&
                    _status.st_ino          == rhs._status.st_ino          &&
                    _status.st_uid          == rhs._status.st_uid
                );
            }

            inline bool exists() const
            {
                return _exists;
            }

            inline auto is_directory() const
                -> Result<bool, runtime_error>
            {
                if(_exists == true)
                    return S_ISDIR(_status.st_mode);

                return runtime_error{"File/Directory not found"};
            }

            inline auto is_character() const
                -> Result<bool, runtime_error>
            {
                if(_exists == true)
                    return S_ISCHR(_status.st_mode);

                return runtime_error{"File/Directory not found"};
            }

            inline auto is_block_file() const
                -> Result<bool, runtime_error>
            {
                if(_exists == true)
                    return S_ISBLK(_status.st_mode);

                return runtime_error{"File/Directory not found"};
            }

            inline auto is_regular_file() const
                -> Result<bool, runtime_error>
            {
                if(_exists == true)
                    return S_ISREG(_status.st_mode);

                return runtime_error{"File/Directory not found"};
            }

            inline auto is_fifo_file() const
                -> Result<bool, runtime_error>
            {
                if(_exists == true)
                    return S_ISFIFO(_status.st_mode);

                return runtime_error{"File/Directory not found"};
            }

            inline auto is_symlink() const
                -> Result<bool, runtime_error>
            {
                if(_exists == true)
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    return S_ISLNK(_status.st_mode);
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    return false;
                    #endif
                }

                return runtime_error{"File/Directory not found"};
            }

            inline auto is_socket() const
                -> Result<bool, runtime_error>
            {
                if(_exists == true)
                {
                    #if defined(HSD_PLATFORM_POSIX)
                    return S_ISSOCK(_status.st_mode);
                    #elif defined(HSD_PLATFORM_WINDOWS)
                    return false;
                    #endif
                }

                return runtime_error{"File/Directory not found"};
            }

            inline auto permissions() const
                -> Result<fs_permissions, runtime_error>
            {
                if(_exists == true)
                {
                    return fs_permissions{
                        .can_owner_read           = (_status.st_mode & S_IRUSR) == S_IRUSR,
                        .can_owner_write          = (_status.st_mode & S_IWUSR) == S_IWUSR,
                        .can_owner_exec           = (_status.st_mode & S_IXUSR) == S_IXUSR,
                        .can_owner_do_everything  = (_status.st_mode & S_IRWXU) == S_IRWXU,
                        .can_group_read           = (_status.st_mode & S_IRGRP) == S_IRGRP,
                        .can_group_write          = (_status.st_mode & S_IWGRP) == S_IWGRP,
                        .can_group_exec           = (_status.st_mode & S_IXGRP) == S_IXGRP,
                        .can_group_do_everything  = (_status.st_mode & S_IRWXG) == S_IRWXG,
                        .can_others_read          = (_status.st_mode & S_IROTH) == S_IROTH,
                        .can_others_write         = (_status.st_mode & S_IWOTH) == S_IWOTH,
                        .can_others_exec          = (_status.st_mode & S_IXOTH) == S_IXOTH,
                        .can_others_do_everything = (_status.st_mode & S_IRWXO) == S_IRWXO,
                        .can_all_do_everything    = (_status.st_mode & S_IRWXA) == S_IRWXA
                    };
                }

                return runtime_error{"File/Directory not found"};
            }

            inline auto size() const
                -> Result<usize, runtime_error>
            {
                if(_exists == true)
                    return static_cast<usize>(_status.st_size);

                return runtime_error{"File/Directory not found"};
            }
        };
    } // namespace filesystem
    //#endif
} // namespace hsd